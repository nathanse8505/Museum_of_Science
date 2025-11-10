#pragma once

#include "Arduino.h"
#include "IMode.h"
#include "AccelStepper.h"
#include "Settings.h"
#include "PolarSettings.h"


class PolarMode : public IMode {
    private:
    AccelStepper* stepper_left;
    AccelStepper* stepper_right;
    int l_left, l_right; // current lengths of cables.

    void go_to(int x, int y)
    {
        Serial.println("GOING TO ");
        Serial.print(x);
        Serial.print(" ");
        Serial.println(y);
        Point p = calc_polar_from_xy(x,y);

        l_left = p.x * STEPS_PER_MM;
        l_right = p.y * STEPS_PER_MM;
        stepper_left->moveTo(l_left);
        stepper_right->moveTo(l_right);

        while(stepper_left->run() || stepper_right->run())
        {
            stepper_left->run();
            stepper_right->run();
        }
    }

    void start_calibrate_switches(AccelStepper* as)
    {
        as->setAcceleration(500);
        as->setMaxSpeed(500);
        as->moveTo(-5000);
    }

    void finish_calibrate_switches(AccelStepper* as, int pos)
    {   
        as->setCurrentPosition(pos);
        as->moveTo(pos);
        as->setMaxSpeed(MAX_SPEED);
        as->setAcceleration(ACCELERATION);
    }   

    void move_counterweights_to_switches()
    {
        Serial.println("moving counterweights to switches");

        start_calibrate_switches(stepper_left);
        start_calibrate_switches(stepper_right);
        bool left_at_target = false;
        bool right_at_target = false;
        while(digitalRead(LEFT_LIMIT_SW_PIN) || digitalRead(RIGHT_LIMIT_SW_PIN))
        {
            if(digitalRead(LEFT_LIMIT_SW_PIN) && !left_at_target)
                stepper_left->run();
            else
                left_at_target = true;

            if(digitalRead(RIGHT_LIMIT_SW_PIN) && !right_at_target)
                stepper_right->run();
            else
                right_at_target = true;
        }
        Point p = calc_polar_from_xy(X_HOME, Y_HOME);
        l_left = p.x * STEPS_PER_MM;
        l_right = p.y * STEPS_PER_MM;
        finish_calibrate_switches(stepper_left, l_left);
        finish_calibrate_switches(stepper_right, l_right);

    }

    void initial_move_cables_up()
    {
        Serial.println("moving cables up");
        stepper_left->moveTo(1500);
        stepper_right->moveTo(1500);
        
        while (stepper_left->run() || stepper_right->run())
        {
            stepper_left->run();
            stepper_right->run();
        }
    }

    /// @brief test if the given coords are in the legal area (defined in settings.h), with a possible margin around the legal area acting as a deadband (bc of possible jittery input).
    bool is_within_bounds(long x, long y, int margin=0)
    {
        if (x < (POLAR_X_MIN_LIMIT + margin) || x > (POLAR_X_MAX_LIMIT - margin) || y < (POLAR_Y_MIN_LIMIT + margin) || y > (POLAR_Y_MAX_LIMIT - margin))
            return false;
        return true;
    }

    public:

    PolarMode(AccelStepper* stepper_left, AccelStepper* stepper_right)
    :stepper_left(stepper_left),stepper_right(stepper_right), l_left(0),l_right(0)
    {
        pinMode(LEFT_LIMIT_SW_PIN, INPUT_PULLUP);
        pinMode(RIGHT_LIMIT_SW_PIN, INPUT_PULLUP);
        stepper_left->setMaxSpeed(MAX_SPEED);
        stepper_right->setMaxSpeed(MAX_SPEED);
        stepper_left->setAcceleration(ACCELERATION);
        stepper_right->setAcceleration(ACCELERATION);
    }
    /**
    given the current lengths of the belts 
    */
    Point calc_xy_from_polar(float l1, float l2)
    {
        // calculate the current height and width
        /**
         h = ?, w = ?
        1. (dis - w)^2 + h^2 = l1^2
        2. w^2 + h^2 = l2^2
        --> (motors_distance-w)^2 - w^2 = l1^2 - l2^2
        --> motors_distance^2 -2*motors_distance*w  = l1^2 - l2^2
        **/

        double w = (l1 * l1 - l2 * l2 - double(MOTORS_DISTANCE) * MOTORS_DISTANCE) / (-2 * double(MOTORS_DISTANCE));
        double h = sqrt(l2 * l2 - w * w);
        #if DEBUG_MODE
        Serial.println("conversion to cartesian was");
        Serial.println("cartesian");
        Serial.print(w);
        Serial.print(" ");
        Serial.println(h);
        Serial.println("polar");
        Serial.print(l1);
        Serial.print(" ");
        Serial.println(l2);
        #endif
        return Point{int(w),int(h)};
    }

    Point calc_polar_from_xy(int x, int y)
    {
        double h = y;
        double w = MOTORS_DISTANCE - x;
        int l1 = int(sqrt(h * h + w * w));
        w = w - double(MOTORS_DISTANCE);
        int l2 = int(sqrt(w * w + h * h));
    
        #if DEBUG_MODE
        Serial.println("conversion to polar was");
        Serial.println("cartesian");
        Serial.print(x);
        Serial.print(" ");
        Serial.println(y);
        Serial.println("polar");
        Serial.print(l1);
        Serial.print(" ");
        Serial.println(l2);
        #endif
        
        return Point{l1,l2};
    }
        
    
    void calibrate() override
    {
        #if DEBUG_MODE
        Point p = calc_polar_from_xy(X_HOME,Y_HOME);
        l_left = p.x * STEPS_PER_MM;
        l_right = p.y * STEPS_PER_MM;
        Point p_2 = calc_xy_from_polar(l_left / STEPS_PER_MM, l_right / STEPS_PER_MM);
        Serial.println("inversion property test");
        Serial.println("cartesian");
        Serial.print(X_HOME);
        Serial.print(" ");
        Serial.println(Y_HOME);
        Serial.println("cartesian(polar(cartesian))");
        Serial.print(p_2.x);
        Serial.print(" ");
        Serial.println(p_2.y);
        finish_calibrate_switches(stepper_left, l_left);
        finish_calibrate_switches(stepper_right, l_right);
        #else

        initial_move_cables_up();
        move_counterweights_to_switches();
        Serial.println("finished calibrate");
        #endif
    }

    bool updateEndEffector(int d_right, int d_left) override
    {
        if(d_left == 0 && d_right == 0)
            return false;
        long n_left = l_left + (d_left * STEPPER_STEPSIZE);
        long n_right = l_right + (d_right * STEPPER_STEPSIZE);
        #if DEBUG_MODE
        Serial.println(n_left);
        Serial.println(n_right);
        #endif
        #if ENABLE_SOFT_LIMIT
        Point n = calc_xy_from_polar(n_left / STEPS_PER_MM, n_right / STEPS_PER_MM);

        if(!is_within_bounds(n.x,n.y))
            return false;
        #endif
        
        l_left = n_left;
        l_right = n_right;

        #if ENABLE_SOFT_LIMIT
        // deadband test - if is within deadband update coords but don't move to the edge.
        if(!is_within_bounds(n.x ,n.y, 5))
            return false;
        #endif

        stepper_left->moveTo(l_left);
        stepper_right->moveTo(l_right);
        return true;
    }

    Point get_motor_lengths()
    {
        
        return Point{l_left,l_right};
    }

    Point get_xy()
    {
        return calc_xy_from_polar(l_left / STEPS_PER_MM, l_right / STEPS_PER_MM);
    }

    void go_home() override
    {
        go_to(X_HOME,Y_HOME);
    }
    
    void go_to_start_of_maze() override
    {
        go_to(X_MAZE_START,Y_MAZE_START);
    }
};
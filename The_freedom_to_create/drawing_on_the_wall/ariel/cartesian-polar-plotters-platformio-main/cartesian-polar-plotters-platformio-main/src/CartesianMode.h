#pragma once

#include <Arduino.h>
#include "IMode.h"
#include "AccelStepper.h"
#include "Settings.h"
#include "CartesianSettings.h"

class CartesianMode : public IMode 
{
    private:
        AccelStepper* stepper_x;
        AccelStepper* stepper_y;
        int x, y; // current end effector position

        /// @brief test if the given coords are in the legal area (defined in settings.h), with a possible margin around the legal area acting as a deadband (bc of possible jittery input).
        bool is_within_bounds(long x, long y, int margin=0)
        {
            if (x < (X_MIN_LIMIT + margin) || x > (X_MAX_LIMIT - margin) || y < (Y_MIN_LIMIT + margin) || y > (Y_MAX_LIMIT - margin))
                return false;
            return true;
        }

        void go_to(int nx, int ny)
        {
            x = nx;
            y = ny;
            stepper_x->moveTo(x);
            stepper_y->moveTo(y);
            while(stepper_x->run() || stepper_y->run())
            {
                stepper_x->run();
                stepper_y->run();
            }
            
        }

    public:
        CartesianMode(AccelStepper* xm, AccelStepper* ym)
            : stepper_x(xm), stepper_y(ym), x(0), y(0)
            {
                stepper_x->setMaxSpeed(X_MAX_SPEED * STEPPER_STEPSIZE);
                stepper_x->setAcceleration(X_ACCELERATION * STEPPER_STEPSIZE);
                stepper_y->setMaxSpeed(Y_MAX_SPEED * STEPPER_STEPSIZE);
                stepper_y->setAcceleration(Y_ACCELERATION * STEPPER_STEPSIZE);
                stepper_y->setEnablePin(EN_PIN);
                stepper_y->setPinsInverted(false,false,true);
                stepper_y->enableOutputs();
            }

        bool updateEndEffector(int dx, int dy) override 
        {
            if(dx == 0 && dy == 0)
                return false;
            long nx = x + dx;
            long ny = y + dy;

            #if ENABLE_SOFT_LIMIT
            if(!is_within_bounds(nx, ny))
                return false;            
            #endif
            x = nx;
            y = ny;
            #if ENABLE_SOFT_LIMIT
            // deadband test - if is within deadband update coords but don't move to the edge.
            if(!is_within_bounds(nx, ny, 5))
                return false;            
            #endif
            stepper_x->moveTo(x * STEPPER_STEPSIZE);
            stepper_y->moveTo(y * STEPPER_STEPSIZE);
            #if DEBUG_MODE
            Serial.println("delta:");
            Serial.println(dx);
            Serial.println(dy);
            Serial.print("moving to");
            Serial.println(x);
            Serial.println(y);
            #endif
            return true;
        }

        void calibrate() override
        {
            // Serial.println("not calibrating now");
            // return;
            Serial.println(X_MAX_LIMIT);
            Serial.println(Y_MAX_LIMIT);
            Serial.println(X_MIN_LIMIT);
            Serial.println(Y_MIN_LIMIT);
            
            Serial.println("homing");
            Serial.print("xMotor loc");
            Serial.println(stepper_x->currentPosition());
            // pin to left
            stepper_x->moveTo(stepper_x->currentPosition() - (X_MAX_LIMIT + 100 - X_MIN_LIMIT) * STEPPER_STEPSIZE);
            float acc = stepper_x->acceleration();
            float max_speed = stepper_x->maxSpeed();
            stepper_x->setAcceleration(100);
            stepper_x->setMaxSpeed(1000);
            stepper_x->runToPosition();
            // move to middle
            stepper_x->setAcceleration(acc);
            stepper_x->setMaxSpeed(max_speed);
            stepper_x->moveTo(stepper_x->currentPosition() + X_HOMING_OFFSET * STEPPER_STEPSIZE);
            stepper_x->runToPosition();
            Serial.println("moved x");
            // move to bottom
            stepper_y->disableOutputs();
            Serial.println("disabled y");
            delay(2000);
            stepper_y->enableOutputs();
            Serial.println("enabled y");
            // move to middle
            stepper_y->moveTo(stepper_y->currentPosition() + Y_HOMING_OFFSET * STEPPER_STEPSIZE);
            stepper_y->runToPosition();
            stepper_x->setCurrentPosition(0);
            stepper_y->setCurrentPosition(0);
        }
            
        Point get_motor_lengths()
        {
            return Point{x,y};
        }

        Point get_xy()
        {
            return Point{x,y};
        }

        void go_home() override
        {
            go_to(0,0);
        }
        
        void go_to_start_of_maze()
        {
            go_to(CARTESIAN_X_MAZE,CARTESIAN_Y_MAZE);
        }
};
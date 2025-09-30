#pragma once

#include <Arduino.h>
#include "AccelStepper.h"
#include "RotaryEncoder.h"
#include "Settings.h"
#include "IEncoder.h"
#include "IMode.h"

class PlotterSystem {
private:
    AccelStepper* motor1;
    AccelStepper* motor2;
    IEncoder* encoder1;
    IEncoder* encoder2;
    IMode* mode;
    bool uv_state = false;
    bool went_to_start_of_maze = false;
    unsigned long lastMovementTime;
    
    void update_moved()
    {
        lastMovementTime = millis();
        if (!uv_state)
        {
            uv_state = HIGH;
            digitalWrite(UV_PIN, uv_state);
        }
        went_to_start_of_maze = false;
    }

    void check_idle()
    {
        if (uv_state && ((millis() - lastMovementTime) > UV_AUTO_TURN_OFF_TIME))
        {
            uv_state = LOW;
            digitalWrite (UV_PIN, uv_state);
        }
        if (!went_to_start_of_maze && (millis() - lastMovementTime) > GO_TO_START_OF_MAZE)
        {
            mode->go_to_start_of_maze();
            went_to_start_of_maze = true;
        }
    }

    void home_when_press(IEncoder* enc)
    {
        static bool was_pressed = false;
        if(enc->is_pressed())
        {
            if(!was_pressed)
            {
                mode->go_home();
            }
            was_pressed = true;
        }
        else
            was_pressed = false;
    }
    
    void print_stats_when_press(IEncoder* e)
    {
        static bool was_pressed = false;
        if(e->is_pressed())
        {
            if(!was_pressed)
            {
                Point locs = mode->get_motor_lengths();
                Serial.print("motor 1 Count: ");
                Serial.println(locs.x);
                Serial.print(", motor 2 Count: ");
                Serial.println(locs.y);
            }
            was_pressed = true;
        }
        else
            was_pressed = false;
    }

    void print_xy_when_press(IEncoder* e)
    {
        static bool was_pressed = false;
        if(e->is_pressed())
        {
            if(!was_pressed)
            {
                Point locs = mode->get_xy();
                Serial.print("X Count: ");
                Serial.println(locs.x);
                Serial.print("Y Count: ");
                Serial.println(locs.y);
            }
            was_pressed = true;
        }
        else
            was_pressed = false;
    }

public:
    PlotterSystem(AccelStepper* m1, AccelStepper* m2, IEncoder* e1, IEncoder* e2, IMode* md)
    : motor1(m1), motor2(m2), encoder1(e1), encoder2(e2), mode(md), lastMovementTime(millis()) 
    { 
        pinMode(UV_PIN, OUTPUT);
    }

    void loop() 
    {
        int delta1 = encoder1->readDelta();
        int delta2 = encoder2->readDelta();
        #if ENCODER_DEBUG
        if(delta1 != 0 || delta2 != 0)
        {
            Serial.print("system got from encoder: ");
            Serial.println(delta1);
            Serial.println(delta2);   
        }
        #endif

        bool moved = mode->updateEndEffector(delta1, delta2);
        motor1->run();
        motor2->run();

        if(moved)
            update_moved();
        else
            check_idle();
        #if !PRODUCTION_MODE
        home_when_press(encoder1);
        print_xy_when_press(encoder2);
        #endif
    }

    void calibrate() 
    { 
        mode->calibrate(); 
        lastMovementTime = millis();
    }

};
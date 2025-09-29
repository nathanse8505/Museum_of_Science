#pragma once

class IMode {
    public:
        virtual bool updateEndEffector(int delta1, int delta2) = 0; // movement
        virtual void calibrate() = 0; // calibration inside the mode]
        virtual Point get_motor_lengths() = 0;
        virtual Point get_xy() = 0;
        virtual void go_home() = 0;
        virtual void go_to_start_of_maze() = 0;
};
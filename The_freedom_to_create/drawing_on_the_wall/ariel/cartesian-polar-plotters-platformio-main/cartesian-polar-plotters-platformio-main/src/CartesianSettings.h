#pragma once

#define MINI_SETUP false

#define X_MAX_SPEED 3000
#define Y_MAX_SPEED 2000
#define X_ACCELERATION 1500
#define Y_ACCELERATION 1000

#define STEPPER_STEPSIZE 4

// settings for the miniature version of the cartesian setup (i.e. not the full-size exhibit one)
#if MINI_SETUP
#define X_HOMING_OFFSET 250
#define Y_HOMING_OFFSET (-200)

#define Y_MIN_LIMIT (-200)
#define X_MIN_LIMIT (-250)
#define Y_MAX_LIMIT 200
#define X_MAX_LIMIT 250
#else
#define X_HOMING_OFFSET 900
#define Y_HOMING_OFFSET (-550)

#define Y_MIN_LIMIT (-500)
#define X_MIN_LIMIT (-850)
#define Y_MAX_LIMIT 500
#define X_MAX_LIMIT 780
#endif

// not verified yet.
#define CARTESIAN_X_MAZE (-200)
#define CARTESIAN_Y_MAZE (500)

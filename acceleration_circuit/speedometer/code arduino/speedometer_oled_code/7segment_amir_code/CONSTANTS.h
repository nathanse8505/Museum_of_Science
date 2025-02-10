
int const  MIN_ELECTROLYSIS_TIME = 5 ; //Seconds
int const  MAX_ELECTROLYSIS_TIME = 60 ; //Seconds
uint32_t const DEFAULT_ELECTROLYSIS_TIME = 30; //sec 

int const PRESCALAR = 4 ; //encoder coun per "click"

int const  MIN_PWM = 0 ; //PWM set electrolysis current
int const  MAX_PWM = 75; //PWM ser electrolysis current
int const  DEFAULT_PWM = 40 ; //asume 3Volt to set 2A current. based on Dec experiments with 50# stainless steel mesh

int const  SPARK_TIME = 500 ; //mSeconds

int const  WAIT_FOR_CALIBRATION = 4000 ; //mSeconds waiting during switch preased for calibration

int const  BLINK_ON = 800 ; //mSeconds
int const  BLINK_OFF = 200 ; //mSeconds
int const  BLINK_TIME = 5;// Seconds !!
int const  BOUNCE_TIME = 50; //ms

#define SERIAL_BAUD_RATE (115200)
#define BCD_A (4) // Output to BCD_A (4511)
#define BCD_B (5) // Output to BCD_B (4511)
#define BCD_C (6) // Output to BCD_C (4511)
#define BCD_D (7) // Output to BCD_D (4511)

#define LE_LEFT (8) // Output Left (100th) digit LE (negative pulse) used as 10th
#define LE_RIGHT (9) // Output right (10th) digit LE (negative pulse) used as units 

//#define LE_EX (10) // Output external (units) digit LE (negative pulse) not used 
#define ELECTROD_PWM (10) // Output to FET drive electrod ecurrent
#define PS_OUT (10) // Output to FET drive power supply 
#define SPARK_OUT (11) // Output to FET drive HV spark module

#define SW_IN (12) // input from pushbutton switch (nead pull up SW/HW)

#define SET_CURRENT_TRIMER_IN (A0) // analod input to set PWM

#define ENCODER_BIT_0 (2) // input IO for gray code bit 0, Gren wire in 400 and 100 ppr encoders 
#define ENCODER_BIT_1 (3) // input IO for gray code bit 1, white wire in 400 and 100 ppr encoders 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ruud Boer April 24, 2015
// DCC Servo Tune v3.0
//
// PURPOSE: Tune the angles of 2 setpopints of servos.
//          Values are shown on Serial Monitor to be (manually) transferred to DCC_Decoder_Servo.
//
// CONNECT THE SERVO TO PIN 3
//
// REMARK 1: when looking at the axle of the servo, CW = lowe angle values, CCW = higher angle values.
//
// REMARK 2: Timing for Servo, Setpoint and Print can be changed in lines 37,38,39.
//
// REMARK 3: To avoid servo movement which can give high current draw at startup,
//           first start the Arduino and after led blinked 5 times switch the servo power on.
//           Before power down, move the servo to setpoint 1 = CCWidle.
//
// OPERATION:
//   A0: Select setpoint 0 MAX angle, CCW (looking at axle)
//   A1: Select setpoint 1 MIN angle, CW (looking at axle)
//   A2: Increase angle of current Setpoint, CCW
//   A3: Decrease angle of current Setpoint. CW
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations and initialization
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

byte led = 13;
byte angle = 90;
byte sp=2; //servo setpoint, initial set at the 2nd setpoint which is 90 degrees 
byte setpoint[4] = {94,86,90};
Servo servo; //Servo functions included via 'include servo.h'
byte servotimer = 60; //Servo update timer. Lower value = higher servo speed. 10ms = 100 deg/s
byte setpointtimer = 250; //Setpoint change 1 degree timer. 250ms = 4 deg/s
int printtimer = 2000; //Serial print timer
unsigned long timeforservo = millis() + servotimer;
unsigned long timetoupdatesetpoint = millis() + setpointtimer;
unsigned long timetoprint = millis() + printtimer;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printData() {
Serial.print("Min angle CW   = ");
Serial.println(setpoint[1]);
Serial.print("Max angle CCW  = ");
Serial.println(setpoint[0]);
Serial.println();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup (run once)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{

pinMode(A0,INPUT_PULLUP);
pinMode(A1,INPUT_PULLUP);
pinMode(A2,INPUT_PULLUP);
pinMode(A3,INPUT_PULLUP);
pinMode(A4,INPUT_PULLUP);
pinMode(A5,INPUT_PULLUP);
pinMode(led,OUTPUT);
digitalWrite(led,LOW);

servo.attach(3); //servo is connected to pin #

Serial.begin(9600);

} // end setup

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main loop (run continuous)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
if (angle==90) digitalWrite(led,HIGH); else digitalWrite(led,LOW);

////////////////////////////////////////////////////////////////
// Setpoint selection
if (digitalRead(A0)==LOW) sp=0; //go to min angle
if (digitalRead(A1)==LOW) sp=1; //go to max angle

////////////////////////////////////////////////////////////////
// Time to move servos 1 degree
if (millis() > timeforservo) {
	timeforservo = millis() + servotimer;
	if (angle < setpoint[sp]) angle++;
	if (angle > setpoint[sp]) angle--;
	servo.write(angle);
}

////////////////////////////////////////////////////////////////
// Time to print data
if (millis() > timetoprint) {
	timetoprint = millis() + printtimer;
	printData();
}

////////////////////////////////////////////////////////////////
// Time to update setpoit
if (millis() > timetoupdatesetpoint) {
	timetoupdatesetpoint = millis() + setpointtimer;
	if (digitalRead(A2)==LOW) setpoint[sp]++; //increase angle, CCW
	if (digitalRead(A3)==LOW) setpoint[sp]--; //decrease angle, CW
}

} // end loop

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

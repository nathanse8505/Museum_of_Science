/*
S88 sensor interface to ECoS.
Connections:
s88 pin 1 data - ARD pin 13 = output
s88 pin 2 GND  to ARD GND
s88 pin 3 clock to ARD pin 2, interrupt 0
s88 pin 4 PS to ARD pin 3, interrupt 1
s88 pin 6 V+ to ARD Vin
ARD pin 12 = dataIn from next Arduino in the S88 chain
sensors: see table in void Setup().
REMARK 1: inputs have a pullup resistor active, the sensors must pull the input to GND.
REMARK 2: Max 16 sensors for now. Not yet tested with multiple Arduino's connected.
That will have to wait until I have a few more Arduino's ordered and shipped in.
*/

int clockCounter=0;
int sensors=0;
int data=0;
const byte dataIn=12;  //data input from next Arduino in S88 chain
const byte dataOut=13; //data output pin=13
boolean loadSensors=false; //flag that says to load sensor bits into dataOut bits

void setup() {
  attachInterrupt(0,clock,RISING); //pin 2 = clock interrupt
  attachInterrupt(1,PS,RISING);    //pin 3 = PS interrupt
  pinMode(dataIn,INPUT_PULLUP);
  pinMode(dataOut, OUTPUT); //pin 13 = data out to ECoS or to previous Arduino in S88 chain
  digitalWrite(dataOut, LOW);   //LED off
  pinMode(A0, INPUT_PULLUP); //sensor 01
  pinMode(A1, INPUT_PULLUP); //sensor 02
  pinMode(A2, INPUT_PULLUP); //sensor 03
  pinMode(A3, INPUT_PULLUP); //sensor 04
  pinMode(A4, INPUT_PULLUP); //sensor 05
  pinMode(A5, INPUT_PULLUP); //sensor 06
  pinMode(0, INPUT_PULLUP);  //sensor 07
  pinMode(1, INPUT_PULLUP);  //sensor 08
  pinMode(4, INPUT_PULLUP);  //sensor 09
  pinMode(5, INPUT_PULLUP);  //sensor 10
  pinMode(6, INPUT_PULLUP);  //sensor 11
  pinMode(7, INPUT_PULLUP);  //sensor 12
  pinMode(8, INPUT_PULLUP);  //sensor 13
  pinMode(9, INPUT_PULLUP);  //sensor 14
  pinMode(10, INPUT_PULLUP); //sensor 15
  pinMode(11, INPUT_PULLUP); //sensor 16
}

void loop() {
  if (!digitalRead(A0)) {bitSet(sensors,0);}
  if (!digitalRead(A1)) {bitSet(sensors,1);}
  if (!digitalRead(A2)) {bitSet(sensors,2);}
  if (!digitalRead(A3)) {bitSet(sensors,3);}
  if (!digitalRead(A4)) {bitSet(sensors,4);}
  if (!digitalRead(A5)) {bitSet(sensors,5);}
  if (!digitalRead(0)) {bitSet(sensors,6);}
  if (!digitalRead(1)) {bitSet(sensors,7);}
  if (!digitalRead(4)) {bitSet(sensors,8);}
  if (!digitalRead(5)) {bitSet(sensors,9);}
  if (!digitalRead(6)) {bitSet(sensors,10);}
  if (!digitalRead(7)) {bitSet(sensors,11);}
  if (!digitalRead(8)) {bitSet(sensors,12);}
  if (!digitalRead(9)) {bitSet(sensors,13);}
  if (!digitalRead(10)) {bitSet(sensors,14);}
  if (!digitalRead(11)) {bitSet(sensors,15);}
  
  if (loadSensors) {
    for (int n=0; n<16; n++) {
      if (bitRead(sensors,n)) {bitSet(data,n);} else {bitClear(data,n);}
      bitClear(sensors,n);
    }
    loadSensors=false;
  }
}

void clock() {
  digitalWrite(dataOut,bitRead(data,clockCounter));
  //if (digitalRead(dataIn)) {bitSet(data,clockCounter);} else {bitClear(data,clockCounter);}
  clockCounter =(clockCounter + 1) % 16;
}

void PS() {
  clockCounter=0;
  loadSensors=true;
}

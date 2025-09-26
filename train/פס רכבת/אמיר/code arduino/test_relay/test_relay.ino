// contorol train barriers, train (Shimon) under floor exhibit
// 30 years museum enaversary, Summer 2022
// 2 Trains on same circular rail divided to 3 sections 
//When one train go into sector i (i = 0,1,2)and pass sensor i - the controler block  
// second train to go into same section by disconnect power from the begining of this section. 
// and open i-1 section - see fulle expalin at 
//https://docs.google.com/document/d/1Xm8vYSGSgmGfYhY095-ccse9BiLPy-t-vI_z9wcCI0Y/edit 
// Hardware/layout   
//3 inputs from IR block light sensors (E18-D80, GND - BLUE !!!, +5V- brown, out - BLACK ! 
// High when light NOT refelected = train cross)
// 3 outputs- throught AO3400) 3 Relays activated via AO3400 
// Used NC connection - tative the relay to stop the strain (so in case of mailfunction we can switch off
// see J:\Bloomfield\Exhibits\היסטוריה לקראת שלושים שנה למוזיאון\רכבת שמעון\פיתוח מחסום נתנאל יוני 2022

//
const int Sample_Time = 100; // ms time between measure 
#define   SEGMENT_1_SENSOR_PIN    A0//14 // A0 (see https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h#L56-L72)
#define   SEGMENT_1_BARRIER_PIN   9
#define   SEGMENT_2_SENSOR_PIN    A1//15  //A1
#define   SEGMENT_2_BARRIER_PIN   8
#define   SEGMENT_3_SENSOR_PIN    A2//16  //A2
#define   SEGMENT_3_BARRIER_PIN   7


// do we get a HIGH/ LOW when a sensor detects a train
const int trainDetectionSignal = LOW; 

// do we write HIGH/LOW to close the barrier
const int barrierCloseSignal = LOW;

const int sensorPins [] = {SEGMENT_1_SENSOR_PIN, SEGMENT_2_SENSOR_PIN, SEGMENT_3_SENSOR_PIN};
const int barrierPins [] = {SEGMENT_1_BARRIER_PIN, SEGMENT_2_BARRIER_PIN, SEGMENT_3_BARRIER_PIN};

const int numOfSegments = sizeof(sensorPins)/sizeof(sensorPins[0]);

void setup() {
  Serial.begin(115200);
  if (trainDetectionSignal == LOW){
      pinMode(SEGMENT_1_SENSOR_PIN, INPUT_PULLUP);
      pinMode(SEGMENT_2_SENSOR_PIN, INPUT_PULLUP);
      pinMode(SEGMENT_3_SENSOR_PIN, INPUT_PULLUP);
  }
  else {// signal gets HIGH when we detect, need an external pulldown / pulldown in sensor
      pinMode(SEGMENT_1_SENSOR_PIN, INPUT);
      pinMode(SEGMENT_2_SENSOR_PIN, INPUT);
      pinMode(SEGMENT_3_SENSOR_PIN, INPUT);
  }

  pinMode(SEGMENT_1_BARRIER_PIN, OUTPUT);
  pinMode(SEGMENT_2_BARRIER_PIN, OUTPUT);
  pinMode(SEGMENT_3_BARRIER_PIN, OUTPUT);

}


void loop() {
 
  int barrierCurrentState = digitalRead(A0);
  if (barrierCurrentState == trainDetectionSignal) {
    digitalWrite(9, HIGH);
    Serial.println("ok");
    delay(100);
  }else{
     digitalWrite(9, LOW);
    Serial.println("ok low");
    delay(100);
  }

}


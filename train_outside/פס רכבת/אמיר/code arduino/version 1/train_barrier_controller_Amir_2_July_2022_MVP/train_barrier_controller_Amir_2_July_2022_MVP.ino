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
const int Sample_Time = 2; // ms time between measure 
#define   SEGMENT_1_SENSOR_PIN    A1//14 // A0 (see https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h#L56-L72)
#define   SEGMENT_1_BARRIER_PIN   8
#define   SEGMENT_2_SENSOR_PIN    A2//15  //A1
#define   SEGMENT_2_BARRIER_PIN   7
#define   SEGMENT_3_SENSOR_PIN    A3//16  //A2
#define   SEGMENT_3_BARRIER_PIN   6


// do we get a HIGH/ LOW when a sensor detects a train
const int trainDetectionSignal = LOW; 

// do we write HIGH/LOW to close the barrier
const int barrierCloseSignal = LOW;

const int sensorPins [] = {SEGMENT_1_SENSOR_PIN, SEGMENT_2_SENSOR_PIN, SEGMENT_3_SENSOR_PIN};
const int barrierPins [] = {SEGMENT_1_BARRIER_PIN, SEGMENT_2_BARRIER_PIN, SEGMENT_3_BARRIER_PIN};

//const int sensorPins [] = {SEGMENT_2_SENSOR_PIN, SEGMENT_3_SENSOR_PIN};
//const int barrierPins [] = {SEGMENT_2_BARRIER_PIN, SEGMENT_3_BARRIER_PIN};
const int numOfSegments = sizeof(sensorPins)/sizeof(sensorPins[0]);

void setup() {
  Serial.begin(115200);
  if (trainDetectionSignal == LOW)
  {
      pinMode(SEGMENT_1_SENSOR_PIN, INPUT_PULLUP);
      pinMode(SEGMENT_2_SENSOR_PIN, INPUT_PULLUP);
      pinMode(SEGMENT_3_SENSOR_PIN, INPUT_PULLUP);
  }
  else // signal gets HIGH when we detect, need an external pulldown / pulldown in sensor
  {
      pinMode(SEGMENT_1_SENSOR_PIN, INPUT);
      pinMode(SEGMENT_2_SENSOR_PIN, INPUT);
      pinMode(SEGMENT_3_SENSOR_PIN, INPUT);
  }

  pinMode(SEGMENT_1_BARRIER_PIN, OUTPUT);
  pinMode(SEGMENT_2_BARRIER_PIN, OUTPUT);
  pinMode(SEGMENT_3_BARRIER_PIN, OUTPUT);

  for (int i = 0; i < numOfSegments; i++) 
  {
    openBarrier(i);
  }

  closeBarrier(0);
}

void closeBarrier(int segmentIndex)
{
    Serial.print("closing barrier: ");
    Serial.print(segmentIndex + 1);
    Serial.print("\r\n");
    digitalWrite(barrierPins[segmentIndex], barrierCloseSignal);
}

void openBarrier(int segmentIndex)
{
    Serial.print("opening barrier: ");
    Serial.print(segmentIndex + 1);
    Serial.print("\r\n");
    digitalWrite(barrierPins[segmentIndex], !barrierCloseSignal);
}

int getPreviuosSegment(int segment)
{
    if (segment > 0)  
    {
      return segment - 1;
    }
    else
    {
      return numOfSegments - 1;
    }
}

void handleTrainDetected(int segmentIndex)
{
    int barrierPin = barrierPins[segmentIndex];
    int barrierCurrentState = digitalRead(barrierPin);
    int prevBarrierPin = barrierPins[getPreviuosSegment(segmentIndex)];
    int prevBarrierCurrentState = digitalRead(prevBarrierPin);
    
    if ((barrierCurrentState != barrierCloseSignal) || (prevBarrierCurrentState == barrierCloseSignal)) 
    // train is detected when the barrier is still open -> close it and open the last barrier
    // if barrier is closed but previous barrier is also closed -> open previous 
    // (for breaking the initial state that everything is closed)
    {
        Serial.print("train detected in new segment (");
        Serial.print(segmentIndex + 1);
        Serial.print(")\r\n");
        closeBarrier(segmentIndex);
        openBarrier(getPreviuosSegment(segmentIndex));
    }
}

void loop() {
 // delay(Sample_Time) ;
//  Serial.println("================");
//   int tttt = digitalRead(14);
//   Serial.println(tttt);
for (int i = 0; i < numOfSegments; i++) {
  int sensorPin = sensorPins[i];
  int barrierCurrentState = digitalRead(sensorPin);
  int prevBarrierPin = barrierPins[getPreviuosSegment(i)];
  if (barrierCurrentState == trainDetectionSignal) {
    //closeBarrier(i);
    //openBarrier(getPreviuosSegment(i));  
    openBarrier(i);
    closeBarrier(getPreviuosSegment(i));  
  }
}

}
  /*
  for (int i = 0; i < numOfSegments; i++) 
  {
      int sensorPin = sensorPins[i];
      int pinState = digitalRead(sensorPin);
      
      if (pinState == trainDetectionSignal)
      {
          handleTrainDetected(i);
      }
  }
}
*/

#define midi_reset 10
#define data_in  2
#define clk  3
#define load 4
#define pwm1 5
#define pwm2 6
#define volume_pin 7
#define less_notes_pin 8
#define led_pin 13

#define SENSORS_NUM  24
#define CHANNEL 1
#define INSTRUMENT  46
#define APPLAUSE 126
#define SEC_TO_MILI 1000ul
#define MINUTE_TO_SEC 60
#define AUTO_PLAY_MINUTES 2
#define AUTO_PLAY_TIME (AUTO_PLAY_MINUTES*MINUTE_TO_SEC*SEC_TO_MILI)
//unsigned long AUTO_PLAY_TIME =  1000*60*5;
#define TIME_BETWEEN_SAMPLES 100    // [mS]
#define DELTA 2.5                                          // conversion from midi ticks to milliseconds
#define HEAD_WIDTH 8                                 // number of covered sensors to enter SAFTY mode
#define SAFETY_SAMPLE_DELAY 100       // [mS]
#define VOLUME_TIMEOUT_MINUTES 1
#define VOLUME_TIMEOUT (VOLUME_TIMEOUT_MINUTES*MINUTE_TO_SEC*SEC_TO_MILI)
//unsigned int VOLUME_TIMEOUT = 1000*60; 
#define LOW_VOLUME 40 //20                               // range from 0-127
#define HIGH_VOLUME 100 //80                             // range from 0-127
#define DEBOUNCING_DELAY 1000
#define PRE_SAMPLE_DELAY 30                 // [uS] time between laser ON to sampling

int LITTLE_YONI[] = {7,-3, 0, 1, -3, 0, -2, 2, 2, 1, 2, 0, 0, 0, -3, 0, 1, -3, 0, -2, 4, 3, 0, -7, 2, 0, 0, 0, 0, 2, 1, -1, 0, 0, 0, 0, 1, 2, 0, -3, 0, 1, -3, 0, -2, 4, 3, 0, -7};
int index_of_yoni = 0 ;
int last_played_note = 0;
int YONI_WINS = 13;

//int notes[SENSORS_NUM] = {48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88};
int notes[SENSORS_NUM] = {60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100};
int less_notes[SENSORS_NUM] = {0,60,0,62,0,64,0,65,0,67,0,69,0,71,0,72,0,74,0,76,0,77,0,79};
int curr_notes[SENSORS_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int prev_notes[SENSORS_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long lastPlayTime = 0;
unsigned long volumeModeStartTime = 0;
int volumeMode = 0;


unsigned long DEBOUNCE_THREASHOLD = 20;
unsigned long  changeTime0[SENSORS_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long  changeTime1[SENSORS_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long  changeTime2[SENSORS_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void setup(){
  Serial.begin(31250);
  pinMode(midi_reset, OUTPUT);
  
  pinMode(data_in, INPUT);
  pinMode(clk, OUTPUT);
  pinMode(load, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);  
  pinMode(volume_pin, INPUT_PULLUP);
  pinMode(less_notes_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  
  digitalWrite(load, HIGH);
  digitalWrite(clk, HIGH);
  digitalWrite(midi_reset, HIGH);
  digitalWrite(pwm1,LOW);
  digitalWrite(pwm2,LOW);  
  digitalWrite(led_pin, LOW);
  delay(1000);
  setProg(CHANNEL, INSTRUMENT);
  setProg(CHANNEL + 1, APPLAUSE);
  setVolume(CHANNEL, HIGH_VOLUME); 
  setVolume(CHANNEL + 1, HIGH_VOLUME*2);
}

void loop(){
   readData();
   playNotes();
   //delay(1000);
   if ((millis() - lastPlayTime) > AUTO_PLAY_TIME){
     setVolume(CHANNEL, LOW_VOLUME); 
     playSong();
     setVolume(CHANNEL, HIGH_VOLUME); 
       //playPsy();
   }
   pollPins();
}

void setControl(int channel, int command, int value){
    Serial.write(0xb0 + channel);
    Serial.write(command);
    Serial.write(value);
}

void setVolume(int channel, int volume){
    setControl(channel, 0x07, volume);
}

void pollPins(){
  int volumePinMode = digitalRead(volume_pin);
  if(!volumePinMode && ((millis() -  volumeModeStartTime) > DEBOUNCING_DELAY)){
     volumeModeStartTime = millis();
     volumeMode = !volumeMode;
     setVolume(CHANNEL, volumeMode? LOW_VOLUME: HIGH_VOLUME); 
  }
  if(volumeMode && (millis() - volumeModeStartTime) > VOLUME_TIMEOUT){
    volumeMode = 0;
    setVolume(CHANNEL, HIGH_VOLUME);   
} 
}

void    playNotes(){
    int sequence = 0;  
    for(int i=0; i<SENSORS_NUM; i++){
       if(curr_notes[i] == 0) sequence = 0;
       else sequence += 1;
       if (sequence >= HEAD_WIDTH){
         safetyShutDown();
         return;
       }
           if (prev_notes[i] == 0 && curr_notes[i] == 1){ 
                if ((changeTime0[i] - changeTime1[i] > DEBOUNCE_THREASHOLD) && (changeTime1[i] - changeTime2[i] > DEBOUNCE_THREASHOLD)){
                    if (digitalRead(less_notes_pin)) playNote(less_notes[SENSORS_NUM-i-1], 80);
                    else                             playNote(notes[SENSORS_NUM-i-1], 80);
                    lastPlayTime = millis();
                }
          }
      }    
}

void safetyShutDown(){
  int headInside = 1;  
  while(headInside){
      headInside = 0;
      delay(SAFETY_SAMPLE_DELAY);
      readData();
      int sequence = 0;  
      for(int i=0; i<SENSORS_NUM; i++){
         if(curr_notes[i] == 0) sequence = 0;
         else sequence += 1;
         if (sequence >= HEAD_WIDTH)
            headInside = 1;         
       }
  }    
}

int delayAndSample(int ms){
    int numOfDelaySegments = ms / TIME_BETWEEN_SAMPLES;
    for (int i = 0; i <  numOfDelaySegments; i++) {
        int delayTime = min((ms - i * TIME_BETWEEN_SAMPLES) , TIME_BETWEEN_SAMPLES);
        delay(delayTime);
        readData();
        for(int j=0; j<SENSORS_NUM; j++){
          if (curr_notes[j] != prev_notes[j]){
            lastPlayTime = millis();  
            return(1);
          }
      }
    }
    return (0);
}
        

void setProg(byte channel, byte instrument){
  Serial.write(0xc0 + channel);
  delay(10);
  Serial.write(instrument);
}

void playNote(byte note, byte velocity){
stopNote(CHANNEL, note, velocity);
startNote(CHANNEL, note, velocity);
littleYoniCompare(note);
last_played_note = note;  

}

void littleYoniCompare(int note){
  if (index_of_yoni == 0){
    if(note%12 == LITTLE_YONI[index_of_yoni]){
      //if(note == 67){
      index_of_yoni += 1;
    }
  }else{
     if(note -  last_played_note == LITTLE_YONI[index_of_yoni]){
        index_of_yoni += 1;
     }else{
       index_of_yoni = 0;
     }
  }
  if (index_of_yoni == YONI_WINS){
    kapaim();
    index_of_yoni = 0;
  }
}

void kapaim(){
  startNote(CHANNEL +1, 80, 80);
  delay(10*1000);
  stopNote(CHANNEL +1, 80, 80);
}

void startNote(byte channel, byte note, byte velocity){
// note on
   Serial.write(0x90 + channel);
   Serial.write(note);
   Serial.write(velocity);
}

void stopNote(byte channel, byte note, byte velocity){
  // note off 
   //Serial.write(0x80 +1);
   Serial.write(0x80 + channel);
   Serial.write(note);
   Serial.write(velocity);
}

void readData(){
      if (!digitalRead(less_notes_pin)) digitalWrite(pwm1,HIGH);  // power the pwm1 lasers only if we are not in the "less notes mode" 
      digitalWrite(pwm2,HIGH);
      delayMicroseconds(PRE_SAMPLE_DELAY);
      digitalWrite(load, LOW);
      delayMicroseconds(PRE_SAMPLE_DELAY);
      digitalWrite(pwm1,LOW);
      digitalWrite(pwm2,LOW);
      //delay(50);
      digitalWrite(load, HIGH);
    unsigned long t = millis();
    for(int i=0; i<SENSORS_NUM; i++){
      prev_notes[i] = curr_notes[i];
      curr_notes[i] = digitalRead(data_in);
      if (curr_notes[i] != prev_notes[i]){
        changeTime2[i] = changeTime1[i];
        changeTime1[i] = changeTime0[i];
        changeTime0[i] = t;
      }
      digitalWrite(clk, LOW);
      //delay(50);
      digitalWrite(clk, HIGH);
      //delay(50);
      
    }
}


void playSong(){
if(delayAndSample(1*DELTA)) return;
startNote( 1, 38, 47);
if(delayAndSample(255*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 58);
startNote( 1, 54, 58);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 37);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 49);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 56);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 46);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 38, 47);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 74);
startNote( 1, 54, 74);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 72);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 88);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 113);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 109);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 59, 121);
startNote( 1, 62, 121);
if(delayAndSample(214*DELTA)) return;
stopNote( 1, 67, 0);
startNote( 1, 69, 95);
if(delayAndSample(22*DELTA)) return;
startNote( 1, 67, 95);
if(delayAndSample(20*DELTA)) return;
startNote( 1, 66, 95);
stopNote( 1, 69, 0);
stopNote( 1, 67, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 57, 103);
startNote( 1, 62, 103);
if(delayAndSample(247*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(9*DELTA)) return;
startNote( 1, 64, 93);
stopNote( 1, 57, 0);
stopNote( 1, 62, 0);
startNote( 1, 52, 99);
startNote( 1, 61, 99);
if(delayAndSample(243*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 66, 109);
stopNote( 1, 52, 0);
stopNote( 1, 61, 0);
startNote( 1, 38, 91);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 116);
startNote( 1, 54, 116);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 97);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 98);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 118);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 109);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 47, 90);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 96);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 106);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 116);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 113);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 47, 95);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 98);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 101);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 115);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 73, 119);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 54, 103);
startNote( 1, 57, 103);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 71, 83);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 73, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 69, 69);
stopNote( 1, 54, 0);
stopNote( 1, 57, 0);
startNote( 1, 61, 86);
if(delayAndSample(256*DELTA)) return;
startNote( 1, 68, 52);
stopNote( 1, 61, 0);
startNote( 1, 53, 45);
if(delayAndSample(5*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(238*DELTA)) return;
stopNote( 1, 68, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 69, 44);
stopNote( 1, 53, 0);
startNote( 1, 54, 39);
if(delayAndSample(512*DELTA)) return;
startNote( 1, 67, 40);
stopNote( 1, 54, 0);
startNote( 1, 52, 36);
if(delayAndSample(10*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(233*DELTA)) return;
stopNote( 1, 67, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 66, 35);
stopNote( 1, 52, 0);
startNote( 1, 38, 33);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 43);
startNote( 1, 54, 43);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 31);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 38);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 45);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 40);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 38, 34);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 73);
startNote( 1, 54, 73);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 68);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 86);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 114);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 109);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 59, 124);
startNote( 1, 62, 124);
if(delayAndSample(214*DELTA)) return;
stopNote( 1, 67, 0);
startNote( 1, 69, 98);
if(delayAndSample(22*DELTA)) return;
startNote( 1, 67, 98);
if(delayAndSample(20*DELTA)) return;
startNote( 1, 66, 98);
stopNote( 1, 69, 0);
stopNote( 1, 67, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 57, 103);
startNote( 1, 62, 103);
if(delayAndSample(256*DELTA)) return;
startNote( 1, 64, 93);
stopNote( 1, 57, 0);
stopNote( 1, 62, 0);
startNote( 1, 52, 97);
startNote( 1, 61, 97);
if(delayAndSample(5*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(238*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 66, 105);
stopNote( 1, 52, 0);
stopNote( 1, 61, 0);
startNote( 1, 38, 90);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 115);
startNote( 1, 54, 115);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 98);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 101);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 114);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 112);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 47, 93);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 99);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 101);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 112);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 107);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 47, 92);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 93);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 99);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 105);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 74, 115);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 57, 102);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 103);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 91);
stopNote( 1, 57, 0);
startNote( 1, 55, 105);
startNote( 1, 57, 105);
if(delayAndSample(256*DELTA)) return;
startNote( 1, 64, 80);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 49, 106);
startNote( 1, 55, 106);
startNote( 1, 57, 106);
if(delayAndSample(5*DELTA)) return;
stopNote( 1, 67, 0);
if(delayAndSample(238*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 62, 96);
stopNote( 1, 49, 0);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 50, 89);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 62, 0);
stopNote( 1, 50, 0);
startNote( 1, 38, 44);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 43);
if(delayAndSample(128*DELTA)) return;
stopNote( 1, 66, 0);
startNote( 1, 66, 50);
stopNote( 1, 38, 0);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 55);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 69, 50);
startNote( 1, 45, 46);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 45, 0);
startNote( 1, 49, 61);
startNote( 1, 55, 61);
startNote( 1, 57, 61);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 64, 43);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 64, 40);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 43);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 69, 50);
stopNote( 1, 49, 0);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 50, 40);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 50, 0);
startNote( 1, 54, 65);
startNote( 1, 57, 65);
startNote( 1, 62, 65);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 47);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 49);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 54);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 74, 57);
stopNote( 1, 54, 0);
stopNote( 1, 57, 0);
stopNote( 1, 62, 0);
startNote( 1, 50, 43);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 50, 0);
startNote( 1, 54, 60);
startNote( 1, 57, 60);
startNote( 1, 60, 60);
startNote( 1, 62, 60);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 38);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 69, 55);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 74, 66);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 74, 68);
stopNote( 1, 54, 0);
stopNote( 1, 57, 0);
stopNote( 1, 60, 0);
stopNote( 1, 62, 0);
startNote( 1, 43, 64);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 43, 0);
startNote( 1, 55, 101);
startNote( 1, 59, 101);
startNote( 1, 62, 101);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 71, 85);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 87);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 74, 114);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 76, 111);
stopNote( 1, 55, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 40, 97);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 40, 0);
startNote( 1, 50, 117);
startNote( 1, 56, 117);
startNote( 1, 59, 117);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 71, 93);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 76, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 100);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 74, 111);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 74, 101);
stopNote( 1, 50, 0);
stopNote( 1, 56, 0);
stopNote( 1, 59, 0);
startNote( 1, 45, 103);
startNote( 1, 50, 103);
startNote( 1, 55, 103);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 73, 95);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 73, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 93);
startNote( 1, 71, 93);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 67, 0);
startNote( 1, 67, 86);
startNote( 1, 69, 86);
stopNote( 1, 45, 0);
stopNote( 1, 50, 0);
stopNote( 1, 55, 0);
startNote( 1, 45, 107);
startNote( 1, 49, 107);
startNote( 1, 55, 107);
startNote( 1, 57, 107);
if(delayAndSample(5*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(238*DELTA)) return;
stopNote( 1, 67, 0);
stopNote( 1, 69, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 66, 87);
stopNote( 1, 45, 0);
stopNote( 1, 49, 0);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 38, 84);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 113);
startNote( 1, 54, 113);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 75);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 99);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 109);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 108);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 38, 91);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 113);
startNote( 1, 54, 113);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 97);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 99);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 110);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 110);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(214*DELTA)) return;
stopNote( 1, 67, 0);
startNote( 1, 69, 90);
if(delayAndSample(22*DELTA)) return;
startNote( 1, 67, 90);
if(delayAndSample(20*DELTA)) return;
startNote( 1, 66, 90);
stopNote( 1, 69, 0);
stopNote( 1, 67, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 57, 107);
startNote( 1, 62, 107);
if(delayAndSample(247*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(9*DELTA)) return;
startNote( 1, 64, 92);
stopNote( 1, 57, 0);
stopNote( 1, 62, 0);
startNote( 1, 52, 95);
startNote( 1, 61, 95);
if(delayAndSample(243*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 66, 107);
stopNote( 1, 52, 0);
stopNote( 1, 61, 0);
startNote( 1, 38, 84);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 38, 0);
startNote( 1, 45, 108);
startNote( 1, 54, 108);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 62, 94);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 62, 101);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 115);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 62, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 111);
stopNote( 1, 45, 0);
stopNote( 1, 54, 0);
startNote( 1, 47, 93);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 101);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 99);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 107);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 71, 114);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 47, 95);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 47, 0);
startNote( 1, 54, 127);
startNote( 1, 59, 127);
startNote( 1, 62, 127);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 66, 103);
if(delayAndSample(7*DELTA)) return;
stopNote( 1, 71, 0);
if(delayAndSample(114*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 66, 97);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 108);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 66, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 74, 120);
stopNote( 1, 54, 0);
stopNote( 1, 59, 0);
stopNote( 1, 62, 0);
startNote( 1, 57, 99);
if(delayAndSample(128*DELTA)) return;
startNote( 1, 69, 106);
if(delayAndSample(2*DELTA)) return;
stopNote( 1, 74, 0);
if(delayAndSample(119*DELTA)) return;
stopNote( 1, 69, 0);
if(delayAndSample(7*DELTA)) return;
startNote( 1, 67, 97);
stopNote( 1, 57, 0);
startNote( 1, 55, 102);
startNote( 1, 57, 102);
if(delayAndSample(256*DELTA)) return;
startNote( 1, 64, 91);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 49, 106);
startNote( 1, 55, 106);
startNote( 1, 57, 106);
if(delayAndSample(5*DELTA)) return;
stopNote( 1, 67, 0);
if(delayAndSample(238*DELTA)) return;
stopNote( 1, 64, 0);
if(delayAndSample(13*DELTA)) return;
startNote( 1, 62, 47);
stopNote( 1, 49, 0);
stopNote( 1, 55, 0);
stopNote( 1, 57, 0);
startNote( 1, 50, 40);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 50, 0);
startNote( 1, 38, 35);
if(delayAndSample(256*DELTA)) return;
stopNote( 1, 62, 0);
stopNote( 1, 38, 0);}

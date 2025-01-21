/*
 * JAN 2024  
 * Sample routins for Solenoids (and switch led) drive 
 * Dekel Robot (4 in a raw) exhibit, 3D exhebition 2023
 * PBC "Water Drop Screen NOV(OCT) 2022"
 * SEE H:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\Drop_Screen\Drop Screen OCT 2022 Drivers
 * FOR ATmega328P board. Take care if old or new bootloader 
 * ! Exhibit (long move) solenoide sink (at 12V) 0.7A each - 2A power supply can't support more then 4 ! 
 * Prefer to drive solenoides one at a time. NOT useing  on_all_outputs() and on_all_soleloids() routins 
 * IO define (see J:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\Drop Screen OCT 2022 Drivers)
 * 2 - Shift register strobe, 74HC565 pin (12) - active high (low/hig/low pulse shift Sr vector to outputs)
 * 4 - Data, 74HC565 pin (14)  
 * 3 - Shift regiister Clock, 74HC565 pin (11) - active high (low/hig/low pulse shift data in)
 * 5 -  Ouput Enable, 74HC565 pin (13), active low (not need to be used, pull Down resistor on board) 
 */

 /* gpios mapping */
#define SR_St_Pin (2) // 74HC565 shift register strob pin (12) - active high (low/hig/low pulse shift Sr vector to outputs)make sure set low during "push"data in
#define SR_Data_Pin (4)// 74HC565 Data in pin (14)  
#define SR_Clk_Pin (3)// 74HC565 shift clock pin (11) - active high (low/hig/low pulse shift data in) make sure initaly set to low 
//#define En_Pin (5)// 74HC565 out put enable pin (13) - active low active low (not need to be used, pull Down resistor on board) 

const int LED_ON_TIME = 100 ; // ms 
const int LED_OFF_TIME = 100 ; // ms
const int LED_BLINK_NUMBER = 10 ; //
const int SOLENOIDE_ON_TIME = 500 ; // ms 
const int SOLENOIDE_SPACE_TIME = 1000 ; // ms, the time between on each soleoide 

const int NUMBER_OF_SOLENOIDS = 7 ; //  
const byte LED_SWITCH = 0x07; // switch led is the last output (most right on PCB, far from input connector)

byte current_state = 0x00;// used to remember and keep current outputs when change only part of the IOs. 

//-----------------------
//Routins 
//-----------------------
void on_current_state_bit(byte out_number){// this routin only update the state but NOT change ouptut 
  byte digit_position = 0x01;
  digit_position = digit_position << out_number;// rotate left 
  current_state = (current_state | digit_position);//bitwise OR operator
}

//-----------------------
void off_current_state_bit(byte out_number){// this routin only update the state but NOT change ouptut 
  byte digit_position = 0x01;
  digit_position = digit_position << out_number;// rotate left 
  current_state = (current_state &(~digit_position));//bitwise AND and NOT operators
}

//-----------------------
void drive_output(byte output_byte){// ! this is the routine that update and change the shift register outputs
  shiftOut(SR_Data_Pin, SR_Clk_Pin, MSBFIRST, output_byte); // shift the all 8 bits into the shift register, most bit first  
  digitalWrite(SR_St_Pin, HIGH);// pulse latch/strobe pin to update shift register outputs
  digitalWrite(SR_St_Pin, LOW);//
}

//-----------------------
// switch on all outputs,  ! without update current state !
void on_all_outputs(){
  drive_output(0xff);
}
//-------------------------------------------------------------------
// switch off all outputs, ! without update current state !
void off_all_outputs(){
  drive_output(0x00);
}

//--------------------------------
void on_all_soleloids(){// and update current state 
  for (int j = 0; j < NUMBER_OF_SOLENOIDS; j++) {
    on_current_state_bit(j);
  }
  drive_output(current_state);
}
//-------------------------------------------------------------------
void off_all_soleloids(){// and update current state 
  for (int j = 0; j < NUMBER_OF_SOLENOIDS; j++) {
    off_current_state_bit(j);
  }
  drive_output(current_state);
}

//--------------------------------
void on_switch_led(){// and update current state 
  on_current_state_bit(LED_SWITCH);
  drive_output(current_state);
}
//--------------------------------
void off_switch_led(){// and update current state 
  off_current_state_bit(LED_SWITCH);
  drive_output(current_state);
}

//========================================================
void setup() {
  Serial.begin(115200);
  delay (500);// wait to make sure serial begin
  Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("START");
// define IO as outputs and set to low 
//pinMode(En_Pin, OUTPUT);
//digitalWrite(En_Pin, HIGH); // disable outputs 
  pinMode(SR_Data_Pin, OUTPUT);
  digitalWrite(SR_Data_Pin, LOW);   
  pinMode(SR_Clk_Pin, OUTPUT);
  digitalWrite(SR_Clk_Pin, LOW);   
  pinMode(SR_St_Pin, OUTPUT);
  digitalWrite(SR_St_Pin, LOW); 
  off_all_outputs();// make sure initialy all outpurs are low 
  current_state = 0x00;// update outputs state
 }

void loop() {
// check/operate all outputs 
/*
  on_all_outputs(); //nead 12V 5A power supply for drive the solenoide 
  current_state = 0xff;
  delay (SOLENOIDE_ON_TIME);
  off_all_outputs();
  current_state = 0x00;
  delay (SOLENOIDE_SPACE_TIME);  
*/  
// check/operate switch led output   
  for (int i = 0; i < LED_BLINK_NUMBER ; i++) {
    on_switch_led();
    delay (LED_ON_TIME);
    off_switch_led();
    delay (LED_OFF_TIME);  
    }  
    
// check/operate solenoids outputs (all and each) !!nead 12V 5A power supply for drive the solenoide 
// all
/*  on_all_soleloids();
  delay (SOLENOIDE_ON_TIME);
  off_all_soleloids();
  delay (SOLENOIDE_SPACE_TIME);
 */
// each 
  for (int i = 0; i < NUMBER_OF_SOLENOIDS ; i++) {
    on_current_state_bit(i);// only update the state but NOT change ouptut 
    drive_output(current_state);// now change outputs 
    delay (SOLENOIDE_ON_TIME);
    off_current_state_bit(i);// only update the state but NOT change ouptut 
    drive_output(current_state);  // now change outputs 
    delay (SOLENOIDE_SPACE_TIME);
  }
}

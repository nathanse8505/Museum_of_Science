/*
 * 

#define ENCODER_A_BIT_1 (2) // input IO for gray code bit 0 
#define ENCODER_A_BIT_0 (3) // input IO for gray code bit 1
//!!! Nano board - only D2 and D3 can be interupt !!
#define ENCODER_B_BIT_1 (4) // input IO for gray code bit 0
#define ENCODER_B_BIT_0 (5) // input IO for gray code bit 1
// cross bits(0-1) for change counting direction (CW<>CCW)
 
byte Old_Encoder_A_Read = 0; // old 2 bits read from first encoder 
byte New_Encoder_A_Read = 0; // new 2 bits read from first encoder 
int16_t Current_Encoder_A_Count = 0;// serial_printed and/or display 
int16_t Old_Encoder_A_Count = 0;// for later use

byte Old_Encoder_B_Read = 0; // old 2 bits read from second encoder 
byte New_Encoder_B_Read = 0; // new 2 bits read from second encoder 
int16_t Current_Encoder_B_Count = 0;// serial_printed and/or display 
int16_t Old_Encoder_B_Count = 0;// for later use

 
 //=======
 
//-------------------
void Display_Encoders_Count(){
  display.clearDisplay();
//  display.display();
  display.setCursor(0, 0);
  display.print("A = ");
  display.println(Current_Encoder_A_Count);
  display.print("B = ");
  display.println(Current_Encoder_B_Count);
  display.display();
}
 //========================
 //-------------------
void Print_Encoders_Count(){
  Serial.print("Encoder A Count: ");
  Serial.print(Current_Encoder_A_Count);
  Serial.print(", Encoder B Count: ");
  Serial.println(Current_Encoder_B_Count);
}
 //======
 int Read_Encoder_A(){
     Old_Encoder_A_Read = New_Encoder_A_Read ;
     New_Encoder_A_Read = (((digitalRead(ENCODER_A_BIT_1)) << 1) + (digitalRead(ENCODER_A_BIT_0))) ;
     byte Check_Direction  = (Old_Encoder_A_Read << 2) + New_Encoder_A_Read  ; // x4 = 2 rotate left 
     switch (Check_Direction)
     {
      case 1: case 7: case 8: case 14:
      return 1 ;
      case 2: case 4: case 11: case 13:
      return -1 ;
      case 0: case 5: case 10: case 15:
      return 0 ;
      default: // need to avoide delay in return 
      return 0 ; // 
    }
}
//=================================



int Read_Encoder_B(){
     Old_Encoder_B_Read = New_Encoder_B_Read ;
     New_Encoder_B_Read = (((digitalRead(ENCODER_B_BIT_1)) << 1) + (digitalRead(ENCODER_B_BIT_0))) ;
     byte Check_Direction  = (Old_Encoder_B_Read << 2) + New_Encoder_B_Read  ; // x4 = 2 rotate left 
     switch (Check_Direction)
     {
      case 1: case 7: case 8: case 14:
      return 1 ;
      case 2: case 4: case 11: case 13:
      return -1 ;
      case 0: case 5: case 10: case 15:
      return 0 ;
      default: // need to avoide delay in return 
      return 0 ; // 
    }
}

void test_Coils(){
  digitalWrite(INTERNAL_LED, HIGH);// 
//  digitalWrite(OUT_COIL_1, HIGH);// 
//  digitalWrite(OUT_COIL_2, HIGH);// 
//  digitalWrite(OUT_COIL_3, HIGH);// 
  delay(1000);
//  digitalWrite(OUT_COIL_1, LOW);// make sure no output 
//  digitalWrite(OUT_COIL_2, LOW);// make sure no output 
//  digitalWrite(OUT_COIL_3, LOW);// make sure no output 
  digitalWrite(INTERNAL_LED, LOW);// 
  }
//---------------------------------
void Update_Encoder_A_Count(){
  int temp_move = Read_Encoder_A();
  Current_Encoder_A_Count += temp_move;
}
//---------------------------------
void Update_Encoder_B_Count(){
  int temp_move = Read_Encoder_B();
  Current_Encoder_B_Count += temp_move;
}
//==============
void setup() {
  // make encoder IO pulsedup inputs:
  pinMode(ENCODER_A_BIT_0, INPUT_PULLUP);  
  pinMode(ENCODER_A_BIT_1, INPUT_PULLUP);  
  pinMode(ENCODER_B_BIT_0, INPUT_PULLUP);  
  pinMode(ENCODER_B_BIT_1, INPUT_PULLUP);  

  attachInterrupt(digitalPinToInterrupt(ENCODER_A_BIT_0), Update_Encoder_A_Count, CHANGE);// arduino nano can only D2 and D3 interupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_BIT_1), Update_Encoder_A_Count, CHANGE);// arduino nano can only D2 and D3 interupt

  Current_Time = millis();

  if(Use_Serial) {Start_Serial();}
  if(Use_Display) {Start_Display();}
}


//=================
void loop() {
  Update_Encoder_B_Count();// encoder A is updated by interupt 
  if (millis() >= Current_Time+Update_Display_Time) {
  if(Use_Serial) {
    Print_Encoders_Count();
    }
  if(Use_Display) {
    Display_Encoders_Count();
    }  
  Current_Time = millis();     
  }

  delay(Main_Loop_Delay_Time);
}



 */

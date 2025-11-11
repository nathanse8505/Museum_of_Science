/*
Ruud Boer - January 2016
2x RGB LED Fade with manual-, kayeboard- and DCC control

Set Serial Interface Baudrate to 115200

pin3=B1 (take care, it flickers on my Arduino's), pin5=G1 pin6=R1
pin9=R2 pin10=G2 pin11=B2
A0 = pushbutton R adjust
A1 = pushbutton G adjust
A2 = pushbutton B adjust
A3 = pushbutton adjust RGB1 or RGB2 
A4 = pushbutton adjust increment +1 or -1
*/
#include <DCC_Decoder.h>
#define kDCC_INTERRUPT 0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parameters to define by user
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte maxaccessories = 16; //The number of accessories you want to define at line 104
byte colors[9][6] = { //color presets, each with 6 output values in the range of 0 - 255
  {40,40,16,40,40,40}, //preset 0
  {80,80,16,80,80,80}, //preset 1 white dim
  {140,140,16,140,140,140}, //preset 2 white dim
  {255,255,16,255,255,255}, //preset 3 white
  {0,0,16,0,0,255},   //preset 4 B
  {255,0,0,255,0,0},   //preset 5 R
  {255,60,16,255,60,16}, //preset 6 orange 1
  {255,140,16,255,140,40}, //preset 7 orange 2
  {255,255,16,255,255,100} //preset 8 yellow
};
unsigned int fade_time = 5000; //Initial time in ms for a fade transition (max = 64000)
unsigned long sequencetime = 20000; //time in ms between preset changes in sequencer mode
byte RGBupdms = 100; //update interval for A0-A2 = LOW manual RGB control
byte print_flag = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations of variables
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  int   address;   // User Configurable. DCC address to respond to
  byte  preset;    // User Configurable. Defines the LED color preset to use with this accessory address 
  byte  fadetime;  // User Configurable. Defines the fade_time in seconds
  byte  on_off;    // Internal use. Stores if the DCC accessory is switched on or off
  byte  pulse;     // Internal use. Captures the pulse that is sent by the DCC accessory switch command
}
DCCAccessoryAddress;
DCCAccessoryAddress accessory[maxaccessories];

byte sequencer = 0;
byte pold = 99; //current color preset
byte pnew = 0; //new color preset number to fade to

byte r1=0; //pin 3 current rgb values for LED strip 1, off at startup.
byte g1=0; //pin 5
byte b1=0; //pin 6
byte r2=0; //pin 9 current rgb values for LED strip 12, off at startup.
byte g2=0; //pin 10
byte b2=0; //pin 11
byte r1sp, g1sp, b1sp, r2sp, g2sp, b2sp; //setpoints for rgb values
byte r1inc, g1inc, b1inc, r2inc, g2inc, b2inc; //increment or decrement for rgb value updates
byte serialread; //serial read input
byte input_mode=0; //change preset (0) or change fade_time (1) via serial input
byte increment; //increment value for update of RGB via A0-A2
unsigned int r1updms; //ms interval to update rgb value
unsigned int g1updms;
unsigned int b1updms;
unsigned int r2updms;
unsigned int g2updms;
unsigned int b2updms;
unsigned long r1updatetime; //time to update r1
unsigned long g1updatetime;
unsigned long b1updatetime;
unsigned long r2updatetime;
unsigned long g2updatetime;
unsigned long b2updatetime;
unsigned long sequpdatetime; //time to update the sequencer
unsigned long RGBupdatetime; //time to test digital inputs A0-A5 for button press and update RGB


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Table to translate from linear to exponential RGB updates, to correct brightness for human eye.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte rgbexp[]= {
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
  2,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,
  7,7,7,8,8,8,9,9,9,10,10,10,11,11,12,12,13,13,14,14,
  15,15,16,16,17,17,18,18,19,19,20,20,21,21,22,23,23,24,24,25,
  26,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,38,38,39,
  40,41,42,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,56,
  57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,73,74,75,76,77,
  78,79,80,81,82,84,85,86,87,88,89,91,92,93,94,95,97,98,99,100,
  102,103,104,105,107,108,109,111,112,113,115,116,117,119,120,121,123,124,126,127,
  128,130,131,133,134,136,137,139,140,142,143,145,146,148,149,151,152,154,155,157,
  158,160,162,163,165,166,168,170,171,173,175,176,178,180,181,183,185,186,188,190,
  192,193,195,197,199,200,202,204,206,207,209,211,213,215,217,218,220,222,224,226,
  228,230,232,233,235,237,239,241,243,245,247,249,251,253,255
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fill in the attributes for every accessory address
// COPY - PASTE as many times as you have functions. The amount must be same as in line 18 above!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigureDecoderFunctions() { // The amount of accessories must be same as in line 26 above!
  accessory[0].address = 100; // RGB 1 or 2
  accessory[0].preset = 0;
  accessory[0].fadetime = 0;

  accessory[1].address = 101; // brightness up or down
  accessory[1].preset = 0;
  accessory[1].fadetime = 0;

  accessory[2].address = 102; // R pulse
  accessory[2].preset = 0;
  accessory[2].fadetime = 0;
  
  accessory[3].address = 103; // G pulse
  accessory[3].preset = 0;
  accessory[3].fadetime = 0;

  accessory[4].address = 104; // B pulse
  accessory[4].preset = 0;
  accessory[4].fadetime = 0;

  accessory[5].address = 105; // fade time 10s pulse
  accessory[5].preset = 0;
  accessory[5].fadetime = 10;

  accessory[6].address = 106; // fade time 30s pulse
  accessory[6].preset = 0;
  accessory[6].fadetime = 30;

  accessory[7].address = 107; // Sequencer 
  accessory[7].preset = 0;
  accessory[7].fadetime = 0;
  
  accessory[8].address = 108; // Preset 1 pulse
  accessory[8].preset = 1;
  accessory[8].fadetime = 0;
  
  accessory[9].address = 109; // Preset 2 pulse
  accessory[9].preset = 2;
  accessory[9].fadetime = 0;
  
  accessory[10].address = 110; // Preset 3 pulse
  accessory[10].preset = 3;
  accessory[10].fadetime = 0;

  accessory[11].address = 111; // Preset 4 pulse
  accessory[11].preset = 4;
  accessory[11].fadetime = 0;

  accessory[12].address = 112; // Preset 5 pulse
  accessory[12].preset = 5;
  accessory[12].fadetime = 0; 
  
  accessory[13].address = 113; // Preset 6 pulse
  accessory[13].preset = 6;
  accessory[13].fadetime = 0;  

  accessory[14].address = 114; // Preset 7 pulse
  accessory[14].preset = 7;
  accessory[14].fadetime = 0;  
  
  accessory[15].address = 115; // Preset 8 pulse
  accessory[15].preset = 8;
  accessory[15].fadetime = 0;  

  }  // END ConfigureDecoderFunctions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DCC accessory packet handler 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data) {
  // Convert NMRA packet address format to human address
  address -= 1;
  address *= 4;
  address += 1;
  address += (data & 0x06) >> 1;

  boolean enable = (data & 0x01) ? 1 : 0;

  for (int i=0; i<maxaccessories; i++) {
    if (address == accessory[i].address) {
      accessory[i].pulse = activate;
      accessory[i].on_off = enable;
    }
  }
} //END BasicAccDecoderPacket_Handler  

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Get new setpoints and calculatetime between RGB updates. Executed once after a change of preset.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calculate_update_parameters () {

  r1sp = colors[pnew][0];
  g1sp = colors[pnew][1];
  b1sp = colors[pnew][2];
  r2sp = colors[pnew][3];
  g2sp = colors[pnew][4];
  b2sp = colors[pnew][5];
  
  if (r1sp > r1) {
    r1updms = fade_time / (r1sp - r1);
    r1inc = 1;
  }  
  else {
    r1updms = fade_time / (r1 - r1sp);
    r1inc = 255;
  }
  r1updatetime = millis() + r1updms;
  
  if (g1sp > g1) {
    g1updms = fade_time / (g1sp - g1);
    g1inc = 1;
  }  
  else {
    g1updms = fade_time / (g1 - g1sp);
    g1inc = 255;
  }
  g1updatetime = millis() + g1updms;
  
  if (b1sp > b1) {
    b1updms = fade_time / (b1sp - b1);
    b1inc = 1;
  }  
  else {
    b1updms = fade_time / (b1 - b1sp);
    b1inc = 255;
  }
  b1updatetime = millis() + b1updms;

  if (r2sp > r2) {
    r2updms = fade_time / (r2sp - r2);
    r2inc = 1;
  }  
  else {
    r2updms = fade_time / (r2 - r2sp);
    r2inc = 255;
  }
  r2updatetime = millis() + r2updms;
  
  if (g2sp > g2) {
    g2updms = fade_time / (g2sp - g2);
    g2inc = 1;
  }  
  else {
    g2updms = fade_time / (g2 - g2sp);
    g2inc = 255;
  }
  g2updatetime = millis() + g2updms;
  
  if (b2sp > b2) {
    b2updms = fade_time / (b2sp - b2);
    b2inc = 1;
  }  
  else {
    b2updms = fade_time / (b2 - b2sp);
    b2inc = 255;
  }
  b2updatetime = millis() + b2updms;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serial Print RGB values
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void print_rgb() {
  Serial.print(pnew); Serial.print(" / ");
  Serial.print(r1); Serial.print(" ");
  Serial.print(g1); Serial.print(" ");
  Serial.print(b1); Serial.print(" ");
  Serial.print(r2); Serial.print(" ");
  Serial.print(g2); Serial.print(" ");
  Serial.println(b2);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup is run once
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  ConfigureDecoderFunctions();
  DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
  DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
  pinMode(2,INPUT); //Interrupt 0, DCC input via opto-coupler
  pinMode(12,INPUT_PULLUP); //Used to switch preset sequencer on (LOW) or off (HIGH)
  pinMode(A0,INPUT_PULLUP); //change R
  pinMode(A1,INPUT_PULLUP); //change G
  pinMode(A2,INPUT_PULLUP); //change B
  pinMode(A3,INPUT_PULLUP); //Used to switch update of RGB1 ot RGB2
  pinMode(A4,INPUT_PULLUP); //Used to switch A0-A2 between increment or decrement
  pinMode(13,OUTPUT); //On board LED
  digitalWrite(13,LOW);
  RGBupdatetime = millis() + RGBupdms;
  sequpdatetime = millis() + sequencetime; //time interval for sequencer to go to next preset
  
  Serial.begin(115200);  //9600 on 123dcircuits, 115200 when live
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main loop (run continuous)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DCC input commands
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  static int addr = 0;
  DCC.loop(); // Loop DCC library
  if( ++addr >= maxaccessories ) addr = 0; // Next address to test
  
  if (accessory[addr].pulse) {
    if (accessory[addr].fadetime) {
      fade_time = accessory[addr].fadetime * 1000;
      Serial.print("Fadetime ");
      Serial.println(fade_time);
      accessory[addr].pulse = 0;
    }
    if (accessory[addr].preset) {
      pnew = accessory[addr].preset;
      Serial.print("Preset ");
      Serial.println(pnew);
      accessory[addr].pulse = 0;
    }
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sequencer ... steps through presets with a given time interval
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (accessory[7].on_off/*|| !digitalRead(12)*/) sequencer = 1; else sequencer = 0;
  if (sequencer){
    if (millis() > sequpdatetime) {
      sequpdatetime = millis() + sequencetime;
      pnew = (pold + 1);
      if (pnew==10) pnew = 2;
      Serial.print("autosequencer preset = "); Serial.println(pnew);
    }
  }

  if (pnew != pold) {
    calculate_update_parameters ();
    pold = pnew;
  }

  if (r1 != r1sp) {
    if (millis() > r1updatetime) {
      r1updatetime = millis() + r1updms;
      r1 = r1 + r1inc;
      analogWrite(6, rgbexp[r1]);
    }
  }
  if (g1 != g1sp) {
    if (millis() > g1updatetime) {
      g1updatetime = millis() + g1updms;
      g1 = g1 + g1inc;
      analogWrite(5, rgbexp[g1]);
    }
  }
  if (b1 != b1sp) {
    if (millis() > b1updatetime) {
      b1updatetime = millis() + b1updms;
      b1 = b1 + b1inc;
      analogWrite(3, rgbexp[b1]);
    }
  }
  if (r2 != r2sp) {
    if (millis() > r2updatetime) {
      r2updatetime = millis() + r2updms;
      r2 = r2 + r2inc;
      analogWrite(9, rgbexp[r2]);
    }
  }
  if (g2 != g2sp) {
    if (millis() > g2updatetime) {
      g2updatetime = millis() + g2updms;
      g2 = g2 + g2inc;
      analogWrite(10, rgbexp[g2]);
    }
  }
  if (b2 != b2sp) {
    if (millis() > b2updatetime) {
      b2updatetime = millis() + b2updms;
      b2 = b2 + b2inc;
      analogWrite(11, rgbexp[b2]);
    }
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serial input commands
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (Serial.available()) {
  	serialread = Serial.read();
    if (serialread==112) {input_mode = 0; Serial.println("Preset Mode");} // P = preset mode selected
    if (serialread==116) {input_mode = 1; Serial.println("Fadetime Mode");} // T = fadetime mode selected
    //more modes can be added here later, like e.g. change RGB values individually in steps of 1,5,10
    
    switch (input_mode) { //input mode is preset
      case 0: 
        switch (serialread) {
          case 48: sequencer = 0; break; // 0 = sequencer off
          case 49: pnew = 1; break; // 1 = preset 1
          case 50: pnew = 2; break;
          case 51: pnew = 3; break;
          case 52: pnew = 4; break;
          case 53: pnew = 5; break;
          case 54: pnew = 6; break;
          case 55: pnew = 7; break;
          case 56: pnew = 8; break; // 8 = preset 8
          case 57: sequencer = 1; break; // 9 = sequencer on
        }
        Serial.print("Preset "); Serial.println(pnew);
      break;
 
      case 1: //input mode is fadetime
        switch (serialread) {
          case 49: fade_time = 2000; break; // 1 = fadetime 2s
          case 50: fade_time = 5000; break;
          case 51: fade_time = 10000; break;
          case 52: fade_time = 15000; break;
          case 53: fade_time = 20000; break;
          case 54: fade_time = 30000; break;
          case 55: fade_time = 40000; break;
          case 56: fade_time = 50000; break;
          case 57: fade_time = 60000; break; // 9 = fadetime 60s
        }
        Serial.print("Fade_time "); Serial.println(fade_time);
      break;
    }
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Digital in commands
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
// Pull A0-A2 to LOW to increase / decrease RGB
// pin A3 HIGH changes RGB1, pin A3 LOW changes RGB2
// pin A4 HIGH step size = 1, pin 4 LOW step size = -1
// if you like to change presets via A0-A2 then change the code into:
// if (!digitalRead(A0)) {pnew = 3; Serial.println("preset = 3 ");}
// if (!digitalRead(A1)) {pnew = 4; Serial.println("preset = 4 ");}
  if (millis() > RGBupdatetime) {
    RGBupdatetime = millis() + RGBupdms;
    if (/*!digitalRead(A4) || */accessory[1].on_off) {
      increment = 1;
      }
    else {
      increment = -1;
    }
    if (/*!digitalRead(A3) || */accessory[0].on_off) { //update RGB1
      if (/*!digitalRead(A2) || */accessory[2].pulse) {r1 = r1+increment; r1sp = r1; analogWrite(6, rgbexp[r1]); print_rgb();}
      if (/*!digitalRead(A1) || */accessory[3].pulse) {g1 = g1+increment; g1sp = g1; analogWrite(5, rgbexp[g1]); print_rgb();}
      if (/*!digitalRead(A0) || */accessory[4].pulse) {b1 = b1+increment; b1sp = b1; analogWrite(3, rgbexp[b1]); print_rgb();}
    }
    else { //update RGB2
      if (/*!digitalRead(A2) || */accessory[2].pulse) {r2 = r2+increment; r2sp = r2; analogWrite(9, rgbexp[r2]); print_rgb();}
      if (/*!digitalRead(A1) || */accessory[3].pulse) {g2 = g2+increment; g2sp = g2; analogWrite(10, rgbexp[g2]); print_rgb();}
      if (/*!digitalRead(A0) || */accessory[4].pulse) {b2 = b2+increment; b2sp = b2; analogWrite(11, rgbexp[b2]); print_rgb();}    
    }
  }  
} // END of LOOP


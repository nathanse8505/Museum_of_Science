//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DCC Function Decoder
// Author: Ruud Boer - October 2015
// This sketch turns an Arduino into a DCC function decoder for F0 - F12
// Output pins used: 3-19 (14-19 = A0-A5). Pin is LOW when Function is ON.
// The DCC signal is fed to pin 2 (=Interrupt 0).
// Optocoupler schematics for conversion of DCC - 5V: www.rudysmodelrailway.wordpress.com/software
// Many thanks to www.mynabay.com for publishing their DCC monitor and -decoder code.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPORTANT: GOTO lines 15 - 28 to configure some data!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int decoderAddress = 1830; // This is the decoder address, change into the number you want.
#define F0_pin 14 // Define the output pin for every Function number in use
#define F0_pin2 15 // 2nd pin for same function is possible. Can use forward / reverse direction ... see line 97.
#define F1_pin 13 // Available pin numbers: 3 - 19 (14-19 = A0-A5)
#define F2_pin 0
#define F3_pin 0
#define F4_pin 0
#define F5_pin 0
#define F6_pin 0
#define F7_pin 0
#define F8_pin 0
#define F9_pin 0
#define F10_pin 0
#define F11_pin 0
#define F12_pin 0
/* F13-F28 commented out 
#define F13_pin 0
#define F14_pin 0
#define F15_pin 0
#define F16_pin 0
#define F17_pin 0
#define F18_pin 0
#define F19_pin 0
#define F20_pin 0
#define F21_pin 0
#define F22_pin 0
#define F23_pin 0
#define F24_pin 0
*/

#include <DCC_Decoder.h>
#define kDCC_INTERRUPT 0

byte Func[4]; // 0=L F4-F1, 1=F9-F5, 2=F12-F9, 3=F20-F13, 4=F28-F21
byte instrByte1;
int Address;
byte forw_rev=1; //0=reverse, 1=forward

boolean RawPacket_Handler(byte pktByteCount, byte* dccPacket) {
  Address=0;
  if (!bitRead(dccPacket[0],7)) { //bit7=0 -> Loc Decoder Short Address
    Address = dccPacket[0];
    instrByte1 = dccPacket[1];
  }
  else if (bitRead(dccPacket[0],6)) { //bit7=1 AND bit6=1 -> Loc Decoder Long Address
    Address = 256 * (dccPacket[0] & B00000111) + dccPacket[1];
    instrByte1 = dccPacket[2];
  }

  if (Address==decoderAddress) {
    byte instructionType = instrByte1>>5;
    switch (instructionType) {
      case 2: // Reverse speed
        forw_rev=0;
        break;
      case 3: // Forward speed
        forw_rev=1;
        break;
      case 4: // Loc Function L-4-3-2-1
        Func[0]=instrByte1&B00011111;
        break;
      case 5: // Loc Function 8-7-6-5
        if (bitRead(instrByte1,4)) {
          Func[1]=instrByte1&B00001111;
        }
        else { // Loc Function 12-11-10-9
          Func[2]=instrByte1&B00001111;
        }
        break;
      /* F13-F28 commented out 
      case 6: // Future Expansions
        switch (instrByte1&B00011111) {
          case B00011110: // Loc Function F13-F20
            Func[3]=dccPacket[pktByteCount-1];
          break;
          case B00011111: // Loc Function F21-F28
            Func[4]=dccPacket[pktByteCount-1];
          break;
        }
      break;
      */
    }
    // F0 is an example of two output pins that alternate based on loc forw_rev driving direction.
    if (Func[0]&B00010000) {digitalWrite(F0_pin,forw_rev); digitalWrite(F0_pin2,!forw_rev);} else digitalWrite(F0_pin,HIGH);
    if (Func[0]&B00000001) digitalWrite(F1_pin,LOW); else digitalWrite(F1_pin,HIGH);
    if (Func[0]&B00000010) digitalWrite(F2_pin,LOW); else digitalWrite(F2_pin,HIGH);
    if (Func[0]&B00000100) digitalWrite(F3_pin,LOW); else digitalWrite(F3_pin,HIGH);
    if (Func[0]&B00001000) digitalWrite(F4_pin,LOW); else digitalWrite(F4_pin,HIGH);
    if (Func[1]&B00000001) digitalWrite(F5_pin,LOW); else digitalWrite(F5_pin,HIGH);
    if (Func[1]&B00000010) digitalWrite(F6_pin,LOW); else digitalWrite(F6_pin,HIGH);
    if (Func[1]&B00000100) digitalWrite(F7_pin,LOW); else digitalWrite(F7_pin,HIGH);
    if (Func[1]&B00001000) digitalWrite(F8_pin,LOW); else digitalWrite(F8_pin,HIGH);
    if (Func[2]&B00000001) digitalWrite(F9_pin,LOW); else digitalWrite(F9_pin,HIGH);
    if (Func[2]&B00000010) digitalWrite(F10_pin,LOW); else digitalWrite(F10_pin,HIGH);
    if (Func[2]&B00000100) digitalWrite(F11_pin,LOW); else digitalWrite(F11_pin,HIGH);
    if (Func[2]&B00001000) digitalWrite(F12_pin,LOW); else digitalWrite(F12_pin,HIGH);

    /*//Print DCC packet bytes for testing purposes
    for (byte n=0; n<5; n++) {
      Serial.print(Func[n],BIN);
      Serial.print(" ");
    }
    Serial.println(" ");
    */
  }
}

void setup() {
  DCC.SetRawPacketHandler(RawPacket_Handler);
  DCC.SetupMonitor( kDCC_INTERRUPT );
  for (byte n=3; n<20; n++) pinMode(n,OUTPUT);
}

void loop() {
  DCC.loop();
}

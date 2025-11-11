//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DCC Sound decoder
// Version: 1.0
// Authors: Ruud Boer and Erik Holewijn
// Purpose: Monitor DCC signals and sends accessory addresses over serial port to the PC
// Hardware required to connects Arduino to DCC: https://rudysmodelrailway.wordpress.com/software/
// The PC runs the DCCSound program, it listens receives the DCC addresses and plays the corresponding sounds
// Thanks: to mynabay .com for creating their DCC monitor software which is used in this sketch
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <DCC_Decoder.h>
#define kDCC_INTERRUPT 0
int oldaddress=0;
long timetoreset;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DCC accessory packet handler 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data)
{
	// Convert NMRA packet address format to human address
	address -= 1;
	address *= 4;
	address += 1;
	address += (data & 0x06) >> 1;

	boolean enable = (data & 0x01) ? 1 : 0;

	if (enable)
    if (address!=oldaddress)
    {
    Serial.print ("on: ");
    Serial.print (address);
    Serial.print (char(10));
    oldaddress = address;
    }
} //END BasicAccDecoderPacket_Handler

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup (run once)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{ 
  DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
  DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );
  pinMode(2,INPUT_PULLUP); //Interrupt 0 with internal pull up resistor (can get rid of external 10k)
  timetoreset = millis()+5000;
  Serial.begin(9600);
} // END setup

void loop()
{   
	DCC.loop();
  
  if (millis() > timetoreset)
  {
    oldaddress=0;
    timetoreset = millis()+5000;
  }
}

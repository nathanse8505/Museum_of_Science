//
// Robert's Smorgasbord 2021
// http://www.robertssmorgasbord.net/
// https://www.youtube.com/channel/UCGtReyiNPrY4RhyjClLifBA
//

#include <SPI.h>
 
const byte max31855_num                   = 2;         // number of modules max31855
const int  max31855_cs_pins[max31855_num] = { 10, 9 }; // max31855 chip select pin
const float TEMP_PER_BITE =  0.25;
const float TEMP_PER_BITE_I =  0.0625;

/*const byte max31855_fault_bit_oc  = 0B00000001; // open connection
const byte max31855_fault_bit_scg = 0B00000010; // short circuit to GND
const byte max31855_fault_bit_scv = 0B00000100; // short circuit to VCC
const byte max31855_fault_bit_any = 0B00001000; // any of the above 
 */
SPISettings max31855_spi(16000000,   // 16Mhz SPI clock
                         MSBFIRST,   
                         SPI_MODE0);
  
void setup() {
   for (int i = 0; i < max31855_num; i++){
      pinMode(max31855_cs_pins[i], OUTPUT);
      digitalWrite(max31855_cs_pins[i], HIGH);
   }
   
   SPI.begin();
   Serial.begin(1000000);
}
 
void loop(){
   float thermocouple_temperature;
   float internal_temperature;
   byte  fault_bits;
   int   i;
   delay(100);

   for (int i = 0; i < max31855_num; i++){ 
      max31855Read(max31855_cs_pins[i],
                   &thermocouple_temperature,
                   &internal_temperature,
                   &fault_bits               );
  
      Serial.print(thermocouple_temperature);
      Serial.print("\t");
      Serial.print(internal_temperature);
      Serial.print("\t");
   }
   
   Serial.println();
}

void max31855Read(int    max31855_cs_pin,
                  float* thermocouple_temperature,
                  float* internal_temperature,
                  byte*  fault_bits) {
   unsigned int data_0;
   unsigned int data_1;

   digitalWrite(max31855_cs_pin, LOW);
   SPI.beginTransaction(max31855_spi);

   data_0 = SPI.transfer16(0);
   data_1 = SPI.transfer16(0);

   SPI.endTransaction();
   digitalWrite(max31855_cs_pin, HIGH);
   
   *fault_bits               =   data_1 & 0B0000000000000111
                             | ((data_0 & 0B0000000000000001) << 3);
 
   *internal_temperature     = ((data_1 & 0B0111111111110000) >> 4) * TEMP_PER_BITE_I;
   if (                          data_1 & 0B1000000000000000)
   *internal_temperature     += -128; 
   
   *thermocouple_temperature = ((data_0 & 0B0111111111111100) >> 2) * TEMP_PER_BITE;
   if (                          data_0 & 0B1000000000000000)
   *thermocouple_temperature += -2048;

   if (*fault_bits) *thermocouple_temperature = NAN;
}

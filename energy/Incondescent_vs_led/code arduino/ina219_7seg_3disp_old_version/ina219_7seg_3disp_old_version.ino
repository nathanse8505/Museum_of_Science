#include "CONSTANTS.h"
#include <Wire.h>
#include <Adafruit_INA219.h>


Adafruit_INA219 ina219;


int Digit_3_To_Display = 0;// Left (100th) digit to disply used as 100th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply - used as 10th
int Digit_1_To_Display = 0;// rigth digit to disply - used as units 



void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}
void Digits_from_Number(int in_number){
    Digit_3_To_Display =  in_number / 100;//left digit to display 
    Digit_2_To_Display = (in_number - 100 * Digit_3_To_Display)/10;// mid digit to display 
    Digit_1_To_Display = in_number % 10 ;// right digit to display 

}
void Display_Digit(int digit_to_show) { 
    digitalWrite(BCD_A, LOW);// asume bit zero 
    if ((digit_to_show & 1) == 1) {digitalWrite(BCD_A, HIGH);}
    digitalWrite(BCD_B, LOW);// asume bit zero 
    if ((digit_to_show & 2) == 2) {digitalWrite(BCD_B, HIGH);}
    digitalWrite(BCD_C, LOW);// asume bit zero 
    if ((digit_to_show & 4) == 4) {digitalWrite(BCD_C, HIGH);}
    digitalWrite(BCD_D, LOW);// asume bit zero 
    if ((digit_to_show & 8) == 8) {digitalWrite(BCD_D, HIGH);}  
}


void enable(int digit_enable){
 switch(digit_enable) {
      case 1: //range 0-9  '00000110'
       digitalWrite(LE_UNITS, LOW);
       digitalWrite(LE_UNITS, HIGH); 
       break;
      case 2: //range 9-99 '000000101'
       digitalWrite(LE_DOZENS, LOW);
       digitalWrite(LE_DOZENS, HIGH); 
       break;
      case 3: //range 100-999 '00000011'
       digitalWrite(LE_CENTURIES, LOW);
       digitalWrite(LE_CENTURIES, HIGH); 
      

       break;
    }
}

void all_zero_digit(){
  digitalWrite(BCD_A, LOW);
  digitalWrite(BCD_B, LOW);
  digitalWrite(BCD_C, LOW);
  digitalWrite(BCD_D, LOW);
  for(int j=1; j <= 4; j++){
         enable(j);
    }
}

void blank_Digit(int digit_enable){ //blank digit right to the left
  digitalWrite(BCD_A, HIGH);
  digitalWrite(BCD_B, HIGH);
  digitalWrite(BCD_C, HIGH);
  digitalWrite(BCD_D, HIGH);

  enable(digit_enable);

}

void blank_All_Digit(){
  for(int j=1 ;j<=4;j++){
         blank_Digit(j);
    }
}

float avg_power(){
   float avg = 0;
   for(int i = 0;i < ITERATION;i++){

    float power_W = ina219.getPower_mW()/1000;
    avg = power_W + avg;
    delay(DELAY_AVG);
  }

  avg = avg / ITERATION;
  Serial.print("Power:         "); Serial.print(avg); Serial.println(" W");
  return avg;


}



void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  while (!Serial) {
    // will pause Zero, Leonardo, etc until serial console opens
    delay(1);
  }
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);

  Init_Output(LE_UNITS);
  digitalWrite(LE_UNITS, HIGH);// lock digit

  Init_Output(LE_DOZENS);
  digitalWrite(LE_DOZENS, HIGH);// lock digit

  Init_Output(LE_CENTURIES);
  digitalWrite(LE_CENTURIES, HIGH);// lock digit

  Serial.println("init");
  Serial.println("Measuring voltage and current with INA219 ...");

}


void loop() {
  /*float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  power_mW = ina219.getPower_mW();
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");*/

 
  float average_power = avg_power();
  average_power = average_power * UNIT_CALIBRATION;//for unit will be to the middle

  Digits_from_Number(average_power);
  Display_Digit(Digit_1_To_Display);
  enable(1);
  Display_Digit(Digit_2_To_Display);
  enable(2);

  if(average_power < 10 * UNIT_CALIBRATION){
    blank_Digit(3);
  }
  else{
    Display_Digit(Digit_3_To_Display);
    enable(3);
  }
    
  //delay(1000);
  //Delay to slow down the serial output for better readability
  //delay(1000); // Wait for 1 second before the next reading
}


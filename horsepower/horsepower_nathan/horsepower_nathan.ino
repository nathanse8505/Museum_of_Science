#include "constants.h"
#include "Fonction.h"

void setup()
{
  Serial.begin(BAUDERATE);
  Wire.begin();
  pinMode(SPACE_BUTTON_IO,INPUT_PULLUP);
  Wire.setClock(400000); // Use 400 kHz I2C
  sensor.setTimeout(500);
 

  if (!sensor.init())
  {
    Serial.println("ERROR Sensor initialization failed!");
    while (1);
  }

  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(30000);//30ms
  sensor.startContinuous(30); // Continuous readings every 30 ms

  // Read initial distance
  
  initialDistance = read_average_distance();; // Initialize smoothed distance
  Serial.println("LOG Initial Distance Set: " + String(initialDistance, 4));
  minDistance = initialDistance -  THRESHOLD;
   
}


void loop()
{

  if (PRESS_BUTTON_SPACE()){
    Serial.write(32); // Send ASCII code for space key
    Serial.println("LOG Button pressed, simulating 'space' key.");
  }

  smoothedDistance = sensor.read() / 1000.0; // Convert mm to meters
  //smoothedDistance = read_average_distance()
  //Serial.println(smoothedDistance);

  if (smoothedDistance < minDistance && Lift_in_motion == false && first_try == false){
      Lift_in_motion = true;
      startTime = millis();
      Serial.println("LOG Lift started moving.");
      Serial.println("Distance: " + String(smoothedDistance));
  }

  while(Lift_in_motion){
    smoothedDistance = sensor.read() / 1000.0; // Convert mm to meters
    //Serial.println(smoothedDistance);
      if( minDistance -  smoothedDistance > maxDistance){
        deltaTime = (millis() - startTime) / 1000;
        float force = WEIGHT_KG * GRAVITY;
        float horsepower = ((force/WEIGHT_KG_POWER_HORSE) * maxDistance) / (deltaTime);
        //double horsepower = (0.1 *9.81* maxDistance) / (deltaTime);
        //float Watt = horsepower * HORSEPOWER_CONVERSION;

        Lift_in_motion = false;
        first_try = true;

        Serial.println("LOG Lift cycle complete. Cooldown started.");
        Serial.println("horsepower :" + String(horsepower));
        Serial.println("time: " + String((millis() - startTime)/1000) + "sec");

      }

  }

  if(smoothedDistance > minDistance && first_try){
    first_try = false;
    Serial.println("LOG Cooldown complete. Ready for next try.");
  }
}


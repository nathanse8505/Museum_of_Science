
#define SENSOR A0
#define BUTTON 5
#define MOTOR A1

void setup() {
  pinMode(SENSOR,INPUT);
  pinMode(BUTTON,INPUT);
  pinMode(MOTOR,OUTPUT);

  // put your setup code here, to run once:

}

const int time_bouncing = 20;
bool check = HIGH;
float first_time;
float stop_time = 20 * 1000;

bool SWITCH()
{
    if(digitalRead(BUTTON) == HIGH && check == LOW){
        check =HIGH;
        delay(time_bouncing);
    }
    if(digitalRead(BUTTON) == LOW && check == HIGH){
        check = LOW;
        return HIGH;
    }
    return LOW;

}



bool sensor_ready(int button_check)
{
    if(button_check){
      first_time = millis();
      if (millis() - first_time < 500){
          digitalWrite(MOTOR,HIGH);
          return HIGH;
      }
    }
    return LOW;

}




void loop() {
    bool button_check = SWITCH();
    bool sensor_ready1 = sensor_ready(button_check);

    if(sensor_ready1){
        digitalWrite(MOTOR,HIGH);

        if(analogRead(SENSOR) == HIGH){
            digitalWrite(MOTOR,LOW);
            delay(stop_time);
        }
    }


   
  // put your main code here, to run repeatedly:

}

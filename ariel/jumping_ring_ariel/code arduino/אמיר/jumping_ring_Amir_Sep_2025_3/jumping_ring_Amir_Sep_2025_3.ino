/*
 * for testing jumping ring based on Nathan code see https://github.com/nathanse8505/Museum_of_Science/tree/main/ariel/jumping_ring_ariel
 */
#include "basic_routine.h"

void setup() {
  // Configure pin modes
  pinMode(BUTTON_IO, INPUT_PULLUP);  // Ignition button (active LOW)
  pinMode(MODE_IO, INPUT_PULLUP);  // Ignition button (active LOW)
  pinMode(LED_ACTIVATION, OUTPUT);   // Activation indicator LED
  pinMode(INTERNAL_LED, OUTPUT);   // Activation indicator LED
  digitalWrite(INTERNAL_LED, LOW) ;// swich led off !!
  pinMode(RELAY_IO, OUTPUT);    // relay control Contactor Coil
  digitalWrite(RELAY_IO, LOW) ;// swich SSR off !!

//  wdt_enable(WDTO_2S); 
  state_timer = millis();
  led_blink_timer = millis();
  current_state = 0;// start with option to active
  digitalWrite(LED_ACTIVATION, HIGH) ;// led in switch on  !! 
  watchdogSetup();
}

void loop() {
  wdt_reset();
  switch (current_state) {
      case WAIT_FOR_BUTTON_PRESSED_STATE:
        // check auto switch (active low) 
        auto_sw_state = digitalRead(MODE_IO) ;// check auto switch 
        if(auto_sw_state == LOW){// auto switch low (= auto mode) now check auto cycle timer 
          if ((millis()-state_timer) >= AUTO_CYCLE_TIME){
              digitalWrite(RELAY_IO, HIGH) ;// swich SSR on !!
              current_state = SSR_ON_STATE;
              digitalWrite(LED_ACTIVATION, LOW) ;// led in switch off  !! 
              state_timer = millis();
              }
          }
        // check user push button 
        button_sw_state = digitalRead(BUTTON_IO) ;// check user switch 
        if(button_sw_state == LOW){// auto switch low (= auto mode) now check auto cycle timer 
          digitalWrite(RELAY_IO, HIGH) ;// swich SSR on !!
          current_state = SSR_ON_STATE;
          digitalWrite(LED_ACTIVATION, LOW) ;// led in switch off  !! 
          state_timer = millis();
          }
        // blink led slow 
        if ((millis()-led_blink_timer) >= LED_BLINK_SLOW){
          internal_led_state = !internal_led_state;
          digitalWrite(INTERNAL_LED, internal_led_state) ;// swich led on/off
          led_blink_timer = millis();
        }
      break;
//========
      case SSR_ON_STATE:
       // wait SST on time 
        if ((millis()-state_timer) >= SSR_ON_TIME){
          digitalWrite(RELAY_IO, LOW) ;// swich SSR off !!
          current_state = SSR_MIN_OFF_STATE;          
          state_timer = millis();
        }
        // blink led fast 
        if ((millis()-led_blink_timer) >= LED_BLINK_FAST){
          internal_led_state = !internal_led_state;
          digitalWrite(INTERNAL_LED, internal_led_state) ;// swich led on/off
          led_blink_timer = millis();
        }
      break;
//========
      case SSR_MIN_OFF_STATE:
       // wait minimum charge time 
        if ((millis()-state_timer) >= SSR_MIN_OFF_TIME){
          digitalWrite(LED_ACTIVATION, HIGH) ;// led in switch off  !! 
          current_state = WAIT_FOR_BUTTON_PRESSED_STATE;          
          state_timer = millis();
        }
        // blink internal led 
        if ((millis()-led_blink_timer) >= LED_BLINK_MID){
          internal_led_state = !internal_led_state;
          digitalWrite(INTERNAL_LED, internal_led_state) ;// swich led on/off
          led_blink_timer = millis();
        }
       break;
//========
      default:
        // statements
        break;
      }
}

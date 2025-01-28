// UV LED controller for multiple sets with buttons, async
// 11.12.24 | Illia Fortus | fortus.illia1@gmail.com
//--------------------------------------------------

#include "CONST_N_GLOBAL.h"

// toggle clk pin of serial communication
void toggle_clk() {
  digitalWrite(UV_Clk_Pin, HIGH);
  digitalWrite(UV_Clk_Pin, LOW);
}

// Turn off all LEDs across all groups
void off_all(int num) {
  digitalWrite(UV_En_Pin, HIGH);
  digitalWrite(UV_Data_Pin, LOW);
  for (int j = 0; j < num; j++) {
    toggle_clk();
  }
  digitalWrite(UV_St_Pin, HIGH);
  digitalWrite(UV_St_Pin, LOW);
  digitalWrite(UV_En_Pin, LOW);
}

// Turn on all LEDs across all groups
void on_all(int num) {
  digitalWrite(UV_En_Pin, HIGH);
  digitalWrite(UV_Data_Pin, HIGH);
  for (int j = 0; j < num; j++) {
    toggle_clk();
  }
  digitalWrite(UV_Data_Pin, LOW);
  digitalWrite(UV_St_Pin, HIGH);
  digitalWrite(UV_St_Pin, LOW);
  digitalWrite(UV_En_Pin, LOW);
}

// Turn on LEDs for a specified group
void on_led(int num, int group) {
  ledStates[group - 1] = true;  // Set group state to ON
  digitalWrite(UV_En_Pin, HIGH);
  for (int i = 0; i < 6; i++) {
    digitalWrite(UV_Data_Pin, LOW);
    toggle_clk();
  }
  for (int i = 0; i < NUM_GROUPS; i++) {

    // FOR FULL GROUP
    // digitalWrite(UV_Data_Pin, ledStates[i] ? HIGH : LOW);  // Set data pin state based on group state
    // for (int j = 0; j < 4; j++) {
    //   digitalWrite(UV_Clk_Pin, HIGH);
    //   digitalWrite(UV_Clk_Pin, LOW);
    // }

    digitalWrite(UV_Data_Pin, ledStates[i]);
    toggle_clk();
    digitalWrite(UV_Data_Pin, ledStates[i]);
    toggle_clk();
  }

  digitalWrite(UV_St_Pin, HIGH);
  digitalWrite(UV_St_Pin, LOW);
  digitalWrite(UV_En_Pin, LOW);
}

// Turn off LEDs for a specified group
void off_led(int num, int group) {
  ledStates[group - 1] = false;  // Set group state to OFF
  digitalWrite(UV_En_Pin, HIGH);
  for (int i = 0; i < 6; i++) {
    digitalWrite(UV_Data_Pin, LOW);
    toggle_clk();
  }
  for (int i = 0; i < NUM_GROUPS; i++) {

    // FOR FULL GROUP
    // digitalWrite(UV_Data_Pin, ledStates[i] ? HIGH : LOW);  // Set data pin state based on group state
    // for (int j = 0; j < 4; j++) {
    //   digitalWrite(UV_Clk_Pin, HIGH);
    //   digitalWrite(UV_Clk_Pin, LOW);
    // }

    digitalWrite(UV_Data_Pin, ledStates[i]);
    toggle_clk();
    digitalWrite(UV_Data_Pin, ledStates[i]);
    toggle_clk();
  }
  digitalWrite(UV_St_Pin, HIGH);
  digitalWrite(UV_St_Pin, LOW);
  digitalWrite(UV_En_Pin, LOW);
}

// Initial setup for pin configurations
void setup() {
  Serial.begin(115200);

  pinMode(UV_St_Pin, OUTPUT);
  pinMode(UV_Clk_Pin, OUTPUT);
  pinMode(UV_Data_Pin, OUTPUT);
  pinMode(UV_En_Pin, OUTPUT);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);

  digitalWrite(UV_En_Pin, HIGH);
  off_all(LED_NUM);  // Initialize all LEDs to OFF
  if (PRINT_LOG) {
    Serial.println("All initialized.");
  }
}

// Main loop to handle button presses and LED timeout
void loop() {
  unsigned long current_time = millis();
  int b1_state = digitalRead(BUTTON1);
  int b2_state = digitalRead(BUTTON2);
  int b3_state = digitalRead(BUTTON3);
  int b4_state = digitalRead(BUTTON4);
  int b5_state = digitalRead(BUTTON5);


  if (!IDLE_MODE && (current_time - LAST_BUTTON_PRESS_TIME >= IDLE_DELAY)) {
    if (PRINT_LOG) {
      Serial.println("Entered idle mode.");
    }
    IDLE_MODE = true;
    do {
      RANDOM_GROUP = random(1, NUM_GROUPS + 1);
    } while (RANDOM_GROUP == LAST_GROUP);
    LAST_GROUP = RANDOM_GROUP;
    on_led(LED_NUM, RANDOM_GROUP);  // Turn on random group
    if (PRINT_LOG) {
      Serial.print("Turned on LED");
      Serial.print(RANDOM_GROUP);
      Serial.print(" in idle.\n");
    }
    LAST_BUTTON_PRESS_TIME = current_time;  // Reset to track random on time
  }

  // If in idle mode, check if time have passed to turn off random LED
  if (IDLE_MODE) {
    if (current_time - LAST_BUTTON_PRESS_TIME >= RANDOM_ON_DURATION) {
      off_led(LED_NUM, RANDOM_GROUP);         // Turn off the random group
      LAST_BUTTON_PRESS_TIME = current_time;  // Reset to track idle cycle
      do {
        RANDOM_GROUP = random(1, NUM_GROUPS + 1);
      } while (RANDOM_GROUP == LAST_GROUP);
      LAST_GROUP = RANDOM_GROUP;

      on_led(LED_NUM, RANDOM_GROUP);
      if (PRINT_LOG) {
        Serial.print("Changed to LED");
        Serial.print(RANDOM_GROUP);
        Serial.print(" in idle.\n");
      }
    } else {
      if ((b1_state == LOW && last_b1_state == HIGH) || (b2_state == LOW && last_b2_state == HIGH) || (b3_state == LOW && last_b3_state == HIGH) || (b4_state == LOW && last_b4_state == HIGH) || (b5_state == LOW && last_b5_state == HIGH)) {
        off_led(LED_NUM, RANDOM_GROUP);  // Turn off the random group
        IDLE_MODE = false;
        if (PRINT_LOG) {
          Serial.println("Exit idle mode.");
        }
      }
    }
  }

  // Button handling code
  // Button 1 toggle and LED state management
  if (b1_state == LOW && last_b1_state == HIGH) {
    IDLE_MODE = false;  // Exit idle mode
    led1_toggle = !led1_toggle;
    if (led1_toggle) {
      if (PRINT_LOG) {
        Serial.println("LED1 on.");
      }
      on_led(LED_NUM, 5);           // Turn on LED group 5
      led1_on_time = current_time;  // Start time for LED group 5
    } else {
      if (PRINT_LOG) {
        Serial.println("LED1 off.");
      }
      off_led(LED_NUM, 5);  // Turn off LED group 5
    }
    LAST_BUTTON_PRESS_TIME = current_time;  // Update last press time
  }
  last_b1_state = b1_state;

  // Button 2 toggle and LED state management
  if (b2_state == LOW && last_b2_state == HIGH) {
    IDLE_MODE = false;  // Exit idle mode
    led2_toggle = !led2_toggle;
    if (led2_toggle) {
      if (PRINT_LOG) {
        Serial.println("LED2 on.");
      }
      on_led(LED_NUM, 4);           // Turn on LED  group 4
      led2_on_time = current_time;  // Start time for LED group 4
    } else {
      if (PRINT_LOG) {
        Serial.println("LED2 off.");
      }
      off_led(LED_NUM, 4);  // Turn off LED group 4
    }
  }
  last_b2_state = b2_state;

  // Button 3 toggle and LED state management
  if (b3_state == LOW && last_b3_state == HIGH) {
    IDLE_MODE = false;  // Exit idle mode
    led3_toggle = !led3_toggle;
    if (led3_toggle) {
      if (PRINT_LOG) {
        Serial.println("LED3 on.");
      }
      on_led(LED_NUM, 3);           // Turn on LED group 3
      led3_on_time = current_time;  // Start time for LED group 3
    } else {
      if (PRINT_LOG) {
        Serial.println("LED3 off.");
      }
      off_led(LED_NUM, 3);  // Turn off LED group 3
    }
  }
  last_b3_state = b3_state;

  // Button 4 toggle and LED state management
  if (b4_state == LOW && last_b4_state == HIGH) {
    IDLE_MODE = false;  // Exit idle mode
    led4_toggle = !led4_toggle;
    if (led4_toggle) {
      if (PRINT_LOG) {
        Serial.println("LED4 on.");
      }
      on_led(LED_NUM, 2);           // Turn on LED group 2
      led4_on_time = current_time;  // Start time for LED group 2
    } else {
      if (PRINT_LOG) {
        Serial.println("LED4 off.");
      }
      off_led(LED_NUM, 2);  // Turn off LED group 2
    }
  }
  last_b4_state = b4_state;

  // Button 5 toggle and LED state management
  if (b5_state == LOW && last_b5_state == HIGH) {
    IDLE_MODE = false;  // Exit idle mode
    led5_toggle = !led5_toggle;
    if (led5_toggle) {
      if (PRINT_LOG) {
        Serial.println("LED5 on.");
      }
      on_led(LED_NUM, 1);           // Turn on LED group 1
      led5_on_time = current_time;  // Start time for LED group 1
    } else {
      if (PRINT_LOG) {
        Serial.println("LED5 off.");
      }
      off_led(LED_NUM, 1);  // Turn off LED group 1
    }
  }
  last_b5_state = b5_state;

  // Automatic turn-off for each LED group after timeout
  if (led1_toggle && (current_time - led1_on_time >= TURN_OFF_DELAY)) {
    led1_toggle = false;
    if (PRINT_LOG) {
      Serial.println("LED1 off automatically.");
    }
    off_led(LED_NUM, 5);  // Turn off LED group 5
  }
  if (led2_toggle && (current_time - led2_on_time >= TURN_OFF_DELAY)) {
    led2_toggle = false;
    if (PRINT_LOG) {
      Serial.println("LED2 off automatically.");
    }
    off_led(LED_NUM, 4);  // Turn off LED group 4
  }
  if (led3_toggle && (current_time - led3_on_time >= TURN_OFF_DELAY)) {
    led3_toggle = false;
    if (PRINT_LOG) {
      Serial.println("LED3 off automatically.");
    }
    off_led(LED_NUM, 3);  // Turn off LED group 3
  }
  if (led4_toggle && (current_time - led4_on_time >= TURN_OFF_DELAY)) {
    led4_toggle = false;
    if (PRINT_LOG) {
      Serial.println("LED4 off automatically.");
    }
    off_led(LED_NUM, 2);  // Turn off LED group 2
  }
  if (led5_toggle && (current_time - led5_on_time >= TURN_OFF_DELAY)) {
    led5_toggle = false;
    if (PRINT_LOG) {
      Serial.println("LED5 off automatically.");
    }
    off_led(LED_NUM, 1);  // Turn off LED group 1
  }

  delay(DEBOUNCE_DELAY);  // Debounce delay
}

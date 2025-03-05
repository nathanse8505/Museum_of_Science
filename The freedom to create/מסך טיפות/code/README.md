**Code for "drop screen" exhibit in the Jerusalem Science Museum**

Directory **"camera"**: python code for handling the camera and sending the images to the connected Arduino.
- **main.py** - main code handling everything, run this.
- **consts.py** - constants file.
- **process_one_image.py** - process one image manualy - not used in the final implementaion, used for testing images.
- **images_for_idle** - sub directory containing all pictures that will be sent by the main code for the Arduino in idle mode.

Directory **"Drop-Screen"**: Arduino code for running the images dropped in water.
- **Drop-Screen.ino** - main code for Arduino, handling the given image and activating the valves and light.
- **Consts.h** - constants file.
- **Routines.h** - basic routines for controlling the valves and light.
- **Display.h** - basic routines for Adafruit screen, currently not used.

Directory **"Drop-Screen-No-Camera"**: Arduino code for dropping static constant images without any camera. Can be used for testing parameters.
- **Drop-Screen.ino** - main code for Arduino, handling the valves and light.
- **Consts.h** - constants file.
- **Routines.h** - basic routines for controlling the valves, light and encoder.
- **Display.h** - basic routines for Adafruit screen.

Directory **"physics"**: physics analysis and simulation for "gravity correction code".
- **time_factoring.xlsx**: physics analysis for calculating the time to open each row in the image to correct the gravity effects on the bottom rows.
-  **simulation.py**: python code for simulating the equations with one column of water.

    NOTE: in the final implementation, I deleted this gravity correction code because it didn't help that much and added unnecessary complexity.
    I think the human eye is correcting the image naturally.

Directory **"Test-Valves"**: Arduino code for testing the valves and boards. Use this if you suspect there is a problem in a valve/board.
- **Test-Valves.ino** - main code for Arduino.
- **Consts.h** - constants file.
- **Routines.h** - basic routines for controlling the valves and encoder.
- **Display.h** - basic routines for Adafruit screen.

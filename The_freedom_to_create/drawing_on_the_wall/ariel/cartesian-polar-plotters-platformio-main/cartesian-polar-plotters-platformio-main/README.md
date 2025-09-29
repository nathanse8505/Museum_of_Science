[Project Logs](https://docs.google.com/document/d/1Dykp1E0tP4Vg5iNTMWY0Hv9gmuN5yl5A7_C2X_2-8zA/edit?tab=t.0#heading=h.mpouvkfogwal)

# Overview
The Cartestian and Polar (or Delta) plotters are 2D plotter positioning systems. Each one lets a user draw on a UV sensitive canvas with the UV pen acting as the end effector of the system.  

This repo utilizes PlatformIO for building all Arduino code.

The cartesian plotter is a cnc-gantry [x-y plotter](https://en.wikipedia.org/wiki/Plotter) (see [here](https://www.instructables.com/X-Y-Plotter/) as well), while the polar is a [Polargraph-style](https://www.instructables.com/Polargraph-Drawing-Machine/) positioning. Both are controlled by 2 rotary-wheels that affect the 2 motors of the plotters (`xy` and `r1r2` location respectively).  
- The user moves the rotary-wheels, which are connected to encoders whose signal is sent to an Arduino.  
- The Arduino sends a signal to the motors to move accordingly.  
- Finally, clicking on the left wheel (x or r1 depending on the plotter type) toggles the end effector, namely the UV light that draws on the photosensitive canvas.

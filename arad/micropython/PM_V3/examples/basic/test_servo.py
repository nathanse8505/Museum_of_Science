from sys import modules
from machine import Pin
from time import sleep_ms
from servo import Servo
from math import sin, radians


servo = Servo(Pin(6))


print('''
Test Servo:
Moves a servo motor in a sinusoidal way.
Press Ctrl-C to exit the test.
''')
try:
    while True:
        for i in range(360):
            servo.set_angle(sin(radians(i + 90)) * 90 + 90)
            sleep_ms(5)
except KeyboardInterrupt:
    pass
servo.set_enable(False)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

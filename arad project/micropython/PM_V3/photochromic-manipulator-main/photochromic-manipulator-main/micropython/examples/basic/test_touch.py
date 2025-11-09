from sys import modules
from machine import Pin, TouchPad
from time import sleep_ms


touch_l = TouchPad(Pin(5))
touch_r = TouchPad(Pin(4))


print('''
Test Touch:
Prints touch-pads states (pressed=large value, released=small value).
Press Ctrl-C to exit the test.
''')
try:
    while True:
        l = touch_l.read()
        r = touch_r.read()
        print('L=', l, ' R=', r, end='\t\t\r', sep='') # print over the same line
        sleep_ms(20)
except KeyboardInterrupt:
    pass
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

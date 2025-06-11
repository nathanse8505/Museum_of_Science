from sys import modules
from machine import Pin
from time import sleep_ms


btn_b = Pin(0, Pin.OUT, drive=Pin.DRIVE_0, value=1) # PULL_UP doesn't pull enough..
btn_l = Pin(12, Pin.OUT, drive=Pin.DRIVE_0, value=1)
btn_r = Pin(34, Pin.OUT, drive=Pin.DRIVE_0, value=1)


print('''
Test Buttons:
Prints button states (pressed=0, released=1).
Press Ctrl-C to exit the test.
''')
try:
    while True:
        b = btn_b.value()
        l = btn_l.value()
        r = btn_r.value()
        print('B=', b, ' L=', l, ' R=', r, end='\t\t\r', sep='') # print over the same line
        sleep_ms(20)
except KeyboardInterrupt:
    pass
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

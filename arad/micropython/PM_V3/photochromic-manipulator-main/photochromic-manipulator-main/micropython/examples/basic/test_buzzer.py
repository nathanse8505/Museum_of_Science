from sys import modules
from machine import Pin
from time import sleep_ms
from buzzer import Buzzer


buzzer = Buzzer(Pin(11), Pin(13))


print('''
Test Buzzer:
Beeps and play melodies.
Press Ctrl-C to exit the test.
''')
try:
    while True:
        for i in range(6):
            buzzer.set_freq(880 if i % 2 else 440)
            sleep_ms(500)
except KeyboardInterrupt:
    pass
buzzer.set_freq(0)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

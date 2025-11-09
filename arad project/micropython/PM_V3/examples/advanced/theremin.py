from sys import modules
from time import sleep_ms
from pm import PM


pm = PM()


print('''
Theremin:
Emulates a theremin musical instrument.
Prints touch-pads states (pressed=large value, released=small value).
Press Ctrl-C to exit.
''')
try:
    l_base = int(pm.touch_l.read() * 0.99)
    r_base = int(pm.touch_r.read() * 0.99)
    last_f = 0
    while True:
        l = pm.touch_l.read() - l_base
        r = pm.touch_r.read() - r_base
        f = abs(l + r)
        if abs(last_f - f) > 40:
            pm.buzzer.beep(f)
            last_f = f
        sleep_ms(20)
except KeyboardInterrupt:
    pass
pm.buzzer.off()
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

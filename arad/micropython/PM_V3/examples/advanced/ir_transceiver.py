from sys import modules
from time import sleep_ms
from pm import PM


pm = PM(ir_active=True)


def set_leds_color(c):
    pm.rgb_leds.fill((
        128 if c & 1 else 0,
        128 if c & 2 else 0,
        128 if c & 4 else 0
    ))


last_c = 0
print('''
IR Transceiver - each button press sends an IR NEC signal,
received IR NEC signals sets the LEDs color and buzzer beeps.
Press Ctrl-C to exit the test.
''')
try:
    while True:
        c = (not pm.btn_l.value()) * 1 + (not pm.btn_r.value()) * 2
        if c and c != last_c:
            print('Transmited IR address:', c)
            pm.ir_tx.transmit(c)
        last_c = c

        if pm.ir_rx.received():
            c, _ = pm.ir_rx.get()
            print('Received IR address:', c)
            set_leds_color(c)
            pm.buzzer.beep(440 * c, -100) # -100ms = non-blocking

        sleep_ms(10)
except KeyboardInterrupt:
    pass
pm.set_enable(False)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

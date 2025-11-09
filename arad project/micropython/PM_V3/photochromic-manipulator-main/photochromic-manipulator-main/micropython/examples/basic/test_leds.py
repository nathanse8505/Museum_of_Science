from sys import modules
from neopixel import NeoPixel
from machine import Pin
from time import time, sleep, sleep_ms


BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
CYAN = (0, 255, 255)
MAGENTA = (255, 0, 255)
WHITE = (255, 255, 255)


rgb_leds = NeoPixel(Pin(9), 3) # we have 2 LEDs but it cause MP to stuck after many calls to write..
uv_led = Pin(33, Pin.OUT)
blue_led = Pin(15, Pin.OUT)


def rgb_leds_set(l=None, r=None):
    if l is not None:
        rgb_leds[0] = l
    if r is not None:
        rgb_leds[1] = r
    rgb_leds.write()


def rgb_leds_fill(c):
    rgb_leds_set(c, c) # rgb_leds.fill(c)


def rgb_leds_swap():
    rgb_leds_set(rgb_leds[1], rgb_leds[0])


def solid_green(duration):
    rgb_leds_fill(GREEN)
    sleep(duration)


def fading_blue(duration):
    c = [0, 0, 0]
    end_time = time() + duration
    while end_time > time():
        uv_led.value(not uv_led.value())
        blue_led.value(uv_led.value())
        s = -5 if c[2] else 5
        for i in range(255/5):
            c[2] += s
            rgb_leds_fill(c)
            sleep_ms(4)
    rgb_leds_fill(BLACK)
    uv_led.off()
    blue_led.off()


def rainbow(duration):
    c = [0, 255, 0]
    def fade(ch): # helper func that fade color channel
        s = -5 if c[ch] else 5
        for i in range(255/5):
            c[ch] += s
            rgb_leds_fill(c)
            sleep_ms(4)
    end_time = time() + duration
    while end_time > time():
        for i in range(3):
            fade(2 - i)
    rgb_leds_fill(BLACK)


def police(duration):
    def swap(l, r, c, d): # helper func that swaps led colors
        rgb_leds_set(l, r)
        for i in range(c):
            rgb_leds_swap()
            sleep_ms(d)

    end_time = time() + duration
    while end_time > time():
        swap(RED, BLUE, 10, 120)
        swap(RED, BLACK, 5, 200)
        for i in range(10):
            rgb_leds_fill(WHITE if i % 2 else BLACK)
            sleep_ms(20)
        swap(BLUE, BLACK, 5, 200)
    rgb_leds_fill(BLACK)


print('''
Test LEDs:
LED effects using 2 RGB LEDs, 1 UV LED and 1 blue LED.
Press Ctrl-C to exit the test.
''')
try:
    while True:
        rainbow(5)
        fading_blue(5)
        solid_green(5)
        police(5)
        sleep(2)
except KeyboardInterrupt:
    pass
uv_led.off()
blue_led.off()
rgb_leds_fill(BLACK)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

#
# rgb_leds.py - MicroPython library for driving Neopixel RGB LEDs.
#
# https://github.com/arduino12/micropython-libs 2023/07/04
#

__version__ = '1.0.0'

from neopixel import NeoPixel
from micropython import const


class RgbColor(list):
    def __init__(self, r=None, g=None, b=None, rgb=None,
                 h=None, s=None, v=None, hsv=None, rgb24=None, name=None, rgb_color=None):
        if isinstance(r, int):
            if g is None:
                rgb24 = r
                r = None
        elif isinstance(r, (tuple, list, bytes)):
            rgb = r
            r = None
        elif isinstance(r, str):
            name = r
            r = None
        else:
            raise ValueError()

        if name is not None:
            rgb = globals()['RGB_COLORS'].get(name.lower(), RGB_BLACK)
                
        if rgb24 is not None:
            rgb = ((rgb24 >> 0) & 0xff, (rgb24 >> 8) & 0xff, (rgb24 >> 16) & 0xff)

        if rgb is None:
            rgb = (r or 0, g or 0, b or 0)

        super().__init__(rgb)


RGB_BLACK = RgbColor(0, 0, 0)
RGB_RED = RgbColor(255, 0, 0)
RGB_YELLOW = RgbColor(255, 255, 0)
RGB_GREEN = RgbColor(0, 255, 0)
RGB_CYAN = RgbColor(0, 255, 255)
RGB_BLUE = RgbColor(0, 0, 255)
RGB_MAGENTA = RgbColor(255, 0, 255)
RGB_WHITE = RgbColor(255, 255, 255)
RGB_COLORS = {
    'black': RGB_BLACK,
    'red': RGB_RED,
    'yellow': RGB_YELLOW,
    'green': RGB_GREEN,
    'cyan': RGB_CYAN,
    'blue': RGB_BLUE,
    'magenta': RGB_MAGENTA,
    'white': RGB_WHITE,
}


class RgbLeds:

    def __init__(self, pin, count=3): # we have 2 LEDs but it cause MP to stuck after many calls to write..
        self.leds = NeoPixel(pin, count)

    def set_leds(self, l=None, r=None):
        if l is not None:
            self.leds[0] = RgbColor(l)
        if r is not None:
            self.leds[1] = RgbColor(r)
        self.leds.write()

    def fill(self, *args):
        self.leds.fill(RgbColor(*args))
        self.leds.write()

    def swap(self):
        self.leds[0], self.leds[1] = self.leds[1], self.leds[0]
        self.leds.write()
    
    def off(self):
        self.fill(0)


if __name__ == '__main__':
    print('\nTest rgb_leds', __version__)

    from machine import Pin
    from time import sleep_ms

    rgb_leds = RgbLeds(Pin(9))

    try:
        for name, color in RGB_COLORS.items():
            print(name)
            rgb_leds.fill(color)
            rgb_leds.fill(name)
            sleep_ms(500)
    except KeyboardInterrupt:
        pass

    rgb_leds.off()

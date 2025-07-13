#
# thermometer.py - MicroPython library for reading an I2C temperature sensor.
# Supports common LM75A, GX75B...
#
# https://github.com/arduino12/micropython-libs 2023/06/24
#

__version__ = '1.0.2'

from machine import I2C


class Thermometer:

    def __init__(self, i2c=None, scl_pin=None, sda_pin=None, address=0x48):
        if isinstance(i2c, int):
            i2c = I2C(i2c, scl=scl_pin, sda=sda_pin)
        self._i2c = i2c
        self._address = address # 0x48 - 0x4F
        self._buf = bytearray(2)

    def read_temp_c(self):
        self._i2c.readfrom_into(self._address, self._buf)
        v = (self._buf[0] << 3) | (self._buf[1] >> 5)
        # two's complement on 11bit register
        return ((v & 0x3ff) - (v & 0x400)) * 0.125

    def read_temp_str_c(self):
        return '{:0.2f}°C'.format(self.read_temp_c())


if __name__ == '__main__':

    print('\nTest thermometer', __version__)

    from machine import Pin
    from time import sleep_ms

    thermometer = Thermometer(0, Pin(10), Pin(8))
    # Can also use existing I2C bus:
    # i2c = I2C(0, scl=Pin(10), sda=Pin(8))
    # thermometer = Thermometer(i2c)
    
    try:
        for i in range(5):
            print(thermometer.read_temp_str_c())
            sleep_ms(500)
    except KeyboardInterrupt:
        pass

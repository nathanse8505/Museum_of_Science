#
# servo.py - MicroPython library for driving a PWM servo motor.
#
# https://github.com/arduino12/micropython-libs 2023/06/17
#

__version__ = '1.0.0'

from machine import PWM


class Servo:

    def __init__(self, pin, min_us=630, max_us=2700, angles=180):
        self._min_us = min_us
        self._max_us = max_us
        self._angles = angles
        self._angle_us = (max_us - min_us) / angles
        self._us = 0
        self._pwm = PWM(pin, freq=50, duty_ns=0)

    def set_us(self, us):
        if us:
            if us < self._min_us:
                us = self._min_us
            elif us > self._max_us:
                us = self._max_us
            self._us = us
        self._pwm.duty_ns(int(us * 1000))

    def set_enable(self, is_on):
        self.set_us(self._us if is_on else 0)

    def set_angle(self, angle):
        if angle < 0:
            angle = 0
        elif angle > self._angles:
            angle = self._angles
        self.set_us(self._min_us + angle * self._angle_us)


if __name__ == '__main__':

    print('\nTest servo', __version__)

    from machine import Pin
    from time import sleep_ms
    from math import sin, radians

    servo = Servo(Pin(18))

    try:
        for i in range(3 * 360 + 1):
            servo.set_angle(sin(radians(i + 90)) * 90 + 90)
            sleep_ms(5)
    except KeyboardInterrupt:
        pass

    servo.set_enable(False)

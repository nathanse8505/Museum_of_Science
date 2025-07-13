#
# pm.py - micropython library for Photochromic Manipulator V2 kit.
#
# ESP32-S2-Mini controlls:
#  2 Servo motors.
#  2 NeoPixel LEDs.
#  1 UV LED.
#  1 Blue LED.
#  1 Piezoelectric buzzer.
#  3 Push-Buttons (1 boot marked "0" on purple PCB).
#  2 Capacitive touch pads.
#  1 Digital temperature sensor.
#  1 IR receiver (interrupt based- not very reliable..).
#  1 IR LED (low power).
#  1 WiFi (can do telegram, remote REPL, ESP-Now...).
#
# https://github.com/arduino12/photochromic_manipulator 2024/07/24
#

__version__ = '3.0.2'

from micropython import const
from machine import Pin, Timer, TouchPad
from buzzer import Buzzer
from servo import Servo
from gc import collect
from rgb_leds import RgbLeds
from utils import set_timer_collect
from ir_nec import IR_RX_NEC, IR_TX_NEC
from thermometer import Thermometer
from secrets import SERVO_US
from math import sin, cos, asin, acos, tau, radians, degrees, sqrt, floor
from time import sleep_ms


HW_VERSION = const(3)
SW_VERSION = const(1)

SDA_PIN = const(8)
SCL_PIN = const(10)
BTN_B_PIN = const(0)
BTN_L_PIN = const(12)
BTN_R_PIN = const(34)
UV_LED_PIN = const(33)
SERVO_L_PIN = const(7)
SERVO_R_PIN = const(6)
BUZZER_A_PIN = const(11)
BUZZER_B_PIN = const(13)
NEOPIXEL_PIN = const(9)
BLUE_LED_PIN = const(15)
IR_RECEIVER_PIN = const(14)
IR_TRANSMITTER_PIN = const(36)
TOUCH_L_PIN = const(5)
TOUCH_R_PIN = const(4)
SERVO_OFFSET = const(35)
SERVO_ANGLES = const(170)


class PM:
    HOME_POINT = (0, 18)
    X_MIN, X_MAX, Y_MIN, Y_MAX = -35, 35, 15, 65

    def __init__(self, bar0=16, bar1=34, bar2=43, ir_active=False):
        self.rgb_leds = RgbLeds(Pin(NEOPIXEL_PIN))
        self.uv_led = Pin(UV_LED_PIN, Pin.OUT)
        self.blue_led = Pin(BLUE_LED_PIN, Pin.OUT)
        self.buzzer = Buzzer(Pin(BUZZER_A_PIN), Pin(BUZZER_B_PIN))
        self.servo_l = Servo(Pin(SERVO_L_PIN), SERVO_US[0], SERVO_US[1], SERVO_ANGLES)
        self.servo_r = Servo(Pin(SERVO_R_PIN), SERVO_US[2], SERVO_US[3], SERVO_ANGLES)
        self.touch_l = TouchPad(Pin(TOUCH_L_PIN))
        self.touch_r = TouchPad(Pin(TOUCH_R_PIN))
        self.btn_l = Pin(BTN_L_PIN, Pin.OUT, drive=Pin.DRIVE_0, value=1)
        self.btn_r = Pin(BTN_R_PIN, Pin.OUT, drive=Pin.DRIVE_0, value=1)
        self.btn_b = Pin(BTN_B_PIN, Pin.OUT, drive=Pin.DRIVE_0, value=1) # PULL_UP doesn't pull enough..
        if ir_active:
            self.ir_rx = IR_RX_NEC(Pin(IR_RECEIVER_PIN))
            self.ir_tx = IR_TX_NEC(Pin(IR_TRANSMITTER_PIN), active_level=False)
        self.thermometer = Thermometer(1, Pin(SCL_PIN), Pin(SDA_PIN))

        self._bar0 = bar0
        self._bar1 = bar1
        self._bar2 = bar2
        self.draw_speed = 3
        self.move_speed = 3
        self._next_points = []
        self._next_x = []
        self._next_y = []
        self._next_z = []
        self._last_x = self.HOME_POINT[0]
        self._last_y = self.HOME_POINT[1]
        self.auto_draw = True
        self.set_enable(True)

    def set_led(self, draw):
        if draw is None:
            return
        self.uv_led.value(draw)

    def set_angles(self, l, r):
        self.servo_l.set_angle(SERVO_ANGLES - l - SERVO_OFFSET)
        self.servo_r.set_angle(r + SERVO_OFFSET)

    def set_axes(self, l, r):
        self.set_angles(180 - l, r)

    def set_axes_fbk(self, x, y):
        if x < self.X_MIN or y < self.Y_MIN or x > self.X_MAX or y > self.Y_MAX:
            return False

        half_b0 = self._bar0 / 2;
        l_x = x + half_b0
        r_x = x - half_b0

        yy = y ** 2
        l_ss = l_x ** 2 + yy
        r_ss = r_x ** 2 + yy

        l_s = sqrt(l_ss);
        r_s = sqrt(r_ss);

        l_a = degrees(asin(y / l_s));
        r_a = degrees(asin(y / r_s));

        if l_x <= 0:
            l_a = 180 - l_a;

        if r_x >= 0:
            r_a = 180 - r_a;

        bb1 = 2 * self._bar1
        b1b2 = self._bar1 ** 2 - self._bar2 ** 2

        l_a += degrees(acos((b1b2 + l_ss) / (bb1 * l_s)));
        r_a += degrees(acos((b1b2 + r_ss) / (bb1 * r_s)));

        r_a = 180 - r_a;

        self.set_axes(l_a, r_a)

    def _move_to(self, p_x, p_y, speed):
        d_x = p_x - self._last_x
        d_y = p_y - self._last_y

        # path lenght in mm, times 4 equals 4 steps per mm
        c = floor(10 * sqrt(d_x * d_x + d_y * d_y));
        if c < 1:
            c = 1

        # draw line point by point
        for i in range(c + 1):
            self.set_axes_fbk(self._last_x + (i * d_x / c), self._last_y + (i * d_y / c))
            sleep_ms(speed);

        self.set_axes_fbk(p_x, p_y);
        self._last_x = p_x;
        self._last_y = p_y;
        sleep_ms(speed);

    def update_blocking(self):
        while self._next_z:
            x = self._next_x.pop(0)
            y = self._next_y.pop(0)
            z = self._next_z.pop(0)
            self.set_led(z)
            s = self.draw_speed if self.uv_led.value() else self.move_speed
#             print(x, y, z, s)
            self._move_to(x, y, s)
        self.stop()

    def stop(self, draw=False):
        self.set_led(draw)
        self.servo_l.set_enable(False)
        self.servo_r.set_enable(False)
        self._next_x.clear()
        self._next_y.clear()
        self._next_z.clear()
#         collect() # free unuesed RAM!

    def goto(self, point, draw=None):
        self._next_x.append(point[0])
        self._next_y.append(point[1])
        self._next_z.append(draw)
        if self.auto_draw:
            self.update_blocking()

    def move_home(self, immediately=False):
        if immediately:
            self.stop()
        self.goto(self.HOME_POINT, False)

    def draw_line(self, p1, p2):
        self.goto(p1, False)
        self.goto(p2, True)

    def draw_poly(self, points, closed=True):
        self.goto(points[0], False)
        for p in points[1:]:
            self.goto(p, True)
        if closed:
            self.goto(points[0], True)

    def draw_regular_poly(self, x, y, r, vertices, is_centered=True, start_angle=0):
        if not is_centered:
            x += r // 2
            y += r // 2
        a = tau / vertices
        b = radians(start_angle)
        self.draw_poly([(x + r * cos(i * a + b), y + r * sin(i * a + b)) for i in range(vertices)])

    def draw_rect(self, x, y, w, h=None, is_centered=True):
        if h is None:
            h = w
        if is_centered:
            x -= w // 2
            y -= h // 2
        self.draw_poly(((x, y), (x+w, y), (x+w, y+h), (x, y+h)))

    def draw_circle(self, x, y, r, is_centered=True):
        self.draw_regular_poly(x, y, r, 20, is_centered)

    def get_drawing_rect(self, scale):
        if not self._next_z:
            return self.X_MIN, self.X_MAX, self.Y_MIN, self.Y_MAX
        return min(self._next_x), max(self._next_x), min(self._next_y), max(self._next_y)

    def drawing_transform(self, func_x, func_y):
        for i in range(len(self._next_z)):
            self._next_x[i] = func_x(self._next_x[i])
            self._next_y[i] = func_y(self._next_y[i])

    def drawing_scale(self, scale):
        self.drawing_transform(lambda x, y: (x * scale, y * scale))
        for i in range(len(self._next_z)):
            self._next_x[i] *= scale
            self._next_y[i] *= scale

    def set_enable(self, is_enabled):
        self.stop()
        set_timer_collect(is_enabled)
        if not is_enabled:
            self.servo_l.set_enable(False)
            self.servo_r.set_enable(False)
        if hasattr(self, 'ir_rx'):
            self.ir_rx.set_enable(is_enabled)
            self.ir_tx.set_enable(is_enabled)
        self.buzzer.off()
        self.rgb_leds.off()
        self.blue_led.off()
        self.uv_led.off()


if __name__ == '__main__':
    pm = PM()

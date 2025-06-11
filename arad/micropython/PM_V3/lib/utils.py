#
# utils.py - MicroPython library for common stuff.
#
# https://github.com/arduino12/micropython-libs 2023/07/04
#

__version__ = '1.0.2'

from machine import Timer
from gc import collect
from network import WLAN, STA_IF
from time import sleep_ms


_PARAM_PATH = './secrets.py'
_timer_collect = Timer(-1)


def set_timer_collect(is_enabled, seconds=60):
    if is_enabled:
        _timer_collect.init(period=seconds * 1000, mode=Timer.PERIODIC,
                            callback=lambda _: collect())
    else:
        _timer_collect.deinit()


def wifi_connect(ssid, password, timeout_ms=6000):
    sta_if = WLAN(STA_IF)
    if not sta_if.isconnected():
        print('WiFi: connecting to', ssid, end='')
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            if timeout_ms <= 0:
                print(' faild!')
                return -1
            print('.', end='')
            timeout_ms -= 200
            sleep_ms(200)
        print(' done!')
    print('WiFi: IP address:', sta_if.ifconfig()[0])
    return 0


def get_param(key, default=None):
    with open(_PARAM_PATH, 'r') as f:
        for l in f:
            s = l.split('=', 1)
            if len(s) == 2 and s[0].strip() == key:
                return eval(s[1].strip())
    return default


def set_param(key, value):
    lines = []
    line = '{} = {}\n'.format(key, repr(value))
    with open(_PARAM_PATH, 'r') as f:
        for l in f:
            s = l.split('=', 1)
            if len(s) == 2 and s[0].strip() == key:
                lines.append(line)
                line = None
            else:
                lines.append(l)
    if line:
        lines.append(line)
    with open(_PARAM_PATH, 'w') as f:
        for l in lines:
            f.write(l)

#
# ir_nec.py - MicroPython library for transmitting and receiving IR NEC signals.
# Supports 8bit and 16bit (extended) NEC, active high/low IR LED and NEC repeats.
# NEC timings based on https://github.com/peterhinch/micropython_ir
#
# https://github.com/arduino12/micropython-libs 2023/06/30
#

__version__ = '1.0.0'

from micropython import const
from machine import Pin, Timer
from esp32 import RMT
from array import array
from time import ticks_us, ticks_diff
from collections import deque
from _thread import allocate_lock


_NEC_ZERO = const(563)
_NEC_ONE = const(1687)
_NEC_CARRIER = const(38000)
_NEC_DUTY = const(33)
_NEC_BITS = const(68)
_NEC_BLOCK_MS = const(80)
_NEC_REPEAT = const(-1)
_NEC_ERROR = const(-2)


class IR_TX_NEC:

    def __init__(self, pin, active_level=True, rmt_id=0):
        self._pin = pin
        self._idle_level = not active_level
        self._rmt_id = rmt_id
        self._bits = array('H', 0 for _ in range(_NEC_BITS))
        self._bits_ptr = memoryview(self._bits)
        self._bits_count = 0
        self.set_enable(True)

    def _append(self, *times):
        for t in times:
            self._bits[self._bits_count] = t
            self._bits_count += 1

    def _bit(self, b):
        self._append(_NEC_ZERO, _NEC_ONE if b else _NEC_ZERO)

    def transmit(self, addr=-1, data=0):
        if not self._rmt.wait_done():
            return -1
        self._bits_count = 0
        if addr < 0:
            self._append(9000, 2250, _NEC_ZERO)
        else:
            self._append(9000, 4500)
            if addr < 256:
                addr |= ((addr ^ 0xff) << 8)
            for _ in range(16):
                self._bit(addr & 1)
                addr >>= 1
            data |= ((data ^ 0xff) << 8)
            for _ in range(16):
                self._bit(data & 1)
                data >>= 1
            self._append(_NEC_ZERO)
        self._rmt.write_pulses(tuple(self._bits_ptr[0: self._bits_count]))
        return 0

    def set_enable(self, is_enabled):
        if is_enabled:
            # if self._pin.value(): # pin read low when RMT use it
            if str(getattr(self, '_rmt', 'RMT()')) == 'RMT()':
                self._rmt = RMT(self._rmt_id, pin=self._pin, clock_div=80,
                                tx_carrier=(_NEC_CARRIER, _NEC_DUTY, 1),
                                idle_level=self._idle_level)
                self._pin.value() # fix first bad transmission somehow 
        else:
            self._rmt.deinit()
            self._pin.init(Pin.OUT, value=self._idle_level)


class IR_RX_NEC():

    def __init__(self, pin, repeats=False, extended=True):
        self._pin = pin
        self._repeats = repeats
        self._extended = extended
        self._callback = None
        self._edge = 0
        self._times = array('i',  (0 for _ in range(_NEC_BITS + 1)))
        self._timer = Timer(-1)
        self._queue = deque((), 100)
        self._lock = allocate_lock()
        self.set_enable(True)

    def _pin_irq(self, line):
        t = ticks_us()
        if self._edge <= _NEC_BITS:
            if not self._edge:
                self._timer.init(period=_NEC_BLOCK_MS, mode=Timer.ONE_SHOT, callback=self._decode)
            self._times[self._edge] = t
            self._edge += 1
        else:
            self._edge = 0

    def _decode(self, _):
        try:
            if self._edge > _NEC_BITS:
                raise RuntimeError(_NEC_ERROR)
            width = ticks_diff(self._times[1], self._times[0])
            if width < 4000:
                raise RuntimeError(_NEC_ERROR)
            width = ticks_diff(self._times[2], self._times[1])
            if width > 3000:
                if self._edge < _NEC_BITS:
                    raise RuntimeError(_NEC_ERROR)
                val = 0
                for edge in range(3, _NEC_BITS - 2, 2):
                    val >>= 1
                    if ticks_diff(self._times[edge + 1], self._times[edge]) > 1120:
                        val |= 0x80000000
            elif width > 1700:
                raise RuntimeError(_NEC_REPEAT if self._repeats and (self._edge == 4) else _NEC_ERROR)
            else:
                raise RuntimeError(_NEC_ERROR)
            addr = val & 0xff
            data = (val >> 16) & 0xff
            if data != (val >> 24) ^ 0xff:
                raise RuntimeError(_NEC_ERROR)
            if addr != ((val >> 8) ^ 0xff) & 0xff:
                if not self._extended:
                    raise RuntimeError(_NEC_ERROR)
                addr |= val & 0xff00
        except RuntimeError as e:
            addr = e.args[0]
            data = 0

        self._edge = 0
        if addr == _NEC_ERROR:
            return
        if self._callback is not None:
            return self._callback(addr, data)
        with self._lock:
            self._queue.append((addr, data))

    def received(self):
        with self._lock:
            return bool(self._queue)

    def get(self):
        with self._lock:
            if not self._queue:
                return (_NEC_ERROR, 0)
            return self._queue.popleft()

    def set_callback(self, callback):
        self._callback = callback

    def set_enable(self, is_enabled):
        if is_enabled:
            self._pin.irq(handler=self._pin_irq, trigger=(Pin.IRQ_FALLING | Pin.IRQ_RISING))
        else:
            self._pin.irq(handler=None)
            self._timer.deinit()


if __name__ == '__main__':
    print('\nTest ir_nec', __version__)

    from machine import Pin
    from time import sleep_ms

    ir_rx = IR_RX_NEC(Pin(14)) # IR receiver VS1838B
    ir_tx = IR_TX_NEC(Pin(36)) # active high IR LED
    #ir_tx = IR_TX_NEC(Pin(21), active_level=False) # active low IR LED

    try:
        print('\ncallback')
        ir_rx.set_callback(lambda address, data: print(address, data, end=', '))
        for i in range(4):
            ir_tx.transmit(i, i)
            sleep_ms(500)

        print('\ndisabled')
        ir_rx.set_enable(False)
        for i in range(4):
            ir_tx.transmit(i, i)
            sleep_ms(500)

        print('\npolling')
        ir_rx.set_enable(True)
        ir_rx.set_callback(None)
        for i in range(4):
            ir_tx.transmit(i, i)
            for j in range(50):
                if ir_rx.received():
                    print(*ir_rx.get(), end=', ')
                sleep_ms(10)
    except KeyboardInterrupt:
        pass

    ir_rx.set_enable(False)
    ir_tx.set_enable(False)

#
# buzzer.py - MicroPython library for driving a piezoelectric buzzer.
# Supports single-pin PWM (buzzer conected to a GPIO and GND),
# and dual-pin complementary PWM (buzzer conected to both GPIOs) - louder than single-pin.
#
# https://github.com/arduino12/micropython-libs 2023/06/17
#

__version__ = '2.0.1'

from machine import Pin, PWM, mem32
from time import sleep_ms
from machine import Timer
from micropython import const
from sys import platform
from rtttl_songs import get_rtttl_song_by_name, get_rtttl_song_names
from _thread import allocate_lock


NOTE = [
    440, 493.9, 261.6, 293.7, 329.6, 349.2, 392,
    0, 466.2, 0, 277.2, 311.1, 0, 370, 415.3, 0,
]

NOTES = ['A', 'A#', 'B', 'C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#']

# https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf#page=186
_GPIO_FUNCn_OUT_SEL_CFG_REG = const(0x3F404554) #  (n: 0-53) (0x3F404000+0x0554+4*n)
_GPIO_FUNCn_OUT_INV_SEL = const(1 << 9)


class RTTTL:

    def __init__(self, tune):
        tune_pieces = tune.split(':')
        if len(tune_pieces) != 3:
            raise ValueError('tune should contain exactly 2 colons')
        self.tune = tune_pieces[2]
        self.tune_idx = 0
        self.parse_defaults(tune_pieces[1])

    def parse_defaults(self, defaults):
        val = 0
        ch = ' '
        for char in defaults:
            char = char.lower()
            if char.isdigit():
                val *= 10
                val += ord(char) - ord('0')
                if ch == 'o':
                    self.default_octave = val
                elif ch == 'd':
                    self.default_duration = val
                elif ch == 'b':
                    self.bpm = val
            elif char.isalpha():
                ch = char
                val = 0
        self.msec_per_whole_note = 240000.0 / self.bpm

    def next_char(self):
        if self.tune_idx < len(self.tune):
            char = self.tune[self.tune_idx]
            self.tune_idx += 1
            if char == ',':
                char = ' '
            return char
        return '|'

    def notes(self):
        while True:
            char = self.next_char()
            while char == ' ':
                char = self.next_char()

            duration = 0
            while char.isdigit():
                duration *= 10
                duration += ord(char) - ord('0')
                char = self.next_char()
            if duration == 0:
                duration = self.default_duration

            if char == '|':
                return

            note = char.lower()
            if note >= 'a' and note <= 'g':
                note_idx = ord(note) - ord('a')
            elif note == 'h':
                note_idx = 1
            else:
                note_idx = 7
            char = self.next_char()

            if char == '#':
                note_idx += 8
                char = self.next_char()

            duration_multiplier = 1.0
            if char == '.':
                duration_multiplier = 1.5
                char = self.next_char()

            if char >= '4' and char <= '7':
                octave = ord(char) - ord('0')
                char = self.next_char()
            else:
                octave = self.default_octave

            if char == '.':
                duration_multiplier = 1.5
                char = self.next_char()

            freq = NOTE[note_idx] * (1 << (octave - 4))
            msec = (self.msec_per_whole_note / duration) * duration_multiplier

            yield freq, msec


class Buzzer:
    
    _NOTE_RATIO = 0.9

    def __init__(self, a_pin, b_pin=None):
        # create a PWM for the given a_pin at 50% duty-cycle
        self._pwm = PWM(a_pin, duty_u16=0)
        self._note_timer = Timer(-1)
        self._beep_timer = Timer(-2) # if -1 no callbacks!
        self._lock = allocate_lock()
        self._notes = []

        # if b_pin is given, output an inverted PWM signal to it!
        # louder buzzer output compared to connecting b_pin to GND.
        if b_pin is not None:
            if platform == 'esp32':
                pn = lambda x: int(str(x)[4:-1]) # get pin number from Pin object
                # ESP32 GPIO matrix register magic!
                mem32[_GPIO_FUNCn_OUT_SEL_CFG_REG + 4 * pn(b_pin)] =\
                mem32[_GPIO_FUNCn_OUT_SEL_CFG_REG + 4 * pn(a_pin)] |\
                _GPIO_FUNCn_OUT_INV_SEL
            else:
                # TODO: add support for RP2040, ESP8266...
                raise NotImplementedError('inverted PWM signal not implemented for this platform!')

    def is_beep(self):
        return bool(self._pwm.duty_u16())

    def off(self, _=None):
        self._pwm.duty_u16(0)

    def beep(self, freq, ms=0):
        ms = int(ms)
        if freq:
            self._pwm.freq(int(freq))
            self._pwm.duty_u16(0x8000)
            if ms < 0:
                self._beep_timer.init(period=-ms, mode=Timer.ONE_SHOT,
                                      callback=self.off)
        else:
            self.off()
        if ms > 0:
            sleep_ms(ms)
            self.off()

    def note(self, note, ms=0):
        n = NOTES.index(note[0:-1])

        if (n < 3):
            n += 12

        n += (int(note[-1]) - 1) * 12

        self.beep(440 * (2 ** (keyNumber- 48) / 12), ms)

    def _note_timer_cb(self, _):
        with self._lock:
            if self._notes:
                freq, ms = self._notes.pop(0)
                self.beep(freq, int(ms * -Buzzer._NOTE_RATIO)) # non-blocking beep
                self._note_timer.init(period=int(ms), mode=Timer.ONE_SHOT,
                                      callback=self._note_timer_cb)
            else:
                self.off()

    def play_rtttl(self, rtttl, blocking=True):
        notes = RTTTL(rtttl).notes()
        if blocking:
            for freq, ms in notes:
                on = ms * Buzzer._NOTE_RATIO
                self.beep(freq, int(on))
                sleep_ms(int(ms - on))
        else:
            with self._lock:
                self._notes.extend(notes)
                self._note_timer.init(period=0, mode=Timer.ONE_SHOT,
                                      callback=self._note_timer_cb)

    def play_rtttl_song(self, name, blocking=True):
        self.play_rtttl(get_rtttl_song_by_name(name), blocking)

    def status(self):
        with self._lock:
            if self._notes:
                return True
        return self.is_beep()

    def stop(self):
        with self._lock:
            return self._notes.clear()
        self.off()


if __name__ == '__main__':
    print('\nTest buzzer', __version__)

    from machine import Pin
    from time import sleep_ms
    buzzer = Buzzer(Pin(11), Pin(13))

    try:
        print('Beep boop')
        for i in range(6):
            buzzer.beep(880 if i % 2 else 440, 500)

        for s in get_rtttl_song_names():
            print(s)
            try:
                buzzer.play_rtttl_song(s)
                sleep_ms(500)
            except KeyboardInterrupt:
                pass
    except KeyboardInterrupt:
        pass

    buzzer.off()

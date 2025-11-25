'''
Created on May 13 2015

@author: Or Levi
'''
import interface
import pyaudio
import time
import numpy
import math
import threading
import queue
import wave
import config as config


FS = 44100
CHUNK = 1024
FREQ_SHIFT = 1  # frequency difference between strobe and sound output
MIN_VALUE_TO_ALLOW_CHANGE = 0.98
MAX_VOL_CHANGE = 0.001
SLEEP_TIME_BUFFER_UNDERRUN = 0.1  # [s] if for some reason player didnt have what to play, wait this amount of seconds for input to gather
BUFFER_QUEUE_SIZE = 10


class Player():
    """
    play sounds out to speakers output,
     supports:
     1. sine wave + second wave (for strobe)
     2. play music from wav file
     3. channel microphone input to speakers output
    """

    def __init__(self, interface):
        self.interface = interface
        self.player = pyaudio.PyAudio()
        self.last_val = 0
        self.vol = self.interface.vol
        self.freq = self.interface.freq

        self.chunk_queue = queue.Queue(maxsize=BUFFER_QUEUE_SIZE)
        self.stream = None

        self.is_playing = False
        self.wav_thread_player_on = False
        self.play_mic_input = False

    # -------------------------------------------------------------------------
    # Public API
    # -------------------------------------------------------------------------
    def play_stop_sine_wave(self):
        if self.is_playing:
            self.stop_sine_wave()
        else:
            self.play_sine_wave()

    def play_sine_wave(self):
        """Start continuous sine + strobe output."""
        self._close_all_players()
        self.is_playing = True
        self.stream = self.player.open(
            format=pyaudio.paFloat32,
            channels=2,
            rate=FS,
            output=True,
            frames_per_buffer=CHUNK,
        )
        creator_t = threading.Thread(target=self._create_chunk)
        creator_t.daemon = True
        creator_t.start()
        player_t = threading.Thread(target=self._play_chunk)
        player_t.daemon = True
        player_t.start()

    def stop_sine_wave(self):
        """Stop sine + strobe output."""
        self.is_playing = False
        time.sleep(0.1)
        # emptying the queue so next time we start without leftovers
        while not self.chunk_queue.empty():
            self.chunk_queue.get_nowait()
        try:
            if self.stream is not None:
                self.stream.stop_stream()
                self.stream.close()
            self.last_val = 0
        except Exception:
            pass

    def play_stop_wav_file(self):
        if self.wav_thread_player_on:
            self.stop_wav_file()
        else:
            self.play_wav_file()

    def play_wav_file(self):
        """Play the wav file defined in config.WAV_FILE."""
        self._close_all_players()
        wf = wave.open(config.WAV_FILE, "rb")
        wav_channels = wf.getnchannels()
        wav_rate = wf.getframerate()
        wav_width = wf.getsampwidth()
        self.stream = self.player.open(
            format=self.player.get_format_from_width(wav_width),
            channels=wav_channels,
            rate=wav_rate,
            output=True,
        )

        self.is_playing = True
        wav_t = threading.Thread(target=self._file_player_thread, args=(wf,))
        wav_t.daemon = True
        wav_t.start()

    def stop_wav_file(self):
        self.is_playing = False
        time.sleep(0.1)
        try:
            if self.stream is not None:
                self.stream.stop_stream()
                self.stream.close()
        except Exception:
            pass

    def play_stop_mic(self):
        if self.is_playing:
            self.stop_mic()
        else:
            self.play_mic()

    def play_mic(self):
        """Route microphone input to speakers (stereo)."""
        self._close_all_players()
        self.stream = self.player.open(
            format=pyaudio.paInt24,
            channels=2,
            rate=FS,
            output=True,
            frames_per_buffer=CHUNK,
        )
        self.is_playing = True

        creator_t = threading.Thread(target=self._mic_create_chunk)
        creator_t.daemon = True
        creator_t.start()
        player_t = threading.Thread(target=self._mic_play_chunk)
        player_t.daemon = True
        player_t.start()

    def stop_mic(self):
        self.is_playing = False
        time.sleep(0.1)
        try:
            if self.stream is not None:
                self.stream.stop_stream()
                self.stream.close()
        except Exception:
            pass
        # emptying the queue so next time we start without leftovers
        while not self.chunk_queue.empty():
            self.chunk_queue.get_nowait()

    def close_nicely(self):
        """Close all active players and PyAudio cleanly."""
        self._close_all_players()

    # -------------------------------------------------------------------------
    # Internal helpers
    # -------------------------------------------------------------------------
    def _mic_create_chunk(self):
        stream = self.player.open(
            format=pyaudio.paInt24,
            channels=2,
            rate=FS,
            input=True,
            frames_per_buffer=CHUNK,
        )
        while self.is_playing:
            if self.chunk_queue.qsize() < BUFFER_QUEUE_SIZE:
                # exception_on_overflow=False pour éviter les erreurs quand le buffer déborde
                chunk = stream.read(CHUNK, exception_on_overflow=False)
                self.chunk_queue.put_nowait(chunk)
        stream.stop_stream()
        stream.close()

    def _mic_play_chunk(self):
        while self.is_playing:
            if self.chunk_queue.qsize() > 0:
                a = self.chunk_queue.get_nowait()
                self.stream.write(a)
            else:
                print("buffer finished")
                time.sleep(SLEEP_TIME_BUFFER_UNDERRUN)

    def _play_chunk(self):
        while self.is_playing:
            if self.chunk_queue.qsize() > 0:
                a = self.chunk_queue.get_nowait()
                self.stream.write(a)
            else:
                print("buffer finished")
                time.sleep(SLEEP_TIME_BUFFER_UNDERRUN)

    def _create_chunk(self):
        """Crée des blocs de CHUNK échantillons (sine + strobe) et les pousse dans la queue."""
        while self.is_playing:
            if self.chunk_queue.qsize() < BUFFER_QUEUE_SIZE:
                chunk = self._create_next_sine_wave_signal()
                # conversion vers bytes pour PyAudio
                chunk = chunk.astype(numpy.float32).tobytes()
                self.chunk_queue.put_nowait(chunk)

    def _close_all_players(self):
        """Ensure all players are off (sine, mic, wav file)."""
        try:
            print("ensuring sine player is off")
            self.stop_sine_wave()
        except Exception:
            pass

        try:
            print("ensuring mic player is off")
            self.stop_mic()
        except Exception:
            pass

        try:
            print("ensuring file player is off")
            self.stop_wav_file()
        except Exception:
            pass

    def _file_player_thread(self, wf):
        self.wav_thread_player_on = True
        d = wf.readframes(CHUNK)
        while d and self.is_playing:
            self.stream.write(d)
            d = wf.readframes(CHUNK)
        self.wav_thread_player_on = False
        wf.close()

    def _create_next_sine_wave_signal(self):
        """
        Génère un bloc de CHUNK échantillons :
        - canal 0 : sinus audio (fréquence self.freq, amplitude self.vol)
        - canal 1 : signal stroboscopique carré basé sur (freq + FREQ_SHIFT)
        """
        n = self.last_val
        output_wave = []

        for _ in range(CHUNK):
            n += 1

            # adaptation de la fréquence quand l'interface change
            if (math.fabs(self.freq - self.interface.freq) >= 0.1 and
                    math.fabs(math.sin(2 * math.pi * self.freq * n / FS)) > MIN_VALUE_TO_ALLOW_CHANGE):
                print("freq change , self- {} , interface- {}".format(self.freq, self.interface.freq))
                n = int(n * self.freq / self.interface.freq)
                self.freq = self.interface.freq

            # ramping du volume
            if self.vol != self.interface.vol:
                diff = self.interface.vol - self.vol
                if diff > MAX_VOL_CHANGE:
                    self.vol += MAX_VOL_CHANGE
                elif diff < -MAX_VOL_CHANGE:
                    self.vol -= MAX_VOL_CHANGE
                else:
                    self.vol = self.interface.vol

            # sinus audio
            audio_sine = self.vol * math.sin(2 * math.pi * self.freq * n / FS)

            # --- strobe : square dérivate by sinus with frequence (freq + FREQ_SHIFT) ---
            phase = 2 * math.pi * (self.freq + FREQ_SHIFT) * n / FS
            s_val = math.sin(phase)

            if 0 <= s_val <= 0.5:
                strobe_square = 0.5
            else:
                strobe_square = 0

            output_wave.append([audio_sine, strobe_square])

        self.last_val = n
        return numpy.array(output_wave)


if __name__ == '__main__':
    import sampler

    i = interface.Interface(frequency=760)
    s = sampler.Sampler()
    p = Player(interface=i)

    # petit test micro -> HP, comme dans ta version 2.7
    p.play_mic()
    time.sleep(20)
    p.stop_mic()
    p.close_nicely()
    s.close_pyaudio_nicely()
    time.sleep(1)

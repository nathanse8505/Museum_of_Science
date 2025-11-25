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
SLEEP_TIME_BUFFER_UNDERRUN = 0.1  # [S] if for some reason player didnt have what to play, wait this amount of secounds for input to gather
BUFFER_QUEUE_SIZE = 10


class Player():
    """
    play sounds out to speakers output,
     supports:
     1. sine wave + second sine wave (for strobe)
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

        #self.keep_playing_file = True
        self.wav_thread_player_on = False

        self.play_mic_input = False

    def play_stop_sine_wave(self):
        if self.is_playing:
            self.stop_sine_wave()
        else:
            self.play_sine_wave()

    def play_sine_wave(self):
        self._close_all_players()
        self.is_playing = True
        self.stream = self.player.open(format=pyaudio.paFloat32, channels=2, rate=FS, output=True, frames_per_buffer=CHUNK)
        creator_t = threading.Thread(target=self._create_chunk)
        creator_t.daemon = True
        creator_t.start()
        player_t = threading.Thread(target=self._play_chunk)
        player_t.daemon = True
        player_t.start()

    def stop_sine_wave(self):
        self.is_playing = False
        time.sleep(0.1)
        while not self.chunk_queue.empty():  # emptying the queue so next time we start without leftovers
            self.chunk_queue.get_nowait()
        try:
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
        #print 'in play_wav_file'
        self._close_all_players()
        #self.stopWave()
        wf = wave.open(config.WAV_FILE)
        wav_channels = wf.getnchannels()
        wav_rate = wf.getframerate()
        wav_width = wf.getsampwidth()
        self.stream = self.player.open(format=self.player.get_format_from_width(wav_width),
                                       channels=wav_channels,
                                       rate=wav_rate,
                                       output=True)

        #self.keep_playing_file = True
        self.is_playing = True
        wav_t = threading.Thread(target=self._file_player_thread, args=(wf,))
        wav_t.start()

    def stop_wav_file(self):
        #print 'in stop_wav_file_play'
        self.is_playing = False
        time.sleep(0.1)
        self.stream.stop_stream()
        self.stream.close()

    def play_stop_mic(self):
        if self.is_playing:
            self.stop_mic()
        else:
            self.play_mic()

    def play_mic(self):

        self._close_all_players()
        self.stream = self.player.open(format=pyaudio.paInt24, channels=2, rate=FS, output=True, frames_per_buffer=CHUNK)
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
        self.stream.stop_stream()
        self.stream.close()
        while not self.chunk_queue.empty():    #emptying the queue so next time we start without leftovers
            self.chunk_queue.get_nowait()

    def close_nicely(self):
        self._close_all_players()

    def _mic_create_chunk(self):
        stream = self.player.open(format=pyaudio.paInt24, channels=2, rate=FS, input=True, frames_per_buffer=CHUNK)
        while self.is_playing:
            if self.chunk_queue.qsize() < BUFFER_QUEUE_SIZE:
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
                print('buffer finished')
                time.sleep(SLEEP_TIME_BUFFER_UNDERRUN)

    def _play_chunk(self):
        while self.is_playing:
            if self.chunk_queue.qsize() > 0:
                a = self.chunk_queue.get_nowait()
                self.stream.write(a)
            else:
                print('buffer finished')
                time.sleep(SLEEP_TIME_BUFFER_UNDERRUN)

    def _create_chunk(self):
        while self.is_playing:
            if self.chunk_queue.qsize() < BUFFER_QUEUE_SIZE:
                chunk = self._create_next_sine_wave_signal()
                chunk = chunk.astype(numpy.float32).tobytes()
                self.chunk_queue.put_nowait(chunk)

    def _close_all_players(self):
        # ensure all players are closed wave, file, mic
        # sfhould be called in the play of every player
        try:
            print('ensuring sine player is off')
            self.stop_sine_wave()
        except Exception:
            pass

        try:
            print('ensuring mic player is off')
            self.stop_mic()
        except Exception:
            pass

        try:
            print('ensuring file player is off')
            self.stop_wav_file()
        except Exception:
            pass

    def _file_player_thread(self, wf):
        self.wav_thread_player_on = True
        d = wf.readframes(CHUNK)
        while d and self.is_playing:
            self.stream.write(d)
            d = wf.readframes(CHUNK)
        #self.stop_wav_file_play()
        self.wav_thread_player_on = False

    def _create_next_sine_wave_signal(self):
        t = self.last_val
        output_wave = []
        for i in range(CHUNK):
            t = t + 1

            if (math.fabs(self.freq - self.interface.freq) >= 0.1) and (math.fabs(math.sin(2 * math.pi * self.freq * t / FS)) > MIN_VALUE_TO_ALLOW_CHANGE):
                print("freq change , self- " + str(self.freq) + " , interface- " + str(self.interface.freq))
                t = int(t * self.freq / self.interface.freq)
                self.freq = self.interface.freq

            if (self.vol != self.interface.vol):
                if (self.interface.vol - self.vol) > MAX_VOL_CHANGE:
                    self.vol = self.vol + MAX_VOL_CHANGE
                elif (self.interface.vol - self.vol) < -MAX_VOL_CHANGE:
                    self.vol = self.vol - MAX_VOL_CHANGE
                else:
                    self.vol = self.interface.vol

            audio_sine = self.vol * math.sin(2 * math.pi * self.freq * t / FS)
            #strobe_sine = 0.5 * math.sin(2 * math.pi * (self.freq + FREQ_SHIFT) * t / FS)
            #output_wave.append([audio_sine, strobe_sine])
            phase = 2 * math.pi * (self.freq + FREQ_SHIFT) * t / FS
            if math.sin(phase) >= 0 and math.sin(phase) <= 0.5:
                strobe_square = 0.5
            else:
                strobe_square = -0.5

            output_wave.append([audio_sine, strobe_square])
            

        self.last_val = t
        return numpy.array(output_wave)


if __name__ == '__main__':

    import sampler
    i = interface.Interface(frequency=760)
    s = sampler.Sampler()
    p = Player(interface=i)

    p.play_mic()
    time.sleep(20)
    p.stop_mic()
    p.close_nicely()
    s.close_pyaudio_nicely()
    time.sleep(1)

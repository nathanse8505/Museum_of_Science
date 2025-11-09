__author__ = 'nfreiman'
import pyaudio
import numpy
import time
from scipy import signal

CHUNK = 1024
FS = 44100
CHUNKS_TO_RECORD = 220

class ImpulseGenerator(object):
    def __init__(self):
        self.player = pyaudio.PyAudio()
        self.outStream = self.player.open(format=pyaudio.paFloat32, channels=2, rate=FS, output=True, frames_per_buffer=CHUNK)
        self.inStream = self.player.open(format=pyaudio.paInt16, channels=1, rate=FS, input=True, frames_per_buffer=CHUNK)
        self.impulse = signal.gaussian(44100, 4000)*5  #numpy.array([1]*4410)
        self.audio = numpy.empty((CHUNKS_TO_RECORD * CHUNK), dtype=numpy.int16)

    def play_impulse(self):
        chunk = self.impulse.astype(numpy.float32).tostring()
        self.outStream.write(chunk)
        self.outStream.stop_stream()
        self.outStream.close()

    def record(self):
        for i in range(CHUNKS_TO_RECORD):
            audio_string = self.inStream.read(CHUNK)
            self.audio[i * CHUNK: (i+1) * CHUNK] = numpy.fromstring(audio_string, dtype=numpy.int16)
        self.inStream.stop_stream()
        self.inStream.close()

    def close_pyaudio(self):
        self.player.terminate()
       
    def break_glass(self):
        self.play_impulse()
        time.sleep(0.5)
        self.record()
        time.sleep(0.01)
        self.close_pyaudio()


if __name__ == '__main__':
    a = ImpulseGenerator()
    a.play_impulse()
    a.record()
    a.close_pyaudio()
    print a.audio.tolist()

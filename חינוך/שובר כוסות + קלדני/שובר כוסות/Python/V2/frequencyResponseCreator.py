__author__ = 'netanel'

import pyaudio
import numpy
import time
import math
import threading

CHUNK = 1024
FS = 44100
CHUNKS_TO_RECORD = 220

'''
class FrequencyResponseCreator(object):

    def __init__(self):
        pass
'''

class FrequencySampler(object):
    def __init__(self, signal_length):
        """
        :param signal_length: signal length is the amount of CHUNK's we want to play/ record
        :return:
        """
        self.player = pyaudio.PyAudio()
        self.outStream = self.player.open(format=pyaudio.paFloat32, channels=2, rate=FS, output=True, frames_per_buffer=CHUNK)
        self.inStream = self.player.open(format=pyaudio.paInt16, channels=1, rate=FS, input=True, frames_per_buffer=CHUNK)

        self.signal_length = signal_length
        self.vol = 0.1
        self.sound_signal = None
        self.sound_raw_signal = []
        #self.recorded_response = numpy.empty(CHUNK * self.signal_length, dtype=numpy.int16)
        self.raw_recorded_data = 'b'

    def _create_signal(self, freq):
        self.sound_signal = []
        self.sound_raw_signal = []
        for i in range(CHUNK * self.signal_length):
            audio_sine = self.vol * math.sin(2 * math.pi * freq * i / FS)
            self.sound_signal.append(audio_sine)
        self.sound_signal = numpy.array(self.sound_signal)

        for i in range(self.signal_length):
            chunk = self.sound_signal[i * CHUNK: (i+1) * CHUNK].astype(numpy.float32).tostring()
            self.sound_raw_signal.append(chunk)

    def start_play_record(self, freq):
        self.raw_recorded_data = b''  # on vide le buffer avant un nouveau record
        self._create_signal(freq=freq)
        recorder_thread = threading.Thread(target=self.record)
        player_thread = threading.Thread(target=self.play)
        recorder_thread.start()
        time.sleep(0.1)
        player_thread.start()
        print('threads started')
        recorder_thread.join()
        player_thread.join()

    def record(self):
        for i in range(self.signal_length):
            audio_string = self.inStream.read(CHUNK)
            #self.recorded_response[i * CHUNK: (i+1) * CHUNK] = numpy.fromstring(audio_string, dtype=numpy.int16)
            self.raw_recorded_data += audio_string
        print ('{}, recorder ended'.format(time.time()))

    def play(self):
        for i in range(self.signal_length):
            #chunk = self.sound_signal[i * CHUNK: (i+1) * CHUNK].astype(numpy.float32).tostring()
            self.outStream.write(self.sound_raw_signal[i])
        print ('{}, player ended'.format(time.time()))

    def get_recorded_response(self):
        data = numpy.frombuffer(self.raw_recorded_data, dtype=numpy.int16)
        return data.tolist()


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    frequencies = range(800, 1000, 5)
    responses = []
    OFS = FrequencySampler(signal_length=100)
    for freq in frequencies:
        OFS.start_play_record(freq=freq)
        responses.append(OFS.get_recorded_response())
        time.sleep(0.5)

    for i in range(len(responses)):
        plt.plot(responses[i])
    plt.show()
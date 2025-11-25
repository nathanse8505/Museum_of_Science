"""
parts of the code were taken from http://www.swharden.com/blog/2013-05-09-realtime-fft-audio-visualization-with-python/

"""
__author__ = 'netanel'

import numpy
import pyaudio
import threading
import time
import logging
import config as config

################# Global Consts
BITRATE = 44100
PYAUDIO_FORMAT = pyaudio.paInt16
NUMPY_DATA_FORMAT = numpy.int16
PYAUDIO_BUFFER_SIZE = 1024
TIME_TO_RECORD = 0.26  # time to record before calculating fft


class Sampler(object):
    def __init__(self):
        self._stop_recording_thread = False  # flag to kill recorder and fft computer threads
        self._new_audio = False  # flag to inform about new audio data from recorder thread

        self._bitrate = BITRATE
        self._buffer_size = PYAUDIO_BUFFER_SIZE
        self._sec_to_record = TIME_TO_RECORD

        self._audio = None  # hold recorded audio

        self._already_started_recording = False

        self._first = True
        logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)
        logging.debug("finished init")
        self._init_recorder()
        self._full_audio_sample = numpy.empty([0, 0], dtype=numpy.int16)

    def start_microphone_sampling(self):
        """
        start to probe microphone for microphone_sampling_time time and compute FFT
        :return:
        """
        # open recorder thread
        recorder_thread = threading.Thread(target=self._record)
        recorder_thread.start()

    def close_pyaudio_nicely(self):
        """
        close streams and pyaudio object
        :return:
        """
        logging.debug("closing PyAudio nicely")
        self._stop_recording_thread = True
        time.sleep(0.3)
        self._inStream.stop_stream()
        self._inStream.close()
        self._p.terminate()

    def _init_recorder(self):
        """
        initialize all kind of buffers
        open stream to microphone
        calculate Frequency Axis of FFT
        :return:
        """
        self._buffers_to_record = max(1, int(self._bitrate * self._sec_to_record / self._buffer_size))  # how many full pyaudio buffers in time*bitrate
        self._samples_to_record = int(self._buffer_size * self._buffers_to_record)
        self._chunks_to_record = int(self._samples_to_record / self._buffer_size)
        self._sec_per_point = 1.0 / self._bitrate

        self._p = pyaudio.PyAudio()
        logging.debug("opening audio stream")

        self._inStream = self._p.open(format=PYAUDIO_FORMAT, channels=1, rate=self._bitrate, input=True, frames_per_buffer=self._buffer_size)
        self._xs_buffer = numpy.arange(self._buffer_size) * self._sec_per_point
        self._xs = numpy.arange(self._chunks_to_record * self._buffer_size) * self._sec_per_point
        self._audio = numpy.empty((self._chunks_to_record * self._buffer_size), dtype=NUMPY_DATA_FORMAT)

    def _record(self):
        """
        record secToRecord seconds of audio.
        save data in self._audio
        when new data was written, raise flag self._new_audio, do not read new data until flag is down
        """

        logging.debug("started recorder thread")

        while not self._stop_recording_thread:
            self._get_audio()
        logging.debug("ended recorder thread")

    def _get_audio(self):
        """
        get a single buffer size worth of audio.
        """
        audio_string = self._inStream.read(self._buffer_size)
        self._full_audio_sample = numpy.append(self._full_audio_sample, numpy.fromstring(audio_string, dtype=NUMPY_DATA_FORMAT))


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    plt.clf()
    r = Sampler()
    r.start_microphone_sampling()
    time.sleep(5)
    r._stop_recording_thread = True
    signal = r._full_audio_sample

    ys = numpy.fft.fftshift(numpy.fft.fft(signal))
    mid = len(ys) // 2  # division entière
    ys = abs(ys[mid:])  # on garde la moitié positive
    #ys = abs(ys[len(ys)/2:])
    xs = numpy.arange(len(ys))
    xs = xs * (44100 / (2.0 * len(ys)))

    plt.plot(xs, ys)
    '''

    start = 0
    end = len(signal)/10
    res = []
    for i in range(10):
        s = signal[start:end]
        ys = numpy.fft.fftshift(numpy.fft.fft(s))
        ys = abs(ys[len(ys)/2:])
        xs = numpy.arange(len(ys)) * 44100 / (2.0 * len(ys))
        res.append([xs, ys])
        start = end
        end = end + len(signal)/10

    for i in range(10):
        plt.plot(res[i][0], res[i][1])
    '''
    #plt.legend(range(10))
    plt.show()
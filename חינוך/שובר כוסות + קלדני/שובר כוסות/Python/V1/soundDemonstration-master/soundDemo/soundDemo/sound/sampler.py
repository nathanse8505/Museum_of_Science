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
PYAUDIO_FORMAT = pyaudio.paInt16  # make sure pyaudio and numpy formats are the same
NUMPY_DATA_FORMAT = numpy.int16
PYAUDIO_BUFFER_SIZE = 1024
TIME_TO_RECORD = 0.26  # time to record before calculating fft


class Sampler(object):
    def __init__(self, microphone_sampling_time=60*60):
        self._peakFFT = ([0, 0], [0, 0])
        self._stop_recording_thread = False  # flag to kill recorder and fft computer threads
        self._new_audio = False  # flag to inform about new audio data from recorder thread
        self._new_fft = False
        self._peak_waveform = None

        self._bitrate = BITRATE
        self._buffer_size = PYAUDIO_BUFFER_SIZE
        self._sec_to_record = TIME_TO_RECORD

        self._log_scale = False  # should FFT be represented in dB
        self._fft_data = None  # hold computed fft

        self._fft_frequencies = None
        self._audio = None  # hold recorded audio

        self._already_started_recording = False
        self._zero_padding_factor = 5  # number of sample length zeros to add (if we sampled 100 audio points add 100 * self._zero_padding_factor zeros)
        self._begin_freq_bin = None
        self._end_frequency_bin = None

        self._fft_compute = True

        self._first = True
        logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)
        logging.debug("finished init")
        self._init_recorder()
        self.reset_max_fft()

    def has_new_fft(self):
        """
        flag to ensure new FFT data is available
        :return:
        """
        return self._new_fft

    def get_fft_data(self):
        """
        return momentary FFT data
        :return: tuple of (FFT frequeny axis, values)
        """
        self._new_fft = False
        return self._fft_frequencies, self._fft_data

    def get_peak_fft(self):
        """
        return strongest frequency (Hold On)
        :return: a tuple of (frequency, value)
        """
        return self._peakFFT

    def reset_max_fft(self):
        """
        reset current max fft data
        """
        self._peakFFT = ([0, 0], [0, 0])
        self._peak_waveform = numpy.zeros(len(self._fft_frequencies))

    def stop_start_FFT_computation(self):
        self._fft_compute = not self._fft_compute
        if self._fft_compute is True:
            self._stop_recording_thread = False
            self.start_microphone_sampling()
        else:
            self._stop_recording_thread = True

    def get_peak_waveform(self):
        """
        :return: the waveform that had the highest peak from the beginning of sampling or from the last time
        reset_max_fft was called
        """
        return self._fft_frequencies, self._peak_waveform

    def start_microphone_sampling(self):
        """
        start to probe microphone and computing FFT on the samples
        """
        # open recorder thread
        recorder_thread = threading.Thread(target=self._record)
        recorder_thread.daemon = True
        recorder_thread.start()

        # open fft computer thread
        fft_thread = threading.Thread(target=self._fft_computer)
        fft_thread.daemon = True
        fft_thread.start()

    def close_pyaudio_nicely(self):
        """
        close streams and pyaudio object
        """
        logging.debug("closing PyAudio nicely")
        self._stop_recording_thread = True
        time.sleep(0.3)
        self._inStream.stop_stream()
        self._inStream.close()
        self._p.terminate()

    def change_frequency_range(self, min=None, max=None):
        """
        change the frequency range that  get_fft_data() and get_peak_waveform() will return
        min and max frequency would not be exact, closest frequency in fft will be chosen
        :param min: min frequency
        :param max: max frequency
        :return:
        """
        self._fft_frequencies = numpy.arange(self._chunks_to_record * self._buffer_size / 2.0) * self._bitrate / (self._chunks_to_record * self._buffer_size * (self._zero_padding_factor + 1.0))  # hold FFT frequncy axis values
        logging.debug("in change_frequency_range, before:")
        logging.debug("# FFT bins: {}".format(len(self._fft_frequencies)))
        logging.debug("bins: {}".format(self._fft_frequencies))
        logging.debug("frequency resolution: {}".format(self._fft_frequencies[1] - self._fft_frequencies[0]))

        if min is not None and max is not None:
            self._begin_freq_bin = numpy.searchsorted(self._fft_frequencies, min)
            self._end_frequency_bin = numpy.searchsorted(self._fft_frequencies, max)
            self._fft_frequencies = self._fft_frequencies[self._begin_freq_bin:self._end_frequency_bin]
        else:
            self._begin_freq_bin = 0
            self._end_frequency_bin = len(self._fft_frequencies)

        logging.debug("in change_frequency_range, after:")
        logging.debug("# FFT bins: {}".format(len(self._fft_frequencies)))
        logging.debug("bins: {}".format(self._fft_frequencies))
        logging.debug("frequency resolution: {}".format(self._fft_frequencies[1] - self._fft_frequencies[0]))

    def _init_recorder(self):
        """
        initialize all kind of buffers
        open stream to microphone
        calculate Frequency Axis of FFT
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
        self.change_frequency_range(min=config.MIN_USED_FREQUENCY, max=config.MAX_USED_FREQUENCY)

    def _record(self):
        """
        record secToRecord seconds of audio.
        save data in self._audio
        when new data was written, raise flag self._new_audio, do not read new data until flag is down
        """
        logging.debug("started recorder thread")
        while not self._stop_recording_thread:
            if self._fft_compute:
                if not self._new_audio:
                    for i in range(self._chunks_to_record):
                        self._audio[i * self._buffer_size: (i+1) * self._buffer_size] = self._get_audio()
                    #logging.debug("read new audio data, {}".format(time.perf_counter()))
                    self._new_audio = True

                time.sleep(0.01)
        logging.debug("ended recorder thread")

    def _get_audio(self):
        """
        get a single buffer size worth of audio.
        """
        audio_string = self._inStream.read(self._buffer_size, exception_on_overflow=False)
        return numpy.frombuffer(audio_string, dtype=NUMPY_DATA_FORMAT)

    def _fft_computer(self):
        """
        compute fft on self._audio
        store new fft in self._current_fft
        update self._new_fft
        :return:
        """
        logging.debug("started fft thread")
        while not self._stop_recording_thread:
            if self._fft_compute:
                if self._new_audio:
                    self._fft()
                    self._new_audio = False
                    #logging.debug("wrote new fft data")
        logging.debug("ended fft thread")

    def _fft(self, log_scale=True):
        """
        compute fft on self._audio
        :param log_scale: if True, update FFT data in dB
        :return:
        """
        data = self._audio
        if self._first:
            logging.debug("data length without padding {}".format(len(data)))
        data = numpy.concatenate((data, numpy.zeros([self._zero_padding_factor * len(data)])), 0)
        if self._first:
            logging.debug("data length with padding {}".format(len(data)))
            self._first = False
        ys = numpy.fft.fftshift(numpy.fft.fft(data))
        ys = abs((ys[len(ys)//2:]))
        ys = ys[self._begin_freq_bin:self._end_frequency_bin]
        if log_scale:
            ys = 20 * numpy.log10(ys)

        self._find_maximas(ys)
        # check if we have a value greater than self._peakFFT
        self._fft_data = ys
        self._new_fft = True

    def _find_maximas(self, fft_values):
        """
        find the two highest valus and there place in the given fft_values.
        1. find first maximum value
        2. search for next one but make sure it is not to close to the first one
            (controlled by DELTA_FREQ_FOR_MAXIMA parameter in config)
        :param fft_values: fft_values array to search in
        """
        # find firs maxima
        max_p = fft_values.argmax()
        max_val = fft_values[max_p]

        if max_val > self._peakFFT[1][0]:
            # calculate new vector, exclude the area of the old maxima
            fft_resolution = self._fft_frequencies[1] - self._fft_frequencies[0]
            delta_bins = int(config.DELTA_FREQ_FOR_MAXIMA / fft_resolution)

            if max_p - delta_bins >= 0:
                d1 = delta_bins
                min_idx = max_p - delta_bins
            else:
                d1 = max_p
                min_idx = 0

            if max_p + delta_bins < len(fft_values):
                d2 = delta_bins
                max_idx = max_p + delta_bins
            else:
                d2 = len(fft_values) - max_p - 1
                max_idx = len(fft_values) - 1

            excluded_fft_values = numpy.concatenate((fft_values[:min_idx], numpy.zeros(d1 + d2), fft_values[max_idx:]))
            max_p2 = excluded_fft_values.argmax()
            max_val2 = excluded_fft_values[max_p2]

            self._peak_waveform = fft_values
            self._peakFFT = ([self._fft_frequencies[max_p], self._fft_frequencies[max_p2]], [max_val, max_val2])


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    plt.ion()
    fig, ax = plt.subplots()
    data, = ax.plot([], [], '.')
    data2, = ax.plot([], [], 'o')
    data3, = ax.plot([], [], '--')
    #ax.set_autoscaley_on(True)
    ax.grid()
    ax.set_ylim([0, 300])
    ax.set_xlim([300, 1050])
    ax.set_title('momentary FFT')
    ax.set_xlabel('frequency [Khz]')
    ax.set_ylabel('power [Log]')

    # create Sampler object
    s = Sampler(microphone_sampling_time=60)
    s.change_frequency_range(340, 1000)
    # when wanted, call start_microphone_sampling()
    s.start_microphone_sampling()
    first = True
    t0 = tc = time.perf_counter()
    while tc - t0 < 15:
        tc = time.perf_counter()
        if s.has_new_fft():  # check if new FFT data is ready
            try:
                x, y = s.get_fft_data()  # get new FFT data
                if first:
                    logging.debug('data size, x: {}, y: {}, '.format(len(x), len(y)))
                    first = False
                a, b = s.get_peak_fft()
                print('max fft20 {},{}'.format(a, b))
                x1, y1 = s.get_peak_waveform()
                if abs(a[0] - a[1]) < config.DELTA_FREQ_FOR_MAXIMA:
                    print('smaller: {},{}'.format(a[0], a[1]))
                data.set_xdata(x)
                data.set_ydata(y)
                data2.set_xdata(a)
                data2.set_ydata(b)
                data3.set_xdata(x1)
                data3.set_ydata(y1)


                ax.relim()
                ax.autoscale_view()
                fig.canvas.draw()
                fig.canvas.flush_events()
                s.reset_max_fft()
            except Exception:
                pass
        fig.canvas.draw()
        fig.canvas.flush_events()
        time.sleep(0.05)

    s.close_pyaudio_nicely()



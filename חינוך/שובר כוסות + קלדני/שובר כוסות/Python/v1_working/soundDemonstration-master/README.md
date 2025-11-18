# soundDemonstration

####installation procedure:

1. install python 2.7.9, https://www.python.org/downloads/release/python-279/
2. install pyaudio, https://people.csail.mit.edu/hubert/pyaudio/packages/pyaudio-0.2.8.py27.exe
3. install pygame, http://pygame.org/ftp/pygame-1.9.1.win32-py2.7.msi
4. install numpy, http://sourceforge.net/projects/numpy/files/NumPy/1.9.2/numpy-1.9.2-win32-superpack-python2.7.exe/download

######installation notes:

1. when installing python be sure to click on 'Add python.exe to Path'

####configuration

1. all configuration are done in file config.py


---------- code guidance

######gui.py
    - gui module, define all buttons and some logic

######player.py
    - module that helps play sounds, supports sine waves, wav files and microphone to speakers channeling.
    - controls the stroboscope sine wave (with a frequency shift)

######sampler.py
    - samples microphone input and calculates FFT on the input

######interface.py
    - holds dome logic and connections mainly between gui and player

######__init__.py
    - start the gui program

######config.py
    - user configuration

######offlineRecorder.py
    - experimental sampler, record a preconfigured time from the microphone and calculate the FFT offline

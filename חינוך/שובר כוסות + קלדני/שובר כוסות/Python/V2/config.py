
#### FFT sampler configurations ####
# # Fenêtre locale (zoom)
WINDOW_SIZE = 400   # largeur totale (par défaut 200 Hz)
MIN_USED_FREQUENCY = 200
MAX_USED_FREQUENCY = 1500
DELTA_FREQ_FOR_MAXIMA = 7  # minimal allowed distance (in Hz) of the second maxima from the first one

VOLUME_MAXIMUM = 1          # maximal volume (when the volume bar is dragged to its end), should be value between 0 to 1, enough to break the glass, but not to violate the current/power limitations
VOLUME_DEFAULT = 0.05       # fraction of the VOLUME_MAXIMUM that will be used when the volume bar is untouched, should be enough to show the glass movement but not break it 
KEYBOARD_VOLUME_JUMP = 0.05 # fraction of the VOLUME_MAXIMUM that will be added by keyboard up/down arrow press
KEYBOARD_VOLUME_TIMEOUT = 3 # time in seconds between leaving the volume keyboard key and until the volume drops to its default

#### Chladni Plate fixed data [frquency, volume] ####
CHLADNI_FIXED_1 = [350, 0.1]
CHLADNI_FIXED_2 = [450, 0.15]
CHLADNI_FIXED_3 = [550, 0.2]
CHLADNI_FIXED_4 = [650, 0.25]
CHLADNI_FIXED_5 = [750, 0.3]
CHLADNI_FIXED_6 = [850, 0.35]

#### Ruben's Tube fixed data [frquency, volume] ####
TUBE_FIXED_1 = [550, 0.4]
TUBE_FIXED_2 = [650, 0.45]
TUBE_FIXED_3 = [750, 0.5]

#### Sweeper's parameters
SWEEP_FREQ_STEP_SIZE = 0.1  # in Hz
SWEEP_TIME_INTERVAL = 200   # in ms
SWEEP_NUMBER_OF_STEPS = 10  # For each direction

WAV_FILE = 'simsg813.wav'



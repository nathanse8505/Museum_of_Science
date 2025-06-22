"""
File: main.py
Purpose: Main file for the Air Pressure UI
"""

import time
from display import *
from arduino import *
from logs import *
from detect_drop import *


def main():
    """
    The main function for the Air Pressure UI
    """
    # pygame setup
    pygame.display.set_caption("Air Pressure")
    screen = pygame.display.set_mode(VIEW_PORT, pygame.FULLSCREEN)
    #
    #screen = pygame.display.set_mode(VIEW_PORT)
    clock = pygame.time.Clock()  # for fps limit

    # initial values for the UI
    language = HEBREW
    previous_language = HEBREW
    check_pressure_value = True
    pressure_value = MIN_PRESSURE_VALUE
    state = MEASURE
    drop_detector = DropDetector()

    # logging setup - log into a file called log.txt in the folder /logs with the format: [TIME] - [MESSAGE], if the file exists, append to it, if not, create it, if it exceeds 1MB, create a new file with a number suffix (before the .txt) and continue logging to it (e.g., log1.txt, log2.txt, etc.)
    logger = get_logger()
    logger.info("Starting Air Pressure UI")

    # arduino setup
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
    time.sleep(1)  # wait for the arduino to reset
    last_time_tried_to_connect = time.time()  # for not trying to connect too often

    while True:

        events = pygame.event.get()
        for event in events:
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    pygame.quit()
                    exit()

                if event.key == pygame.K_SPACE:
                    language = (language + 1) % len(LANGUAGES)  # toggle language
                    
                if event.key == pygame.K_UP:
                    pressure_value = min(pressure_value + 5000, MAX_PRESSURE_VALUE)


                if event.key == pygame.K_DOWN:
                    pressure_value = max(pressure_value - 5000, MIN_P_ATM)


                if event.key == pygame.K_RETURN:
                    state = (state + 1) % len(STATES)  # toggle state from OPENING to MEASURE

        data_from_arduino = read_line(ser, logger=logger)  # try to read from arduino
        if data_from_arduino == SERIAL_ERROR:  # if arduino WAS connected at start, but now failed to read:
            print("Arduino disconnected. Trying to reconnect to Arduino...")
            logger.info("Arduino disconnected. Trying to reconnect to Arduino...")

            ser = None
            language = HEBREW
            pressure_value = MIN_PRESSURE_VALUE
            state = MEASURE

        # if arduino was connecetd at start, but now failed to read, try to reconnect
        if not ser and time.time() - last_time_tried_to_connect > RECONNECT_INTERVAL:
            arduino_port = find_arduino_port(logger=logger)  # find the serial port
            ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
            last_time_tried_to_connect = time.time()  # update the last time tried to connect

        if data_from_arduino and data_from_arduino != SERIAL_ERROR:  # if data is vaild
            # print(data_from_arduino)
            pressure_value, sensor_analogread, language, error = parse_data(data_from_arduino, logger=logger)
            pressure_value = pressure_value * kPa_TO_Pa
            state = MEASURE if pressure_value >= SWITCH_TO_MEASURE_SCREEN_PRESSURE_THRESHOLD else OPENING  # if you got data, change the screen automatically based on the pressure value
            # print(f"parsed: pressure {pressure_value} sensor_analogread {sensor_analogread} language {language}")
            
            if not error:
                check_pressure_value, previous_language = log_air_pressure(logger,pressure_value,check_pressure_value, language, previous_language)
                #drop_detector.detect_drop(pressure_value, logger=logger)  # detect if there was a drop in voltage
                # if language != previous_language:
                #     logger.info(f"your language is: {dic_lang.get(language)}")
                #     previous_language = language

        screen.fill(BLACK)  # reset screen
        display_state(screen, state=state, language=language, pressure_value=pressure_value)  # render the screen
        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()
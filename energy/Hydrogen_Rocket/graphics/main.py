"""
File: main.py
Purpose: Main file for the Hydrogen Rocket UI
"""

import time
from display import *
from arduino import *
from logs import *


def main():
    """
    The main function for the Hydrogen Rocket UI
    """
    # pygame setup
    pygame.display.set_caption("Hydrogen Rocket")
    screen = pygame.display.set_mode(VIEW_PORT, pygame.FULLSCREEN)
    clock = pygame.time.Clock()  # for fps limit

    # initial values for the UI
    language = HEBREW
    previous_language = HEBREW
    charge = 0.0
    current = 0.0
    state = OPENING

    # logging setup - log into a file called log.txt in the folder /logs with the format: [TIME] - [MESSAGE], if the file exists, append to it, if not, create it, if it exceeds 1MB, create a new file with a number suffix (before the .txt) and continue logging to it (e.g., log1.txt, log2.txt, etc.)
    logger = get_logger()
    logger.info("Starting Hydrogen Rocket UI")

    # arduino setup
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
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
                    charge = min(charge + 1, MAX_CHARGE)

                if event.key == pygame.K_DOWN:
                    charge = max(charge - 1, MIN_CHARGE)

                if event.key == pygame.K_LEFT:
                    current = max(current - 0.2, MIN_CURRENT)

                if event.key == pygame.K_RIGHT:
                    current = min(current + 0.2, MAX_CURRENT)

                if event.key == pygame.K_RETURN:
                    state = (state + 1) % len(STATES)  # toggle state from OPENING to MEASURE

        data_from_arduino = read_line(ser, logger=logger)  # try to read from arduino
        if data_from_arduino == SERIAL_ERROR:  # if arduino WAS connected at start, but now failed to read:
            print("Arduino disconnected! Going to default settings")
            print("Trying to reconnect to Arduino...")
            logger.info("Arduino disconnected... Going to default settings")
            logger.info("Trying to reconnect to Arduino...")

            ser = None
            language = HEBREW
            charge = 0.0
            current = 0.0
            state = OPENING

        # if arduino was connecetd at start, but now failed to read, try to reconnect
        if not ser and time.time() - last_time_tried_to_connect > RECONNECT_INTERVAL:
            arduino_port = find_arduino_port(logger=logger)  # find the serial port
            ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
            last_time_tried_to_connect = time.time()  # update the last time tried to connect

        if data_from_arduino and data_from_arduino != SERIAL_ERROR:  # if data is vaild
            # print(data_from_arduino)
            current, charge, has_ignited, language, error = parse_data(data_from_arduino, logger=logger)
            # print(f"parsed: current {current} charge {charge} has_ignited {has_ignited} language {language}")
            if not error:
                if has_ignited:
                    logger.info(f"Rocket has ignited!")
                    print("Rocket has ignited!")
                if language != previous_language:
                    logger.info(f"your language is: {dic_lang.get(language)}")
                    print(f"your language is: {dic_lang.get(language)}")
                    previous_language = language

            state = MEASURE if current >= SWITCH_TO_MEASURE_SCREEN_CURRENT_THRESHOLD else OPENING  # if you got data, change the screen automatically based on the current value

        screen.fill(BLACK)  # reset screen
        display_state(screen, state=state, language=language, charge=charge, current=current)  # render the screen
        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()
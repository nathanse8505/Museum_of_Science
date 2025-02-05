"""
File: main.py
Purpose: Main file for the Jumping Ring UI
"""
import pygame
import time
from consts import *
from display import *
from arduino import *
from logs import *

class VoltageMonitor:
    def __init__(self, threshold=10):
        self.previous_voltage = 0
        self.threshold = threshold

    def detect_drop(self, voltage ,logger):
        #print(f"Voltage: {voltage}V")
        if self.previous_voltage > voltage + self.threshold and voltage<10:
            logger.info(f"Ring jumped!")
        self.previous_voltage = voltage


def main():
    """
    The main function for the Jumping Ring UI
    """
    # pygame setup
    pygame.display.set_caption("Jumping Ring")
    screen = pygame.display.set_mode(VIEW_PORT, pygame.FULLSCREEN)
    clock = pygame.time.Clock()  # for fps limit

    # initial values for the UI
    language = HEBREW
    voltage = MIN_VOLTAGE
    state = MEASURE

    # logging setup - log into a file called log.txt in the folder /logs with the format: [TIME] - [MESSAGE], if the file exists, append to it, if not, create it, if it exceeds 1MB, create a new file with a number suffix (before the .txt) and continue logging to it (e.g., log1.txt, log2.txt, etc.)
    logger = get_logger()
    logger.info("Starting Jumping Ring UI")

    # arduino setup
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
    time.sleep(1)
    last_time_tried_to_connect = time.time()  # for not trying to connect too often

    has_ignited = VoltageMonitor()


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
                    voltage = min(voltage + 5, MAX_VOLTAGE)

                if event.key == pygame.K_DOWN:
                    voltage = max(voltage - 5, MIN_VOLTAGE)


        data_from_arduino = read_line(ser, logger=logger)  # try to read from arduino
        if data_from_arduino == SERIAL_ERROR:  # if arduino WAS connected at start, but now failed to read:
            print("Arduino disconnected. Trying to reconnect to Arduino...")
            logger.info("Arduino disconnected. Trying to reconnect to Arduino...")

            ser = None
            language = HEBREW
            voltage = 0.0
            state = MEASURE

        #if arduino was connecetd at start, but now failed to read, try to reconnect
        if not ser and time.time() - last_time_tried_to_connect > RECONNECT_INTERVAL:
            arduino_port = find_arduino_port(logger=logger)  # find the serial port
            ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
            last_time_tried_to_connect = time.time()  # update the last time tried to connect

        if data_from_arduino and data_from_arduino != SERIAL_ERROR:  # if data is vaild

            # print(data_from_arduino)
            voltage, voltage_analogread, language = parse_data(data_from_arduino, logger=logger)
            # print(f"parsed: voltage {voltage} voltage_analogread{voltage_analogread} language {language}")
            # logger.info(f"{voltage},{voltage_analogread},{language}")
            #has_ignited.detect_drop(voltage=voltage, logger=logger)
        has_ignited.detect_drop(voltage=voltage,logger= logger)
        screen.fill(BLACK)  # reset screen
        display_state(screen, state=state, language=language, voltage=voltage)  # render the screen
        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()




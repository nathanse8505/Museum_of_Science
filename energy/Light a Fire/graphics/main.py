"""
File: main.py
Purpose: Main file for the Light a Fire UI
"""
#import pygame
#import time

from consts import *
from display import *
from arduino import *
from logs import *

def Quit_pygame(cap):
    if cap:
        cap.release()
    pygame.quit()
    exit()

def main():
    """
    The main function for the Light a Fire UI
    """
    # camera setup
    cap = None
    cap = camera_init()

    # pygame setup
    pygame.display.set_caption("Light a Fire")
    # screen = pygame.display.set_mode(VIEW_PORT, pygame.FULLSCREEN)
    screen = pygame.display.set_mode(VIEW_PORT)

    clock = pygame.time.Clock()  # for fps limit

    # initial values for th
    Temperature = MIN_TEMPERATURE_VALUE
    sensor_analogread = 0
    check_temperature_value = True

    # logging setup - log into a file called log.txt in the folder /logs with the format: [TIME] - [MESSAGE], if the file exists, append to it, if not, create it, if it exceeds 1MB, create a new file with a number suffix (before the .txt) and continue logging to it (e.g., log1.txt, log2.txt, etc.)
    logger = get_logger()
    logger.info("Starting Light a Fire UI")

    # arduino setup
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
    time.sleep(1)  # wait for the arduino to reset
    last_time_tried_to_connect = time.time()  # for not trying to connect too often
    runing = True
    while runing:

        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                Quit_pygame(cap)

            if event.type == pygame.KEYDOWN:

                if event.key == pygame.K_ESCAPE:
                    Quit_pygame(cap)

                if event.key == pygame.K_UP:
                    Temperature = min(Temperature + 1, MAX_TEMPERATURE_VALUE)

                if event.key == pygame.K_DOWN:
                    Temperature = max(Temperature - 1, MIN_TEMPERATURE_VALUE)

        data_from_arduino = read_line(ser, logger=logger)  # try to read from arduino
        if data_from_arduino == SERIAL_ERROR:  # if arduino WAS connected at start, but now failed to read:
            print("Arduino disconnected. Trying to reconnect to Arduino...")
            logger.info("Arduino disconnected. Trying to reconnect to Arduino...")

            ser = None
            Temperature = MIN_TEMPERATURE_VALUE
            state = MEASURE

        # if arduino was connecetd at start, but now failed to read, try to reconnect
        if not ser and time.time() - last_time_tried_to_connect > RECONNECT_INTERVAL:
            arduino_port = find_arduino_port(logger=logger)  # find the serial port
            ser = open_serial_connection(arduino_port, logger=logger)  # Open the serial port
            last_time_tried_to_connect = time.time()  # update the last time tried to connect

        if data_from_arduino and data_from_arduino != SERIAL_ERROR:  # if data is vaild
            # print(data_from_arduino)
            Temperature, sensor_analogread, error = parse_data(data_from_arduino, logger=logger)
            # print(f"parsed: pressure {Temperature} sensor_analogread {sensor_analogread}")
            
            if not error:
                check_temperature_value = log_temperature(logger,Temperature,check_temperature_value)
        check_temperature_value = log_temperature(logger, Temperature, check_temperature_value)
        screen.fill(BLACK)  # reset screen
        runing = camera_setup(screen,cap)
        display_measure(screen, sensor_analogread=sensor_analogread, Temperature=Temperature)  # render the screen
        pygame.display.flip()
        clock.tick(FPS)

    Quit_pygame(cap)

if __name__ == "__main__":
    main()



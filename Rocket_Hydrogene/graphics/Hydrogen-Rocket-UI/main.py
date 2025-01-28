"""
File: main.py
Purpose: Main file for the Hydrogen Rocket UI
"""
import pygame
from consts import *
from display import *
from arduino import *


def main():
    """
    The main function for the Hydrogen Rocket UI
    """
    pygame.display.set_caption("Hydrogen Rocket")
    screen = pygame.display.set_mode(VIEW_PORT, pygame.FULLSCREEN)

    language = HEBREW
    charge = 0.0
    current = 0.0
    state = OPENING

    arduino_port = find_arduino_port()  # find the serial port
    ser = open_serial_connection(arduino_port)  # Open the serial port

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

        data_from_arduino = read_line(ser)  # read from arduino
        if data_from_arduino == SERIAL_ERROR:  # if arduino WAS connected at start, but now failed to read:
            print("Disconnecting from Arduino...")
            print("Going to default settings...")
            ser = None
            language = HEBREW
            charge = 0.0
            current = 0.0
            state = OPENING

        if data_from_arduino and data_from_arduino != SERIAL_ERROR:  # if data is vaild
            # print(data_from_arduino)
            current, charge, has_ignited, language = parse_data(data_from_arduino)
            # print(f"parsed: current {current} charge {charge} has_ignited {has_ignited} language {language}")
            state = MEASURE if current >= SWITCH_TO_MEASURE_SCREEN_CURRENT_THRESHOLD else OPENING

        screen.fill((0,0,0))  # reset screen
        display_state(screen, state=state, language=language, charge=charge, current=current)  # render the screen
        pygame.display.flip()
        

if __name__ == "__main__":
    main()
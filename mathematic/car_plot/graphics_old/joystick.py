"""
Filename: joystick.py
Purpose: Joystick class for the car plotter exhibit.
"""

import pygame
from pygame.locals import *
from consts import JOYSTICK_MAX_VALUE, JOYSTICK_MIN_VALUE

class Joystick:
    def __init__(self, joystick_index=0, logger=None, max_value=JOYSTICK_MAX_VALUE, min_value=JOYSTICK_MIN_VALUE):
        """
        Initialize the joystick with the given index.
        :param joystick_index: The index of the joystick to use.
        """
        self.joystick_index = joystick_index
        self.logger = logger
        self.max_value = max_value
        self.min_value = min_value

        self.joystick = None
        self.value = 0
        self.reconnect_waiting = False

        # Try initial connect
        joystick = self.try_connect()
        if not joystick:
            self.logger.info("No joystick connected.")
            self.logger.info("Trying to reconnect...")
            self.reconnect_waiting = True

    def try_connect(self):
        """
        Try to connect to the joystick.
        :return: The joystick object if found, None otherwise.
        """
        pygame.joystick.quit()
        pygame.joystick.init()
        if pygame.joystick.get_count() > 0:
            js = pygame.joystick.Joystick(self.joystick_index)
            js.init()
            self.logger.info("Joystick found.")
            self.joystick = js
            return js
        
        self.joystick = None
        return None
    
    def get_value(self):
        """
        Get the value of the joystick.
        :return: The value of the joystick (if not connected, return the last value).
        """
        if self.joystick:
            try:
                self.value = round(self.joystick.get_axis(0), 4)
            except pygame.error:
                self.logger.info("Joystick read error.")
                self.joystick = None
                self.reconnect_waiting = True

        return self.value

    def map_value(self, value, min_value_mapping, max_value_mapping):
        """
        This works like the "map" function in Arduino. It first sees how the value is like between self.MIN_VALUE and self.MAX_VALUE.
        Then it maps it to the range of min_value_mapping and max_value_mapping.
        :param value: The value to map.
        :param min_value_mapping: The minimum value of the mapping range.
        :param max_value_mapping: The maximum value of the mapping range.
        :return: The mapped value. 
        """
        if value < self.min_value:
            value = self.min_value
        elif value > self.max_value:
            value = self.max_value

        mapped_value = (value - self.min_value) * (max_value_mapping - min_value_mapping) / (self.max_value - self.min_value) + min_value_mapping
        return mapped_value


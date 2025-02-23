"""
Filename: detect_drop.py
Purpose: Class for detecting a drop for the Air Pressure UI
"""
from consts import MIN_DROP_DISTANCE, PRESSURE_AFTER_DROP


class DropDetector:
    def __init__(self, min_drop_distance=MIN_DROP_DISTANCE, pressure_after_drop=PRESSURE_AFTER_DROP):
        """
        Create a DropDetector object to detect a drop in Air pressure
        """
        self.max_pressure = 0  # stores the max voltage so far (after the last drop)
        self.min_drop_distance = min_drop_distance
        self.pressure_after_drop = pressure_after_drop

    def detect_drop(self, pressure_value, logger=None):
        """
        Detect a drop in air pressure
        :param pressure_value: the current Air Pressure
        """
        if self.max_pressure - pressure_value > self.min_drop_distance and pressure_value < self.pressure_after_drop:

            print(f"The Bottle flew! pressure: {self.max_pressure} -> {pressure_value}")
            if logger:
                logger.info(f"The Bottle flew! pressure: {self.max_pressure} -> {pressure_value}")

            self.max_pressure = pressure_value
            return True

        if pressure_value > self.max_pressure:
            self.max_pressure = pressure_value  # update the max voltage

        return False

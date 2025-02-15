"""
Filename: detect_drop.py
Purpose: Class for detecting a drop for the Air Pressure UI
"""
from consts import MIN_DROP_DISTANCE, PRESSURE_AFTER_DROP


class DropDetector:
    def __init__(self, min_voltage_distance=MIN_DROP_DISTANCE, voltage_after_drop=PRESSURE_AFTER_DROP):
        """
        Create a DropDetector object to detect a drop in voltage
        """
        self.max_voltage = 0  # stores the max voltage so far (after the last drop)
        self.min_voltage_distance = min_voltage_distance
        self.voltage_after_drop = voltage_after_drop

    def detect_drop(self, voltage, logger=None):
        """
        Detect a drop in voltage
        :param voltage: the current voltage
        """
        if self.max_voltage - voltage > self.min_voltage_distance and voltage < self.voltage_after_drop:
            
            print(f"Ring jumped! voltage: {self.max_voltage} -> {voltage}") 
            if logger:
                logger.info(f"Ring jumped! voltage: {self.max_voltage} -> {voltage}")

            self.max_voltage = voltage
            return True
        
        if voltage > self.max_voltage:
            self.max_voltage = voltage  # update the max voltage

        return False
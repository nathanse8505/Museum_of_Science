"""
Filename: arduino.py
Purpose: Arduino functions for the HorsePower UI
"""

import serial
import serial.tools.list_ports
import sys
from consts import *


def find_arduino_port(logger=None):
    """
    Locate the Arduino's serial port (e.g., ttyUSB0 or ttyACM0 on Linux).
    """
    ports = serial.tools.list_ports.comports()

    for port in ports:
        # Check if the device description matches an Arduino
        #print(f"port: {port.device}")
        if "Arduino" in port.description or "ttyUSB" in port.device or "ttyACM" in port.device:
            print(f"Found Arduino on port {port.device}")
            if logger:
                logger.info(f"Found Arduino! port: {port.device}")

            return port.device
    
    print("Arduino not found")
    if logger:
        logger.error("Error: Arduino not found")

    return None  # continue without Arduino (if USE_ARDUINO is false)


def open_serial_connection(port=None, baud_rate=BAUDRATE, timeout=1, logger=None):
    """
    Open a serial connection to the specified port.
    """
    if not port:
        return None

    try:
        ser = serial.Serial(port, baud_rate, timeout=timeout)
        print(f"Connected to {port}")
        if logger:
            logger.info(f"Connected to {port}")

        return ser
    
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port} - {e}")
        if logger:
            logger.error(f"Error: Could not open serial port {port} - {e}")

        return None


def read_line(ser=None, logger=None):
    """
    Read one line from the serial buffer.
    :param ser: a serial buffer reader object
    """
    if not ser:
        return None
    
    try:
        line = ser.readline().decode('utf-8').strip()  # Read and decode a line
        return line if line else None  # Return the line if it's not empty
    
    except serial.SerialException as e:
        print(f"Error reading from serial: {e}")
        if logger:
            logger.error(f"Error reading from serial, Arduino probably disconnected")

        return SERIAL_ERROR
    

def parse_data(raw_data, logger=None):
    """
    parse the raw data line (<voltage> <language>)
    :param raw_data: the raw data
    """
    try:
        data = raw_data.split(" ")
        # ----------------- voltage ------------------------------------ language
        return max(min(float(data[0]), MAX_HORSEPOWER), MIN_HORSEPOWER), int(data[1])
    
    except:
        print(f"Error parsing data: {raw_data}")
        if logger:
            logger.error(f"Error parsing data: {raw_data}")

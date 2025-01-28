import serial
import serial.tools.list_ports
import sys
from consts import *


def find_arduino_port():
    """
    Locate the Arduino's serial port (e.g., ttyUSB0 or ttyACM0 on Linux).
    """
    print("Locating Arduino...")
    ports = serial.tools.list_ports.comports()

    for port in ports:
        print(f"Device: {port.device}, Name: {port.name}, Description: {port.description}")
        if "COM" in port.device or " USB-SERIAL CH340" in port.description:
            return port.device
        # Check if the device description matches an Arduino
        # print(f"port: {port}")
        # if "Arduino" in port.description or "ttyUSB" in port.device or "ttyACM" in port.device:
        #     print(f"Arduino found on port: {port}")
        #     return port.device

    print("No Arduino device found. Check connections.")

    if FORCE_ARDUINO:
        sys.exit(1)  # quit if USE_ARDUINO flag is on (must use arduino, but couldn't connect to it)

    print("Continuing without Arduino...")
    return None  # continue without Arduino (if USE_ARDUINO is false)


def open_serial_connection(port=None, baud_rate=BAUDRATE):
    """
    Open a serial connection to the specified port.
    """
    if not port:
        return None

    try:
        ser = serial.Serial(port, baud_rate, timeout=1)
        print(f"Connected to {port}")
        return ser
    
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port} - {e}")
        sys.exit(1)


def read_line(ser=None):
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
        return SERIAL_ERROR
    

def parse_data(raw_data):
    """
    parse the raw data line (<current> <charge> <has_ignited> <language>)
    :param raw_data: the raw data
    """
    try:
        data = raw_data.split(" ")
        # ---------- current ------- charge --- has_ignited --- language
        return min(float(data[0]), MAX_CURRENT) , min(float(data[1]), MAX_CHARGE), int(data[2]), int(data[3])
    
    except:
        print(f"Error parsing data: {raw_data}")

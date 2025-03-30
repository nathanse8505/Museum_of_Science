"""
Filename: arduino.py
Purpose: arduino functions for the Droping Drops UI
"""
import serial
from const import *
from random import randint
import serial.tools.list_ports


def send_one_number(value, arduino):
    try:
        arduino.write(value)
    except Exception:
        print(f"{ord(value)} not sent")
        return False
    return True


def send_drop_key(arduino):
    arduino.write(drop_key.encode())


def send_start_key(arduino):
    arduino.write(start_key.encode())


def send_end_key(arduino):
    arduino.write(end_key.encode())


def send_variables_to_arduino(arduino):
    arduino.write(bytearray([output_height]))
    arduino.write(bytearray([valve_on_time]))
    arduino.write(bytearray([drawing_depth]))
    # arduino.write(bytearray([full_light]))
    # arduino.write(bytearray([led_power]))
    # arduino.write(bytearray([colors_num]))
    # for color in colors:
    #     for value in color:
    #         arduino.write(bytearray([value]))
    # response = arduino.read()
    # if response is not None:
    #     # print(chr(ord(response)))
    #     pass
    # else:
    #     print('Error: No response from Arduino')
    #     return False
    return True


def reset_buffer_arduino(arduino, log):
    if log:
        print("sending to arduino")
    # clear Serial buffer
    arduino.flush()
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()

def send_data_to_arduino(arduino,byte_list):
    cassette = randint(0, cassettes_num - 1)  # choose a random cassette number to drop the image in
    send_start_key(arduino)  # send the start key to the arduino
    if not send_one_number(bytearray([cassette])):  # send the cassette number to the arduino
        print("Error: Could not send cassette number")
        return False
    if not send_image_to_arduino(byte_list):  # send the image to the arduino
        print("Error: Could not send image to arduino")
        return False
    send_end_key(arduino)  # send the end key to the arduino
    return True


def send_image_to_arduino(log, byte_list, arduino):
    if log:
        print('Sending data to Arduino...')
    for i in range(len(byte_list)):
        if not send_one_number(bytearray([byte_list[i]])):
            print(f"Error: Could not send {ord(byte_list[i])}")
            return False
        if (i + 1) % 8 == 0:
            response = arduino.read()
            if response is None:
                print('Error: No response from Arduino')
                return False
    return True


# Initialisation Arduino
def find_arduino_port(logger=None):
    """
    Locate the Arduino's serial port (e.g., ttyUSB0 or ttyACM0 on Linux).
    """
    ports = serial.tools.list_ports.comports()

    for port in ports:
        # Check if the device description matches an Arduino
        # print(f"port: {port}")
        if "Arduino" in port.description or "ttyUSB" in port.device or "ttyACM" in port.device:
            print(f"Found Arduino on port {port.device}")
            if logger:
                logger.info(f"Found Arduino! port: {port.device}")

            if hasattr(find_arduino_port, "already_sent_error"):
                find_arduino_port.already_sent_error = False

            return port.device

    if logger:
        # for not spamming the logs
        if not hasattr(find_arduino_port, "already_sent_error"):
            find_arduino_port.already_sent_error = False

        if not find_arduino_port.already_sent_error:
            logger.error("Error: Arduino not found")
            print("Arduino not found")
            find_arduino_port.already_sent_error = True

    return None  # continue without Arduino


def open_serial_connection(port=None, baud_rate=BAUDRATE, timeout=0.05, logger=None):
    """
    Open a serial connection to the specified port.
    """
    if not port:
        return None, False

    try:
        ser = serial.Serial(port, baud_rate, timeout=timeout)

        print(f"Connected to {port}")
        if logger:
            logger.info(f"Connected to {port}")

        return ser, True

    except Exception as e:
        print(f"Error: Could not open serial port {port} - {e}")
        if logger:
            logger.error(f"Error: Could not open serial port {port} - {e}")

        return None, False



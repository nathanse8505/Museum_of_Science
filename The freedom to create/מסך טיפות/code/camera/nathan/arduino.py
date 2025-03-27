import serial
from const import *
from random import randint

def send_one_number(value,arduino):
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

def reset_buffer_arduino(arduino,byte_list):
        if log:
            print("sending to arduino")
        # clear Serial buffer
        arduino.flush()
        arduino.reset_input_buffer()
        arduino.reset_output_buffer()
        cassette = randint(0, cassettes_num - 1)  # choose a random cassette number to drop the image in
        send_start_key()  # send the start key to the arduino
        if not send_one_number(bytearray([cassette])):  # send the cassette number to the arduino
            print("Error: Could not send cassette number")
            return arduino, False
        if not send_to_arduino(byte_list):  # send the image to the arduino
            print("Error: Could not send image to arduino")
            return arduino, False
        send_end_key()  # send the end key to the arduino
        return arduino,True

def send_to_arduino(log ,byte_list, arduino):
    if log:
        print('Sending data to Arduino...')
    for i in range(len(byte_list)):
        if not send_one_number(bytearray([byte_list[i]])):
            print(f"Error: Could not send {ord(byte_list[i])}")
            return False
        if (i+1) % 8 == 0:
            response = arduino.read()
            if response is None:
                print('Error: No response from Arduino')
                return False
    return True


# Initialisation Arduino
def open_serial():
    arduino = None
    log = None
    try:
        arduino = serial.Serial(port, baudrate, timeout=1)
        found_arduino = True
        print("Found Arduino")
    except Exception as e:
        found_arduino = False
        log = space_time/1000 > 5
        print(f"Serial port error: {e}")
        print('ARDUINO NOT CONNECTED')
    return arduino,found_arduino,log

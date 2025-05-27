"""
Filename: laser.py
Purpose: Handle all the laser functions
"""

import serial
import serial.tools.list_ports
from consts import *
import time


class Laser():

    def __init__(self, laser_cutting_pos, laser_cutting_size, logger):
        self.laser_cutting_pos = laser_cutting_pos
        self.laser_cutting_size = laser_cutting_size
        self.logger = logger

        self.drawing = False
        self.sent_init_params = False
        self.points = []
        self.frame_points = []
        self.index = 0
        self.mode = "user_points"  # user_points OR frame_points - which to send right now
        self.batch_size = POINTS_BATCH_SIZE

        self.last_time_sent_data = time.time()
        self.waiting_for_ack = False
        self.drawing_start_time = time.time()
        self.dc_motor_on = False
        self.last_dc_motor_time = time.time()

        self.arduino = self.open_serial_connection()
        self.last_time_tried_to_connect = time.time()


    def find_arduino_port(self):
        """
        Locate the Arduino's serial port (e.g., ttyUSB0 or ttyACM0 on Linux).
        """
        ports = serial.tools.list_ports.comports()

        for port in ports:
            if "Arduino" in port.description or "ttyUSB" in port.device or "ttyACM" in port.device:
                print(f"Found Arduino on port {port.device}")
                self.logger.info(f"Found Arduino on port: {port.device}")

                return port.device

        return None

    def open_serial_connection(self, timeout=1):
        """
        Open a serial connection to the specified port.
        """
        port = self.find_arduino_port()

        if not port:
            print(f"Error: Arduino not found")
            self.logger.error(f"Error: Arduino not found")
            return None

        try:
            ser = serial.Serial(port, BAUDRATE, timeout=timeout)
            
            print(f"Connected to {port}")
            self.logger.info(f"Connected to {port}")
            time.sleep(0.01)
            return ser
        
        except serial.SerialException as e:
            print(f"Error: Could not open serial port {port} - {e}")
            self.logger.error(f"Error: Could not open serial port {port} - {e}")
            return None
        
    def exist(self):
        return self.arduino != None
    
    def is_drawing(self):
        return self.drawing
    
    def send_values(self, *values, delay=False, delay_time=0.01):
        for value in values:
            encoded_value = f"{value}\n".encode()
            self.arduino.write(encoded_value)

        if delay:
            time.sleep(delay_time) # small delay

    def send_initial_parameters(self):
        if not self.arduino:
            return
        
        self.logger.info("Sending initial parameters for Arduino")

        try:        
            self.send_values("PARAMS")
            self.send_values(self.laser_cutting_pos[0],
                             self.laser_cutting_pos[1],
                             self.laser_cutting_pos[0] + self.laser_cutting_size[0], 
                             self.laser_cutting_pos[1] + self.laser_cutting_size[1],
                             delay=True)
            self.send_values(LASER_POWER, FRAME_POWER, LASER_OFF_RATE, LASER_ON_RATE, FRAME_RATE, MAX_DC_MOTOR_TIME * 1000, delay=True)
            self.logger.info("Waiting for 'OK'...")

            start = time.time()
            while time.time() - start < INIT_PARAMS_TIMEOUT:
                if self.arduino.in_waiting:
                    response = self.arduino.readline().strip()

                    if response == b"OK":
                        self.logger.info("Got OK - Arduino acknowledged parameters")
                        self.sent_init_params = True
                        return True
                    
                    else:
                        self.logger.info(f"Unexpected response: {response}")
                        self.arduino = None
                        return False
                    
            self.logger.error("Error: No 'OK' from Arduino")
            self.arduino = None
            return False

        except serial.SerialException as e:
            self.logger.error(f"Error: Failed during transmission: {e}")
            self.arduino = None
            return False

    def init_drawing(self, points, frame_points):
        self.points = points
        self.frame_points = frame_points
        self.drawing = True
        self.drawing_start_time = time.time()
        self.waiting_for_ack = False
        self.dc_motor_on = False
        self.mode = "user_points"
        self.logger.info("Started drawing...")

        try:
            self.send_values("START")
            return True
        
        except:
            self.logger.error(f"Error: Failed during transmission")
            return False

    def get_status(self):        
        # Waiting for Arduino ACK (ack of finishing drawing the batch)
        if self.waiting_for_ack:
            if self.arduino.in_waiting:
                line = self.arduino.readline().strip()
                if line == b"OK":
                    self.waiting_for_ack = False
                    self.last_time_sent_data = None
                else:
                    return "ACK"

            elif time.time() - self.last_time_sent_data > ARDUINO_DRAWING_BATCH_TIMEOUT:
                self.send_values("RESET")
                return "RESET"  # timeout error, abort
            
            else:
                return "ACK"  # waiting for arduino to acknowlage batch
            
        if self.dc_motor_on:
            if time.time() - self.last_dc_motor_time > MAX_DC_MOTOR_TIME:
                self.dc_motor_on = False
                return "SUCCESS"
            return "DC_MOTOR"  # dc motor still on

        points = self.points if self.mode == "user_points" else self.frame_points

        if self.index >= len(points):
            status = "D_DONE" if self.mode == "user_points" else "F_DONE"
            self.send_values(status)  # drawing done or frame done

            if status == "F_DONE":
                self.dc_motor_on = True
                self.last_dc_motor_time = time.time()

            self.waiting_for_ack = False
            return status  # done points list (user points or frame points)
        
        batch = points[self.index:self.index + self.batch_size]  # get batch and send it
        for point in batch:
            if point is None:
                self.send_values("None")
            else:
                x, y = point
                self.send_values(f"{x},{y}")

        self.send_values("B_DONE")  # batch done
        self.index += self.batch_size
        self.waiting_for_ack = True
        self.last_time_sent_data = time.time()
        return "ACK"  # waiting for arduino to acknowlage batch

    def end_drawing(self):
        self.drawing = False
        self.points.clear()
        self.frame_points.clear()
        self.index = 0
        self.mode = "user_points"
        self.waiting_for_ack = False
        self.dc_motor_on = False

    def check_on_laser(self):
        if not self.exist:
            return "ERROR"
        
        if not self.drawing:
            return "NOT DRAWING"
        
        try:
            status = self.get_status()

            if status == "RESET":
                self.logger.error("Error: Timeout waiting for Arduino. Stopping transmission.")
                self.end_drawing()
                return "DONE"
            
            if status == "ACK" or status == "DC_MOTOR":
                return "DRAWING"
            
            if status == "D_DONE":  # finished drawing the points of the user (without frame)
                self.mode = "frame_points"
                self.index = 0
                return "DRAWING"
            
            if status == "F_DONE":  # finished all drawing (user points + frame), dc motor is now on
                total_time = f"{(time.time() - self.drawing_start_time):.1f}"
                self.logger.error(f"Finished drawing in {total_time} seconds")
                return "DRAWING"
            
            if status == "SUCCESS":  # finished drawing (user points + frame) and DC motor finished
                self.end_drawing()
                return "DONE"
            
            return "DRAWING"
        
        except:
            return "ERROR"
        

            
        


        
            


                

import os
import serial
from typing import List
from datetime import datetime
os.environ["OPENCV_LOG_LEVEL"]="SILENT" # to suppress OpenCV warnings and errors
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "hide" # to suppress Pygame welcome message
import cv2
import time
import pygame
import numpy as np
from pygame.locals import *
from consts import *
import random


# display message on screen with the current status of the camera and the arduino, used when the camera mode is off
def msg_on_screen():
    global font, screen, screen_width, screen_height, font_size, found_arduino, camera_working, threshold
    text0 = "Press P to start/stop camera mode"
    text1 = "Press RIGHT/LEFT to change threshold for black and white filter"
    text2 = "threshold: " + str(threshold)
    text3 = "----------------------------------------------------------------------"
    text4 = "Arduino not found" if not found_arduino else "Arduino found"
    text5 = "Camera is connected" if camera_working else "Camera is NOT connected - reconnect the camera and press P to start"
    text = [text0, text1, text2, text3, text4, text5]
    y_position = screen_height // 2 - len(text) * font_size // 2
    for line in text:
        text_surface = font.render(line, True, (255, 255, 255))
        text_rect = text_surface.get_rect()
        text_rect.center = (screen_width // 2, y_position)
        screen.blit(text_surface, text_rect)
        y_position += font_size

# display message on screen when the camera mode is on (showing the image and the black and white filter of the image)
def msg_on_screen2():
    global font2, screen
    text0 = "Press P for information"
    text_surface = font2.render(text0, True, (255, 255, 255))
    text_rect = text_surface.get_rect()
    text_rect.center = (screen_width * 3 // 4, screen_height - 30)
    screen.blit(text_surface, text_rect)

# take a picture from the camera and return it
def take_picture():
    global cap, camera_working
    if not camera_working:
        return None
    ret, frame = cap.read()
    if ret:
        return frame
    return None

# send one byte to the arduino through the serial port
def send_one_number(value):
    try:
        arduino.write(value)
    except Exception:
        print(f"{ord(value)} not sent")
        return False
    return True

# send drop key to the arduino to drop (in water) the image in the buffer
def send_drop_key():
    global arduino
    arduino.write(drop_key.encode())

# send start key to the arduino to start receiving the image
def send_start_key():
    global arduino
    arduino.write(start_key.encode())

# send end key to the arduino to end receiving the image
def send_end_key():
    global arduino
    arduino.write(end_key.encode())

# send the variables to the arduino on the start of the program, check Consts.py for the variables meaning
def send_variables_to_arduino():
    global arduino, valve_on_time, space_time, full_light, led_start_time, led_on_time, drawing_depth, led_power, colors, colors_num
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


# send the image (byte list) to the arduino, 8 bytes at a time. After sending 8 bytes, the arduino will send a response to the computer
# to indicate that it is ready to receive the next 8 bytes. This is done to avoid buffer overflow in the arduino
def send_to_arduino(byte_list):
    global arduino
    if log:
        print('Sending data to Arduino...')
    for i in range(len(byte_list)):
        if log:
            print(f"----  {bytearray([byte_list[i]]).hex()}  ----")
        if not send_one_number(bytearray([byte_list[i]])):  # send the byte to the arduino
            print(f"Error: Could not send {ord(byte_list[i])}")
            return False
        if (i+1) % 8 == 0:  # after sending 8 bytes, wait for the arduino to be ready to receive the next 8 bytes
            if log:
                print()
            response = arduino.read()  # read the response from the arduino
            if response is None:
                print('Error: No response from Arduino')
                return False
    if log:
        print('Data sent to Arduino')
    return True


# process the image and save it as a black and white image with the same name as the original image, but with "_bw" at the end
# the image is resized to (output_width) x (output_height) pixels, then converted to grayscale, then a binary thresholding is applied to create a black and white image
def process_and_save_image(input_path, output_path):
    global log
    image = cv2.imread(input_path)  # read the image from the input path
    if image is not None:
        resized_image = cv2.resize(image, (output_width, output_height))  # resize the image to (output_width) x (output_height) pixels. probably 64 x (25 or 30) pixels
        gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)  # convert the image to grayscale
        _, bw_image = cv2.threshold(gray_image, threshold, 255, cv2.THRESH_BINARY)  # Apply binary thresholding to create a black and white image
        cv2.imwrite(output_path, bw_image)  # save the black and white image to the output path
        flattened_array = bw_image.flatten()  # flatten the image to a 1D array
        arr = []
        black_pixels = 0  # count the number of black pixels in the image (black pixels are the pixels with value 0). This is to calculate the percentage of black pixels in the image
        for i in range(0, len(flattened_array), 8):
            if i % 64 == 0 and log:
                print()
            # for each 8 pixels, create a byte value from the 8 pixels. The byte value is the binary representation of the 8 pixels
            byte_value = 0
            for j in range(8):
                pixel_value = flattened_array[i + j]  # get the value of the pixel
                byte_value |= (pixel_value & 1) << (7 - j)  # shift the pixel value to the left by (7 - j) bits, then add it to the byte value
                if pixel_value == 0:  # if the pixel is black, increment the black pixels counter
                    black_pixels += 1
                if log:
                    print(" " if (pixel_value & 1) else "#", end="")
            arr.append(byte_value)  # add the byte value to the array
        byte_array = bytearray(arr)  # convert the array to a byte array
        if log:
            print('\n\nImage processed and saved successfully.')
            # print("\n\n len(byte_array): ", len(byte_array))
        return byte_array, black_pixels / len(flattened_array)  # return the byte array and the percentage of black pixels in the image
    else:
        print('Error loading the image.')
        return None


folder_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), folder_name_for_saved_pictures)  # the folder to save the images in
idle_folder_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), folder_name_for_idle_pictures)  # the folder to get the idle images from (when no hand is detected in the image)
arduino = None  # the serial port to connect to the arduino
camera_on = True  # True if camera mode is on
camera_working = False  # True if the camera is connected and working
empty_captures_in_a_row = 0  # count the number of empty images in a row, for the idle mode when no hand is detected in the image
last_capture = time.time()  # the time of the last capture 

# check if the arduino is connected to the computer
try:
    arduino = serial.Serial(port, baudrate, timeout=1)
    found_arduino = True
    print("Found Arduino")
except Exception as e:
    found_arduino = False
    log = space_time/1000 > 5  # only write print statements if taking each picture takes more than 5 seconds to avoid spamming the console
    print(f"Serial port error: {e}")
    print('ARDUINO NOT CONNECTED')

# check if the camera is connected to the computer
cap = None
while True:
    try:
        cap = cv2.VideoCapture(camera_index)  # open the camera
        cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0 if not auto_exposure else 1)  # disable automatic exposure - IMPORTANT
        cap.set(cv2.CAP_PROP_AUTO_WB, 0 if not auto_white_balance else 1)  # disable automatic white balance - IMPORTANT
        cap.set(cv2.CAP_PROP_EXPOSURE, fixed_exposure)  # set the exposure to a fixed value - IMPORTANT
        if cap.isOpened():
            print("Camera is ready")
            camera_working = True
            break  # if the camera is connected and working, break the loop
        print("\nError: Could not open camera. please check if the camera is connected properly.\nRetrying in 2 seconds...\n\n")
        time.sleep(2)  # wait for 2 seconds before trying to open the camera again
    except Exception as e:
        pass


screen_width, screen_height = 720, 640  # the width and height of the window to display the images on
pygame.init()  # initialize pygame
screen = pygame.display.set_mode((screen_width, screen_height))  # set the window size
pygame.display.set_caption("Camera")  # set the window title

# clear Serial buffer
if found_arduino:
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()
    time.sleep(0.1) # wait for the serial buffer to clear
    
font_size = 40  # the font size for the messages on the screen
font2_size = 40  # the font size for the messages on the screen when the camera mode is on
font = pygame.font.Font(None, font_size)  # Use default system font
font2 = pygame.font.Font(None, font2_size)  # Use default system font
is_folder_created = os.path.exists(folder_name)  # check if the folder to save the images in is already created. If it is, do not create it again
sample_index = 0  # the index of the sample image (idle image) to send to the arduino when the image is empty
arduino_done = True  # True if the arduino is done processing the previous image and ready to receive the next image
idle = False  # True if the camera is in idle mode (no hand detected in the image for 'empty_captures_before_idle' times in a row)
send_parameters = False  # True if the parameters already sent to the arduino

running = True  # True if the program is running
while(running):

    img = take_picture()  # take a picture every 0.002 seconds to keep the buffer clean with the latest image and avoid delay in the images. Do not remove for Raspberry Pi
    time.sleep(0.002)
    
    if not send_parameters and found_arduino:  # send the parameters to the arduino on the start of the program
        send_variables_to_arduino()
        send_parameters = True
        time.sleep(0.2) # wait for the arduino to process the variables and be ready to receive the image, at least 0.2 second

    for event in pygame.event.get():
        if event.type == QUIT:  # if the window is closed, quit the program
            running = False
            cap.release()  # release the camera
            cv2.destroyAllWindows()  # close the camera window
            print("quitting...")
        if event.type == KEYDOWN:
            if event.key == K_ESCAPE or event.key == K_q:  # if the escape key or 'q' key is pressed, quit the program
                running = False
                cap.release()  # release the camera
                cv2.destroyAllWindows()  # close the camera window
                print("quitting...")
            if event.key == K_p:  # if the 'p' key is pressed, turn on/off the camera mode
                if not camera_working:  # if the camera is not working, try to open the camera
                    try:
                        cap = cv2.VideoCapture(camera_index)  # open the camera
                        if cap.isOpened():
                            camera_working = True
                            print("Camera is ready")
                    except Exception as e:
                        print("Error: Could not open camera. please check if the camera is connected properly")
                        continue
                camera_on = not camera_on  # turn on/off the camera mode
                print(f"Camera is {'on' if camera_on else 'off'}")
                last_capture = time.time()  # reset the last capture time
            if event.key == K_RIGHT:  # if the right arrow key is pressed, increase the threshold for the black and white filter
                threshold += 10
                print(f"Threshold: {threshold}")
            if event.key == K_LEFT:  # if the left arrow key is pressed, decrease the threshold for the black and white filter
                threshold -= 10
                print(f"Threshold: {threshold}")

    if camera_on and time.time() - last_capture >= space_time/1000 and arduino_done:  
        # if the time since the last capture is more than 'space_time' milliseconds 
        # and the arduino is done processing the previous image, take a new picture and send it to the arduino
        img = take_picture()  # take a picture
        if img is None:
            print("Error: Could not take picture")
            camera_on = False  # if the camera is not working, turn off the camera mode
            camera_working = False  # if the camera is not working, set the camera working flag to False
            continue
        else:
            camera_working = True
        img = cv2.flip(img, 0)  # flip the image vertically (the camera is upside down)
        img = cv2.rotate(img,cv2.ROTATE_90_CLOCKWISE)

        # Calculate the dimensions of the central region to keep after cropping
        height, width = img.shape[:2]
        crop_height = int(height * crop_percentage[1] / 100)
        crop_width = int(width * crop_percentage[0] / 100)
        # Calculate the starting point for cropping
        start_x = (width - crop_width) // 2
        start_y = (height - crop_height) // 2
        # Crop the central region of the image
        cropped_image = img[start_y:start_y + crop_height, start_x:start_x + crop_width]
        # Create a new image with the same dimensions as the original, filled with white
        new_image = np.full_like(img, 255, dtype=np.uint8)
        # Place the cropped image in the center of the new image
        new_image[start_y:start_y + crop_height, start_x:start_x + crop_width] = cropped_image
        img = new_image

        time_stamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")  # get the current time to use it as the name of the image
        if not is_folder_created:  # if the folder to save the images in is not created, create it
            os.makedirs(folder_name, exist_ok=True)
        in_path = os.path.join(folder_name,time_stamp + ".png")  # the path to save the original image
        out_path = os.path.join(folder_name,time_stamp + "_bw.png")  # the path to save the black and white image
        cv2.imwrite(in_path, img)  # save the original image
        byte_list, black_percentage = process_and_save_image(in_path, out_path)  # process the image and save it as a black and white image
        # show on screen the image
        screen.fill((0, 0, 0))
        image_display = pygame.image.load(in_path)
        image_bw_display = pygame.image.load(out_path)
        image_bw_display = pygame.transform.scale(image_bw_display, (screen_width//2, screen_width//2 * output_height // output_width))
        image_display = pygame.transform.scale(image_display, (screen_width//2, screen_height))
        screen.blit(image_display, (0, 0))
        screen.blit(image_bw_display, (screen_width//2, 0))
        msg_on_screen2()  # display the message on the screen
        pygame.display.flip()  # update the screen

        # if the image is not empty (black_percentage > empty_image_threshold), send the image to the arduino
        if found_arduino and byte_list is not None and black_percentage > empty_image_threshold:
            empty_captures_in_a_row = 0  # reset the empty captures counter if the image is not empty
            idle = False  # the camera is not in idle mode
            if log:
                print("sending to arduino")
            # clear Serial buffer
            arduino.flush()  
            arduino.reset_input_buffer()
            arduino.reset_output_buffer()

            cassette = random.randint(0, cassettes_num-1)  # choose a random cassette number to drop the image in
            send_start_key()  # send the start key to the arduino
            if not send_one_number(bytearray([cassette])):  # send the cassette number to the arduino
                print("Error: Could not send cassette number")
                continue
            if not send_to_arduino(byte_list):  # send the image to the arduino
                print("Error: Could not send image to arduino")
                continue
            send_end_key()  # send the end key to the arduino
            last_capture = time.time()  # reset the last capture time
            arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)
        
        # if the image is empty (black_percentage <= empty_image_threshold), send a sample image to the arduino from the idle_images_folder
        elif found_arduino and byte_list is not None:
            empty_captures_in_a_row += 1  # increment the empty captures counter
            if empty_captures_in_a_row >= empty_captures_before_idle:  # if the empty captures counter is more than 'empty_captures_before_idle', go to idle mode
                if log:
                    print("Image is empty, sending sample image to arduino...")
                idle = True  # the camera is in idle mode
                byte_list, _ = process_and_save_image(os.path.join(idle_folder_name, idle_images[sample_index]), out_path)  # process the sample image
                if byte_list is not None:
                    # clear Serial buffer
                    arduino.flush()
                    arduino.reset_input_buffer()
                    arduino.reset_output_buffer()
                    
                    cassette = random.randint(0, cassettes_num-1)  # choose a random cassette number to drop the image in
                    send_start_key()  # send the start key to the arduino
                    if not send_one_number(bytearray([cassette])):  # send the cassette number to the arduino
                        print("Error: Could not send cassette number")
                        continue
                    if not send_to_arduino(byte_list):  # send the image to the arduino
                        print("Error: Could not send image to arduino")
                        continue
                    send_end_key()  # send the end key to the arduino
                    sample_index += 1  # increment the sample index to send the next sample image next time
                    if sample_index >= len(idle_images):  # if the sample index is more than the number of sample images, reset the sample index to the first image
                        sample_index = 0
            else:
                # if the image is empty but the empty captures counter is less then 'empty_captures_before_idle', drop the last image again (which is already in the arduino's buffer and not empty)
                send_drop_key() # drop the image in the arduino's buffer (last non-empty image)
            arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)
            last_capture = time.time()  # reset the last capture time
        elif found_arduino and byte_list is None:
            print("Error processing image")

        # delete images if not saving, or if the time to take the picture is less than 2 seconds for not saving too many images in a short time
        if not save_picture or space_time/1000 <= 2:
            try:
                os.remove(in_path)  # delete the original image
                os.remove(out_path)  # delete the black and white image
            except Exception as e:
                print(f"Error deleting images: {e}")
    
    elif found_arduino and not arduino_done:  # check if the arduino is done processing the previous image and ready to receive the next image
        if arduino.in_waiting > 0:  # if there is data in the serial buffer
            received_data = arduino.readline().decode().rstrip()
            # print("Received from Arduino:", received_data) # print the response from the arduino (for debugging)
            arduino_done = True  # the arduino is done processing the image and ready to receive the next image
    elif not camera_on:  # if the camera mode is off, display the messages on the screen
        screen.fill((0, 0, 0))
        msg_on_screen()
        pygame.display.flip()

# closing the program....
if found_arduino:
    arduino.close()  # close the serial port
if not is_folder_created and not save_picture:  # if the folder was created in this execution, delete it. If it was already there, keep it
    try: 
        os.rmdir(folder_name)
    except Exception as e:
        print(f"Error deleting folder: {e}")
cap.release()  # release the camera
cv2.destroyAllWindows()  # close the camera window
pygame.quit()  # quit pygame

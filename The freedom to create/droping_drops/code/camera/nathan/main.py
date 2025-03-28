"""
Filename: main.py
Purpose: main functions for the Droping Drops UI
"""
from pygame.locals import *
from arduino import *
from image_processing import *
from logs_Drop import *


def main():
    # logger
    logger = get_logger()
    logger.info("Starting Droping Drops UI")
    log_arduino = False
    # Initialisation arduino
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    arduino, found_arduino = open_serial_connection(arduino_port, logger=logger)  # Open the serial port

    # Reset des buffers série si Arduino présent
    if found_arduino:
        log_arduino = True
        arduino.reset_input_buffer()
        arduino.reset_output_buffer()
        time.sleep(0.1)

    # Initialisation caméra
    camera_on = True
    camera_working = False
    cap = None
    threshold = 80  # 60-80 is a good value for the threshold to convert the image to black and white. value 0-255 (higher values will make the image darker)
    while not camera_working:
        camera_working, cap = camera_init()
    empty_captures_in_a_row = 0  # count the number of empty images in a row, for the idle mode when no hand is detected in the image
    last_capture = time.time()  # the time of the last capture

    # Initialisation Pygame
    screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN)
    pygame.display.set_caption("Camera")

    # Initialisation dossier et variables
    is_folder_created = os.path.exists(folder_name)
    sample_index = 0
    arduino_done = True
    idle = False
    send_parameters = False
    running = True

    # Boucle principale
    while running:

        for event in pygame.event.get():
            if event.type == QUIT:
                running = False
            elif event.type == KEYDOWN:

                if event.key in (K_ESCAPE, K_q):
                    running = False
                elif event.key == K_RIGHT:
                    threshold += 5
                    print(f"Threshold: {threshold}")
                elif event.key == K_LEFT:
                    threshold -= 5
                    print(f"Threshold: {threshold}")
                elif event.key == K_p:
                    if not camera_working:
                        camera_working, cap = camera_init()
                        if not camera_working:
                            continue
                    camera_on = not camera_on
                    print(f"Camera is {'on' if camera_on else 'off'}")
                    last_capture = time.time()

        # img = take_picture(cap, camera_working)
        # time.sleep(0.002)

        if not send_parameters and found_arduino:
            send_variables_to_arduino(arduino)
            send_parameters = True
            time.sleep(0.2)
        ###################################  part 1   ####################################
        ################################### P information ###################################
        if not camera_on:
            screen.fill((0, 0, 0))
            msg_on_screen(screen, camera_working, found_arduino,threshold)
            pygame.display.flip()
        ###################################  part 2   ####################################
        ##################################  camera working ###################################
        elif camera_on and time.time() - last_capture >= space_time / 1000 and arduino_done:
            # if the time since the last capture is more than 'space_time' milliseconds
            # and the arduino is done processing the previous image, take a new picture and send it to the arduino

            img, camera_working = take_pic(cap, camera_working)
            # img = config_cam(img)
            time.sleep(0.002)
            if not camera_working:
                continue
            # saving picture and create path
            in_path, out_path = save_pictures(img, is_folder_created)
            # process the image
            byte_list, black_percentage = process_and_save_image(in_path, out_path, log_arduino,threshold)  # process the image and save it as a black and white image

            # display info
            screen.fill((0, 0, 0))
            display_camera_and_process_image(screen, img, in_path, out_path)
            pygame.display.flip()

            ################################## send arduino data from camera ###################################
            ####################################################################################################
            # if the image is not empty (black_percentage > empty_image_threshold), send the image to the arduino
            if found_arduino and byte_list is not None and black_percentage > empty_image_threshold:
                idle = False  # the camera is not in idle mode
                empty_captures_in_a_row = 0  # reset the empty captures counter if the image is not empty
                VALID = reset_buffer_arduino(arduino, byte_list, log_arduino)
                if not VALID:
                    continue
                last_capture = time.time()  # reset the last capture time
                arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)

            ################################## send arduino data from idle ###################################
            ####################################################################################################
            elif found_arduino and byte_list is not None:
                empty_captures_in_a_row += 1  # increment the empty captures counter
                if empty_captures_in_a_row >= empty_captures_before_idle:  # if the empty captures counter is more than 'empty_captures_before_idle', go to idle mode
                    if log_arduino:
                        print("Image is empty, sending sample image to arduino...")
                    idle = True  # the camera is in idle mode
                    in_path_idle = os.path.join(idle_folder_name, idle_images[sample_index])
                    byte_list, _ = process_and_save_image(in_path_idle, out_path, log_arduino)  # process the sample image
                    VALID = reset_buffer_arduino(arduino, byte_list, log_arduino)
                    if not VALID:
                        continue
                    sample_index += 1  # increment the sample index to send the next sample image next time
                    if sample_index >= len(
                            idle_images):  # if the sample index is more than the number of sample images, reset the sample index to the first image
                        sample_index = 0
                else:
                    # if the image is empty but the empty captures counter is less then 'empty_captures_before_idle', drop the last image again (which is already in the arduino's buffer and not empty)
                    send_drop_key(arduino)  # drop the image in the arduino's buffer (last non-empty image)
                arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)
                last_capture = time.time()  # reset the last capture time

            ########################### error processing image arduino is found ################################
            ####################################################################################################
            elif found_arduino and byte_list is None:
                print("Error processing image")

            in_path, out_path = delete_image(in_path, out_path)

        ###################################  part 3   ##########################################
        ####################### arduino working & ready for new session #######################
        ########################################################################################
        elif found_arduino and not arduino_done:  # check if the arduino is done processing the previous image and ready to receive the next image
            if arduino.in_waiting > 0:  # if there is data in the serial buffer
                received_data = arduino.readline().decode().rstrip()
                # print("Received from Arduino:", received_data) # print the response from the arduino (for debugging)
                arduino_done = True  # the arduino is done processing the image and ready to receive the next image

    # Fin de programme
    if found_arduino:
        arduino.close()
    if not is_folder_created and not save_picture:
        try:
            os.rmdir(folder_name)
        except Exception as e:
            print(f"Error deleting folder: {e}")
    cap.release()
    cv2.destroyAllWindows()
    pygame.quit()


if __name__ == "__main__":
    main()

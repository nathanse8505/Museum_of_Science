"""
Filename: main.py
Purpose: main functions for the Droping Drops UI
"""
from pygame.locals import *
from arduino import *
from image_processing import *
from logs_Drop import *
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "hide" # to suppress Pygame welcome message
os.environ["OPENCV_LOG_LEVEL"]="SILENT"

def main():
    # logger
    logger = get_logger()
    logger.info("Starting Droping Drops UI")
    log_arduino = False
    # Initialisation arduino
    arduino_port = find_arduino_port(logger=logger)  # find the serial port
    arduino, found_arduino = open_serial_connection(port=arduino_port,timeout=0.05,logger=logger)  # Open the serial port

    # Reset des buffers série si Arduino présent
    if found_arduino:
        log_arduino = space_time > 5   # if True, the program will print logs to the console. if the time between sending pictures is less than 5 seconds, no logs will be printed to avoid spamming the console
        arduino.reset_input_buffer()
        arduino.reset_output_buffer()
        time.sleep(0.1)

    # Initialisation caméra
    camera_on = True
    camera_working = False
    cap = None
    threshold = 80  # 60-80 is a good value for the threshold to convert the image to black and white. value 0-255 (higher values will make the image darker)
    exposure = get_current_control(exposure_n)
    wb_temp = get_current_control(wb_temp_n)
    gain = get_current_control(gain_n)

    while not camera_working:
        camera_working, cap = camera_init()

    empty_captures_in_a_row = 0  # count the number of empty images in a row, for the idle mode when no hand is detected in the image
    last_capture = time.time()  # the time of the last capture

    # Initialisation Pygame
    screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN)
    pygame.display.set_caption("Camera")

    # Initialisation dossier et variables
    sample_index = 0
    arduino_done = True
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
                elif event.key == K_DOWN:
                    exposure -= 20
                    set_manual_controls(exposure, wb_temp, gain)
                elif event.key == K_UP:
                    exposure += 20
                    set_manual_controls(exposure, wb_temp, gain)
                elif event.key == K_MINUS:
                    gain -= 2
                    set_manual_controls(exposure, wb_temp, gain)
                elif event.key == K_PLUS:
                    gain += 2
                    set_manual_controls(exposure, wb_temp, gain)
                elif event.key == K_p:
                    if not camera_working:
                        camera_working, cap = camera_init()
                        if not camera_working:
                            continue
                    camera_on = not camera_on
                    print(f"Camera is {'on' if camera_on else 'off'}")
                    last_capture = time.time()


        if not send_parameters and found_arduino:
            send_variables_to_arduino(arduino)
            send_parameters = True
            time.sleep(0.2)
        ###################################  part 1   ####################################
        ################################### P information ################################
        if not camera_on:
            screen.fill((0, 0, 0))
            msg_on_screen(screen, camera_working, found_arduino,threshold)
            pygame.display.flip()
        ###################################  part 2   ####################################
        ##################################  camera working ###############################
        elif camera_on and (time.time() - last_capture) >= space_time and arduino_done:
            # if the time since the last capture is more than 'space_time' seconds
            # and the arduino is done processing the previous image, take a new picture and send it to the arduino
            img, camera_working, byte_list, black_percentage = main_process(cap, screen, camera_working, log_arduino, threshold, logger)
            if not camera_working or img is None:
                print("Camera not working. Skipping this cycle.")
                logger.info("Camera not working. Skipping this cycle.")
                continue
            ################################## send data from camera to arduino ################################
            ####################################################################################################
            # if the image is not empty (black_percentage > empty_image_threshold), send the image to the arduino
            if found_arduino and byte_list is not None and black_percentage is not None and black_percentage > empty_image_threshold:
                empty_captures_in_a_row = 0  # reset the empty captures counter if the image is not empty
                reset_buffer_arduino(arduino, log_arduino)
                VALID = send_data_to_arduino(arduino, byte_list, log_arduino)
                if not VALID:
                    continue
                last_capture = time.time()  # reset the last capture time
                arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)

            ################################## send data from idle to arduino ##################################
            ####################################################################################################
            elif found_arduino and byte_list is not None and black_percentage is not None:
                empty_captures_in_a_row += 1  # increment the empty captures counter
                if empty_captures_in_a_row >= empty_captures_before_idle:  # if the empty captures counter is more than 'empty_captures_before_idle', go to idle mode
                    if log_arduino:
                        print("Image is empty, sending sample image to arduino...")
                        logger.info("Image is empty, sending sample image to arduino...")

                    in_path_idle = os.path.join(idle_folder_name, idle_images[sample_index])
                    image_idle = cv2.imread(in_path_idle)  # read the image from the input path
                    byte_list, _ = process_image(image_idle, log_arduino, logger, threshold)  # process the sample image
                    reset_buffer_arduino(arduino, log_arduino)
                    VALID = send_data_to_arduino(arduino, byte_list, log_arduino)
                    if not VALID:
                        continue
                    sample_index += 1  # increment the sample index to send the next sample image next time
                    if sample_index >= len(idle_images):  # if the sample index is more than the number of sample images, reset the sample index to the first image
                        sample_index = 0
                else:
                    # if the image is empty but the empty captures counter is less then 'empty_captures_before_idle', drop the last image again (which is already in the arduino's buffer and not empty)
                    send_drop_key(arduino)  # drop the image in the arduino's buffer (last non-empty image)
                arduino_done = False  # the arduino is not done processing the image yet (it will send a response to the computer when it is done)
                last_capture = time.time()  # reset the last capture time

            ########################### error processing image. arduino is found ###############################
            ####################################################################################################
            elif found_arduino and byte_list is None:
                print("Error processing image")
                logger.info("Error processing image")


        ###################################  part 3   #########################################
        ####################### arduino working & ready for new session #######################
        #######################################################################################
        elif found_arduino and not arduino_done:  # check if the arduino is done processing the previous image and ready to receive the next image
            if arduino.in_waiting > 0:  # if there is data in the serial buffer
                received_data = arduino.readline().decode().rstrip()
                print("Received from Arduino:", received_data) # print the response from the arduino (for debugging)
                logger.info("Received from Arduino:", received_data)
                arduino_done = True  # the arduino is done processing the image and ready to receive the next image

    # Fin de programme
    if found_arduino:
        arduino.close()
    if not is_folder_created and not save_picture:
        try:
            os.rmdir(folder_name)
        except Exception as e:
            print(f"Error deleting folder: {e}")
            logger.info(f"Error deleting folder: {e}")
    cap.release()
    cv2.destroyAllWindows()
    pygame.quit()


if __name__ == "__main__":
    main()

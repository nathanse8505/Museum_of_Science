import time
from datetime import datetime
from pygame.locals import *
from const import *
from arduino import *
from image_processing import *

# Initialisation arduino
arduino,found_arduino,log = open_serial()
# Reset des buffers série si Arduino présent
if found_arduino:
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()
    time.sleep(0.1)

# Initialisation caméra
camera_on = True
camera_working , cap = camera_init()

# Initialisation Pygame
screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN)
pygame.display.set_caption("Camera")

# Initialisation dossier et variables
is_folder_created = os.path.exists(folder_name)
sample_index = 0
arduino_done = True
idle = False
send_parameters = False
last_capture = time.time()
empty_captures_in_a_row = 0
running = True



# Boucle principale
while running:
    img = take_picture(cap, camera_working)
    time.sleep(0.002)

    if not send_parameters and found_arduino:
        send_variables_to_arduino(arduino, output_height, valve_on_time, drawing_depth)
        send_parameters = True
        time.sleep(0.2)

    for event in pygame.event.get():
        if event.type == QUIT:
            running = False
        elif event.type == KEYDOWN:
            if event.key in (K_ESCAPE, K_q):
                running = False
            elif event.key == K_p:
                if not camera_working:
                    try:
                        cap = cv2.VideoCapture(camera_index)
                        if cap.isOpened():
                            camera_working = True
                            print("Camera is ready")
                    except Exception:
                        continue
                camera_on = not camera_on
                print(f"Camera is {'on' if camera_on else 'off'}")
                last_capture = time.time()
            elif event.key == K_RIGHT:
                threshold += 10
                print(f"Threshold: {threshold}")
            elif event.key == K_LEFT:
                threshold -= 10
                print(f"Threshold: {threshold}")


    # Affichage info si caméra off
    if not camera_on:
        screen.fill((0, 0, 0))
        msg_on_screen(font, screen, screen_width, screen_height, font_size, found_arduino, camera_working, threshold)
        pygame.display.flip()

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

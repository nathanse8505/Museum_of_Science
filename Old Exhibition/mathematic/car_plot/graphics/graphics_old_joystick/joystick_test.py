import pygame
import time

pygame.init()
pygame.joystick.init()

joystick = None
reconnect_waiting = False

def try_connect():
    pygame.joystick.quit()
    pygame.joystick.init()
    if pygame.joystick.get_count() > 0:
        js = pygame.joystick.Joystick(0)
        js.init()
        print("Joystick found.")
        return js
    return None

# Try initial connect
joystick = try_connect()
if not joystick:
    print("No joystick connected.")
    print("Trying to reconnect...")
    reconnect_waiting = True

try:
    while True:
        events = pygame.event.get()  # Must use this to receive device events

        # Check for device removal
        for event in events:
            if event.type == pygame.JOYDEVICEREMOVED:
                print("Joystick disconnected.")
                print("Trying to reconnect...")
                joystick = None
                reconnect_waiting = True

            elif event.type == pygame.JOYDEVICEADDED:
                if reconnect_waiting:
                    joystick = try_connect()
                    if joystick:
                        reconnect_waiting = False

        if joystick:
            try:
                axis_0 = joystick.get_axis(0)
                print(f"Axis 0 value: {axis_0:.4f}")
            except pygame.error:
                print("Joystick read error.")
                joystick = None
                reconnect_waiting = True

        time.sleep(0.05)

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    pygame.quit()

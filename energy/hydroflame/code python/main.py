"""
File: main.py
Purpose: Main file for the Hydrogen Rocket UI (no pygame)
"""

import time
from arduino import *
from logs import *
from consts import RECONNECT_INTERVAL, SERIAL_ERROR, FPS


class FPSLimiter:
    """Simple FPS limiter without pygame."""
    def __init__(self, fps: float):
        self.dt = 1.0 / float(fps)
        self.next_tick = time.perf_counter()

    def wait(self):
        """Sleep just enough to hit target FPS, then schedule next frame.

        - Rattrape un retard sans 'spiral of death' :
          si on est très en retard, on remet simplement la prochaine frame à maintenant + dt.
        """
        now = time.perf_counter()
        if self.next_tick > now:
            time.sleep(self.next_tick - now)
            now = time.perf_counter()

        # Planifier l’instant de la frame suivante
        self.next_tick += self.dt

        # Si on a pris du retard de plus d'une frame, recaler pour éviter l'accumulation
        if self.next_tick < now - self.dt:
            self.next_tick = now + self.dt


def main():
    """
    The main function for the Hydrogen Rocket UI
    """
    # logging setup
    logger = get_logger()
    logger.info("Starting HydroFlame UI (no pygame)")

    # arduino setup
    arduino_port = find_arduino_port(logger=logger)          # find the serial port
    ser = open_serial_connection(arduino_port, logger=logger)  # open the serial port
    last_time_tried_to_connect = time.time()

    # FPS limiter (remplace pygame.time.Clock)
    limiter = FPSLimiter(FPS)

    while True:
        # Lire le flux Arduino (ta fonction doit idéalement être non-bloquante ou avec timeout court)
        data_from_arduino = read_line(ser, logger=logger)

        # Si l'Arduino était connecté mais qu'on a une erreur de lecture -> considérer déconnecté
        if data_from_arduino == SERIAL_ERROR:
            print("Arduino disconnected! Going to default settings")
            print("Trying to reconnect to Arduino...")
            logger.info("Arduino disconnected... Going to default settings")
            logger.info("Trying to reconnect to Arduino...")
            ser = None

        # Tentative de reconnexion espacée
        if ser is None and (time.time() - last_time_tried_to_connect) > RECONNECT_INTERVAL:
            arduino_port = find_arduino_port(logger=logger)
            ser = open_serial_connection(arduino_port, logger=logger)
            last_time_tried_to_connect = time.time()

        # Si on a reçu des données valides, on les traite
        if data_from_arduino and data_from_arduino != SERIAL_ERROR:
            parse_data(data_from_arduino, logger=logger)

        # Limitation FPS (remplace clock.tick(FPS))
        limiter.wait()


if __name__ == "__main__":
    main()

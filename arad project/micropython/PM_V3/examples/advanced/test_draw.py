from sys import modules
from math import degrees, sin, cos
from time import sleep, sleep_ms
from pm import PM

pm = PM()
HEART_POLY = [
    (-26, 42), (-21, 43), (-17, 43), (-8, 41), (-3, 42), (2, 46),
    (5, 52), (5, 58), (2, 60), (-1, 58), (-2, 56), (-3, 55), (-4, 57),
    (-6, 59), (-9, 59), (-11, 57), (-11, 54), (-9, 50), (4, 41), (14, 44),
    (16, 47), (17, 50), (16, 52), (14, 53), (12, 51), (11, 52), (9, 53),
    (7, 51), (8, 48), (12, 43), (20, 41), (24, 43), (29, 42), (34, 40)]

print('''
Test drawings!
Press Ctrl-C to exit.
''')
try:
    pm.move_home()

    pm.draw_circle(0, 40, 10)

    # draw centered squares
    for i in range(3):
        size = 16 + i * 4
        pm.draw_rect(0, 40, size)

    # draw a hexagon
    pm.draw_regular_poly(0, 40, 20, 6)

    pm.draw_poly(HEART_POLY, False)

    pm.move_home()
except KeyboardInterrupt:
    pass
pm.set_enable(False)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')


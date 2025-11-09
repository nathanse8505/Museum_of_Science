from sys import modules
from pm import PM


HEART_POLY = [
    (-26, 42), (-21, 43), (-17, 43), (-8, 41), (-3, 42), (2, 46),
    (5, 52), (5, 58), (2, 60), (-1, 58), (-2, 56), (-3, 55), (-4, 57),
    (-6, 59), (-9, 59), (-11, 57), (-11, 54), (-9, 50), (4, 41), (14, 44),
    (16, 47), (17, 50), (16, 52), (14, 53), (12, 51), (11, 52), (9, 53),
    (7, 51), (8, 48), (12, 43), (20, 41), (24, 43), (29, 42), (34, 40)]


pm = PM()
pm._last_y += 1
pm.move_home()
pm.move_home()
last_c = 0
try:
    while True:
        c = (not pm.btn_l.value()) * 1 + (not pm.btn_r.value()) * 2
        if c and c != last_c:
            print('Press:', c)
            if c == 1:
                pm.rgb_leds.fill('red')
                pm.draw_poly(HEART_POLY, False)
                pm.move_home()
                pm.rgb_leds.off()
            elif c == 2:
                pm.rgb_leds.fill('blue')
                pm.draw_regular_poly(0, 40, 16, 3)
                pm.draw_regular_poly(0, 40, 16, 3, True, 180)
                pm.draw_line((-35, 60), (35, 60))
                pm.draw_line((-35, 20), (35, 20))
                pm.move_home()
                pm.rgb_leds.off()
        last_c = c
except KeyboardInterrupt:
    pass
pm.set_enable(False)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press ctrl-d to soft-reset.
''')

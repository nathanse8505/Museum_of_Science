from sys import modules
from pm import PM, SERVO_ANGLES, SERVO_OFFSET
from ansi import *
from utils import set_param
from secrets import SERVO_US


_STEP = sgr(SGR_CYAN, SGR_BOLD)
pm = PM()


try:
    servo_us = [2030, 2080, 970, 1000] # lh rv lv rh
    print('''
{1}Photochromic manipulator calibration:{0}
{3}Press Ctrl-C to exit.{0}

{2}1. Make sure no arm is connected and press <ENTER>!{0}'''.format(
        SGR_END, sgr(SGR_GREEN, SGR_BOLD, SGR_UNDERLINE),
        _STEP, sgr(SGR_RED, SGR_BOLD)))

    input() # wait for ENTER
    pm.servo_l._min_us = pm.servo_r._min_us = 630
    pm.servo_l._max_us = pm.servo_r._max_us = 2500
    pm.servo_l.set_us(1520)
    pm.servo_r.set_us(1520) 
#     pm.set_angles(43, 43) # arm is 34mm, add 10mm from its center to edge resulting with 43°

    print('''{1}2. Please connect the arms as follows:{0}
{2}   +---------------------------------------+
   |                                       |
   |                  (O)                  |
   |                /     \                |
   |             /           \             |
   |          /                 \          |
   |       /                       \       |
   |    /                             \    |
   | /                                   \ |
   (O)                                   (O)
   | \                                   / |
   |   \                               /   |
   |     \                           /     |
   |       \                       /       |
   |         \                   /         |
   |           \               /           |
   +-----------(O)----------(O)------------+{0}
   {1}Press <ENTER> when done!{0}'''.format(
       SGR_END, _STEP, sgr(SGR_YELLOW, SGR_BOLD)))

    input() # wait for ENTER

    for i in range(4):
        print('''{1}{2}. Please align the {3} arm {4} as follows:{0}
       Press <+> <ENTER> to rotate the arm counter clockwise.
       Press <-> <ENTER> to rotate the arm clockwise.
       Press <ENTER> when done!'''.format(
           SGR_END, _STEP, i + 3,
           'right' if i & 1 else 'left',
           'vertically' if i % 3 else 'horizontally'))

        while True:
            pm.servo_l.set_us(servo_us[i & 2])
            pm.servo_r.set_us(servo_us[(i & 2) + 1])
            c = input()
            if c == '':
                break
            elif c == '+':
                servo_us[i] += 10
            elif c == '-':
                servo_us[i] -= 10

    lf = (servo_us[0] - servo_us[2]) / 90
    rf = (servo_us[1] - servo_us[3]) / 90
    l_min_us = int(servo_us[2] - lf * (SERVO_ANGLES - SERVO_OFFSET - 90))
    l_max_us = int(l_min_us + lf * SERVO_ANGLES)
    r_min_us = int(servo_us[3] - rf * SERVO_OFFSET)
    r_max_us = int(r_min_us + rf * SERVO_ANGLES)
    servo_us = [l_min_us, l_max_us, r_min_us, r_max_us]

    pm.servo_l._min_us = servo_us[0]
    pm.servo_l._max_us = servo_us[1]
    pm.servo_l._angle_us = (servo_us[1] - servo_us[0]) / SERVO_ANGLES
    pm.servo_r._min_us = servo_us[2]
    pm.servo_r._max_us = servo_us[3]
    pm.servo_r._angle_us = (servo_us[3] - servo_us[2]) / SERVO_ANGLES
    pm.set_angles(13, 13) # home

    print('{1}6. Press <ENTER> to save calibration!{0}'.format(SGR_END, _STEP))
    input() # wait for ENTER

    set_param('SERVO_US', [l_min_us, l_max_us, r_min_us, r_max_us])
except KeyboardInterrupt:
    pass
pm.set_enable(False)
modules.clear() # make sure we can re-import the example!
print('''
Done!
Can press Ctrl-D to soft-reset.
''')

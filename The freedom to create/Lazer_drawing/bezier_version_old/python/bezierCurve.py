import math
import logging
import serial
import struct
import time
from datetime import datetime
from consts import *
from logging.handlers import RotatingFileHandler

pygame.init()

port = '/dev/ttyUSB0'
baudrate = 115200
arduino = None

# logging setup
logging.basicConfig(filename=LOG_FILE_PATH, filemode='a', level=logging.INFO)
handler = RotatingFileHandler(LOG_FILE_PATH, maxBytes=37500, backupCount=100)
logger = logging.getLogger("Rotating Log")
formatter = logging.Formatter('%(asctime)s.%(msecs)03d;%(message)s', '%Y-%m-%d %H:%M:%S')
handler.setFormatter(formatter)
logger.propagate = False
logger.addHandler(handler)
logger.info('--------------- PROGRAM START ---------------')

try:
    arduino = serial.Serial(port, baudrate)
    found_arduino = True
    print("Found Arduino")
    logger.info('Found Arduino')
except Exception as e:
    print(f"Serial port error: {e}")
    print('ARDUINO NOT CONNECTED')
    logger.error('Arduino not connected')

# fonts for text
font_style2 = pygame.font.SysFont("calibri", 45)
font_style2.bold = True
font_style = pygame.font.SysFont("calibri", 30)
font_style.bold = True


class Button(object):
    def __init__(self, pos, size, color, coloron, img, imgon, function):
        self.pos = pos
        self.size = size
        self.color = color
        self.coloron = coloron
        self.img = img  #image when not clicking
        self.imgon = imgon  #image when clicking
        self.tempimg = img  #also saves img but not changing (tempimg = temporary image)
        self.function = function
        self.done = True
#        self.last_pos_mouse_up = [0,0]

    def check(self):
        global buttons_enabled
        mouse = pygame.mouse.get_pos()
        click = pygame.mouse.get_pressed()
        rect = pygame.Rect(self.pos[0], self.pos[1], self.size[0], self.size[1])
        # on_button = rect.collidepoint(mouse) # good for hovering image but this is a touch screen so not needed
        on_button = rect.collidepoint(mouse)
        if click[0] == 0:
            self.img = self.tempimg
            # pygame.draw.rect(screen, self.color, rect)
            if self.img is not None:
                screen.blit(self.img, self.img.get_rect(center=rect.center))
            else:
                pygame.draw.rect(screen, self.color, rect)
            self.done = True
        elif on_button and self.done:
            # pygame.draw.rect(screen, self.coloron, rect)
            if self.imgon is not None:
                screen.blit(self.imgon, self.imgon.get_rect(center=rect.center))
            else:
                pygame.draw.rect(screen, self.coloron, rect)
            self.done = False
            self.img = self.imgon
            if self.function is not None:
                self.function()
            buttons_enabled = False
#        elif on_button:
#            pygame.draw.rect(screen, self.coloron, rect)
#            screen.blit(self.imgon, self.imgon.get_rect(center=rect.center))
        else:
            # pygame.draw.rect(screen, self.color, rect)
            if self.img is not None:
                screen.blit(self.img, self.img.get_rect(center=rect.center))
            else:
                pygame.draw.rect(screen, self.color, rect)

    def draw_static(self):
        rect = pygame.Rect(self.pos[0], self.pos[1], self.size[0], self.size[1])
        # pygame.draw.rect(screen, self.color, rect)
        if self.img is not None:
            screen.blit(self.img, self.img.get_rect(center=rect.center))
        else:
            pygame.draw.rect(screen, self.color, rect)


# object of a curve, defined by 4 points
class BezierCurve(object):
    def __init__(self, p0, p1, p2, p3, moveable, color, width):
        self.vertices = [p0, p1, p2, p3]
        self.moveable = moveable
        self.color = color
        self.width = width

    def compute_bezier_points(self, numPoints=None):
        if numPoints is None:
            numPoints = 30
        if numPoints < 2 or len(self.vertices) != 4:
            return None

        result = []

        b0x = self.vertices[0][0]
        b0y = self.vertices[0][1]
        b1x = self.vertices[1][0]
        b1y = self.vertices[1][1]
        b2x = self.vertices[2][0]
        b2y = self.vertices[2][1]
        b3x = self.vertices[3][0]
        b3y = self.vertices[3][1]

        # Compute polynomial coefficients from Bezier points
        ax = (-b0x + 3 * b1x + -3 * b2x + b3x)
        ay = (-b0y + 3 * b1y + -3 * b2y + b3y)

        bx = (3 * b0x + -6 * b1x + 3 * b2x)
        by = (3 * b0y + -6 * b1y + 3 * b2y)

        cx = (-3 * b0x + 3 * b1x)
        cy = (-3 * b0y + 3 * b1y)

        dx = (b0x)
        dy = (b0y)

        # Set up the number of steps and step size
        numSteps = numPoints - 1  # arbitrary choice
        h = 1.0 / numSteps  # compute our step size

        # Compute forward differences from Bezier points and "h"
        pointX = dx
        pointY = dy

        firstFDX = (ax * (h * h * h) + bx * (h * h) + cx * h)
        firstFDY = (ay * (h * h * h) + by * (h * h) + cy * h)

        secondFDX = (6 * ax * (h * h * h) + 2 * bx * (h * h))
        secondFDY = (6 * ay * (h * h * h) + 2 * by * (h * h))

        thirdFDX = (6 * ax * (h * h * h))
        thirdFDY = (6 * ay * (h * h * h))

        # Compute points at each step
        result.append((int(pointX), int(pointY)))

        for i in range(numSteps):
            pointX += firstFDX
            pointY += firstFDY

            firstFDX += secondFDX
            firstFDY += secondFDY

            secondFDX += thirdFDX
            secondFDY += thirdFDY

            result.append((int(pointX), int(pointY)))

        return result

    def draw(self, surface = None):
        global screen
        # Draw control points
        control_points = self.vertices
        if show_control_lines and self.moveable and surface is None:
            # Draw control "lines"
            # pygame.draw.lines(screen, lightgray, False, [(x[0], x[1]) for x in control_points])
            pygame.draw.lines(screen, control_lines_color, False,
                              [(x[0], x[1]) for x in control_points[:2]], controlLineWidth)
            pygame.draw.lines(screen, control_lines_color, False,
                              [(x[0], x[1]) for x in control_points[2:]], controlLineWidth)

            for p in control_points:
                # draw start and end points of the curve in circleColor1 and slope points in circleColor2
                if control_points.index(p) == 1 or control_points.index(p) == 2:
                    # pygame.draw.circle(screen, circleColor2, (int(p[0]), int(p[1])), circleRadius2)
                    # draw the picture if the double arrow
                    screen.blit(pic_doubleArrow, (int(p[0]) - doubleArrowSize[0]/2, int(p[1]) - doubleArrowSize[1]/2))
                    pygame.draw.circle(screen, circleColor2, (int(p[0]), int(p[1])), circleRadius2)
                elif control_points.index(p) == 0:
                    pygame.draw.circle(screen, circleColor0, (int(p[0]), int(p[1])), circleRadius0)
                else:
                    pygame.draw.circle(screen, circleColor1, (int(p[0]), int(p[1])), circleRadius1)
        # Draw bezier curve
        b_points = self.compute_bezier_points()
        pygame.draw.lines(screen if surface is None else surface, self.color, False, b_points, self.width)

    def get_length(self):
        # get the spatial length of the curve
        b_points = self.compute_bezier_points()
        length = 0
        for i in range(len(b_points)-1):
            length += distance(b_points[i], b_points[i+1])
        return length


curves = []  # list of all curves
curves_to_send = []
contour = []
selected_curve = None
selected = None  # The currently selected point
show_control_lines = True
show_picture = False
show_estimated_time = False
estimated_time = 0
last_send_time = 0
delta = [0, 0, 0]
delta_outside = [0, 0, 0]
auto_run = False
run_index = 0
sample_index = 0
letter_index = 0
info_time_start = time.time()
preview_time_start = time.time()
idle_clock = time.time()
idle_clock_draw = time.time()
last_time_dc_motor = time.time()
dc_motor_on = False
red_border_on = False

screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)


# screen = pygame.display.set_mode((screen_width, screen_height))
def insert_drawing_to_surface(surf):
    global curves
    global contour
    for i in range(len(curves)):
        new_curve = BezierCurve([int((curves[i].vertices[0][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((curves[i].vertices[0][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((curves[i].vertices[1][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((curves[i].vertices[1][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((curves[i].vertices[2][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((curves[i].vertices[2][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((curves[i].vertices[3][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((curves[i].vertices[3][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                False, curveColor, curveWidth)
        new_curve.draw(surface=surf)
    for i in range(len(contour)):
        new_curve = BezierCurve([int((contour[i].vertices[0][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((contour[i].vertices[0][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((contour[i].vertices[1][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((contour[i].vertices[1][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((contour[i].vertices[2][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((contour[i].vertices[2][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                [int((contour[i].vertices[3][0] - borderLineX) / (borderLine2X - borderLineX) * saved_image_width),
                                 int((contour[i].vertices[3][1] - borderLineHeight) / (
                                             borderLine2Height - borderLineHeight) * saved_image_height)],
                                False, contourColor, contourWidth)
        new_curve.draw(surface=surf)

def save_drawing_img():
    saving_surface = pygame.Surface((saved_image_width, saved_image_height))
    saving_surface.fill(bgColor)
    insert_drawing_to_surface(saving_surface)
    currentDirectory = os.getcwd()
    drawings_dir = os.path.join(currentDirectory, r'drawings')
    if not os.path.exists(drawings_dir):
        os.makedirs(drawings_dir)
    # save the image with the current date and time in the folder drawings_dir png format
    pygame.image.save(saving_surface, os.path.join(drawings_dir, datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + '.png'))


def check_buttons():
    global buttons
    global buttons_enabled
    if buttons_enabled:
        for button in buttons:
            button.check()
    else:
        for button in buttons:
            button.draw_static()



def check_dc_motor():
    global dc_motor_on
    global last_time_dc_motor
    global ButtonPrint
    global send_to_arduino
    if dc_motor_on:
        if time.time() - last_time_dc_motor > MAX_DC_MOTOR_TIME:
            dc_motor_on = False
            send_to_arduino = False
            ButtonPrint.tempimg = pic_buttonPrint
            ButtonPrint.img = pic_buttonPrint
            ButtonPrint.imgon = pic_buttonPressedPrint


def check_arduino(log_flag=True):
    global curves_to_send
    global waiting
    global last_time
    global drawing_curve
    global curve_index
    global send_to_arduino
    global estimated_time
    global show_estimated_time
    global logger
    global idle_clock_draw
    global idle_clock
    global idle_mode
    global dc_motor_on
    global last_time_dc_motor

    if waiting[1]:
        if arduino.in_waiting > 0:
            received_data = arduino.readline().decode('utf-8').rstrip()
            waiting[1] = False
            print("arduino finished drawing curve " + str(curve_index))
            # if log_flag:
            #     logger.info("arduino finished drawing curve " + str(curve_index))
            curve_index += 1
            if curve_index >= len(curves_to_send):
                dc_motor_on = True
                last_time_dc_motor = time.time()
                # send a key that will tell the arduino to stop reading
                print("sent all curves")
                if log_flag:
                    # logger.info("sent all curves")
                    logger.info("arduino finished drawing all curves successfully")
                send_one_number(end_key)
                print("sent end key for arduino")
                # if log_flag:
                #     logger.info("sent end key for arduino")
                estimated_time = 0
                show_estimated_time = False
                idle_clock = time.time()
                idle_clock_draw = time.time()
                if idle_mode:
                    clear_all(log_flag = False)
                    heart(log_flag = False)
                    add_curve0(log_flag = False)
                return True
            else:
                drawing_curve = False
        elif time.time() - last_time[1] > MAX_DRAWING_TIME_FOR_ARDUINO:
            print("ERROR: arduino didn't send drawing done key")
            logger.error("ERROR: arduino didn't send drawing done key")
            return False
        return True

    if waiting[0]:
        if arduino.in_waiting > 0:
            received_data = arduino.readline().decode('utf-8').rstrip()
            waiting[0] = False
            waiting[1] = True
            last_time[1] = time.time()
            print("arduino finished reading curve " + str(curve_index))
            # if log_flag:
            #     logger.info("arduino finished reading curve " + str(curve_index))
        elif time.time() - last_time[0] > MAX_TIME_WAITING_FOR_ARDUINO:
            print("ERROR: arduino didn't send reading done key")
            logger.error("ERROR: arduino didn't send reading done key")
            return False
        return True

    if not drawing_curve:
        curve = curves_to_send[curve_index]
        for point in curve.vertices:
            send_one_number(point[1])
            send_one_number(point[0])
        # if log_flag:
        #     logger.info("sent curve " + str(curve_index))
        drawing_curve = True
        waiting[0] = True  # waiting for arduino to send key that will tell us it finished reading the curve
        waiting[1] = False  # NOT waiting for arduino to send key that will tell us it finished drawing the curve
        last_time[0] = time.time()
    return True


# send the points as ratio between place and screen size
def send_to_laser(log_flag=True):
    global curves
    global curves_to_send
    global drawing_curve
    global curve_index
    global send_to_arduino
    global ButtonPrint
    global found_arduino
    global contour
    global estimated_time
    global show_estimated_time
    global last_send_time
    global logger
    global idle_mode

    if send_to_arduino:
        return False
    if log_flag:
        logger.info("clicked on print button")
    # send all of the curve's vertices to log file in one line
    if log_flag:
        str_log = f"{len(curves)};["
        for curve in curves:
            str_log += str(curve.vertices) + ","
        str_log = str_log[:-1] + "]"
        logger.info(str_log)

    if not found_arduino:
        print("ERROR: No Laser Connected")
        logger.error("ERROR: No Laser Connected")
        return False
    if len(curves) == 0:
        print("No curves to send")
        # if log_flag:
        #     logger.warning("Clicked on print button with no curves to send")
        return True
    save_drawing_img()
    estimated = 0 # estimated time to finish drawing in seconds
    last_send_time = time.time()
    curves_to_send = curves.copy()
    # calculate the estimated time to finish drawing
    last_point = [centerInsideBorders[0]-cuttingAreaWidth/2, centerInsideBorders[1]-cuttingAreaHeight/2]
    for curve in curves_to_send:
        estimated += curve.get_length() * pulse_per_pixel[0] * LASER_ON_RATE / 1000
        estimated += distance(curve.vertices[0], last_point) * pulse_per_pixel[0] * LASER_OFF_RATE / 1000
        last_point = curve.vertices[-1]
    print(f"estimated time to finish drawing (WITHOUT contour): {estimated:.2f} seconds")
    # add all the curves in the contour to curves_to_send
    times = 1 # number of times to draw the contour
    for i in range(times):
        for curve in contour:
            curves_to_send.append(curve)
            estimated += curve.get_length() * pulse_per_pixel[0] * CONTOUR_RATE / 1000
            estimated += distance(curve.vertices[0], last_point) * pulse_per_pixel[0] * LASER_OFF_RATE / 1000
            last_point = curve.vertices[-1]
    print(f"estimated time to finish drawing (with contour): {estimated:.2f} seconds")
    if log_flag:
        logger.info(f"estimated time to finish drawing (with contour): {estimated:.2f} seconds")
    estimated_time = estimated
    show_estimated_time = True
    msgEstimatedTime(estimated_time)
    if not send_one_number(starting_key):  # first, send a key that will tell the arduino to start reading
        return False
    print("sent starting key to laser")
    # if log_flag:
    #     logger.info("sent starting key to laser")
    # then, send the number of curves
    if not send_one_number(-len(curves_to_send)):
        return False
    if not send_one_number(-times*len(contour)):
        return False
    print("sent number of curves and contour")
    # if log_flag:
    #     logger.info("sent number of curves and contour")
    drawing_curve = False
    curve_index = 0
    send_to_arduino = True
    # change the picture of the send to laser button
    ButtonPrint.img = pic_buttonOffPrint
    ButtonPrint.imgon = pic_buttonOffPrint
    ButtonPrint.tempimg = pic_buttonOffPrint
    return True


def send_one_number(value):
    try:
        byte_value = bytearray(struct.pack("f", value))  # Convert float to bytes
        arduino.write(byte_value)
        arduino.flush()
    except Exception:
        print(str(value) + "not sent")
        logger.error(str(value) + "NOT SENT")
        return False
    print("sent " + str(value))
    return True


def take_control():
    global arduino
    print("taking control over Arduino...")
    logger.info("taking control over Arduino...")
    pytxt = "PY\n"
    try:
        arduino.write(pytxt.encode())
    except:
        print("Error: python failed taking control over arduino - use another serial monitor")
        logger.error("Error: python failed taking control over arduino - use another serial monitor")
        return False
    print("success!")
    logger.info("success!")
    return True


def heart(log_flag=True):
    global contour
    global ButtonSquare
    global ButtonHeart
    global ButtonDrop
    global logger
    ButtonSquare.tempimg = pic_buttonSquare
    ButtonSquare.img = pic_buttonSquare
    ButtonHeart.tempimg = pic_buttonPressedHeart
    ButtonDrop.tempimg = pic_buttonDrop
    ButtonDrop.img = pic_buttonDrop
    contour = []
    for i in range(len(contour_heart)):
        add_contour(contour_heart[i][0], contour_heart[i][1], contour_heart[i][2], contour_heart[i][3])
    if log_flag:
        logger.info("clicked on heart contour")

def sqaure(log_flag=True):
    global contour
    global ButtonSquare
    global ButtonHeart
    global ButtonDrop
    global logger
    ButtonSquare.tempimg = pic_buttonPressedSquare
    ButtonHeart.tempimg = pic_buttonHeart
    ButtonHeart.img = pic_buttonHeart
    ButtonDrop.tempimg = pic_buttonDrop
    ButtonDrop.img = pic_buttonDrop
    contour = []
    for i in range(len(contour_square)):
        add_contour(contour_square[i][0], contour_square[i][1], contour_square[i][2], contour_square[i][3])
    if log_flag:
        logger.info("clicked on square contour")

def drop(log_flag=True):
    global contour
    global ButtonSquare
    global ButtonHeart
    global ButtonDrop
    global logger
    ButtonSquare.tempimg = pic_buttonSquare
    ButtonSquare.img = pic_buttonSquare
    ButtonHeart.tempimg = pic_buttonHeart
    ButtonHeart.img = pic_buttonHeart
    ButtonDrop.tempimg = pic_buttonPressedDrop
    contour = []
    for i in range(len(contour_drop)):
        add_contour(contour_drop[i][0], contour_drop[i][1], contour_drop[i][2], contour_drop[i][3])
    if log_flag:
        logger.info("clicked on drop contour")

def preview(log_flag=True):
    global show_control_lines
    global logger
    global preview_time_start
    show_control_lines = False
    preview_time_start = time.time()
    if log_flag:
        logger.info("clicked on preview")


def add_curve0(log_flag=True):
    global curves
    if len(curves) < maxCurves:
        add_curve()
    else:
        print("cannot add more curves")
#        if log_flag:
#            logger.warning("clicked on add curve button when max curves reached")


def msgNumCurves(num):
    if num > 0:
        value = font_style2.render("ורתונ םיווק " + str(num), True, black)
    else:
        value = font_style2.render("תומוקע ורתונ אל", True, red)
    text_rect = value.get_rect(center=((borderLine2X+screen_width)/2,borderLineHeight/2))
    screen.blit(value, text_rect)

def msgEstimatedTime(time):
    value = font_style.render(f" תוינש {time:.1f} :ךרעומ ןמז ", True, black)
    text_rect = value.get_rect(center=((borderLine2X+screen_width)/2,buttonPrintPosition[1]+1.5*buttonPrintSize[1]))
    screen.blit(value, text_rect)

# def msg():
#     global screen
#     value = font_style2.render("!תודוקנה תועצמאב וקה תרוצ תא ונש", True, white)
#     text_rect = value.get_rect(center=(screen_width / 2, 50))
#     screen.blit(value, text_rect)


# distance is NOT sqrt of sum of squares because laser is going in zigzag and not in straight line
def distance(point1, point2):
    # return math.sqrt((point1[0] - point2[0]) ** 2 + (point1[1] - point2[1]) ** 2)
    return max(abs(point1[0] - point2[0]), abs(point1[1] - point2[1]))


# Function to rotate a point around a fixed point
def rotate_point(point, angle, center_p):
    x, y = point
    cx, cy = center_p

    # Translate the point and center to the origin
    translated_x = x - cx
    translated_y = y - cy

    # Perform the rotation
    new_x = translated_x * math.cos(angle) - translated_y * math.sin(angle)
    new_y = translated_x * math.sin(angle) + translated_y * math.cos(angle)

    # Translate the point back to its original position
    rotated_x = new_x + cx
    rotated_y = new_y + cy

    return int(rotated_x), int(rotated_y)

# clear the LAST curve
def clear(log_flag=True):
    global curves
    global selected_curve
    global selected
    global delta
    global delta_outside
    global logger
    if len(curves) == 0:
#        if log_flag:
#            logger.warning("clicked on delete button with no curves on screen")
        return
    curves.pop()
    selected_curve = None
    selected = None
    # if log_flag:
    #     logger.info("deleted curve: now " + str(len(curves)) + " curves")
    if len(curves) == 0:  # just in case something doesnt work
        delta = [0, 0, 0]
        delta_outside = [0, 0, 0]
        return
    if len(curves) > MAX_LINES_GENERATED_INSIDE_CONTOUR:
        if delta_outside[0] == 0:
            if delta_outside[2] > 0:
                delta_outside[2] -= 1
            delta_outside[1] = delta_outside[2] * delta1Z
            delta_outside[0] = delta1X * (MAX_LINES_PER_ROW_OUTSIDE_CONTOUR-1)
        else:
            delta_outside[0] -= delta1X
            delta_outside[1] -= delta1Y
        return
    # if got to here, then len(curves) <= MAX_LINES_GENERATED_INSIDE_CONTOUR
    delta_outside = [0, 0, 0] # reset delta_outside just in case
    if len(curves) == MAX_LINES_GENERATED_INSIDE_CONTOUR:
        return
    if delta[0] == 0:
        if delta[2] > 0:
            delta[2] -= 1
        delta[1] = delta[2] * delta0Z
        delta[0] = delta0X * (MAX_LINES_PER_ROW-1)
        return
    delta[0] -= delta0X
    delta[1] -= delta0Y


def clear_all(log_flag=True):
    global curves
    global selected_curve
    global selected
    global delta
    global delta_outside
    selected_curve = None
    selected = None
    curves.clear()
    if log_flag:
        logger.info("deleted all curves")
    delta = [0, 0, 0]
    delta_outside = [0, 0, 0]


def add_curve(log_flag=True):
    global curves
    global selected_curve
    global selected
    global delta
    global delta_outside
    global logger

    if len(curves) < MAX_LINES_GENERATED_INSIDE_CONTOUR:
        deltaX = delta[0]
        deltaY = delta[1]
        new_curve = BezierCurve([x0 - deltaX, y0 - deltaY], [x1 - deltaX, y1 - deltaY], [x2 - deltaX, y2 - deltaY],
                                [x3 - deltaX, y3 - deltaY], True, curveColor, curveWidth)
        selected = None
        curves.append(new_curve)

        delta[0] += delta0X
        delta[1] += delta0Y

        if delta[0] >= delta0X * MAX_LINES_PER_ROW:
            delta[2] += 1
            delta[0] = 0
            delta[1] = delta[2] * delta0Z
        if len(curves) == MAX_LINES_GENERATED_INSIDE_CONTOUR:
            delta_outside = [0, 0, 0]
    else:
        deltaX = delta_outside[0]
        deltaY = delta_outside[1]
        new_curve = BezierCurve([x0_outside - deltaX, y0_outside - deltaY],
                                [x1_outside - deltaX, y1_outside - deltaY],
                                [x2_outside - deltaX, y2_outside - deltaY],
                                [x3_outside - deltaX, y3_outside - deltaY], True, curveColor, curveWidth)
        selected = None
        curves.append(new_curve)
        # move the curve by delta0X and delta0Y to the left and up
        delta_outside[0] += delta1X
        delta_outside[1] += delta1Y
        if delta_outside[0] >= delta1X * MAX_LINES_PER_ROW_OUTSIDE_CONTOUR:
            delta_outside[2] += 1
            delta_outside[0] = 0
            delta_outside[1] = delta_outside[2] * delta1Z

#    if log_flag:
#        logger.info("added curve: now " + str(len(curves)) + " curves")

def add_contour(p0, p1, p2, p3):
    global contour
    new_contour = BezierCurve(p0, p1, p2, p3, False, contourColor, contourWidth)
    contour.append(new_contour)

def insert_sample(index_sample):
    global curves
    global samples
    if index_sample == 0:
        heart(log_flag=False)
    elif index_sample == 1:
        drop(log_flag=False)
    elif index_sample == 2:
        sqaure(log_flag=False)
    for i in range(len(samples[index_sample])):
        new_curve = BezierCurve([samples[index_sample][i][0][0],samples[index_sample][i][0][1]], 
                                [samples[index_sample][i][1][0],samples[index_sample][i][1][1]],
                                [samples[index_sample][i][2][0],samples[index_sample][i][2][1]],
                                [samples[index_sample][i][3][0],samples[index_sample][i][3][1]],
                                True, curveColor, curveWidth)
        curves.append(new_curve)

def insert_letter():
    global curves
    global letters
    global letter_index
    if maxCurves - len(curves) < len(letters[letter_index]):
        return
    for i in range(len(letters[letter_index])):
        new_curve = BezierCurve([letters[letter_index][i][0][0],letters[letter_index][i][0][1]],
                                [letters[letter_index][i][1][0],letters[letter_index][i][1][1]],
                                [letters[letter_index][i][2][0],letters[letter_index][i][2][1]],
                                [letters[letter_index][i][3][0],letters[letter_index][i][3][1]],
                                True, curveColor, curveWidth)
        curves.append(new_curve)

def letter_left_arrow():
    global letter_index
    global ButtonLetters
    letter_index -= 1
    if letter_index < 0:
        letter_index = len(letters) - 1
    # update the button letters to the new letter
    ButtonLetters.tempimg = pic_letters[2*letter_index]
    ButtonLetters.img = pic_letters[2*letter_index]
    ButtonLetters.imgon = pic_letters[2*letter_index+1]

def letter_right_arrow():
    global letter_index
    global ButtonLetters
    letter_index += 1
    if letter_index >= len(letters):
        letter_index = 0
    # update the button letters to the new letter
    ButtonLetters.tempimg = pic_letters[2*letter_index]
    ButtonLetters.img = pic_letters[2*letter_index]
    ButtonLetters.imgon = pic_letters[2*letter_index+1]


def check_idle():
    global idle_mode
    global idle_clock
    global idle_clock_draw
    global sample_index
    global auto_run
    global send_to_arduino
    global logger
    global enable_idle_drawing
    global letter_index
    global ButtonLetters

    if not idle_mode and time.time() - idle_clock > IDLE_TIME and not auto_run and not send_to_arduino:
        idle_mode = True
        logger.info("idle mode triggered")
        heart(log_flag=False)
        clear_all(log_flag=False)
        add_curve0(log_flag=False)
        letter_index = 0
        ButtonLetters.tempimg = pic_letters[0]
        ButtonLetters.img = pic_letters[0]
        ButtonLetters.imgon = pic_letters[1]
#            idle_clock = time.time()
        idle_clock_draw = time.time()

    if idle_mode and time.time() - idle_clock_draw > IDLE_TIME_DRAW and not auto_run and not send_to_arduino and enable_idle_drawing:
        logger.info("drawing sample number " + str(sample_index))
        clear_all(log_flag=False)
        insert_sample(sample_index)
        sample_index+=1
        if sample_index >= len(samples):
            sample_index = 0
        send_to_laser(log_flag=False)
        idle_clock_draw = time.time()
        idle_clock = time.time()


def draw_all():
    global curves
    for curve in curves:
        curve.draw()
    for curve in contour:
        curve.draw()


def show_popup():
    global show_picture
    global logger
    global info_time_start
    show_picture = True
    logger.info("clicked on info page")
    info_time_start = time.time()
'''
def draw_dialog_box():
    global yes_button, no_button
    pygame.draw.rect(screen, gray, (50, 50, dialog_width - 100, HEIGHT - 100))
    pygame.draw.rect(screen, WHITE, (50, 50, WIDTH - 100, HEIGHT - 100), 2)

    text = FONT.render("Are you sure?", True, WHITE)
    screen.blit(text, (WIDTH // 2 - text.get_width() // 2, 80))

    yes_button = pygame.draw.rect(screen, (0, 255, 0), (100, 120, 100, 50))
    no_button = pygame.draw.rect(screen, (255, 0, 0), (200, 120, 100, 50))

    yes_text = FONT.render("Yes", True, WHITE)
    no_text = FONT.render("No", True, WHITE)
    screen.blit(yes_text, (yes_button.centerx - yes_text.get_width() // 2, yes_button.centery - yes_text.get_height() // 2))
    screen.blit(no_text, (no_button.centerx - no_text.get_width() // 2, no_button.centery - no_text.get_height() // 2))
'''

# define all the buttons and make an array of them
ButtonAdd = Button(buttonAddPosition, buttonAddSize, buttonInactiveColour, buttonPressedColour, pic_buttonAdd,
                   pic_buttonPressedAdd, add_curve0)
ButtonDelete = Button(buttonDeletePosition, buttonDeleteSize, buttonInactiveColour, buttonPressedColour,
                      pic_buttonDelete, pic_buttonPressedDelete, clear)
ButtonInfo = Button(buttonInfoPosition, buttonInfoSize, buttonInactiveColour, buttonPressedColour, pic_buttonInfo,
                    pic_buttonPressedInfo, show_popup)
ButtonPreview = Button(buttonPreviewPosition, buttonPreviewSize, buttonInactiveColour, buttonPressedColour,
                       pic_buttonPreview, pic_buttonPressedPreview, preview)
ButtonPrint = Button(buttonPrintPosition, buttonPrintSize, buttonInactiveColour, buttonPressedColour, pic_buttonPrint,
                     pic_buttonPressedPrint, send_to_laser)
ButtonHeart = Button(buttonHeartPosition, buttonHeartSize, buttonInactiveColour, buttonPressedColour, pic_buttonHeart,
                     pic_buttonPressedHeart, heart)
ButtonDrop = Button(buttonDropPosition, buttonDropSize, buttonInactiveColour, buttonPressedColour, pic_buttonDrop,
                    pic_buttonPressedDrop, drop)
ButtonSquare = Button(buttonSquarePosition, buttonSquareSize, buttonInactiveColour, buttonPressedColour,
                      pic_buttonSquare, pic_buttonPressedSquare, sqaure)
ButtonLetters = Button(buttonLettersPosition, buttonLettersSize, buttonInactiveColour, buttonPressedColour,
                       pic_letters[0], pic_letters[1], insert_letter)
ButtonLettersLeftArrow = Button(buttonLettersLeftPosition, buttonLettersLeftSize, buttonInactiveColour, buttonPressedColour,
                          pic_buttonLettersLeft, pic_buttonPressedLettersLeft, letter_left_arrow)
ButtonLettersRightArrow = Button(buttonLettersRightPosition, buttonLettersRightSize, buttonInactiveColour, buttonPressedColour,
                            pic_buttonLettersRight, pic_buttonPressedLettersRight, letter_right_arrow)

buttons = [ButtonAdd, ButtonDelete, ButtonInfo, ButtonPreview, ButtonPrint, ButtonHeart, ButtonDrop, ButtonSquare, ButtonLetters, ButtonLettersLeftArrow, ButtonLettersRightArrow]


# rotate the square contour 45 degrees
angle = math.radians(45)
for i in range(len(contour_square)):
    for j in range(len(contour_square[i])):
        contour_square[i][j] = rotate_point(contour_square[i][j], angle, centerInsideBorders)


def main():
    global curves
    global selected_curve
    global selected
    global show_control_lines
    global show_picture
    global show_estimated_time
    global last_send_time
    global send_to_arduino
    global found_arduino
    global buttons_enabled
    global arduino
    global auto_run
    global run_index
    global sample_index
    global logger
    global preview_time_start
    global info_time_start
    global idle_clock
    global idle_clock_draw
    global idle_mode
    global red_border_on
    
    clock = pygame.time.Clock()
    heart(log_flag=False)
    add_curve0(log_flag=False)

    sent_border = False
    running = True
    while running:
        events = pygame.event.get()
        for event in events:
            if event.type == QUIT:
                logger.info("PROGRAM ENDED BY USER")
                running = False
            elif event.type == KEYDOWN:
                if event.key == pygame.K_p:  #if p is pressed then start running the laser automatically
                    auto_run = True
                    run_index = 0
                else:
                    if auto_run:
                        print("auto run stopped after: " + str(run_index) + " runs")
                        logger.info("auto run stopped after: " + str(run_index) + " runs")
                    auto_run = False
                    if event.key == pygame.K_r:
                        clear()
                    elif event.key == pygame.K_c:
                        clear_all()
                    elif event.key == pygame.K_a:
                        add_curve0()
                    elif event.key == pygame.K_s:
                        insert_sample(sample_index)
                        sample_index+=1
                        if sample_index >= len(samples):
                            sample_index = 0
                    elif event.key == pygame.K_l:
                        insert_letter()
                    elif event.key == pygame.K_LEFT:
                        letter_left_arrow()
                    elif event.key == pygame.K_RIGHT:
                        letter_right_arrow()
                    elif event.key == pygame.K_ESCAPE:
                        running = False
                        logger.info("PROGRAM ENDED BY USER")
                    else:
                        running = False
                        logger.info("PROGRAM ENDED BY USER")
            elif event.type == MOUSEBUTTONDOWN and event.button == 1:
                idle_clock = time.time()
                idle_clock_draw = time.time()
                idle_mode = False
                if auto_run:
                    print("auto run stopped after: " + str(run_index) + " runs")
                    logger.info("auto run stopped after: " + str(run_index) + " runs")
                    auto_run = False
                for curve in curves:
                    for p in curve.vertices:
                        if math.dist(p, event.pos) < toleranceTouch:
                            if selected is None:
                                selected_curve = curve
                                selected_curve.color = selectedCurveColor
                                selected = p
                                buttons_enabled = False
                            elif math.dist(p, event.pos) < math.dist(selected, event.pos):
                                selected_curve.color = curveColor
                                selected_curve = curve
                                selected_curve.color = selectedCurveColor
                                selected = p
                                buttons_enabled = False
            elif event.type == MOUSEBUTTONUP and event.button == 1:
                idle_clock = time.time()
                idle_clock_draw = time.time()
                idle_mode = False
                if auto_run:
                    print("auto run stopped after: " + str(run_index) + " runs")
                    logger.info("auto run stopped after: " + str(run_index) + " runs")
                    auto_run = False
                selected = None
                buttons_enabled = True
                if selected_curve is not None:
                    selected_curve.color = curveColor
                    selected_curve = None
                if show_picture:
                    show_picture = False
                    logger.info("closed info page after " + str(int((time.time() - info_time_start)*10)/10.0) + " seconds")
                if not show_control_lines:
                    show_control_lines = True
                    logger.info("closed preview mode after " + str(int((time.time() - preview_time_start)*10)/10.0) + " seconds")

        if selected is not None:
            if borderLineHeight + circleRadius1 < pygame.mouse.get_pos()[1] < borderLine2Height - circleRadius1\
                    and borderLineX + circleRadius1 < pygame.mouse.get_pos()[0] < borderLine2X - circleRadius1:
                pygame.draw.circle(screen, green, (selected[0], selected[1]), circleRadiusClicked)
                # if clicked on the purple point, which moves the whole curve
                if IS_MOVING_ALL_CURVE and selected_curve.vertices.index(selected) == 0:
                    # check if all points are in the screen
                    inScreen = True
                    for i in range(1, 4):
                        if selected_curve.vertices[i][0] + (
                                pygame.mouse.get_pos()[0] - selected[0]) <= borderLineX + circleRadius1 or \
                                selected_curve.vertices[i][0] + (
                                        pygame.mouse.get_pos()[0] - selected[0]) >= borderLine2X - circleRadius1 or \
                                selected_curve.vertices[i][1] + (
                                        pygame.mouse.get_pos()[1] - selected[1]) <= borderLineHeight + circleRadius1 or \
                                selected_curve.vertices[i][1] + (
                                        pygame.mouse.get_pos()[1] - selected[1]) >= borderLine2Height - circleRadius1:
                            inScreen = False
                    # if so, move the curve
                    if inScreen:
                        for i in range(1, 4):
                            selected_curve.vertices[i][0] = selected_curve.vertices[i][0] + (
                                        pygame.mouse.get_pos()[0] - selected[0])
                            selected_curve.vertices[i][1] = selected_curve.vertices[i][1] + (
                                        pygame.mouse.get_pos()[1] - selected[1])
                        selected[0], selected[1] = pygame.mouse.get_pos()
                else:
                    # move the selected point
                    selected[0], selected[1] = pygame.mouse.get_pos()

        # Draw everything
        screen.fill(bgColor)
        # draw the border rectangles
        pygame.draw.rect(screen, colorOutSideBorder, (0,0, borderLineX, screen_height))
        pygame.draw.rect(screen, colorOutSideBorder, (borderLine2X, 0, screen_width - borderLine2X, screen_height))
        pygame.draw.rect(screen, colorOutSideBorder, (0, 0, screen_width, borderLineHeight))
        pygame.draw.rect(screen, colorOutSideBorder, (0, borderLine2Height, screen_width, screen_height - borderLine2Height))
        if SHOW_RED_BORDER:
            # for each curve, check if there are any points outside the border
            for curve in curves:
                for p in curve.vertices:
                    if p[0] < borderLineX + circleRadius1 + redBorderWidth or\
                            p[0] > borderLine2X - circleRadius1 - redBorderWidth or\
                            p[1] < borderLineHeight + circleRadius1 + redBorderWidth or\
                            p[1] > borderLine2Height - circleRadius1 - redBorderWidth:
                        pygame.draw.rect(screen, redBorderColor, (redBorderPos, redBorderSize), 0)
                        pygame.draw.rect(screen, drawingAreaColor, (drawingAreaPos, drawingAreaSize), 0)
                        break

        # draw the text above
        screen.blit(pic_textAbove, textAbovePosition)
        # draw the "frame:" text
        screen.blit(pic_textFrame, textFramePosition)
        # draw a rectangle in the middle of the screen to show the laser cutting area
        pygame.draw.rect(screen, cuttingAreaColor, (cuttingAreaPos[0], cuttingAreaPos[1], cuttingAreaSize[0], cuttingAreaSize[1]))
        draw_all()
        msgNumCurves(maxCurves - len(curves))
        if show_picture:
            popup_x = centerInsideBorders[0] - infoHebSize[0]/2
            popup_y = centerInsideBorders[1] - infoHebSize[1]/2
            screen.blit(pic_infoHeb, (popup_x, popup_y))
        if show_estimated_time:
            msgEstimatedTime(max(estimated_time-time.time()+last_send_time,0))

        check_buttons()
        check_idle()
        check_dc_motor()

        pygame.display.update()
        # Flip screen
        pygame.display.flip()

        # check if sending to arduino
        if found_arduino:
            if send_to_arduino:
                if not check_arduino():
                    print("--- SOMETHING WENT WRONG WITH THE ARDUINO !!! ---")
                    logger.error("--- SOMETHING WENT WRONG WITH THE ARDUINO !!! ---")
                    send_to_arduino = False
                    show_estimated_time = False
                    if auto_run:
                        print("auto run stopped after: " + str(run_index) + " runs")
                        logger.info("auto run stopped after: " + str(run_index) + " runs")
                    auto_run = False
            else:
                try:
                    if arduino.in_waiting > 0:
                        received_data = arduino.readline().decode().rstrip()
                        print("Received from Arduino:", received_data)
                        if not sent_border:
                            take_control()
                            time.sleep(0.5)
                            sent_border = True
                            # send the cutting area size
                            print(send_one_number(centerInsideBorders[0] - cuttingAreaWidth / 2))
                            print(send_one_number(centerInsideBorders[1] - cuttingAreaHeight / 2))
                            print(send_one_number(centerInsideBorders[0] + cuttingAreaWidth / 2))
                            print(send_one_number(centerInsideBorders[1] + cuttingAreaHeight / 2))
                            print(send_one_number(LASER_POWER))
                            print(send_one_number(CONTOUR_POWER))
                            print(send_one_number(LASER_OFF_RATE))
                            print(send_one_number(LASER_ON_RATE))
                            print(send_one_number(CONTOUR_RATE))
                            print(send_one_number(MAX_DC_MOTOR_TIME*1000))
                            logger.info("sent laser variables to arduino")
                            time.sleep(time_delay_arduino)

                    else:
                        pass
                finally:
                    pass
                if auto_run:
                    if run_index < MAX_RUNS:
                        time.sleep(MAX_DC_MOTOR_TIME+2)
                        send_to_laser()
                        run_index += 1
                    else:
                        auto_run = False

        clock.tick(100)
        # print clock.get_fps()
    pygame.quit()
    quit()


if __name__ == '__main__':
    main()

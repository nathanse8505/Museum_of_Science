import pygame
import os
from pygame.locals import *

# consts
gray = (100, 100, 100)
lightgray = (180, 180, 180)
verylightgray = (220, 220, 220)
red = (255, 0, 0)
pink = (255, 0, 255)
light_red = (255, 100, 100)
green = (0, 255, 0)
purple = (255, 0, 255)
blue = (0, 0, 255)
yellow = (255, 255, 0)
white = (255, 255, 255)
black = (0, 0, 0)
brown = (139, 69, 19)
orange = (255, 165, 0)
button_color = (199, 33, 47)
button_pressed_color = (242, 188, 27)
colorOutSideBorder = (226, 233, 241)
cuttingAreaColor = white
bgColor = white

# screen_width = 1000
# screen_height = 600
const_width_screen = 1366  # DO NOT CHANGE - for calculations of proportional sizes
const_height_screen = 768  # DO NOT CHANGE - for calculations of proportional sizes
saved_image_width, saved_image_height = 720, 480

pygame.init()
infoObject = pygame.display.Info()
print(infoObject)
screen_width = infoObject.current_w
screen_height = infoObject.current_h
screenColor = gray

# logging values
log_name = "log.txt"
log_dir = "logs"
currentDir = os.path.dirname(os.path.realpath(__file__))
if not os.path.exists(os.path.join(currentDir, log_dir)):
    os.makedirs(os.path.join(currentDir, log_dir))
LOG_FILE_PATH = os.path.join(currentDir, log_dir, log_name)

# borderLineHeight = int(142 / const_height_screen * screen_height)
borderLineHeight = int(142 / const_height_screen * screen_height)
# borderLine2Height = int((const_height_screen - 142) / const_height_screen * screen_height)
borderLine2Height = screen_height-int(60/const_height_screen*screen_height)
borderLineX = int(130 / const_width_screen * screen_width)
# borderLine2X = int((const_width_screen - 330) / const_width_screen * screen_width)
borderLine2X = int((const_width_screen - 385) / const_width_screen * screen_width)
centerInsideBorders = (int((borderLineX + borderLine2X) / 2), int((borderLineHeight + borderLine2Height) / 2))
center = (int(screen_width / 2), int(screen_height / 2))

#cuttingAreaColor = white
cuttingAreaWidth = int((const_height_screen - 142) / const_height_screen * screen_height) - int(142 / const_height_screen * screen_height)
cuttingAreaHeight = cuttingAreaWidth
cuttingAreaPos = (centerInsideBorders[0] - cuttingAreaWidth / 2, centerInsideBorders[1] - cuttingAreaHeight / 2)
cuttingAreaSize = (cuttingAreaWidth, cuttingAreaHeight)

# red border values
redBorderWidth = int(10 / const_width_screen * screen_width)
redBorderPos = (borderLineX, borderLineHeight)
redBorderSize = (borderLine2X - borderLineX, borderLine2Height - borderLineHeight)
redBorderColor = red

drawingAreaPos = (borderLineX + redBorderWidth, borderLineHeight + redBorderWidth)
drawingAreaSize = (borderLine2X - borderLineX - 2* redBorderWidth, borderLine2Height - borderLineHeight - 2*redBorderWidth)
drawingAreaColor = white

delta0X = int(35 / const_width_screen * screen_width)
delta0Y = int(0 / const_height_screen * screen_height)
delta0Z = int(-50 / const_height_screen * screen_height)
delta1X = int(30 / const_width_screen * screen_width)
delta1Y = int(0 / const_height_screen * screen_height)
delta1Z = int(-50 / const_height_screen * screen_height)
MAX_LINES_PER_ROW = 7
MAX_LINES_PER_ROW_OUTSIDE_CONTOUR = 4
MAX_LINES_GENERATED_INSIDE_CONTOUR = 1

toleranceTouch = 20

# control points values
circleColor0 = purple
circleColor1 = blue
circleColor2 = yellow
circleRadius0 = int(8 / const_width_screen * screen_width)
circleRadius1 = int(8 / const_width_screen * screen_width)
circleRadius2 = int(5 / const_width_screen * screen_width)
circleRadiusClicked = int(10 / const_width_screen * screen_width)
doubleArrowSize = (int(40 / const_width_screen * screen_width), int(40 / const_height_screen * screen_height))

x0 = int(790 / const_width_screen * screen_width) + centerInsideBorders[0] - center[0]
y0 = int(470 / const_height_screen * screen_height) + centerInsideBorders[1] - center[1]
x1 = int(840 / const_width_screen * screen_width) + centerInsideBorders[0] - center[0]
y1 = int(370 / const_height_screen * screen_height) + centerInsideBorders[1] - center[1]
x2 = int(740 / const_width_screen * screen_width) + centerInsideBorders[0] - center[0]
y2 = int(370 / const_height_screen * screen_height) + centerInsideBorders[1] - center[1]
x3 = int(790 / const_width_screen * screen_width) + centerInsideBorders[0] - center[0]
y3 = int(270 / const_height_screen * screen_height) + centerInsideBorders[1] - center[1]

shiftX = int(250 / const_width_screen * screen_width)
shiftY = int(-70 / const_height_screen * screen_height)
x0_outside = x0 + shiftX
y0_outside = y0 + shiftY
x1_outside = x1 + shiftX
y1_outside = y1 + shiftY
x2_outside = x2 + shiftX
y2_outside = y2 + shiftY
x3_outside = x3 + shiftX
y3_outside = y3 + shiftY

# mm_per_pixel_x = 295/1366
# mm_per_pixel_y = 165/768
pixel_per_cm_screen = 90 / 1.9

# bezier curve values
curveColor = red
selectedCurveColor = green
control_lines_color = lightgray
curveWidth = int(6 / const_width_screen * screen_width)
controlLineWidth = int(3 / const_width_screen * screen_width)
maxCurves = 15

# contour values
contourColor = black
contourWidth = int(8 / const_width_screen * screen_width)

'''
# dialog box values
dialogBoxColor = lightgray
dialogBoxWidth = 400
dialogBoxHeight = 200
dialogBoxX = screen_width/2 - dialogBoxWidth/2
dialogBoxY = screen_height/2 - dialogBoxHeight/2
dialogBoxTextX = dialogBoxX + 20
dialogBoxTextY = dialogBoxY + 20
dialogBoxTextWidth = dialogBoxWidth - 40
dialogBoxTextHeight = dialogBoxHeight - 40
dialogBoxTextSize = 30
dialogBoxTextColour = black
dialogBoxButtonWidth = 100
dialogBoxButtonHeight = 50
dialogBoxButtonX = dialogBoxX + dialogBoxWidth/2 - dialogBoxButtonWidth/2
dialogBoxButtonY = dialogBoxY + dialogBoxHeight - dialogBoxButtonHeight - 20
dialogBoxButtonColour = gray
dialogBoxButtonHoverColour = lightgray
dialogBoxButtonPressedColour = verylightgray
dialogBoxButtonInactiveColour = gray
dialogBoxButtonBorderWidth = 5
dialogBoxButtonBorderColour = black
dialogBoxButtonBorderHoverColour = black
dialogBoxButtonBorderPressedColour = black
dialogBoxButtonBorderInactiveColour = black
'''

# button values
buttonInactiveColour = yellow
buttonHoverColour = red
buttonPressedColour = green
buttonOfflineColour = gray

button_height0 = 1.3
button_contour_height0 = 1.7
button_height = int(button_height0 / 16.5 * screen_height)
# sizes of buttons and images
buttonAddSize = (int(7.2 / 29.5 * screen_width), button_height)
buttonDeleteSize = (int(4.5 / 29.5 * screen_width), button_height)
buttonInfoSize = (int(button_contour_height0 / 16.5 * screen_height), int(button_contour_height0 / 16.5 * screen_height))
buttonPreviewSize = (int(7.2 / 29.5 * screen_width), button_height)
buttonPrintSize = (int(4.5 / 29.5 * screen_width), button_height)
buttonHeartSize = buttonInfoSize
buttonDropSize = buttonInfoSize
buttonSquareSize = buttonInfoSize
buttonLettersSize = (int(1 / 16.5 * screen_height), int(1 / 16.5 * screen_height))
buttonLettersLeftSize = buttonLettersSize
buttonLettersRightSize = buttonLettersSize

infoHebSize = (int(18.5 / 29.5 * screen_width), int(12.2 / 16.5 * screen_height))
# infoEngSize = infoHebSize
# infoArabSize = infoHebSize
textAboveSize = (int(14 / 29.5 * screen_width), int(3 / 16.5 * screen_height))
textFrameSize = buttonHeartSize

button_contour_x = int(0.5 / 29.5 * screen_width)
button_operation_x = int(29 / 29.5 * screen_width)
button_operation_x0 = 5
# positions of the buttons
buttonAddPosition = (button_operation_x - buttonAddSize[0], int(button_operation_x0 / 16.5 * screen_height))
buttonPreviewPosition = (button_operation_x - buttonPreviewSize[0], int((button_operation_x0 + button_height0 + 1) / 16.5 * screen_height))
buttonDeletePosition = (button_operation_x - 1.3*buttonDeleteSize[0], int((button_operation_x0 + 2 * (button_height0 + 1)) / 16.5 * screen_height))
buttonInfoPosition = (button_contour_x, borderLineHeight+int(0.5/16.5 * screen_height))
buttonPrintPosition = (button_operation_x - 1.3*buttonPrintSize[0], int((button_operation_x0 + 3 * (button_height0 + 1)) / 16.5 * screen_height))
buttonHeartPosition = (button_contour_x, int((6 + button_contour_height0 + 0.5) / 16.5 * screen_height))
buttonDropPosition = (button_contour_x, int((6 + 2 * (button_contour_height0 + 0.5)) / 16.5 * screen_height))
buttonSquarePosition = (button_contour_x, int((6 + 3 * (button_contour_height0 + 0.5)) / 16.5 * screen_height))
buttonLettersPosition = (buttonAddPosition[0] + 0.5*(buttonAddSize[0]-buttonLettersSize[0]), int((button_operation_x0 - 0.3) / 16.5 * screen_height)-buttonLettersSize[1])
buttonLettersLeftPosition = (buttonLettersPosition[0] - 1.5*buttonLettersSize[0], buttonLettersPosition[1])
buttonLettersRightPosition = (buttonLettersPosition[0] + 1.5*buttonLettersSize[0], buttonLettersPosition[1])
textAbovePosition = (centerInsideBorders[0]-textAboveSize[0]/2, borderLineHeight-textAboveSize[1])
textFramePosition = (buttonHeartPosition[0], buttonHeartPosition[1]-textFrameSize[0])

# get the image from the directory "pictures"
pic_buttonDelete = pygame.image.load("pictures/buttonDelete.jpg")
pic_buttonPressedDelete = pygame.image.load("pictures/buttonPressedDelete.jpg")
pic_buttonInfo = pygame.image.load("pictures/buttonInfo.jpg")
pic_buttonPressedInfo = pygame.image.load("pictures/buttonPressedInfo.jpg")
pic_buttonAdd = pygame.image.load("pictures/buttonAdd.jpg")
pic_buttonPressedAdd = pygame.image.load("pictures/buttonPressedAdd.jpg")
pic_buttonPreview = pygame.image.load("pictures/buttonPreview.jpg")
pic_buttonPressedPreview = pygame.image.load("pictures/buttonPressedPreview.jpg")
pic_buttonPrint = pygame.image.load("pictures/buttonPrint.jpg")
pic_buttonPressedPrint = pygame.image.load("pictures/buttonPressedPrint.jpg")
pic_buttonOffPrint = pygame.image.load("pictures/buttonOffPrint.jpg")
pic_bg0 = pygame.image.load("pictures/vertical_bg_0.jpg")
pic_bg1 = pygame.image.load("pictures/bgWithButtons.jpg")
pic_infoHeb = pygame.image.load("pictures/infoHeb2.jpg")
# pic_infoEng = pygame.image.load("pictures/infoEng.jpg")
# pic_infoArab = pygame.image.load("pictures/infoArab.jpg")
pic_buttonHeart = pygame.image.load("pictures/buttonHeart.jpg")
pic_buttonPressedHeart = pygame.image.load("pictures/buttonPressedHeart.jpg")
pic_buttonDrop = pygame.image.load("pictures/buttonDrop.jpg")
pic_buttonPressedDrop = pygame.image.load("pictures/buttonPressedDrop.jpg")
pic_buttonSquare = pygame.image.load("pictures/buttonSquare.jpg")
pic_buttonPressedSquare = pygame.image.load("pictures/buttonPressedSquare.jpg")
# pic_buttonLetters = pygame.image.load("pictures/buttonLetters.jpg")
# pic_buttonPressedLetters = pygame.image.load("pictures/buttonPressedLetters.jpg")
pic_buttonLettersLeft = pygame.image.load("pictures/left_arrow.png")
pic_buttonPressedLettersLeft = pygame.image.load("pictures/left_arrow.png")
pic_buttonLettersRight = pygame.image.load("pictures/right_arrow.png")
pic_buttonPressedLettersRight = pygame.image.load("pictures/right_arrow.png")
pic_doubleArrow = pygame.image.load("pictures/double_arrow.png")
pic_textAbove = pygame.image.load("pictures/textAbove2.jpg")
pic_textFrame = pygame.image.load("pictures/frame.jpg")

# load the images of the letters and transform them to the right size
pic_letters = []
for i in range(0, 27):
    pic_letters.append(pygame.image.load("pictures/letters/" + str(i) + ".png"))
    pic_letters[2*i] = pygame.transform.scale(pic_letters[2*i], buttonLettersSize)
    pic_letters.append(pygame.image.load("pictures/letters/" + str(i) + "_pressed.png"))
    pic_letters[2*i+1] = pygame.transform.scale(pic_letters[2*i+1], buttonLettersSize)

# resize the images
pic_buttonDelete = pygame.transform.scale(pic_buttonDelete, buttonDeleteSize)
pic_buttonPressedDelete = pygame.transform.scale(pic_buttonPressedDelete, buttonDeleteSize)
pic_buttonInfo = pygame.transform.scale(pic_buttonInfo, buttonInfoSize)
pic_buttonPressedInfo = pygame.transform.scale(pic_buttonPressedInfo, buttonInfoSize)
pic_buttonAdd = pygame.transform.scale(pic_buttonAdd, buttonAddSize)
pic_buttonPressedAdd = pygame.transform.scale(pic_buttonPressedAdd, buttonAddSize)
pic_buttonPreview = pygame.transform.scale(pic_buttonPreview, buttonPreviewSize)
pic_buttonPressedPreview = pygame.transform.scale(pic_buttonPressedPreview, buttonPreviewSize)
pic_buttonPrint = pygame.transform.scale(pic_buttonPrint, buttonPrintSize)
pic_buttonPressedPrint = pygame.transform.scale(pic_buttonPressedPrint, buttonPrintSize)
pic_buttonOffPrint = pygame.transform.scale(pic_buttonOffPrint, buttonPrintSize)
pic_bg0 = pygame.transform.scale(pic_bg0, (screen_width, screen_height))
pic_bg1 = pygame.transform.scale(pic_bg1, (screen_width, screen_height))
pic_infoHeb = pygame.transform.scale(pic_infoHeb, infoHebSize)
# pic_infoEng = pygame.transform.scale(pic_infoEng, infoEngSize)
# pic_infoArab = pygame.transform.scale(pic_infoArab, infoArabSize)
pic_buttonHeart = pygame.transform.scale(pic_buttonHeart, buttonHeartSize)
pic_buttonPressedHeart = pygame.transform.scale(pic_buttonPressedHeart, buttonHeartSize)
pic_buttonDrop = pygame.transform.scale(pic_buttonDrop, buttonDropSize)
pic_buttonPressedDrop = pygame.transform.scale(pic_buttonPressedDrop, buttonDropSize)
pic_buttonSquare = pygame.transform.scale(pic_buttonSquare, buttonSquareSize)
pic_buttonPressedSquare = pygame.transform.scale(pic_buttonPressedSquare, buttonSquareSize)
pic_buttonLetters = pygame.transform.scale(pic_letters[0], buttonLettersSize)
pic_buttonPressedLetters = pygame.transform.scale(pic_letters[1], buttonLettersSize)
pic_buttonLettersLeft = pygame.transform.scale(pic_buttonLettersLeft, buttonLettersLeftSize)
pic_buttonPressedLettersLeft = pygame.transform.scale(pic_buttonPressedLettersLeft, buttonLettersLeftSize)
pic_buttonLettersRight = pygame.transform.scale(pic_buttonLettersRight, buttonLettersRightSize)
pic_buttonPressedLettersRight = pygame.transform.scale(pic_buttonPressedLettersRight, buttonLettersRightSize)
pic_doubleArrow = pygame.transform.scale(pic_doubleArrow, doubleArrowSize)
pic_textAbove = pygame.transform.scale(pic_textAbove, textAboveSize)
pic_textFrame = pygame.transform.scale(pic_textFrame, textFrameSize)

x_length = 16
x_height = 15 # height from the top of the heart downwards
contour_heart = [[(screen_width / 2 - int(x_length / const_width_screen * screen_width), int((250 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 + int(x_length / const_width_screen * screen_width),int((250 + x_height + 2*x_length) / const_height_screen * screen_height)),
                  (screen_width / 2 - int(x_length / const_width_screen * screen_width), int((250 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 + int(x_length / const_width_screen * screen_width),int((250 + x_height + 2*x_length) / const_height_screen * screen_height))],
                [(screen_width / 2 + int(x_length / const_width_screen * screen_width), int((250 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 - int(x_length / const_width_screen * screen_width),int((250 + x_height + 2*x_length) / const_height_screen * screen_height)),
                  (screen_width / 2 + int(x_length / const_width_screen * screen_width), int((250 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 - int(x_length / const_width_screen * screen_width),int((250 + x_height + 2*x_length) / const_height_screen * screen_height))],
                [(screen_width / 2, int(600 / const_height_screen * screen_height)),
                  (int(1145 / const_width_screen * screen_width), int(345 / const_height_screen * screen_height)), (
                  screen_width / 2 + (int(120 / const_width_screen * screen_width)),
                  int(80 / const_height_screen * screen_height)),
                  (screen_width / 2, int(250 / const_height_screen * screen_height))],
                 [(screen_width / 2, int(250 / const_height_screen * screen_height)), (
                 screen_width / 2 - int(120 / const_width_screen * screen_width),
                 int(80 / const_height_screen * screen_height)),
                  (int(225 / const_width_screen * screen_width), int(345 / const_height_screen * screen_height)),
                  (screen_width / 2, int(600 / const_height_screen * screen_height))]]
square_side = [int(193 / const_width_screen * screen_width),int(193 / const_width_screen * screen_width)]
x_height = int(60/const_width_screen * screen_width)
x_length = int(24/const_width_screen * screen_width)
contour_square = [[[screen_width / 2 - square_side[0] + x_height - x_length, screen_height/2-square_side[1] + x_height],
                   [screen_width / 2 - square_side[0] + x_height + x_length, screen_height/2-square_side[1] + x_height],
                   [screen_width / 2 - square_side[0] + x_height - x_length, screen_height/2-square_side[1] + x_height],
                   [screen_width / 2 - square_side[0] + x_height + x_length, screen_height/2-square_side[1] + x_height]],
                [[screen_width / 2 - square_side[0] + x_height, screen_height/2-square_side[1] + x_height - x_length],
                   [screen_width / 2 - square_side[0] + x_height, screen_height/2-square_side[1] + x_height + x_length],
                   [screen_width / 2 - square_side[0] + x_height, screen_height/2-square_side[1] + x_height - x_length],
                   [screen_width / 2 - square_side[0] + x_height, screen_height/2-square_side[1] + x_height + x_length]],
                [[screen_width / 2, screen_height / 2 + square_side[1]],
                   [screen_width / 2 + square_side[0],
                    screen_height / 2 + square_side[1]],
                   [screen_width / 2 + square_side[0],
                    screen_height / 2 + square_side[1]],
                   [screen_width / 2 + square_side[0],
                    screen_height / 2]], [
                      [screen_width / 2 + square_side[0],
                       screen_height / 2],
                      [screen_width / 2 + square_side[0],
                       screen_height / 2 - square_side[1]],
                      [screen_width / 2 + square_side[0],
                       screen_height / 2 - square_side[1]],
                      [screen_width / 2, screen_height / 2 - square_side[1]]],
                  [[screen_width / 2, screen_height / 2 - square_side[1]],
                   [screen_width / 2 - square_side[0],
                    screen_height / 2 - square_side[1]],
                   [screen_width / 2 - square_side[0],
                    screen_height / 2 - square_side[1]],
                   [screen_width / 2 - square_side[0],
                    screen_height/2]], [
                      [screen_width / 2 - square_side[0],
                       screen_height / 2],
                      [screen_width / 2 - square_side[0],
                       screen_height / 2 + square_side[1]],
                      [screen_width / 2 - square_side[0],
                       screen_height / 2 + square_side[1]],
                      [screen_width / 2, screen_height / 2 + square_side[1]]]]
x_height = 35
x_length = 16
contour_drop = [[(screen_width / 2 - int(x_length / const_width_screen * screen_width), int((145 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 + int(x_length / const_width_screen * screen_width),int((145 + x_height + 2*x_length) / const_height_screen * screen_height)),
                  (screen_width / 2 - int(x_length / const_width_screen * screen_width), int((145 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 + int(x_length / const_width_screen * screen_width),int((145 + x_height + 2*x_length) / const_height_screen * screen_height))],
                [(screen_width / 2 + int(x_length / const_width_screen * screen_width), int((145 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 - int(x_length / const_width_screen * screen_width),int((145 + x_height + 2*x_length) / const_height_screen * screen_height)),
                  (screen_width / 2 + int(x_length / const_width_screen * screen_width), int((145 + x_height) / const_height_screen * screen_height)),
                  (screen_width/2 - int(x_length / const_width_screen * screen_width),int((145 + x_height + 2*x_length) / const_height_screen * screen_height))],
                [(screen_width/2, int(610 / const_height_screen * screen_height)),
                 (screen_width/2 + int(62 / const_width_screen * screen_width), int(610 / const_height_screen * screen_height)),
                 (screen_width/2 + int(184 / const_width_screen * screen_width), int(550 / const_height_screen * screen_height)),
                 (screen_width/2 + int(184 / const_width_screen * screen_width), int(407 / const_height_screen * screen_height))],
                [(screen_width/2 + int(184 / const_width_screen * screen_width), int(407 / const_height_screen * screen_height)),
                 (screen_width/2 + int(184 / const_width_screen * screen_width), int(263 / const_height_screen * screen_height)),
                 (screen_width/2 + int(43 / const_width_screen * screen_width), int(170 / const_height_screen * screen_height)),
                 (screen_width/2, int(145 / const_height_screen * screen_height))],
                [(screen_width/2, int(145 / const_height_screen * screen_height)),
                 (screen_width/2 - int(43 / const_width_screen * screen_width), int(170 / const_height_screen * screen_height)),
                 (screen_width/2 - int(184 / const_width_screen * screen_width), int(263 / const_height_screen * screen_height)),
                 (screen_width/2 - int(184 / const_width_screen * screen_width), int(407 / const_height_screen * screen_height))],
                [(screen_width/2 - int(184 / const_width_screen * screen_width), int(407 / const_height_screen * screen_height)),
                 (screen_width/2 - int(184 / const_width_screen * screen_width), int(550 / const_height_screen * screen_height)),
                 (screen_width/2 - int(62 / const_width_screen * screen_width), int(610 / const_height_screen * screen_height)),
                 (screen_width/2, int(610 / const_height_screen * screen_height))]]

contours = [contour_heart, contour_square, contour_drop]

# move the conour heart to the center inside the borders
for k in range(len(contours)):
    for i in range(len(contours[k])):
        for j in range(len(contours[k][i])):
            contours[k][i][j] = (
            contours[k][i][j][0] + int(centerInsideBorders[0] - center[0]),
            contours[k][i][j][1] + int(centerInsideBorders[1] - center[1]))


sample0 = [[(int(756/const_width_screen*screen_width),int(359/const_height_screen*screen_height)),
                (int(770/const_width_screen*screen_width),int(481/const_height_screen*screen_height)),
                (int(667/const_width_screen*screen_width),int(537/const_height_screen*screen_height)),
                (int(659/const_width_screen*screen_width),int(384/const_height_screen*screen_height))],
            [(int(671/const_width_screen*screen_width),int(437/const_height_screen*screen_height)),
                (int(705/const_width_screen*screen_width),int(413/const_height_screen*screen_height)),
                (int(722/const_width_screen*screen_width),int(445/const_height_screen*screen_height)),
                (int(714/const_width_screen*screen_width),int(369/const_height_screen*screen_height))],
            [(int(595/const_width_screen*screen_width),int(490/const_height_screen*screen_height)),
                (int(745/const_width_screen*screen_width),int(283/const_height_screen*screen_height)),
                (int(502/const_width_screen*screen_width),int(478/const_height_screen*screen_height)),
                (int(612/const_width_screen*screen_width),int(317/const_height_screen*screen_height))],
            [(int(559/const_width_screen*screen_width),int(489/const_height_screen*screen_height)),
                (int(559/const_width_screen*screen_width),int(402/const_height_screen*screen_height)),
                (int(567/const_width_screen*screen_width),int(368/const_height_screen*screen_height)),
                (int(550/const_width_screen*screen_width),int(369/const_height_screen*screen_height))],
            [(int(437/const_width_screen*screen_width),int(366/const_height_screen*screen_height)),
                (int(511/const_width_screen*screen_width),int(373/const_height_screen*screen_height)),
                (int(529/const_width_screen*screen_width),int(351/const_height_screen*screen_height)),
                (int(520/const_width_screen*screen_width),int(491/const_height_screen*screen_height))],
            [(int(519/const_width_screen*screen_width),int(491/const_height_screen*screen_height)),
                (int(438/const_width_screen*screen_width),int(493/const_height_screen*screen_height)),
                (int(437/const_width_screen*screen_width),int(501/const_height_screen*screen_height)),
                (int(448/const_width_screen*screen_width),int(378/const_height_screen*screen_height))],
            [(int(401/const_width_screen*screen_width),int(451/const_height_screen*screen_height)),
                (int(402/const_width_screen*screen_width),int(378/const_height_screen*screen_height)),
                (int(399/const_width_screen*screen_width),int(400/const_height_screen*screen_height)),
                (int(405/const_width_screen*screen_width),int(341/const_height_screen*screen_height))],
            [(int(396/const_width_screen*screen_width),int(479/const_height_screen*screen_height)),
                (int(416/const_width_screen*screen_width),int(491/const_height_screen*screen_height)),
                (int(409/const_width_screen*screen_width),int(447/const_height_screen*screen_height)),
                (int(396/const_width_screen*screen_width),int(478/const_height_screen*screen_height))]]

sample1 = [[[int(615/const_width_screen*screen_width), int(419/const_height_screen*screen_height)],
            [int(615/const_width_screen*screen_width), int(371/const_height_screen*screen_height)],
            [int(615/const_width_screen*screen_width), int(391/const_height_screen*screen_height)],
            [int(615/const_width_screen*screen_width), int(352/const_height_screen*screen_height)]],
            [[int(495/const_width_screen*screen_width), int(416/const_height_screen*screen_height)],
            [int(494/const_width_screen*screen_width), int(382/const_height_screen*screen_height)],
            [int(497/const_width_screen*screen_width), int(379/const_height_screen*screen_height)],
            [int(496/const_width_screen*screen_width), int(353/const_height_screen*screen_height)]],
            [[int(416/const_width_screen*screen_width), int(447/const_height_screen*screen_height)],
            [int(463/const_width_screen*screen_width), int(554/const_height_screen*screen_height)],
            [int(633/const_width_screen*screen_width), int(554/const_height_screen*screen_height)],
            [int(690/const_width_screen*screen_width), int(449/const_height_screen*screen_height)]],
            [[int(510/const_width_screen*screen_width), int(510/const_height_screen*screen_height)],
            [int(524/const_width_screen*screen_width), int(487/const_height_screen*screen_height)],
            [int(586/const_width_screen*screen_width), int(484/const_height_screen*screen_height)],
            [int(594/const_width_screen*screen_width), int(513/const_height_screen*screen_height)]],
            [[int(414/const_width_screen*screen_width), int(439/const_height_screen*screen_height)],
            [int(461/const_width_screen*screen_width), int(472/const_height_screen*screen_height)],
            [int(628/const_width_screen*screen_width), int(483/const_height_screen*screen_height)],
            [int(691/const_width_screen*screen_width), int(441/const_height_screen*screen_height)]],
            [[int(445/const_width_screen*screen_width), int(334/const_height_screen*screen_height)],
            [int(457/const_width_screen*screen_width), int(307/const_height_screen*screen_height)],
            [int(484/const_width_screen*screen_width), int(296/const_height_screen*screen_height)],
            [int(518/const_width_screen*screen_width), int(301/const_height_screen*screen_height)]],
            [[int(666/const_width_screen*screen_width), int(333/const_height_screen*screen_height)],
            [int(649/const_width_screen*screen_width), int(313/const_height_screen*screen_height)],
            [int(646/const_width_screen*screen_width), int(303/const_height_screen*screen_height)],
            [int(607/const_width_screen*screen_width), int(300/const_height_screen*screen_height)]]]

# sample 2 before scaling
sample2 = [[[int(689/const_width_screen*screen_width), int(307/const_height_screen*screen_height)],
            [int(705/const_width_screen*screen_width), int(533/const_height_screen*screen_height)],
            [int(644/const_width_screen*screen_width), int(174/const_height_screen*screen_height)],
            [int(654/const_width_screen*screen_width), int(377/const_height_screen*screen_height)]],
            [[int(629/const_width_screen*screen_width), int(380/const_height_screen*screen_height)],
             [int(632/const_width_screen*screen_width), int(340/const_height_screen*screen_height)],
             [int(626/const_width_screen*screen_width), int(347/const_height_screen*screen_height)],
             [int(629/const_width_screen*screen_width), int(314/const_height_screen*screen_height)]],
            [[int(596/const_width_screen*screen_width), int(379/const_height_screen*screen_height)],
             [int(645/const_width_screen*screen_width), int(354/const_height_screen*screen_height)],
             [int(583/const_width_screen*screen_width), int(374/const_height_screen*screen_height)],
             [int(597/const_width_screen*screen_width), int(315/const_height_screen*screen_height)]],
            [[int(569/const_width_screen*screen_width), int(355/const_height_screen*screen_height)],
             [int(571/const_width_screen*screen_width), int(334/const_height_screen*screen_height)],
             [int(569/const_width_screen*screen_width), int(343/const_height_screen*screen_height)],
             [int(569/const_width_screen*screen_width), int(324/const_height_screen*screen_height)]],
            [[int(543/const_width_screen*screen_width), int(378/const_height_screen*screen_height)],
             [int(497/const_width_screen*screen_width), int(379/const_height_screen*screen_height)],
             [int(494/const_width_screen*screen_width), int(330/const_height_screen*screen_height)],
             [int(543/const_width_screen*screen_width), int(328/const_height_screen*screen_height)]],
            [[int(499/const_width_screen*screen_width), int(391/const_height_screen*screen_height)],
             [int(499/const_width_screen*screen_width), int(340/const_height_screen*screen_height)],
             [int(496/const_width_screen*screen_width), int(371/const_height_screen*screen_height)],
             [int(498/const_width_screen*screen_width), int(319/const_height_screen*screen_height)]],
            [[int(471/const_width_screen*screen_width), int(391/const_height_screen*screen_height)],
             [int(471/const_width_screen*screen_width), int(332/const_height_screen*screen_height)],
             [int(466/const_width_screen*screen_width), int(372/const_height_screen*screen_height)],
             [int(470/const_width_screen*screen_width), int(316/const_height_screen*screen_height)]],
            [[int(445/const_width_screen*screen_width), int(423/const_height_screen*screen_height)],
             [int(443/const_width_screen*screen_width), int(323/const_height_screen*screen_height)],
             [int(442/const_width_screen*screen_width), int(396/const_height_screen*screen_height)],
             [int(443/const_width_screen*screen_width), int(316/const_height_screen*screen_height)]],
            [[int(687/const_width_screen*screen_width), int(501/const_height_screen*screen_height)],
             [int(700/const_width_screen*screen_width), int(425/const_height_screen*screen_height)],
             [int(674/const_width_screen*screen_width), int(437/const_height_screen*screen_height)],
             [int(642/const_width_screen*screen_width), int(443/const_height_screen*screen_height)]],
            [[int(652/const_width_screen*screen_width), int(499/const_height_screen*screen_height)],
             [int(655/const_width_screen*screen_width), int(484/const_height_screen*screen_height)],
             [int(653/const_width_screen*screen_width), int(484/const_height_screen*screen_height)],
             [int(655/const_width_screen*screen_width), int(463/const_height_screen*screen_height)]],
            [[int(615/const_width_screen*screen_width), int(447/const_height_screen*screen_height)],
             [int(637/const_width_screen*screen_width), int(618/const_height_screen*screen_height)],
             [int(573/const_width_screen*screen_width), int(340/const_height_screen*screen_height)],
             [int(591/const_width_screen*screen_width), int(502/const_height_screen*screen_height)]],
            [[int(553/const_width_screen*screen_width), int(487/const_height_screen*screen_height)],
             [int(576/const_width_screen*screen_width), int(482/const_height_screen*screen_height)],
             [int(573/const_width_screen*screen_width), int(419/const_height_screen*screen_height)],
             [int(531/const_width_screen*screen_width), int(460/const_height_screen*screen_height)]],
            [[int(552/const_width_screen*screen_width), int(514/const_height_screen*screen_height)],
             [int(583/const_width_screen*screen_width), int(488/const_height_screen*screen_height)],
             [int(537/const_width_screen*screen_width), int(440/const_height_screen*screen_height)],
             [int(548/const_width_screen*screen_width), int(484/const_height_screen*screen_height)]],
            [[int(502/const_width_screen*screen_width), int(448/const_height_screen*screen_height)],
             [int(465/const_width_screen*screen_width), int(479/const_height_screen*screen_height)],
             [int(464/const_width_screen*screen_width), int(542/const_height_screen*screen_height)],
             [int(512/const_width_screen*screen_width), int(507/const_height_screen*screen_height)]],
            [[int(514/const_width_screen*screen_width), int(502/const_height_screen*screen_height)],
             [int(517/const_width_screen*screen_width), int(480/const_height_screen*screen_height)],
             [int(497/const_width_screen*screen_width), int(467/const_height_screen*screen_height)],
             [int(472/const_width_screen*screen_width), int(445/const_height_screen*screen_height)]]]


samples = [sample0, sample1, sample2]

alef = [[[943, 255], [879, 257], [878, 190], [946, 188]],
        [[879, 260], [879, 216], [879, 234], [879, 176]]]
alef = [[[int(943/const_width_screen*screen_width), int(255/const_height_screen*screen_height)],
         [int(879/const_width_screen*screen_width), int(257/const_height_screen*screen_height)],
         [int(878/const_width_screen*screen_width), int(190/const_height_screen*screen_height)],
         [int(946/const_width_screen*screen_width), int(188/const_height_screen*screen_height)]],
        [[int(879/const_width_screen*screen_width), int(260/const_height_screen*screen_height)],
         [int(879/const_width_screen*screen_width), int(216/const_height_screen*screen_height)],
         [int(879/const_width_screen*screen_width), int(234/const_height_screen*screen_height)],
         [int(879/const_width_screen*screen_width), int(176/const_height_screen*screen_height)]]]
bet = [[[916, 266], [940, 204], [889, 174], [859, 205]],
        [[867, 267], [883, 235], [900, 236], [917, 267]]]
bet = [[[int(916/const_width_screen*screen_width), int(266/const_height_screen*screen_height)],
        [int(940/const_width_screen*screen_width), int(204/const_height_screen*screen_height)],
        [int(889/const_width_screen*screen_width), int(174/const_height_screen*screen_height)],
        [int(859/const_width_screen*screen_width), int(205/const_height_screen*screen_height)]],
        [[int(867/const_width_screen*screen_width), int(267/const_height_screen*screen_height)],
        [int(883/const_width_screen*screen_width), int(235/const_height_screen*screen_height)],
        [int(900/const_width_screen*screen_width), int(236/const_height_screen*screen_height)],
        [int(917/const_width_screen*screen_width), int(267/const_height_screen*screen_height)]]]
gimel = [[[933, 261], [882, 273], [882, 220], [931, 225]],
        [[932, 225], [931, 194], [932, 208], [931, 184]]]
gimel = [[[int(933/const_width_screen*screen_width), int(261/const_height_screen*screen_height)],
        [int(882/const_width_screen*screen_width), int(273/const_height_screen*screen_height)],
        [int(882/const_width_screen*screen_width), int(220/const_height_screen*screen_height)],
        [int(931/const_width_screen*screen_width), int(225/const_height_screen*screen_height)]],
        [[int(931/const_width_screen*screen_width), int(225/const_height_screen*screen_height)],
        [int(931/const_width_screen*screen_width), int(194/const_height_screen*screen_height)],
        [int(931/const_width_screen*screen_width), int(208/const_height_screen*screen_height)],
        [int(931/const_width_screen*screen_width), int(184/const_height_screen*screen_height)]]]
dalet = [[[913, 273], [926, 240], [921, 200], [898, 228]],
        [[899, 230], [941, 232], [928, 164], [861, 200]]]
dalet = [[[int(913/const_width_screen*screen_width), int(273/const_height_screen*screen_height)],
        [int(926/const_width_screen*screen_width), int(240/const_height_screen*screen_height)],
        [int(921/const_width_screen*screen_width), int(200/const_height_screen*screen_height)],
        [int(898/const_width_screen*screen_width), int(228/const_height_screen*screen_height)]],
        [[int(899/const_width_screen*screen_width), int(230/const_height_screen*screen_height)],
        [int(941/const_width_screen*screen_width), int(232/const_height_screen*screen_height)],
        [int(928/const_width_screen*screen_width), int(164/const_height_screen*screen_height)],
        [int(861/const_width_screen*screen_width), int(200/const_height_screen*screen_height)]]]
he = [[[926, 254], [930, 200], [934, 177], [874, 182]],
        [[885, 252], [885, 226], [886, 237], [885, 213]]]
he = [[[int(926/const_width_screen*screen_width), int(254/const_height_screen*screen_height)],
        [int(930/const_width_screen*screen_width), int(200/const_height_screen*screen_height)],
        [int(934/const_width_screen*screen_width), int(177/const_height_screen*screen_height)],
        [int(874/const_width_screen*screen_width), int(182/const_height_screen*screen_height)]],
        [[int(885/const_width_screen*screen_width), int(252/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(226/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(237/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(213/const_height_screen*screen_height)]]]
vav = [[[923, 255], [923, 209], [923, 229], [923, 176]]]
vav = [[[int(923/const_width_screen*screen_width), int(255/const_height_screen*screen_height)],
        [int(923/const_width_screen*screen_width), int(209/const_height_screen*screen_height)],
        [int(923/const_width_screen*screen_width), int(229/const_height_screen*screen_height)],
        [int(923/const_width_screen*screen_width), int(176/const_height_screen*screen_height)]]]
zayin = [[[888, 265], [952, 275], [950, 207], [885, 224]],
        [[885, 225], [884, 203], [884, 209], [885, 188]]]
zayin = [[[int(888/const_width_screen*screen_width), int(265/const_height_screen*screen_height)],
        [int(952/const_width_screen*screen_width), int(275/const_height_screen*screen_height)],
        [int(950/const_width_screen*screen_width), int(207/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(224/const_height_screen*screen_height)]],
        [[int(885/const_width_screen*screen_width), int(225/const_height_screen*screen_height)],
        [int(884/const_width_screen*screen_width), int(203/const_height_screen*screen_height)],
        [int(884/const_width_screen*screen_width), int(209/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(188/const_height_screen*screen_height)]]]
het = [[[916, 277], [933, 178], [846, 178], [861, 279]]]
het = [[[int(916/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(933/const_width_screen*screen_width), int(178/const_height_screen*screen_height)],
        [int(846/const_width_screen*screen_width), int(178/const_height_screen*screen_height)],
        [int(861/const_width_screen*screen_width), int(279/const_height_screen*screen_height)]]]
tet = [[[942, 247], [916, 325], [854, 220], [913, 176]]]
tet = [[[int(942/const_width_screen*screen_width), int(247/const_height_screen*screen_height)],
        [int(916/const_width_screen*screen_width), int(325/const_height_screen*screen_height)],
        [int(854/const_width_screen*screen_width), int(220/const_height_screen*screen_height)],
        [int(913/const_width_screen*screen_width), int(176/const_height_screen*screen_height)]]]
yud = [[[921, 230], [921, 199], [922, 211], [922, 180]]]
yud = [[[int(921/const_width_screen*screen_width), int(230/const_height_screen*screen_height)],
        [int(921/const_width_screen*screen_width), int(199/const_height_screen*screen_height)],
        [int(922/const_width_screen*screen_width), int(211/const_height_screen*screen_height)],
        [int(922/const_width_screen*screen_width), int(180/const_height_screen*screen_height)]]]
kaf = [[[872, 258], [946, 276], [949, 174], [872, 188]]]
kaf = [[[int(872/const_width_screen*screen_width), int(258/const_height_screen*screen_height)],
        [int(946/const_width_screen*screen_width), int(276/const_height_screen*screen_height)],
        [int(949/const_width_screen*screen_width), int(174/const_height_screen*screen_height)],
        [int(872/const_width_screen*screen_width), int(188/const_height_screen*screen_height)]]]
kaf_sofit = [[[869, 190], [943, 169], [934, 251], [880, 245]],
             [[879, 325], [881, 274], [879, 296], [879, 245]]]
kaf_sofit = [[[int(869/const_width_screen*screen_width), int(190/const_height_screen*screen_height)],
            [int(943/const_width_screen*screen_width), int(169/const_height_screen*screen_height)],
            [int(934/const_width_screen*screen_width), int(251/const_height_screen*screen_height)],
            [int(880/const_width_screen*screen_width), int(245/const_height_screen*screen_height)]],
            [[int(879/const_width_screen*screen_width), int(325/const_height_screen*screen_height)],
            [int(881/const_width_screen*screen_width), int(274/const_height_screen*screen_height)],
            [int(879/const_width_screen*screen_width), int(296/const_height_screen*screen_height)],
            [int(879/const_width_screen*screen_width), int(245/const_height_screen*screen_height)]]]
lamed = [[[887, 278], [948, 286], [866, 178], [934, 181]],
        [[889, 277], [867, 277], [887, 241], [908, 267]]]
lamed = [[[int(887/const_width_screen*screen_width), int(278/const_height_screen*screen_height)],
        [int(948/const_width_screen*screen_width), int(286/const_height_screen*screen_height)],
        [int(866/const_width_screen*screen_width), int(178/const_height_screen*screen_height)],
        [int(934/const_width_screen*screen_width), int(181/const_height_screen*screen_height)]],
        [[int(889/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(867/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(887/const_width_screen*screen_width), int(241/const_height_screen*screen_height)],
        [int(908/const_width_screen*screen_width), int(267/const_height_screen*screen_height)]]]
mem = [[[926, 310], [942, 528], [857, 168], [871, 382]]]
mem = [[[int(926/const_width_screen*screen_width), int(310/const_height_screen*screen_height)],
        [int(942/const_width_screen*screen_width), int(528/const_height_screen*screen_height)],
        [int(857/const_width_screen*screen_width), int(168/const_height_screen*screen_height)],
        [int(871/const_width_screen*screen_width), int(382/const_height_screen*screen_height)]]]
mem_sofit = [[[815, 349], [815, 162], [952, 368], [823, 340]]]
mem_sofit = [[[int(815/const_width_screen*screen_width), int(349/const_height_screen*screen_height)],
            [int(815/const_width_screen*screen_width), int(162/const_height_screen*screen_height)],
            [int(952/const_width_screen*screen_width), int(368/const_height_screen*screen_height)],
            [int(823/const_width_screen*screen_width), int(340/const_height_screen*screen_height)]]]
nun = [[[926, 177], [930, 275], [931, 258], [885, 257]]]
nun = [[[int(926/const_width_screen*screen_width), int(177/const_height_screen*screen_height)],
        [int(930/const_width_screen*screen_width), int(275/const_height_screen*screen_height)],
        [int(931/const_width_screen*screen_width), int(258/const_height_screen*screen_height)],
        [int(885/const_width_screen*screen_width), int(257/const_height_screen*screen_height)]]]
nun_sofit = [[[921, 295], [921, 210], [921, 258], [921, 179]]]
nun_sofit = [[[int(921/const_width_screen*screen_width), int(295/const_height_screen*screen_height)],
            [int(921/const_width_screen*screen_width), int(210/const_height_screen*screen_height)],
            [int(921/const_width_screen*screen_width), int(258/const_height_screen*screen_height)],
            [int(921/const_width_screen*screen_width), int(179/const_height_screen*screen_height)]]]
sameh = [[[810, 204], [827, 387], [958, 167], [814, 205]]]
sameh = [[[int(810/const_width_screen*screen_width), int(204/const_height_screen*screen_height)],
        [int(827/const_width_screen*screen_width), int(387/const_height_screen*screen_height)],
        [int(958/const_width_screen*screen_width), int(167/const_height_screen*screen_height)],
        [int(814/const_width_screen*screen_width), int(205/const_height_screen*screen_height)]]]
ayin = [[[859, 178], [709, 287], [951, 288], [805, 179]]]
ayin = [[[int(859/const_width_screen*screen_width), int(178/const_height_screen*screen_height)],
        [int(709/const_width_screen*screen_width), int(287/const_height_screen*screen_height)],
        [int(951/const_width_screen*screen_width), int(288/const_height_screen*screen_height)],
        [int(805/const_width_screen*screen_width), int(179/const_height_screen*screen_height)]]]
pei = [[[859, 191], [943, 172], [938, 265], [878, 264]],
        [[879, 263], [847, 266], [849, 211], [894, 234]]]
pei = [[[int(859/const_width_screen*screen_width), int(191/const_height_screen*screen_height)],
        [int(943/const_width_screen*screen_width), int(172/const_height_screen*screen_height)],
        [int(938/const_width_screen*screen_width), int(265/const_height_screen*screen_height)],
        [int(878/const_width_screen*screen_width), int(264/const_height_screen*screen_height)]],
        [[int(879/const_width_screen*screen_width), int(263/const_height_screen*screen_height)],
        [int(847/const_width_screen*screen_width), int(266/const_height_screen*screen_height)],
        [int(849/const_width_screen*screen_width), int(211/const_height_screen*screen_height)],
        [int(894/const_width_screen*screen_width), int(234/const_height_screen*screen_height)]]]
pei_sofit = [[[905, 265], [881, 305], [946, 290], [893, 197]],
             [[903, 245], [788, 266], [958, 200], [893, 197]]]
pei_sofit = [[[int(905/const_width_screen*screen_width), int(265/const_height_screen*screen_height)],
            [int(881/const_width_screen*screen_width), int(305/const_height_screen*screen_height)],
            [int(946/const_width_screen*screen_width), int(290/const_height_screen*screen_height)],
            [int(893/const_width_screen*screen_width), int(197/const_height_screen*screen_height)]],
            [[int(903/const_width_screen*screen_width), int(245/const_height_screen*screen_height)],
            [int(788/const_width_screen*screen_width), int(266/const_height_screen*screen_height)],
            [int(958/const_width_screen*screen_width), int(200/const_height_screen*screen_height)],
            [int(893/const_width_screen*screen_width), int(197/const_height_screen*screen_height)]]]
tsadi =[[[878, 277], [940, 285], [947, 226], [878, 230]],
        [[879, 230], [929, 233], [932, 187], [876, 198]]]
tsadi =[[[int(878/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(940/const_width_screen*screen_width), int(285/const_height_screen*screen_height)],
        [int(947/const_width_screen*screen_width), int(226/const_height_screen*screen_height)],
        [int(878/const_width_screen*screen_width), int(230/const_height_screen*screen_height)]],
        [[int(879/const_width_screen*screen_width), int(230/const_height_screen*screen_height)],
        [int(929/const_width_screen*screen_width), int(233/const_height_screen*screen_height)],
        [int(932/const_width_screen*screen_width), int(187/const_height_screen*screen_height)],
        [int(876/const_width_screen*screen_width), int(198/const_height_screen*screen_height)]]]
tsadi_sofit = [[[886, 267], [840, 299], [936, 309], [874, 199]],
                [[852, 199], [812, 228], [936, 217], [878, 199]]]
tsadi_sofit = [[[int(886/const_width_screen*screen_width), int(267/const_height_screen*screen_height)],
                [int(840/const_width_screen*screen_width), int(299/const_height_screen*screen_height)],
                [int(936/const_width_screen*screen_width), int(309/const_height_screen*screen_height)],
                [int(874/const_width_screen*screen_width), int(199/const_height_screen*screen_height)]],
                [[int(852/const_width_screen*screen_width), int(199/const_height_screen*screen_height)],
                [int(812/const_width_screen*screen_width), int(228/const_height_screen*screen_height)],
                [int(936/const_width_screen*screen_width), int(217/const_height_screen*screen_height)],
                [int(878/const_width_screen*screen_width), int(199/const_height_screen*screen_height)]]]
kuf =[[[918, 260], [945, 209], [917, 174], [872, 202]],
        [[894, 308], [893, 259], [894, 275], [893, 234]]]
kuf =[[[int(918/const_width_screen*screen_width), int(260/const_height_screen*screen_height)],
        [int(945/const_width_screen*screen_width), int(209/const_height_screen*screen_height)],
        [int(917/const_width_screen*screen_width), int(174/const_height_screen*screen_height)],
        [int(872/const_width_screen*screen_width), int(202/const_height_screen*screen_height)]],
        [[int(894/const_width_screen*screen_width), int(308/const_height_screen*screen_height)],
        [int(893/const_width_screen*screen_width), int(259/const_height_screen*screen_height)],
        [int(894/const_width_screen*screen_width), int(275/const_height_screen*screen_height)],
        [int(893/const_width_screen*screen_width), int(234/const_height_screen*screen_height)]]]
resh = [[[927, 255], [938, 208], [918, 171], [870, 186]]]
resh = [[[int(927/const_width_screen*screen_width), int(255/const_height_screen*screen_height)],
        [int(938/const_width_screen*screen_width), int(208/const_height_screen*screen_height)],
        [int(918/const_width_screen*screen_width), int(171/const_height_screen*screen_height)],
        [int(870/const_width_screen*screen_width), int(186/const_height_screen*screen_height)]]]
shin = [[[924, 267], [899, 306], [832, 269], [865, 216]],
        [[866, 216], [889, 180], [930, 229], [865, 266]]]
shin = [[[int(924/const_width_screen*screen_width), int(267/const_height_screen*screen_height)],
        [int(899/const_width_screen*screen_width), int(306/const_height_screen*screen_height)],
        [int(832/const_width_screen*screen_width), int(269/const_height_screen*screen_height)],
        [int(865/const_width_screen*screen_width), int(216/const_height_screen*screen_height)]],
        [[int(866/const_width_screen*screen_width), int(216/const_height_screen*screen_height)],
        [int(889/const_width_screen*screen_width), int(180/const_height_screen*screen_height)],
        [int(930/const_width_screen*screen_width), int(229/const_height_screen*screen_height)],
        [int(865/const_width_screen*screen_width), int(266/const_height_screen*screen_height)]]]
taf = [[[937, 278], [944, 184], [878, 181], [884, 277]],
        [[885, 278], [857, 279], [862, 279], [838, 279]]]
taf = [[[int(937/const_width_screen*screen_width), int(278/const_height_screen*screen_height)],
        [int(944/const_width_screen*screen_width), int(184/const_height_screen*screen_height)],
        [int(878/const_width_screen*screen_width), int(181/const_height_screen*screen_height)],
        [int(884/const_width_screen*screen_width), int(277/const_height_screen*screen_height)]],
        [[int(885/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(857/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(862/const_width_screen*screen_width), int(277/const_height_screen*screen_height)],
        [int(838/const_width_screen*screen_width), int(277/const_height_screen*screen_height)]]]

letters = [alef, bet, gimel, dalet, he, vav, zayin, het, tet, yud, kaf, kaf_sofit, lamed, mem, mem_sofit, nun, nun_sofit, sameh, ayin, pei, pei_sofit, tsadi, tsadi_sofit, kuf, resh, shin, taf]

buttons_enabled = True
IS_MOVING_ALL_CURVE = True
IDLE_TIME = 100 # seconds
IDLE_TIME_DRAW = 120 # seconds
enable_idle_drawing = False
idle_mode = False
MAX_RUNS = 60

SHOW_RED_BORDER = True

# arduino
MAX_TIME_WAITING_FOR_ARDUINO = 5  # seconds
MAX_DRAWING_TIME_FOR_ARDUINO = 150  # seconds
time_delay_arduino = 0.005  # seconds

LASER_POWER = 255  # (0 <= x <= 255)
CONTOUR_POWER = 255  # (0 <= x <= 255)
LASER_OFF_RATE = 6
LASER_ON_RATE = 6
CONTOUR_RATE = 50 # for green/yellow paper
MAX_DC_MOTOR_TIME = 1.5  # seconds

mm_per_pulse = [2*80.0/800, 2*80.0/800]  # mm per pulse for each motor
board_size = [83.0, 83.0]  # size of the board in mm
screen_scale = [board_size[0]/cuttingAreaWidth, board_size[1]/cuttingAreaHeight]  # scale from the screen to the board on arduino in mm per pixel
pulse_per_pixel = [screen_scale[0]/mm_per_pulse[0], screen_scale[1]/mm_per_pulse[1]]  # pulse per pixel for each motor

starting_key = -2
next_curve_key = -3
end_key = -4

found_arduino = False
send_to_arduino = False
drawing_curve = False
curve_index = 0
waiting = [False,False]  # two flags to indicate if we are waiting for the arduino to send us data: first is reading a curve, second is drawing one
last_time = [0, 0]  # to limit the time we wait for the arduino to send us data

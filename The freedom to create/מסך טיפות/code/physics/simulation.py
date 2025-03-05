# simulation of a row of nozzles dripping water from above

import pygame
import math
import time

height = 1.5 # meters
g = 9.8 # m/s^2
N = 20 # number of rows in image
W = 64 # number of columns in image

# color of the rainbows
Colors = [(255, 0, 0), (255, 127, 0), (255, 255, 0), (0, 255, 0), (0, 0, 255), (75, 0, 130), (148, 0, 211)]
screen_width = 800
screen_height = 600
water_width = 10
pixels_per_meter = screen_height / 1.5
pause_time = 1 # seconds
tv0 = 0.015 # seconds

def calc_time_row(row):
    global height, g, N
    return math.sqrt(height / ( N*g*(N - row - 0.5)))

def update_height():
    global height, tv0, g, N
    height = tv0**2 * N * g * (N - 0.5) 

def draw_points():
    global points, screen, pixels_per_meter
    if not points:
        return
    screen.fill((0, 0, 0))
    for i in range(len(points)-1):
        pygame.draw.line(screen, Colors[i % len(Colors)], (screen_width/2, points[i] * pixels_per_meter), (screen_width/2, points[i+1] * pixels_per_meter), water_width)
    pygame.draw.line(screen, Colors[(len(points)-1) % len(Colors)], (screen_width/2, points[-1] * pixels_per_meter), (screen_width/2, 0), water_width)
    pygame.display.flip()

def get_avarage_lenth():
    global height, g, N, points
    sum = 0
    for i in range(len(points)-1):
        sum += points[i+1] - points[i]
    sum += points[-1]
    return -sum / len(points)

def get_total_length():
    global height, g, N, points
    sum = 0
    for i in range(len(points)-1):
        sum += points[i+1] - points[i]
    sum += points[-1]
    return -sum

def get_error():
    global height, g, N, points
    av = get_avarage_lenth()
    sum = 0
    for i in range(len(points)-1):
        sum += (-points[i+1] + points[i] - av)**2
    sum += (points[-1] - av)**2
    return math.sqrt(sum / len(points))

pygame.init()
screen = pygame.display.set_mode((screen_width, screen_height))
screen.fill((0, 0, 0))
pygame.display.set_caption("Simulation")

running = True
water_on = False
last_time = time.time()
row = 0
points = []
last_avarage = 0
last_error = 0
last_total_length = 0
starting_times = []
time_per_row = []
clock = pygame.time.Clock()
update_height()
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN: 
            if event.key == pygame.K_ESCAPE:
                running = False
            if event.key == pygame.K_LEFT:
                tv0 -= 0.001
                update_height()
                print("Height: {:.2f} meters, tv0: {:.3f} s".format(height, tv0))
            if event.key == pygame.K_RIGHT:
                tv0 += 0.001
                update_height()
                print("Height: {:.2f} meters, tv0: {:.3f} s".format(height, tv0))
            if event.key == pygame.K_a:
                print("Average length: {:.3f} meters".format(last_avarage))
                print("Error: {:.3f}".format(last_error))
                print("Total length: {:.1f} meters".format(last_total_length))
    if not water_on:
        water_on = True
        last_time = time.time()
        points.append(0)
        starting_times.append(time.time())
        time_per_row.append(calc_time_row(row))
    else:
        if time.time() - last_time > time_per_row[row]:
            water_on = False
            row += 1
            if row == N:
                last_avarage = get_avarage_lenth()
                last_error = get_error()
                last_total_length = get_total_length()
                draw_points()
                time.sleep(pause_time)
                screen.fill((0, 0, 0))
                pygame.display.flip()
                row = 0
                last_time = time.time()
                points.clear()
                starting_times.clear()
                time_per_row.clear()
    for i in range(len(points)):
        points[i] = 0.5 * g * (time.time() - starting_times[i])**2 
    draw_points()
'''
Created on May 11 2015

@author: Or Levi
'''
import pygame
import math
import config as config
from pygame.locals import *


#canvas size constants
WIDTH = 600
HEIGHT = 600

#drawing constants
OFFSET = 1
GRAY = (192,192,192)
LIGHT_GRAY = (224,224,224)
DARK_GRAY = (128,128,128)
WHITE = (255,255,255)
BLACK = (0,0,0)
RED = (255,0,0)
LIGHT_RED = (255,115,115)
LIGHT_GREEN = (115,255,115)
GREEN = (0,255,0)
BLUE = (0,0,255)

Y_SCALE = 2.0/600

#events constants
LEFT = 1

class Plotter():
    
    def __init__(self, pos_factor, size_factor):
        self.pos_factor = pos_factor
        self.size_factor = size_factor
        self.update_layout(WIDTH, HEIGHT)
        self.font = pygame.font.SysFont("monospace", 10)
        
    def update_layout(self, width, height):
        self.position = (self.pos_factor[0] * width, self.pos_factor[1] * height)
        self.size = (self.size_factor[0] * width, self.size_factor[1] * height)
        self.y_scale = Y_SCALE * self.size[1] 

        
    def draw(self, canvas, freq, x_data, y_data_1, y_data_2 = []):
        self.draw_frame(canvas)
        self.draw_freq_marker(canvas, freq, x_data)
        self.draw_graphs(canvas, x_data, y_data_1, y_data_2)
        self.draw_labels(canvas, x_data)
        self.draw_ticks(canvas)
    
    def draw_freq_marker(self, canvas, freq, x_data):
        if x_data is not None and len(x_data) > 0 and x_data[0] <= freq <= x_data[-1]:
        #if x_data != [] and freq >= x_data[0] and freq <= x_data[-1]:
            x_pos = (float(freq - x_data[0]) / (x_data[-1] - x_data[0])) * self.size[0]
            pygame.draw.line(canvas, RED, (self.position[0] + x_pos, self.position[1] + self.size[1]-1), (self.position[0] + x_pos, self.position[1]), 2)   

    
    def draw_frame(self, canvas):
        pygame.draw.rect(canvas, WHITE, (self.position[0],self.position[1],self.size[0],self.size[1]), 0)        
        pygame.draw.rect(canvas, BLACK, (self.position[0],self.position[1],self.size[0],self.size[1]), 1)
     
    def draw_graphs(self, canvas, x_data, y_data_1, y_data_2):
        #if y_data_2 is not None and len(y_data_2) > 0:
        if y_data_2 != []:
            graph_2 = self.create_graph_vector(x_data, y_data_2)
            pygame.draw.lines(canvas, DARK_GRAY, 0, graph_2)
        #if y_data_1 is not None and len(y_data_1) > 0:
        if y_data_1 != []:
            graph_1 = self.create_graph_vector(x_data, y_data_1)
            pygame.draw.lines(canvas, BLUE, 0, graph_1)
             
    def draw_labels(self, canvas, x_data):
        #if x_data is not None and len(x_data) > 0:
        if x_data != []:
            label0 = self.font.render("{0:.1f}".format(x_data[0]), 1, BLACK)
            canvas.blit(label0, (self.position[0]-label0.get_width()/2,self.position[1]+(self.size[1]+self.font.get_height()/8)))
            label1 = self.font.render("{0:.1f}".format(x_data[len(x_data)//4]), 1, BLACK)
            canvas.blit(label1, (self.position[0]+self.size[0]/4-label1.get_width()/2,self.position[1]+(self.size[1]+self.font.get_height()/8)))
            label2 = self.font.render("{0:.1f}".format(x_data[len(x_data)//2]), 1, BLACK)  
            canvas.blit(label2, (self.position[0]+self.size[0]/2-label2.get_width()/2,self.position[1]+(self.size[1]+self.font.get_height()/8)))
            label3 = self.font.render("{0:.1f}".format(x_data[3*len(x_data)//4]), 1, BLACK)  
            canvas.blit(label3, (self.position[0]+3*self.size[0]/4-label3.get_width()/2,self.position[1]+(self.size[1]+self.font.get_height()/8)))
            label4 = self.font.render("{0:.1f}".format(x_data[len(x_data)-1]), 1, BLACK)  
            canvas.blit(label4, (self.position[0]+self.size[0]-label4.get_width()/2,self.position[1]+(self.size[1]+self.font.get_height()/8)))      
        
    def draw_ticks(self, canvas):
        pygame.draw.line(canvas, BLACK, (self.position[0] + self.size[0]/8, self.position[1] + self.size[1]-1), (self.position[0] + self.size[0]/8,self.position[1] + 0.98*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + self.size[0]/4, self.position[1] + self.size[1]-1), (self.position[0] + self.size[0]/4,self.position[1] + 0.96*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + 3*self.size[0]/8, self.position[1] + self.size[1]-1), (self.position[0] + 3*self.size[0]/8,self.position[1] + 0.98*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + self.size[0]/2, self.position[1] + self.size[1]-1), (self.position[0] + self.size[0]/2,self.position[1] + 0.96*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + 5*self.size[0]/8, self.position[1] + self.size[1]-1), (self.position[0] + 5*self.size[0]/8,self.position[1] + 0.98*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + 3*self.size[0]/4, self.position[1] + self.size[1]-1), (self.position[0] + 3*self.size[0]/4,self.position[1] + 0.96*self.size[1]))   
        pygame.draw.line(canvas, BLACK, (self.position[0] + 7*self.size[0]/8, self.position[1] + self.size[1]-1), (self.position[0] + 7*self.size[0]/8,self.position[1] + 0.98*self.size[1]))    
      
    def create_graph_vector(self, x_data, y_data):
        x_scale = float(self.size[0]-2) / (len(x_data)+1)
        line = []
        real_x_pos = self.position[0]+1
        for y in y_data:
            real_x_pos = float(real_x_pos + x_scale)
            y_shift = y * self.y_scale    
            if y_shift <= 0:      
                y_shift = 1     
            elif y_shift > self.size[1]:
                y_shift = self.size[1]
            real_y_pos = self.position[1] + self.size[1] - y_shift
            line.append((real_x_pos, real_y_pos))
        return line
    
    def check_click(self, pos, x_data):
        if x_data != []:
            if (pos[0] >= self.position[0]) and (pos[0] <= self.position[0]+self.size[0]) and (pos[1] >= self.position[1]) and (pos[1] <= self.position[1]+self.size[1]):
                return self.get_click_x_pos(pos, x_data)
        return 0

    def get_click_x_pos(self, pos, x_data):
        x_pos = int(x_data[0] + (x_data[-1] - x_data[0]) * (pos[0] - self.position[0]) / float(self.size[0]))
        return x_pos
        

class Scroll():
    
    def __init__(self, pos_factor, size_factor, drag_func, untouched_factor = 0, max_value = 1, color = LIGHT_GRAY):
        self.pos_factor = pos_factor
        self.size_factor = size_factor
        self.drag_function = drag_func
        self.color = color
        self.update_layout(WIDTH, HEIGHT)
        self.is_clicked = False
        self.key_was_pressed = False
        self.timeout_count = False
        self.timeout_value = 0.0
        self.control_key_pressed = 0
        self.click_height = 0
        self.untouched_value = untouched_factor * self.size[1]
        self.max_value = max_value
        self.height_offset = self.untouched_value
        
    def update_layout(self, width, height):
        self.position = (self.pos_factor[0] * width, self.pos_factor[1] * height)
        self.size = (self.size_factor[0] * width, self.size_factor[1] * height)
        
    def draw(self, canvas):
        if self.is_clicked:
            self.mouse_position_update()
            self.drag_function(float(self.max_value * self.height_offset / self.size[1]))  
        if self.key_was_pressed:
            self.keyboard_position_update()
            self.key_was_pressed = False
            self.drag_function(float(self.max_value * self.height_offset / self.size[1]))   
        pygame.draw.line(canvas, DARK_GRAY, (self.position[0], self.position[1]), (self.position[0], self.position[1] + self.size[1]), 3)
        pygame.draw.rect(canvas, self.color, (self.position[0] - self.size[0]/2, self.position[1] + 39.0/40*self.size[1] - self.height_offset, self.size[0],self.size[1]/20), 0)
        
    def mouse_position_update(self):
        pos = pygame.mouse.get_pos()
        self.height_offset = self.click_height - pos[1] + self.untouched_value
        self.update_scroll_position()
        
    def keyboard_position_update(self):
        self.height_offset = self.untouched_value + self.control_key_pressed * config.KEYBOARD_VOLUME_JUMP * self.size[1]
        self.update_scroll_position()
    
    def update_scroll_position(self):
        if self.height_offset < 0:
            self.height_offset = 0
        elif self.height_offset > self.size[1]:
            self.height_offset = self.size[1]

    def check_click(self, pos):
        if not self.is_clicked and (pos[0] >= self.position[0]-self.size[0]/2) and (pos[0] <= self.position[0]+self.size[0]/2) and (pos[1] >= self.position[1]+(39.0/40)*self.size[1] - self.untouched_value) and (pos[1] <= self.position[1]+(41.0/40)*self.size[1] - self.untouched_value):
            self.click_height = pos[1]
            self.is_clicked = True
              
    def click_release(self):
        if self.is_clicked:
            self.is_clicked = False       
            self.click_height = 0
            self.height_offset = self.untouched_value
            self.drag_function(float(self.height_offset / self.size[1]))
            
    def change_key_pressed(self, value):
        self.control_key_pressed = self.control_key_pressed + value
        self.key_was_pressed = True
        self.timeout_count = False
        self.timeout_value = 0.0
            
    def change_key_released(self, value):
        self.timeout_count = True
        
    def handle_control_timeout(self, ms):
        if self.timeout_count:
            self.timeout_value = self.timeout_value + ms / 1000.0
            if self.timeout_value > config.KEYBOARD_VOLUME_TIMEOUT:
                self.control_key_pressed = 0
                self.keyboard_position_update()
                self.drag_function(float(self.height_offset / self.size[1]))

class Button():
    '''
    this class represnts a button (used by left mouse clicks).
    '''
    
    def __init__(self, pos_factor, size_factor, text, press_func = None, release_func = None, color = GRAY, second_color = None):
        '''
        constructor.
        pos_factor   - is the relative position of the button in the program layout (for example, [0.5,0.5] means the upper 
                       left corner of the button will be positioned in the middle of the program window)
        size_factor  - is the relative size of the button in the program layout (for example, [0.5,0.5] means the button size
                       will be half from the program window length and half of the window width - quarter of the window area)
        text         - the text that will be displayed on the button
        press_func   - if given, this function will be called when the button is clicked
        release_func - if given, this function will be called when the button is released
        color        - the base color of the interior area of the button. default is gray.
        second_color - if given, the button will change its color to this color when it is clicked (and back to the original 
                       when it is released)   
        '''
        self.pos_factor = pos_factor
        self.size_factor = size_factor
        self.update_layout(WIDTH, HEIGHT)
        self.text = text
        self.first_color = color
        self.second_color = second_color
        self.color = color
        self.is_clicked = False
        self.press_func = press_func
        self.release_func = release_func
        self.font = pygame.font.SysFont("monospace", 18)
        
    def update_layout(self, width, height):
        '''
        update the actual position and size of the of the button in case the program window size was changed 
        '''
        self.position = (self.pos_factor[0] * width, self.pos_factor[1] * height)
        self.size = (self.size_factor[0] * width, self.size_factor[1] * height)
                
    def check_click(self, pos):
        '''
        check if the button was clicked. calls the press function if it was given.
        '''
        if (pos[1] >= self.position[0]) and (pos[1] <= self.position[0]+self.size[0]) and (pos[0] >= self.position[1]) and (pos[0] <= self.position[1]+self.size[1]):
            self.is_clicked = True
            if self.second_color != None:
                self.color = self.second_color
            if self.press_func != None:
                self.press_func()
                          
    def click_release(self):
        '''
        upon a left mouse button release event, if the button was pressed, it is released and the release function is 
        called (if such function was given) 
        '''
        if self.is_clicked:
            self.is_clicked = False
            self.color = self.first_color
            if self.release_func != None:
                self.release_func()
            
    def draw(self, canvas):
        '''
        draw the button to the screen
        '''
        if self.is_clicked:
            color1 = DARK_GRAY
            color2 = LIGHT_GRAY
        else:
            color1 = LIGHT_GRAY
            color2 = DARK_GRAY   
                       
        pygame.draw.lines(canvas, color1, 0, [(self.position[1]-OFFSET,self.position[0]+self.size[0]+OFFSET-1), (self.position[1]-OFFSET, self.position[0]-OFFSET), (self.position[1]+self.size[1]+OFFSET-1, self.position[0]-OFFSET)])
        pygame.draw.lines(canvas, color2, 0, [(self.position[1]+self.size[1]+OFFSET-1, self.position[0]-OFFSET), (self.position[1]+self.size[1]+OFFSET-1, self.position[0]+self.size[0]+OFFSET-1), (self.position[1]-OFFSET,self.position[0]+self.size[0]+OFFSET-1)])
        pygame.draw.rect(canvas, self.color, (self.position[1],self.position[0],self.size[1],self.size[0]), 0)
        label = self.font.render(self.text, 1, BLACK)
        canvas.blit(label, (self.position[1]+(self.size[1]-label.get_width())/2,self.position[0]+(self.size[0]-self.font.get_height())/2))
        
class FrequencyController():
    '''
    this class contains several buttons to change the frequency and displays the current frequency
    '''
    def __init__(self, pos_factor, size_factor, window_height, window_width, interface):
        '''
        constructor
        '''
        self.window_height = window_height
        self.window_width = window_width
        self.size_factor = size_factor
        self.pos_factor = pos_factor
        self.interface = interface
        
        self.buttons = []
        
        self.update_layout(HEIGHT, WIDTH)
        self.find_font_size()
                
        self.kilo_digit = 0
        self.centum_digit = 0 
        self.deci_digit = 0
        self.unit_digit = 0
        self.fraction_digit = 0 
        
        self.buttons.append(Button((self.pos_factor[0],self.pos_factor[1]+5.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"+",self.centum_up))
        self.buttons.append(Button((self.pos_factor[0]+8.0/11*self.size_factor[0],self.pos_factor[1]+5.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"-",self.centum_down))
        self.buttons.append(Button((self.pos_factor[0],self.pos_factor[1]+10.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"+",self.deci_up))
        self.buttons.append(Button((self.pos_factor[0]+8.0/11*self.size_factor[0],self.pos_factor[1]+10.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"-",self.deci_down))
        self.buttons.append(Button((self.pos_factor[0],self.pos_factor[1]+15.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"+",self.unit_up))
        self.buttons.append(Button((self.pos_factor[0]+8.0/11*self.size_factor[0],self.pos_factor[1]+15.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"-",self.unit_down))
        self.buttons.append(Button((self.pos_factor[0],self.pos_factor[1]+20.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"+",self.fraction_up))
        self.buttons.append(Button((self.pos_factor[0]+8.0/11*self.size_factor[0],self.pos_factor[1]+20.0/24*self.size_factor[1]),(3.0/11*self.size_factor[0],4.0/24*self.size_factor[1]),"-",self.fraction_down))
        
    def find_font_size(self):
        size = 1
        font_letter_space = pygame.font.SysFont("monospace", size + 1).size("0")
        print("font_letter_space" + str(font_letter_space))

        while font_letter_space[1] <= self.size[0]/3.0 and font_letter_space[0] <= self.size[1]/5.0:
            size = size + 1
            font_letter_space = pygame.font.SysFont("monospace", size).size("0")
            print("size" + str(size))

        self.font = pygame.font.SysFont("monospace", size)
        
    def centum_up(self):
        self.interface.setFreq(self.interface.freq + 100)
        
    def centum_down(self):
        self.interface.setFreq(self.interface.freq - 100)
        
    def deci_up(self):
        self.interface.setFreq(self.interface.freq + 10)
        
    def deci_down(self):
        self.interface.setFreq(self.interface.freq - 10)
        
    def unit_up(self):
        self.interface.setFreq(self.interface.freq + 1)
        
    def unit_down(self):
        self.interface.setFreq(self.interface.freq - 1)
    
    def fraction_up(self):
        self.interface.setFreq(self.interface.freq + 0.1)
        
    def fraction_down(self):
        self.interface.setFreq(self.interface.freq - 0.1)
    
    def update_freq(self, freq):
        '''
        gets a new frequency and extracts the different digits from it
        '''
        self.kilo_digit = int(math.floor(freq / 1000.0))
        freq = freq - self.kilo_digit * 1000
        self.centum_digit = int(math.floor(freq / 100.0))
        freq = freq - self.centum_digit * 100        
        self.deci_digit = int(math.floor(freq / 10.0))
        freq = freq - self.deci_digit * 10                
        self.unit_digit = int(math.floor(freq))
        freq = freq - self.unit_digit
        self.fraction_digit = int(math.floor(freq / 0.1))
        
    def check_click(self, pos):
        '''
        check for clicks on the freq controller buttons
        '''
        for button in self.buttons:
            button.check_click(pos)
            
    def click_release(self):
        '''
        release pressed button (from the frequency controller buttons)
        '''
        for button in self.buttons:
            button.click_release()
            
    def update_layout(self, height, width):
        for button in self.buttons:
            button.update_layout(height, width) 
        self.size = (self.size_factor[1] * width, self.size_factor[0] * height)
        self.pos = (self.pos_factor[1] * width, self.pos_factor[0] * height)
        self.find_font_size()
    
    def draw(self, canvas):
        '''
        draws the frequency controller
        '''
        self.update_freq(self.interface.freq)
        for button in self.buttons:
            button.draw(canvas)
            
        fraction_digit_label = self.font.render(str(self.fraction_digit)+"Hz", 1, BLACK)
        unit_digit_label = self.font.render(str(self.unit_digit)+".", 1, BLACK)
        deci_digit_label = self.font.render(str(self.deci_digit), 1, BLACK)
        centum_digit_label = self.font.render(str(self.centum_digit), 1, BLACK)
        kilo_digit_label = self.font.render(str(self.kilo_digit), 1, BLACK)    
        
        canvas.blit(fraction_digit_label, (self.pos[0]+20.0/24*self.size[0], self.pos[1]+4.0/11*self.size[1]))
        canvas.blit(unit_digit_label, (self.pos[0]+15.0/24*self.size[0], self.pos[1]+4.0/11*self.size[1]))
        canvas.blit(deci_digit_label, (self.pos[0]+10.0/24*self.size[0], self.pos[1]+4.0/11*self.size[1]))
        canvas.blit(centum_digit_label, (self.pos[0]+5.0/24*self.size[0], self.pos[1]+4.0/11*self.size[1]))
        if self.kilo_digit != 0:
            canvas.blit(kilo_digit_label, (self.pos[0], self.pos[1]+4.0/11*self.size[1]))
              
class Gui():
    '''
    this class is for the graphic user interface of the program. it prints to the screen the buttons, graphs, 
    scrolls etc., and handles the user interaction with it (mouse clicks, keyboard key presses etc.) 
    '''

    def __init__(self, interface, sampler, player):
        '''
        Constructor
        '''
        pygame.init()                                           # initialize pygame
        self.fps_Clock = pygame.time.Clock()                    # set the FPS clock
        self.width = WIDTH
        self.height = HEIGHT
        self.canvas = pygame.display.set_mode((self.width, self.height), RESIZABLE)
        pygame.display.set_caption('Sound Demonstrations')
        icon = pygame.transform.scale(pygame.image.load('glass.png'), (32, 32))
        pygame.display.set_icon(icon)
        self.done = False
        self.font = pygame.font.SysFont("monospace", 30)

        self.interface = interface
        self.sampler = sampler
        self.player = player
                
        self.is_playing = False
        self.is_playing_wav_file = False
        
        self.in_glass = True
        self.in_chladni = False
        self.in_ruben = False
        
        self.time=0#####################################################################################################################S
        self.first_peak = 0
        self.second_peak = 0
        self.first_peak_button_text = ''
        self.second_peak_button_text = ''
        
        
        self.top_buttons = []
        self.glass_buttons = []
        self.chladni_buttons = []
        self.ruben_buttons = []  
        
        self.top_buttons.append(Button((1.0/60, 5.0/60),(3.0/60, 15.0/60),"Wine Glass", self.set_glass))
        self.top_buttons.append(Button((1.0/60, 22.0/60),(3.0/60, 15.0/60),"Chladni Plate", self.set_chladni))
        self.top_buttons.append(Button((1.0/60, 39.0/60),(3.0/60, 15.0/60),"Ruben's Tube", self.set_ruben))

        self.top_buttons.append(Button((41.0/60,3.0/60),(5.0/60,10.0/60),"Play", self.play_stop_wave, color = LIGHT_GREEN))

        self.glass_buttons.append(Button((53.0/60,3.0/60),(5.0/60,10.0/60),"RESET MAX", self.sampler.reset_max_fft))
        self.glass_buttons.append(Button((47.0/60,34.0/60),(5.0/60,25.0/60),'1st peak', self.set_first_peak))
        self.glass_buttons.append(Button((53.0/60,34.0/60),(5.0/60,25.0/60),'2nd peak', self.set_second_peak))
        self.glass_buttons.append(Button((47.0/60,3.0/60),(5.0/60,10.0/60),"Sweep", self.sweep_enable_disable))
        #self.glass_buttons.append(Button((35.0/60,5.0/60),(5.0/60,10.0/60),"FFT", self.sampler.stop_start_FFT_computation))
        
        self.chladni_buttons.append(Button((5.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq I", self.chladni_fixed_1))
        self.chladni_buttons.append(Button((11.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq II", self.chladni_fixed_2))
        self.chladni_buttons.append(Button((17.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq III", self.chladni_fixed_3))
        self.chladni_buttons.append(Button((23.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq IV", self.chladni_fixed_4))
        self.chladni_buttons.append(Button((29.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq V", self.chladni_fixed_5))
        self.chladni_buttons.append(Button((35.0/60,17.0/60),(5.0/60,33.0/60),"Chladni fixed freq VI", self.chladni_fixed_6))
        
        self.ruben_buttons.append(Button((5.0/60,17.0/60),(5.0/60,33.0/60),"Rubn's tube fixed freq I", self.ruben_fixed_1))
        self.ruben_buttons.append(Button((11.0/60,17.0/60),(5.0/60,33.0/60),"Rubn's tube fixed freq II", self.ruben_fixed_2))
        self.ruben_buttons.append(Button((17.0/60,17.0/60),(5.0/60,33.0/60),"Rubn's tube fixed freq III", self.ruben_fixed_3))
        self.ruben_buttons.append(Button((23.0/60,17.0/60),(5.0/60,33.0/60),"wav file", self.player.play_stop_wav_file))
        self.ruben_buttons.append(Button((29.0/60,17.0/60),(5.0/60,33.0/60),"microphone play", self.player.play_stop_mic))

        self.plotter = Plotter((17.5/60,5.0/60), (40.0/60,40.0/60))
        
        self.freq_controller = FrequencyController((47.0/60,14.0/60), (11.0/60,15.0/60), self.height, self.width, self.interface)
        
        self.volume_scroll = Scroll((8.0/60,5.5/60), (2.5/60,34.0/60), self.interface.setVol, config.VOLUME_DEFAULT, config.VOLUME_MAXIMUM)
        
        self.freq_line = []
        self.fft_data = []
        self.fft_peak_data = []
      
        self.sweep_enable = False
        self.sweep_time_counter = 0
        self.sweep_step_counter = 0
        self.sweep_center_freq = 0

        self.main_loop()                                        # start the main loop of the gui


    
    def draw(self):
        '''
        this method handles all the drawing to the screen. it is executed on every iteration of the main loop (should be
        ~60 frames per second).
        '''
        #get data necessary for the drawing
        if not self.is_playing:
            self.first_peak = self.sampler.get_peak_fft()[0][0]
            self.second_peak = self.sampler.get_peak_fft()[0][1]
        self.glass_buttons[1].text = "1st peak " + "{0:.1f}".format(self.first_peak) + "Hz"
        self.glass_buttons[2].text = "2nd peak " + "{0:.1f}".format(self.second_peak) + "Hz"  
        if self.is_playing:
            self.top_buttons[3].text = "Stop"
            self.top_buttons[3].color = LIGHT_RED
        else:
            self.top_buttons[3].text = "Play"
            self.top_buttons[3].color = LIGHT_GREEN
        if self.sampler.has_new_fft():
            self.freq_line, self.fft_data = self.sampler.get_fft_data()
            self.freq_line, self.fft_peak_data = self.sampler.get_peak_waveform()
        
        #draw display background
        self.canvas.fill(GRAY)
        
        #draw the elements that are shared between different demonstrations (glass, chladni, tube)
        for button in self.top_buttons:
            button.draw(self.canvas) 
        self.volume_scroll.draw(self.canvas)
        self.freq_controller.draw(self.canvas)
        
        #draw the elements that are unique to the glass demonstration
        if self.in_glass:
            for button in self.glass_buttons:
                button.draw(self.canvas) 
            if not self.is_playing:
                self.plotter.draw(self.canvas, self.interface.freq, self.freq_line, self.fft_data, self.fft_peak_data)
            
        #draw the elements that are unique to the chladni plate demonstration        
        elif self.in_chladni:
            for button in self.chladni_buttons:
                button.draw(self.canvas)
        
        #draw the elements that are unique to the ruben's tube demonstration
        elif self.in_ruben:
            for button in self.ruben_buttons:
                button.draw(self.canvas)

    def play_stop_wave(self):
        ''' 
        this method is binded to the play/stop button
        '''
        if self.is_playing:
            self.player.stop_sine_wave()
        else:
            self.player.play_sine_wave()
        self.sampler.stop_start_FFT_computation()

        #self.player.play_stop_sine_wave()
        self.is_playing = not self.is_playing

    def set_first_peak(self):
        ''' 
        this method is binded to the first peak button
        '''
        self.interface.setFreq(self.first_peak)
        
    def set_second_peak(self):
        ''' 
        this method is binded to the second peak button
        '''
        self.interface.setFreq(self.second_peak)
        
    def update_locations(self, size):
        '''
        this method is called upon window size change, it changes the layout of the elements on display
        according to the new window size.
        '''
        self.width = size[0]
        self.height = size[1]
        for button in self.top_buttons:
            button.update_layout(self.height, self.width)
        for button in self.glass_buttons:
            button.update_layout(self.height, self.width)
        for button in self.chladni_buttons:
            button.update_layout(self.height, self.width)
        for button in self.ruben_buttons:
            button.update_layout(self.height, self.width)
        self.plotter.update_layout(self.width, self.height)
        self.volume_scroll.update_layout(self.width, self.height)
        self.freq_controller.update_layout(self.height, self.width)

    def set_freq_from_plotter(self, pos):
        '''
        this method sets the play frequency according to the position that was clicked on the graph frame 
        '''
        freq = self.plotter.check_click(pos, self.freq_line)
        if freq != 0:
            self.interface.setFreq(freq)
            
    def set_glass(self):
        '''
        this method is binded to the "wine glass button", sets the display to the wine demonstration layout
        '''
        self.in_glass = True
        self.in_chladni = False
        self.in_ruben = False
        
    def set_chladni(self):
        '''
        this method is binded to the "chladni plate button", sets the display to the chladni demonstration layout
        '''
        self.in_glass = False 
        self.in_chladni = True
        self.in_ruben = False
        
    def set_ruben(self):
        '''
        this method is binded to the "Ruben tube button", sets the display to the tube demonstration layout
        '''
        self.in_glass = False
        self.in_chladni = False
        self.in_ruben = True 
        
    def chladni_fixed_1(self):
        '''
        this method is binded to the 1st chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_1[0])
        self.interface.setVol(config.CHLADNI_FIXED_1[1])

    def chladni_fixed_2(self):
        '''
        this method is binded to the 2nd chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_2[0])
        self.interface.setVol(config.CHLADNI_FIXED_2[1])
    
    def chladni_fixed_3(self):
        '''
        this method is binded to the 3rd chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_3[0])
        self.interface.setVol(config.CHLADNI_FIXED_3[1])
    
    def chladni_fixed_4(self):
        '''
        this method is binded to the 4th chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_4[0])
        self.interface.setVol(config.CHLADNI_FIXED_4[1])
    
    def chladni_fixed_5(self):
        '''
        this method is binded to the 5th chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_5[0])
        self.interface.setVol(config.CHLADNI_FIXED_5[1])
    
    def chladni_fixed_6(self):
        '''
        this method is binded to the 6th chladni plate fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.CHLADNI_FIXED_6[0])
        self.interface.setVol(config.CHLADNI_FIXED_6[1])
      
    def ruben_fixed_1(self):
        '''
        this method is binded to the 1st ruben tube fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.TUBE_FIXED_1[0]) 
        self.interface.setVol(config.TUBE_FIXED_1[1])
         
    def ruben_fixed_2(self):
        '''
        this method is binded to the 2nd ruben tube fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.TUBE_FIXED_2[0])
        self.interface.setVol(config.TUBE_FIXED_1[1])
                
    def ruben_fixed_3(self):
        '''
        this method is binded to the 3rd ruben tube fixed frequency button, sets pre-defined frequency and volume
        '''
        self.interface.setFreq(config.TUBE_FIXED_3[0])
        self.interface.setVol(config.TUBE_FIXED_1[1])

    def sweep_enable_disable(self):
        '''
        this is binded to the sweep button, starts/stops the sweep
        '''
        self.sweep_enable = not self.sweep_enable

    def sweep_freq(self,ms):
        '''
        self.sweep_time_counter += ms
        step_delta = math.floor(self.sweep_time_counter/self.sweep_time_interval)
        if step_delta > 0:
            self.sweep_step_counter += step_delta #idially step_delta=1 if not, we need larger time_interval
            sc=self.sweep_step_counter
            self.sweep_time_counter -= step_delta*self.sweep_time_interval
            print('total shift ' +str((1-2*math.fmod(math.floor(sc/(self.sweep_number_of_steps*2)),2))*(self.sweep_number_of_steps-math.fmod(sc,self.sweep_number_of_steps*2)-1)*self.sweep_freq_step_size))

            freq = self.sweep_center_freq+(1-2*math.fmod(math.floor(sc/(self.sweep_number_of_steps*2)),2))*(self.sweep_number_of_steps-math.fmod(sc,self.sweep_number_of_steps*2)-1)*self.sweep_freq_step_size
            print('freq change by sweeper to ' + str(freq))
            self.interface.setFreq(freq)
            if self.sweep_step_counter == 4*self.sweep_number_of_steps:
                # the formula of freq is periodic at 4*self.sweep_step_size, meant to prevent overflow
                self.sweep_step_counter=0
        '''
        self.sweep_time_counter += ms
        if (self.sweep_time_counter/config.SWEEP_TIME_INTERVAL) > 1:
            print('sweep_time_counter is: ' + str(self.sweep_time_counter))
            self.sweep_time_counter = 0
            freq= self.sweep_center_freq + (self.sweep_step_counter - config.SWEEP_NUMBER_OF_STEPS) * config.SWEEP_FREQ_STEP_SIZE
            self.interface.setFreq(freq)
            self.sweep_step_counter += 1
            print('sweep_step_counter is: ' + str(self.sweep_step_counter))
            print('sweep_number_of_steps is: ' + str(config.SWEEP_NUMBER_OF_STEPS))
        if self.sweep_step_counter > config.SWEEP_NUMBER_OF_STEPS * 2:
            self.sweep_step_counter = 0


    def main_loop(self):
        '''
        this is the main loop of the GUI. the commands within the loop are executed ~60/Sec. 
        it has two main objectives:
        1. monitor the different events and call the relevant methods
        2. call the 'draw' function in the appropriate rate  
        '''
        while not self.done:
            for event in pygame.event.get():
                # exit if ESC pressed or 'x' clicked. 
                if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):     
                    self.sampler.close_pyaudio_nicely()
                    self.player.close_nicely()
                    self.done = True
                
                # change window size event
                elif event.type==VIDEORESIZE:
                    self.canvas=pygame.display.set_mode(event.dict['size'],RESIZABLE)
                    pygame.display.flip()
                    self.update_locations(event.dict['size'])
                
                # left mouse button clicked events
                elif event.type == pygame.MOUSEBUTTONDOWN and event.button == LEFT:
                    for button in self.top_buttons:
                        button.check_click(event.pos)
                    self.volume_scroll.check_click(event.pos)
                    self.freq_controller.check_click(event.pos)
                        
                    if self.in_glass:
                        for button in self.glass_buttons:
                            button.check_click(event.pos)
                        if not self.is_playing:
                            self.set_freq_from_plotter(event.pos)
                        
                    elif self.in_chladni:
                        for button in self.chladni_buttons:
                            button.check_click(event.pos)
                            
                    elif self.in_ruben:
                        for button in self.ruben_buttons:
                            button.check_click(event.pos)
                
                # left mouse button released events
                elif event.type == pygame.MOUSEBUTTONUP and event.button == LEFT:
                    self.volume_scroll.click_release()
                    self.freq_controller.click_release()
                    for button in self.top_buttons:
                        button.click_release()
                    for button in self.glass_buttons:
                        button.click_release()
                    for button in self.chladni_buttons:
                        button.click_release()
                    for button in self.ruben_buttons:
                        button.click_release()
                        
                # keyboard keys events
                elif event.type == KEYDOWN: 
                    if event.key == K_LEFT:
                        self.interface.decreaseFreqFine()
                    elif event.key == K_RIGHT:
                        self.interface.increaseFreqFine()
                    elif event.key == K_UP:
                        self.volume_scroll.change_key_pressed(1)
                    elif event.key == K_DOWN:
                        self.volume_scroll.change_key_pressed(-1)
                elif event.type == KEYUP:
                    if event.key == K_UP:
                        self.volume_scroll.change_key_released(1)
                    elif event.key == K_DOWN:
                        self.volume_scroll.change_key_released(-1)
                    
            # draw to the screen, and "tick" the clock                       
            self.draw()         
            pygame.display.update()
            ms = self.fps_Clock.tick(60)

            if (self.sweep_enable) and (self.in_glass) and (self.is_playing):
                if self.sweep_center_freq==0: #if it's the first time of the loop, make sure to save the center frequency
                    self.sweep_center_freq = self.interface.freq
                self.sweep_freq(ms)
            else: #return the center freq to normal
                if self.sweep_center_freq != 0:
                    self.interface.setFreq(self.sweep_center_freq)
                self.sweep_center_freq=0

            # track the time for the volume to jump back to minimum after the key was released
            self.volume_scroll.handle_control_timeout(ms)
                
            # track FPS and print it once per second (DEBUG)
            self.time = self.time + (ms/1000.0)
            if self.time > 1:
                self.time = self.time - 1
                print(self.fps_Clock.get_fps())
            
            
            
        pygame.quit()
        
     

                    
        
            
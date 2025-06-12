from sys import modules
from pm import PM
import utime 

HEART_POLY = [
    (-26, 42), (-21, 43), (-17, 43), (-8, 41), (-3, 42), (2, 46),
    (5, 52), (5, 58), (2, 60), (-1, 58), (-2, 56), (-3, 55), (-4, 57),
    (-6, 59), (-9, 59), (-11, 57), (-11, 54), (-9, 50), (4, 41), (14, 44),
    (16, 47), (17, 50), (16, 52), (14, 53), (12, 51), (11, 52), (9, 53),
    (7, 51), (8, 48), (12, 43), (20, 41), (24, 43), (29, 42), (34, 40)]



    
C_POLY = [
    (  0.0, 60.0),
    (-26.25,60.0),
    (-35.0, 52.0),
    (-35.0, 28.0),
    (-26.25,20.0),
    (  0.0, 20.0),
]

 
# Centre de la zone
cx, cy = 0.0, 40.0

# Facteur d’échelle (par ex. 0.5 pour moitié taille)
scale = 0.2

# Transformation centrée
C_POLY_centered = [
    ((x - cx) * scale + cx, (y - cy) * scale + cy)
    for x, y in C_POLY
]

CIRCLE_CENTER = (0, 40)   # X=0 (centre), Y=40 (centre vertical)
CIRCLE_RADIUS =  15
CIRCLE_SIDES  = 60


LETTER_POLYS = {
    'C': [[(  0, 60), (-26.25, 60), (-35, 52),
           (-35, 28), (-26.25, 20), (  0, 20)]],
    'H': [[(-30, 10), (-30, 60)],
          [(  0, 10), (  0, 60)],
          [(-30, 35), (  0, 35)]],
    'A': [[(-40, 10), (-40, 60)],
          [(-40, 60), (  0, 60)],
          [(-40, 35), (-5,  35)],
          [(-40, 10), (  0, 10)]],
    'N': [[(-30, 10), (-30, 60)],
          [(-30, 10), ( 10, 10)]],
    'E': [[(-15, 10), (-5,  60)],
          [( 15, 10), ( 5,  60)],
          [(-8,  35), ( 8,  35)]],
    'L': [[(-30, 10), (-30, 60)],
          [(-30, 60), (  0, 10)],
          [(  0, 10), (  0, 60)]],
   
}

# ------------------------------------------------
# Offsets horizontaux pour espacer les lettres
# ------------------------------------------------
LETTER_OFFSETS = {
    'C':  -24,
    'H':  -12,
    'N':   36,#L
    'E':   -5,#A
    'A':   31,#E
    'L':   17,#N
}
LETTER_SCALE = 0.3



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
                #pm.draw_poly(HEART_POLY, False)
                #pm.draw_poly(C_POLY_centered, False)
                #cx, cy = CIRCLE_CENTER
                #pm.draw_regular_poly(cx,cy,CIRCLE_RADIUS,CIRCLE_SIDES,True,0)
                for letter, polys in LETTER_POLYS.items():
                    x_off = LETTER_OFFSETS[letter]
                    #x_off =0
                    for poly in polys:
                        # on translate chaque polygone
                        scaled = [
                            ((x - cx) * LETTER_SCALE + cx + x_off,
                             (y - cy) * LETTER_SCALE + cy)
                            for x, y in poly
                        ]
                        pm.draw_poly(scaled, False)
                    #utime.sleep_ms(3000)

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

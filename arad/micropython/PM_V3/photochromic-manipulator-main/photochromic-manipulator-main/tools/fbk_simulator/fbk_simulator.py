#
# fbk_simulator.py - a Python GUI visualizing the 5 bar mechanism.
# Made for testing and finding optimal bar length.
#
# https://github.com/arduino12/ 2023/06/10
#
# +-----------------------------------------------------------------+
# | Symmetrical five bar linkage mechanism:                         |
# |   Fixed points: c=center, w=west, e=east, p0, p1                |
# |   Bars lengths: b02 = b13, b24 = b34                            |
# |                                                                 |
# |                               (4)                               |
# |                            /       \                            |
# |                        /               \                        |
# |                    /                       \                    |
# |                /                               \                |
# |            /                                       \            |
# |        /                                               \        |
# |      (2)                                               (3)      |
# |        \                                               /        |
# |          \                                           /          |
# |            \                                       /            |
# |              \                                   /              |
# |                \                               /                |
# |                  \                           /                  |
# +-----{w}----------{0}----------{c}----------{1}----------{e}-----+

__version__ = '1.0.0'

try:
    import matplotlib, shapely
except ImportError:
    __import__('os').system('pip install matplotlib shapely')
else:
    del matplotlib, shapely
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider, RangeSlider
from shapely.geometry import LineString


class FBK(object):
    '''
    Symmetrical five bar linkage mechanism, see above diagram.
    '''
    EP_OK = 0
    EP_OUT_OF_LIMIT = -1
    EP_OUT_OF_REACH = -2
    DEFAULT_BARS_LENGTHS = (22, 34, 43)
    DEFAULT_ANGLES_LIMITS = ((30, 300), (30, 150), (20, 154))

    def __init__(self, bars_lengths=DEFAULT_BARS_LENGTHS, angles_limits=DEFAULT_ANGLES_LIMITS):
        '''
        @param bars_lengths: Bars lengths in milimeters, see above diagram.
        @param angles_limits: Limits of the angles between the bars - view from inside the polygon.
        '''
        self._angles = []
        self.set_bars_lengths_and_angles_limits(bars_lengths, angles_limits)

    def set_bars_lengths_and_angles_limits(self, bars_lengths, angles_limits):
        '''
        @param bars_lengths: Bars lengths in milimeters, see above diagram.
        @param angles_limits: Limits of the angles between the bars - view from inside the polygon.
        '''
        self._bars_lengths = tuple(bars_lengths)
        self._angles_limits = tuple(angles_limits)

    def get_bars(self):
        return self._bars

    def get_points(self):
        return self._points

    def get_angles(self):
        return self._angles

    def get_angles_limits(self):
        return self._angles_limits

    @staticmethod
    def _degrees(rad):
        return math.degrees(rad) % 360

    @staticmethod
    def _angle(a, b, c):
        return FBK._degrees(math.atan2(c[1]-b[1], c[0]-b[0]) - math.atan2(a[1]-b[1], a[0]-b[0]))

    def set_edge_point(self, p4):
        '''
        @param p4: (x, y) coordinate of the target end-effector point.
        preform inverse-kinematics with the given end-effector point.
        '''
        self.state = self.EP_OUT_OF_REACH
        p4x, p4y = p4
        b00, b1, b2 = self._bars_lengths

        p4y_sq = p4y ** 2

        b0 = b00 / 2
        b04_dx = b0 + p4x
        b14_dx = b0 - p4x
        b04_dist_sq =  b04_dx ** 2 + p4y_sq
        b14_dist_sq =  b14_dx ** 2 + p4y_sq

        b1_du = b1 * 2
        b1_sq = b1 ** 2
        b2_sq = b2 ** 2
        b1_b2_diff_sq = b1_sq - b2_sq

        if not b04_dist_sq or not b14_dist_sq:
            return self.state

        a204 = (b1_b2_diff_sq + b04_dist_sq) / (b1_du * math.sqrt(b04_dist_sq))
        a314 = (b1_b2_diff_sq + b14_dist_sq) / (b1_du * math.sqrt(b14_dist_sq))
        TWO_PI = 2 * math.pi
    # alpha1_calc = (l1**2 + ( (l0 + x)**2 + y**2 ) - l2**2) / (2*l1*math.sqrt( (l0 + x)**2 + y**2 ))  
    # alpha2_calc = (l1**2 + ( (l0 - x)**2 + y**2 ) - l2**2) / (2*l1*math.sqrt( (l0 - x)**2 + y**2 ))  
    # alpha1 = math.acos(alpha1_calc)
    # alpha2 = math.acos(alpha2_calc)
    # shoulder1 = beta1 + alpha1
    # shoulder2 = math.pi - beta2 - alpha2
        if abs(a204) > 1 or abs(a314) > 1:
            return self.state

        a204 = math.acos(a204)
        a314 = math.acos(a314)
        a40e = math.atan2(p4y, b04_dx)
        a41e = math.atan2(p4y, b14_dx)
        a20e = (a40e + a204) # % TWO_PI
        a31e = (math.pi - a41e - a314) # % TWO_PI

        self.p0 = (-b0, 0)
        self.p1 = (b0, 0)
        self.p2 = (b1 * math.cos(a20e) - b0, b1 * math.sin(a20e))
        self.p3 = (b1 * math.cos(a31e) + b0, b1 * math.sin(a31e))
        self.p4 = p4

        self.a0 = self._degrees(a20e)
        self.a1 = self._degrees(math.pi - a31e)
        self.a2 = self._angle(self.p0, self.p2, p4)
        self.a3 = self._angle(p4, self.p3, self.p1)
        self.a4 = self._angle(self.p2, p4, self.p3)

        al = self._angles_limits
        self.state = self.EP_OK if \
            al[0][0] <= self.a0 <= al[0][1] and \
            al[0][0] <= self.a1 <= al[0][1] and \
            al[1][0] <= self.a2 <= al[1][1] and \
            al[1][0] <= self.a3 <= al[1][1] and \
            al[2][0] <= self.a4 <= al[2][1] \
                else self.EP_OUT_OF_LIMIT

        # print(int(self.a0), int(self.a1), int(self.a2), int(self.a3), int(self.a4), self.state)
        return self.state


class FiveBarKinematicsGUI(object):
    PLOT_TOP = 90
    PLOT_BOTTOM = -10
    PLOT_SIDE = 50
    PLOT_LW = 2
    ANGLE_SLIDER_ARGS = {'valmin': 0, 'valmax': 360, 'valstep': 1, 'closedmax': False}
    LENGTH_SLIDER_ARGS = {'valmin': 10, 'valmax': PLOT_SIDE * 2, 'valstep': 1}
    SLIDERS = [
        [Slider, 'Base Bar', FBK.DEFAULT_BARS_LENGTHS[0], LENGTH_SLIDER_ARGS],
        [Slider, 'Mid Bars', FBK.DEFAULT_BARS_LENGTHS[1], LENGTH_SLIDER_ARGS],
        [Slider, 'Edge Bars', FBK.DEFAULT_BARS_LENGTHS[2], LENGTH_SLIDER_ARGS],
        [Slider, 'Rect Width', 38, LENGTH_SLIDER_ARGS], # 38 44
        [RangeSlider, 'Base Angles', FBK.DEFAULT_ANGLES_LIMITS[0], ANGLE_SLIDER_ARGS],
        [RangeSlider, 'Mid Angles', FBK.DEFAULT_ANGLES_LIMITS[1], ANGLE_SLIDER_ARGS],
        [RangeSlider, 'Edge Angle', FBK.DEFAULT_ANGLES_LIMITS[2], ANGLE_SLIDER_ARGS],
    ]

    def __init__(self):
        self.sliders = []
        self.cid_m = None
        self.rect_w = 38
        self.rect_y_min = self.PLOT_TOP
        self.rect_y_max = self.PLOT_BOTTOM
        self.base_angle_range = (0, 0)
        self.draw_points = [[],[]]
        self.rect_points = [[],[]]
        self.fbk = FBK()
        self.fig, self.ax = plt.subplots()
        # plt.axis('equal')
        # plt.axis('square')
        plt.axis('scaled')
        # self.fig.set_aspect('equal', adjustable='box')
        self.fig.canvas.manager.set_window_title('FBK')
        self.ax.axis([-self.PLOT_SIDE, self.PLOT_SIDE, self.PLOT_BOTTOM, self.PLOT_TOP])
        self.fbk_plots = [self.ax.plot([], [], lw=self.PLOT_LW, animated=True,c=c)[0] for c in 'bbrr']
        self.fbk_plot = self.ax.plot([], [], lw=5, animated=True,c='b')[0]
        self.rect_plot = self.ax.plot([], [], lw=2, animated=True,c='y')[0]
        self.draw_plot = self.ax.plot([], [], '.', lw=5, animated=True,c='g')[0]
        self.rect_text = self.ax.text(0, 0, '123', ha='center', va='center', color='y')
        self.fbk.set_edge_point((0, 40))
        self.fig.canvas.mpl_connect('button_press_event', self.button_press_event)
        self.fig.canvas.mpl_connect('button_release_event', self.button_release_event)
        self.ani = animation.FuncAnimation(self.fig, self.update, frames=None, blit=True, interval=25, cache_frame_data=False)

    def draw_if_valid(self, x, y):
        if not self.fbk.set_edge_point((x, y)):
            self.draw_points[0].append(x)
            self.draw_points[1].append(y)
            return True
        return False

    def button_press_event(self, event):
        if event.inaxes != self.ax:
            return
        self.cid_m = self.fig.canvas.mpl_connect('motion_notify_event', self.mouse_drag_event)

    def button_release_event(self, event):
        if self.cid_m is not None:
            self.fig.canvas.mpl_disconnect(self.cid_m)
            self.cid_m = None

    def mouse_drag_event(self, event):
        if event.inaxes == self.ax:
            self.draw_if_valid(event.xdata, event.ydata)

    def update(self, s):
        # self.fbk_plots[0].set_data(self.fbk.p0, self.fbk.p2)
        # self.fbk_plots[1].set_data(self.fbk.p1, self.fbk.p3)
        # self.fbk_plots[2].set_data(self.fbk.p2, self.fbk.p4)
        # self.fbk_plots[3].set_data(self.fbk.p3, self.fbk.p4)

        line = LineString((self.fbk.p0, self.fbk.p2, self.fbk.p4, self.fbk.p3, self.fbk.p1))
        self.fbk_plot.set_data(line.xy)

        self.rect_plot.set_data(self.rect_points)
        self.rect_text.set_text(f'{self.rect_w * 2}x{self.rect_y_max - self.rect_y_min}mm {self.base_angle_range}° {self.rect_y_max}mm')
        self.rect_text.set_y(self.rect_y_max + 3)

        self.draw_plot.set_data(self.draw_points)

        return self.draw_plot, self.rect_plot, self.fbk_plot, self.rect_text
        # return [self.draw_plot] + self.fbk_plots

    def draw_area(self, val=None):
        self.draw_points[0].clear()
        self.draw_points[1].clear()
        v = [i.val for i in self.sliders]
        self.fbk.set_bars_lengths_and_angles_limits(v[:3], v[4:])
        self.rect_w = v[3]
        self.rect_y_min = self.PLOT_TOP
        self.rect_y_max = self.PLOT_BOTTOM
        edge = self.PLOT_SIDE - self.rect_w
        for y in range(self.PLOT_BOTTOM, self.PLOT_TOP + 1):
            valid_x = []
            for x in range(-self.PLOT_SIDE, self.PLOT_SIDE + 1):
                valid_x.append(self.draw_if_valid(x, y))
            if all(valid_x[edge: -edge]):
                if self.rect_y_min > y:
                    self.rect_y_min = y
                elif self.rect_y_max < y:
                    self.rect_y_max = y

        self.rect_points[0] = (-self.rect_w, -self.rect_w, self.rect_w, self.rect_w, -self.rect_w)
        self.rect_points[1] = (self.rect_y_min, self.rect_y_max, self.rect_y_max, self.rect_y_min, self.rect_y_min)
        self.fbk.set_edge_point((-self.rect_w, self.rect_y_min))
        a0 = self.fbk.a0
        self.fbk.set_edge_point((self.rect_w, self.rect_y_max))
        self.base_angle_range = int(a0 - self.fbk.a0)
        self.fbk.set_edge_point((0, 40))

    def draw_sliders(self):
        for i, (cls, label, value, kwargs) in enumerate(self.SLIDERS):
            kwargs.update({'label': label, 'valinit': value})
            slider = cls(
                ax=self.fig.add_axes([0.15, (len(self.SLIDERS) - i) * 0.05, 0.65, 0.03]),
                **kwargs
            )
            slider.on_changed(self.draw_area)
            self.sliders.append(slider)

        self.fig.subplots_adjust(top=0.95, bottom=len(self.SLIDERS) * 0.05+0.15)

    def mainloop(self):
        self.draw_sliders()
        self.draw_area()
        plt.show()


if __name__ == '__main__':
    fbk = FiveBarKinematicsGUI()
    fbk.mainloop()

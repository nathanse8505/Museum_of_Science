#
# ansi.py - MicroPython library for setting ANSI colors and style.
# Supports a subset of the SGR (Select Graphic Rendition),
# See https://en.wikipedia.org/wiki/ANSI_escape_code
#
# https://github.com/arduino12/micropython-libs 2023/07/04
#

__version__ = '1.0.0'

from micropython import const

SGR_NORMAL = const(0)
SGR_BOLD = const(1)
SGR_ITALIC = const(3)
SGR_UNDERLINE = const(4)
SGR_NOT_BOLD = const(22)
SGR_NOT_ITALIC = const(23)
SGR_NOT_UNDERLINE = const(24)

SGR_BLACK = const(30)
SGR_RED = const(31)
SGR_GREEN = const(32)
SGR_YELLOW = const(33)
SGR_BLUE = const(34)
SGR_MAGENTA = const(35)
SGR_CYAN = const(36)
SGR_WHITE = const(37)

SGR_BRIGHT = const(60)
SGR_BACKGROUND = const(10)

SGR_END = '\033[0m'


def sgr(*args):
    return '\033[{}m'.format(';'.join(map(str, args)))


def sgr_text(text, *args):
    return '{}{}{}'.format(sgr(*args), text, SGR_END)


if __name__ == '__main__':
    print('\nTest ansi', __version__)
    G = globals()
    COLORS = ['SGR_BLACK', 'SGR_RED', 'SGR_GREEN', 'SGR_YELLOW',
              'SGR_BLUE', 'SGR_MAGENTA', 'SGR_CYAN', 'SGR_WHITE']

    print('COLORS:\t\t', *[sgr_text(c, G[c]) for c in COLORS])
    print('BOLD:\t\t', *[sgr_text(c, G[c], SGR_BOLD) for c in COLORS])
    print('BRIGHT:\t\t', *[sgr_text(c, G[c] + SGR_BRIGHT) for c in COLORS])
    print('BOLD BRIGHT:\t', *[sgr_text(c, G[c] + SGR_BRIGHT, SGR_BOLD) for c in COLORS])

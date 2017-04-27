import ctypes
import os
from PIL import Image
from Xlib import display

LibName = 'prtscn.so'
AbsLibPath = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + LibName
grab = ctypes.CDLL(AbsLibPath)

def mousepos():
    """mousepos() --> (x, y) get the mouse coordinates on the screen (linux, Xlib)."""
    data = display.Display().screen().root.query_pointer()._data
    return data["root_x"], data["root_y"]
	
def grab_screen(x1,y1,x2,y2):
    w, h = x1+x2, y1+y2
    size = w * h
    objlength = size * 3

    grab.getScreen.argtypes = []
    result = (ctypes.c_ubyte*objlength)()

    grab.getScreen(x1,y1, w, h, result)
    return Image.frombuffer('RGB', (w, h), result, 'raw', 'RGB', 0, 1)

if __name__ == '__main__':
  print("The mouse position on the screen is {0}".format(mousepos()))
  im = grab_screen(0,0,1440,900)
  print im
  im.show()


import os,sys,time,ctypes
from PIL import Image
from lcdmodule import *
from Xlib import display

LibName = 'prtscn.so'
AbsLibPath = os.path.dirname(os.path.abspath(__file__)) + os.path.sep + LibName
grab = ctypes.CDLL(AbsLibPath)

def mousepos():
    """mousepos() --> (x, y) get the mouse coordinates on the screen (linux, Xlib)."""
    data = display.Display().screen().root.query_pointer()._data
    return data["root_x"], data["root_y"]

def grab_screenRGB565(x1,y1,w,h):
    size = w * h

    grab.getScreenRGB565.argtypes = []
    result = (ctypes.c_uint*size)()

    grab.getScreenRGB565(x1,y1, w, h, result)
    
    return result

print "Openining", lcdOpen();	
print "Init", lcdInit();
print "Clear", lcdClear(0xFFFF);

swidth = 480
sheight = 320
sx = 0
sy = 0
smx, smy = mousepos()

cmatrix = grab_screenRGB565(sx,sy,swidth,sheight)
smatrix = []
for i in cmatrix: smatrix.append( i )

while True:
    smx, smy = mousepos()
    
    if(smx>sx+swidth): sx = smx-swidth
    elif(smx<sx): sx = smx

    if(smy>sy+sheight): sy = smy-sheight
    elif(smy<sy): sy = smy
    

    cmatrix = grab_screenRGB565(sx,sy,swidth,sheight)
    smatrix = []
    for i in cmatrix: smatrix.append( i )

    for y in range(smy-sy-1, smy-sy+2):
        for x in range(smx-sx-1, smx-sx+2):
            i = y*swidth+x
            if(i>=0 and i<len(smatrix)):
                smatrix[i] = ~ smatrix[i]
            
    
    lcdMatrix(0,0,swidth,sheight,smatrix)

    #lcdArea(smx-sx,smy-sy,smx-sx+4,smy-sy+4)
    #for i in range(0, 25): lcdData(0x0000)
    
    #lcdArea(smx-sx+1,smy-sy+1,smx-sx+3,smy-sy+3)
    #for i in range(0, 9): lcdData(0xFFFF)

    #lcdArea(smx-sx+2,smy-sy+2,smx-sx+2,smy-sy+2)
    #lcdData(0x0000)

print "Closing", lcdClose();


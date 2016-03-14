
import os,sys
from PIL import Image
from lcdmodule import *

img = Image.open("rpi.bmp")
pix = img.load();
width = img.size[0]
height = img.size[1]
matrix =[]

for y in range(0, height):
 for x in range(0, width):
  rgb = img.getpixel((x,y))
  matrix.append(rgb)

		
print "Openining", lcdOpen();
print "Init", lcdInit();
print "Clear", lcdClear(0x00);
print "Matrix" ,lcdMatrix(10,10,width+9,height+9,matrix);
print "Closing", lcdClose();


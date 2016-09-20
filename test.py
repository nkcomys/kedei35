
import os,sys
from PIL import Image
from lcdmodule import *

img = Image.open("rpi.bmp")
width = img.size[0]
height = img.size[1]
matrix =[]

for y in range(0, height):
 for x in range(0, width):
  rgb = img.getpixel((x,y))
  matrix.append(rgb)

fnt = Image.open("font.bmp");
fw = fnt.size[0];
fh = fnt.size[1];
fm = [];
for fy in range(0, 16):
	for fx in range(0, fw):
		frgb = fnt.getpixel((fx,fy));
		fm.append(frgb);
		
print "Openining", lcdOpen();	
print "Init", lcdInit();
print "Clear", lcdClear(0x00);
print "Matrix" ,lcdMatrix(10,10,width+9,height+9,matrix);
print "Loading font", lcdLoadChars(12,16,96, fm);
print "Draw symbol", lcdDrawSymbol(0,300,24, 0xfa);
print "Draw string", lcdDrawString(0,220, 0x20, "Hello!", 0xfa);
print "Closing", lcdClose();


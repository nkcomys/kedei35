
import os,sys, time
from PIL import Image
from lcdmodule import *

img = Image.open("rpi2.bmp")
width = img.size[0]
height = img.size[1]
matrix =[]

img = img.convert('RGB')

for y in range(0, height):
 for x in range(0, width):
  r,g,b = img.getpixel((x,y))
  #print(rgb)
  r=(r) >> 3
  g=(g) >> 2
  b=(b) >> 3
  rgb = (r << 11) | (g << 5) | b
  #print(rgb)
  matrix.append(rgb)

print(img.mode)


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
print "Clear", lcdClear(0xFFFF);
print "Matrix" ,lcdMatrix(10,10,width+9,height+9,matrix);
print "Loading font", lcdLoadChars(12,16,96, fm);
print "Draw symbol", lcdDrawSymbol(0,300,32, 0xfa);
print "Draw string", lcdDrawString(0,220, 0x20, "Hello!", 0xf800);

print "Draw string", lcdArea(0,0,9,9);
for i in range(0, 100):
    #print "Draw data", i
    lcdData(0xF800)


print "Closing", lcdClose();


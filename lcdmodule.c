#include <Python.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <bcm2835.h>


int delayus(int us) {
	struct timespec tim, timr;
	tim.tv_sec = 0;
	tim.tv_nsec = (long)(us * 1000L);
	return nanosleep(&tim, &timr);
}

int delayms(int ms) {
	struct timespec tim, timr;
	tim.tv_sec = 0;
	tim.tv_nsec = (long)(ms * 1000000L);
	return nanosleep(&tim,&timr);
}

int delays(int s) {
	struct timespec tim, timr;
	tim.tv_sec = s;
	tim.tv_nsec = 0;
	return nanosleep(&tim, &timr);
}

uint32_t *currentFont, cSH, cSW, cSC;


static PyObject* lcd_open(PyObject* self, PyObject *args) {
	int r;
	uint32_t v;
	r = bcm2835_init();
	if(r != 1) return Py_BuildValue("l",-1);
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_8);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
	return Py_BuildValue("l", 0);
}

static PyObject* lcd_close(PyObject* self, PyObject *args) {
	bcm2835_spi_end();
	int r = bcm2835_close();
	if(r!=1) return Py_BuildValue("l", -1);
	if(currentFont!=NULL) free(currentFont);
	return Py_BuildValue("l", 0);
}


static  PyObject* lcd_hello(PyObject *self, PyObject *args) {
	char *s = "Hello from C!";
	return Py_BuildValue("s", s);
};

int spi_transmit(int devsel, uint8_t *data, int len) {
	if(devsel == 0) 
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	} else {
		bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
	}

	bcm2835_spi_transfern((char*)data, len);
}

#define LCD_CS 1
#define TOUCH_CS 0
#define LCD_WIDTH 480
#define LCD_HEIGHT 320

static PyObject* lcd_reset(PyObject* self, PyObject *args) {
	uint8_t buff[4] = { 0, 0, 0, 0 };
	spi_transmit(LCD_CS, &buff[0], 4);
	delayms(50);
	buff[0]= buff[1]= buff[2]= 0;
	buff[3]= 0x01;
	spi_transmit(LCD_CS, &buff[0], 4);
	delayms(200);
	return Py_BuildValue("l", 0);
}
	


void lcd_data(uint16_t data) {
	uint8_t b1[4], b2[4];
	memset(&b1, 0, sizeof(b1));
	memset(&b2, 0, sizeof(b2));
	b2[1]= b1[1]= data>>8;
	b2[2]= b1[2]= data&0x00FF;
	b1[3]= 0x15; // DATA_BE constant
	b2[3]= 0x1F; // DATA_AF constant
	spi_transmit(LCD_CS, &b1[0], 4);
	spi_transmit(LCD_CS, &b2[0], 4);
}

void lcd_cmd(uint16_t cmd) {
	uint8_t b1[4], b2[4];
	memset(&b1, 0, sizeof(b1));
	memset(&b2, 0, sizeof(b2));
	b2[1]= b1[1] = cmd>>8;
	b2[2]= b1[2] = cmd&0x00FF;
	b1[3]= 0x11; // CMD_BE constant
	b2[3]= 0x1B; // CMD_AF constant
	
	spi_transmit(LCD_CS, &b1[0], 4);
	spi_transmit(LCD_CS, &b2[0], 4);
}

void lcd_setptr(void) {
	lcd_cmd(0x002b);
	lcd_data(0x0000);
	lcd_data(0x0000); // starting coordinat height 0
	lcd_data(0x0001);
	lcd_data(0x003f); // end coordinat height 319
	
	lcd_cmd(0x002a);
	lcd_data(0x0000);
	lcd_data(0x0000); // start coordinat width 0
	lcd_data(0x0001);
	lcd_data(0x00df); // end coordinat width 479
	
	lcd_cmd(0x0002c);
}

static PyObject* lcd_clear(PyObject* self, PyObject* args) {
	int clr = 0;
	PyArg_ParseTuple(args, "i", &clr);
	lcd_setptr();
	
	for(int x=0; x<307202; x++) {
		lcd_data(clr);
	}
	return Py_BuildValue("l",0);
}

void lcd_area(uint16_t sx, uint16_t sy, uint16_t x, uint16_t y) {
	if(sx>479) sx= 0;
	if(sy>319) sy= 0;
	if(x>479) x= 479;
	if(y>319) y= 319;
	lcd_cmd(0x002b); // set area Y command
	lcd_data(sy>>8);
	lcd_data(sy&0x00ff);
	lcd_data(y>>8);
	lcd_data(y&0x00ff);
	
	lcd_cmd(0x002a); // set area X command
	lcd_data(sx>>8);
	lcd_data(sx&0x00ff);
	lcd_data(x>>8);
	lcd_data(x&0x00ff);
	
	lcd_cmd(0x002c); // fill command
	
}

void lcd_rectangle_native(uint32_t x, uint32_t y, uint32_t ex, uint32_t ey, uint32_t clr)
{
	lcd_area(x,y,ex,ey);
	uint32_t count;
	count = (ex-x)*(ey-y) + (ey-y);
	for(uint32_t i=0;i<count;i++) {
		lcd_data(clr);
		lcd_data(clr);
	}
}

void lcd_matrix_native(uint32_t x,
		       uint32_t y,
		       uint32_t ex,
		       uint32_t ey,
		       uint32_t *matrix,
		       uint32_t size,
		       uint8_t transform,
		       uint32_t transformer)
{
	uint32_t val;
	lcd_area(x,y,ex,ey);
	for(uint32_t i=0; i<size; i++) {
		val = matrix[i];
		if(transform == 1) val = val *transformer;
		lcd_data(val);
		lcd_data(val);
	}
}

static PyObject* lcd_matrix(PyObject* self, PyObject* args) {
	uint32_t x,y,ex,ey,size;
	PyObject* listObject;
	PyObject* temp;
	uint32_t tmp;
	uint32_t *matrix;
	PyArg_ParseTuple(args,"IIIIO!",&x,&y,&ex,&ey,&PyList_Type, &listObject);
	size = PyList_Size(listObject);
	matrix = (uint32_t *)malloc(sizeof(uint32_t)*size);
	memset(matrix,0,sizeof(uint32_t)*size);
	for(uint32_t i=0;i<size;i++) {
	  temp = PyList_GetItem(listObject, i);
	  tmp = PyInt_AsLong(temp);
	  matrix[i] = tmp;
	}
	lcd_matrix_native(x,y,ex,ey,matrix,size,0,0);
	free(matrix);
	return Py_BuildValue("l",0);
}


static PyObject* lcd_load_font(PyObject* self, PyObject* args)
{
	uint32_t c_w, c_h, c_c, *fnt, size, val;
	PyObject *fontMatrix, *temp;
	PyArg_ParseTuple(args,"IIIO!",&c_w, &c_h, &c_c, &PyList_Type, &fontMatrix);
	size = PyList_Size(fontMatrix);
	fnt = (uint32_t *)malloc(sizeof(uint32_t)*size);
	memset(fnt,0, sizeof(uint32_t)*size);
	for(uint32_t i=0;i<size;i++) {
		temp = PyList_GetItem(fontMatrix, i);
	 	val = PyInt_AsLong(temp);
		if(val>0) {
		  fnt[i] = 1;
		} else {
		  fnt[i] = 0;
	 	}
	}
	if(currentFont != NULL) {
	 free(currentFont);
	}
	currentFont =fnt;
	cSW = c_w;
	cSH = c_h;
	cSC = c_c;

	return Py_BuildValue("l", 0);
}

void lcd_draw_symbol_native(uint32_t x, uint32_t y, uint32_t sym, uint32_t clr)
{
	uint32_t curp;
	lcd_area(x,y,x+cSW-1,y+cSH);
	for(uint32_t i=0; i<cSH; i++) {
		curp = (cSC*i+sym)*cSW;
		for(uint32_t j=0; j<cSW; j++, curp++) {
			if(currentFont[curp]>0) {
				lcd_data(clr);
				lcd_data(clr);
			} else {
				lcd_data(0x00);
				lcd_data(0x00);
			}
		}
	}
}

static PyObject* lcd_draw_symbol(PyObject* self, PyObject* args)
{
	uint32_t sym, x, y, clr;
	PyArg_ParseTuple(args, "IIII", &x, &y, &sym, &clr);
	lcd_draw_symbol_native(x,y,sym,clr);
	return Py_BuildValue("l",0);
}

static PyObject* lcd_draw_string(PyObject* self, PyObject* args)
{
	uint32_t x,y,curp, base, clr, i;
	const char *s;
	PyArg_ParseTuple(args, "IIIsI", &x, &y, &base, &s, &clr);
	i=0;
	while(s[i]!='\0') {
	 uint32_t sym = s[i]-base;
	 lcd_draw_symbol_native(x,y,sym, clr);
	 x= x+cSW;
	 i++;
	}
	return Py_BuildValue("l",0);
}




static PyObject* lcd_rectangle(PyObject* self, PyObject* args) {
	uint32_t clr, x, y, ex, ey;
	PyArg_ParseTuple(args,"IIIII", &x, &y, &ex, &ey, &clr);
	lcd_rectangle_native(x,y,ex,ey,clr);
	return Py_BuildValue("l",0);
}


static PyObject* lcd_rectangle_empty(PyObject *self, PyObject* args) {
	uint32_t clr2, clr,x,y,ex,ey;
	PyArg_ParseTuple(args, "IIIIII", &x, &y, &ex, &ey, &clr, &clr2);
	lcd_rectangle_native(x,y,ex,ey, clr2);
	lcd_rectangle_native(x+1, y+1, ex-1, ey-1, clr);

	return Py_BuildValue("l",0);
}


static PyObject* lcd_init(PyObject* self, PyObject*args) {
	lcd_reset(self, args);
	delayms(100);
	lcd_cmd(0x0000);
	delayms(1);

	lcd_cmd(0x00B0);
	lcd_data(0x0000);

	lcd_cmd(0x0011);
	delayms(50);
	
	lcd_cmd(0x00B3);
	lcd_data(0x0002);
	lcd_data(0x0000);
	lcd_data(0x0000);
	lcd_data(0x0000);
	
	lcd_cmd(0x00C0);
	lcd_data(0x0010); //13
	lcd_data(0x003b); //480
	lcd_data(0x0000);
	lcd_data(0x0002);
	lcd_data(0x0000);
	lcd_data(0x0001);
	lcd_data(0x0000);
	lcd_data(0x0043);
	
	lcd_cmd(0x00C1);
	lcd_data(0x0008);
	lcd_data(0x0016); //CLOCK
	lcd_data(0x0008);
	lcd_data(0x0008);
	
	lcd_cmd(0x00C4);
	lcd_data(0x0011);
	lcd_data(0x0007);
	lcd_data(0x0003);
	lcd_data(0x0003);

	lcd_cmd(0x00C6);
	lcd_data(0x0000);
	
	lcd_cmd(0x00C8); // Gamma
	lcd_data(0x0003);
	lcd_data(0x0003);
	lcd_data(0x0013);
	lcd_data(0x005c);
	lcd_data(0x0003);
	lcd_data(0x0007);
	lcd_data(0x0014);
	lcd_data(0x0008);
	lcd_data(0x0000);
	lcd_data(0x0021);
	lcd_data(0x0008);
	lcd_data(0x0014);
	lcd_data(0x0007);
	lcd_data(0x0053);
	lcd_data(0x000c);
	lcd_data(0x0013);
	lcd_data(0x0003);
	lcd_data(0x0003);
	lcd_data(0x0021);
	lcd_data(0x0000);
	
	lcd_cmd(0x0035);
	lcd_data(0x0000);
	
	lcd_cmd(0x0036);
	lcd_data(0x0028);
	
	lcd_cmd(0x003A);
	lcd_data(0x0055);
	
	lcd_cmd(0x0044);
	lcd_data(0x0000);
	lcd_data(0x0001);

	lcd_cmd(0x00B6);
	lcd_data(0x0000);
	lcd_data(0x0002);
	lcd_data(0x003b);
	
	lcd_cmd(0x00D0);
	lcd_data(0x0007);
	lcd_data(0x0007);
	lcd_data(0x001D);

	lcd_cmd(0x00D1);
	lcd_data(0x0000);
	lcd_data(0x0003);
	lcd_data(0x0000);

	lcd_cmd(0x00D2);
	lcd_data(0x0003);
	lcd_data(0x0014);
	lcd_data(0x0004);

	lcd_cmd(0xE0);
	lcd_data(0x001f);
	lcd_data(0x002c);
	lcd_data(0x002c);
	lcd_data(0x000b);
	lcd_data(0x000c);
	lcd_data(0x0004);
	lcd_data(0x004c);
	lcd_data(0x0064);
	lcd_data(0x0036);
	lcd_data(0x0003);
	lcd_data(0x000e);
	lcd_data(0x0001);
	lcd_data(0x0010);
	lcd_data(0x0001);
	lcd_data(0x0000);

	lcd_cmd(0xE1);
	lcd_data(0x001f);
	lcd_data(0x003f);
	lcd_data(0x003f);
	lcd_data(0x000f);
	lcd_data(0x001f);
	lcd_data(0x000f);
	lcd_data(0x007f);
	lcd_data(0x0032);
	lcd_data(0x0036);
	lcd_data(0x0004);
	lcd_data(0x000b);
	lcd_data(0x0000);
	lcd_data(0x0019);
	lcd_data(0x0014);
	lcd_data(0x000f);

	lcd_cmd(0xE2);
	lcd_data(0x000f);
	lcd_data(0x000f);
	lcd_data(0x000f);

	lcd_cmd(0xE3);
	lcd_data(0x000f);
	lcd_data(0x000f);
	lcd_data(0x000f);

	lcd_cmd(0x13);
	
	lcd_cmd(0x0029);
	delayms(20);
	
	lcd_cmd(0x00b4);
	lcd_data(0x0000);
	delayms(20);

	lcd_cmd(0x002C);

	lcd_cmd(0x002A);
	lcd_data(0x0000);
	lcd_data(0x0000);
	lcd_data(0x0001);
	lcd_data(0x00df);
	
	lcd_cmd(0x002B);
	lcd_data(0x0000);
	lcd_data(0x0000);
	lcd_data(0x0001);
	lcd_data(0x003f);
	
	lcd_cmd(0x002c); // seems to be 2c is 'prepare for draw'

	return Py_BuildValue("l", 0);
}

static PyMethodDef LcdMethods[] = {
	{ "hello", lcd_hello, METH_VARARGS},
	{ "lcdOpen", lcd_open, METH_VARARGS},
	{ "lcdClose", lcd_close, METH_VARARGS},
	{ "lcdReset", lcd_reset, METH_VARARGS},
	{ "lcdInit", lcd_init, METH_VARARGS},
	{ "lcdClear", lcd_clear, METH_VARARGS},
	{ "lcdRectangle", lcd_rectangle, METH_VARARGS},
	{ "lcdRectangleEmpty", lcd_rectangle_empty, METH_VARARGS},
	{ "lcdMatrix", lcd_matrix, METH_VARARGS},
	{ "lcdLoadFont", lcd_load_font, METH_VARARGS},
	{ "lcdDrawSymbol", lcd_draw_symbol, METH_VARARGS},
	{ "lcdDrawString", lcd_draw_string, METH_VARARGS},
	{NULL, NULL}
};



void initlcdmodule() {
	(void) Py_InitModule("lcdmodule",LcdMethods);
}
 

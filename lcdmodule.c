#include <Python.h>
#include "libkedei.h"
static PyObject* py_lcd_open(PyObject* self, PyObject *args) {
	int r = lcd_open();
	return Py_BuildValue("l",r);
}

static PyObject* py_lcd_close(PyObject* self, PyObject *args) {
	int r = lcd_close();
	return Py_BuildValue("l", r);
}

static PyObject* py_lcd_reset(PyObject* self, PyObject *args) {
	lcd_reset();
	return Py_BuildValue("l", 0);
}

static PyObject* py_lcd_clear(PyObject* self, PyObject* args) {
	int clr = 0;
	PyArg_ParseTuple(args, "i", &clr);
	lcd_clear(clr);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_matrix(PyObject* self, PyObject* args) {
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
	lcd_matrix(x,y,ex,ey,matrix,size,0,0);
	free(matrix);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_load_chars(PyObject* self, PyObject* args)
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
	lcd_load_chars(c_w, c_h, c_c, *fnt, size);
	return Py_BuildValue("l", 0);
}

static PyObject* py_lcd_draw_symbol(PyObject* self, PyObject* args)
{
	uint32_t sym, x, y, clr;
	PyArg_ParseTuple(args, "IIII", &x, &y, &sym, &clr);
	lcd_draw_symbol(x,y,sym,clr);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_draw_string(PyObject* self, PyObject* args)
{
	uint32_t x,y, base, clr;
	const char *s;
	PyArg_ParseTuple(args, "IIIsI", &x, &y, &base, &s, &clr);
	lcd_draw_string(x,y,base,clr, s);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_rectangle(PyObject* self, PyObject* args) {
	uint32_t clr, x, y, ex, ey;
	PyArg_ParseTuple(args,"IIIII", &x, &y, &ex, &ey, &clr);
	lcd_rectangle(x,y,ex,ey,clr);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_rectangle_empty(PyObject *self, PyObject* args) {
	uint32_t clr2, clr,x,y,ex,ey;
	PyArg_ParseTuple(args, "IIIIII", &x, &y, &ex, &ey, &clr, &clr2);
	lcd_rectangle(x,y,ex,ey, clr2);
	lcd_rectangle(x+1, y+1, ex-1, ey-1, clr);
	return Py_BuildValue("l",0);
}

static PyObject* py_lcd_init(PyObject* self, PyObject*args) {
	lcd_init();
	return Py_BuildValue("l", 0);
}

static PyMethodDef LcdMethods[] = {
	{ "lcdOpen", py_lcd_open, METH_VARARGS},
	{ "lcdClose", py_lcd_close, METH_VARARGS},
	{ "lcdReset", py_lcd_reset, METH_VARARGS},
	{ "lcdInit", py_lcd_init, METH_VARARGS},
	{ "lcdClear", py_lcd_clear, METH_VARARGS},
	{ "lcdRectangle", py_lcd_rectangle, METH_VARARGS},
	{ "lcdRectangleEmpty", py_lcd_rectangle_empty, METH_VARARGS},
	{ "lcdMatrix", py_lcd_matrix, METH_VARARGS},
	{ "lcdLoadChars", py_lcd_load_chars, METH_VARARGS},
	{ "lcdDrawSymbol", py_lcd_draw_symbol, METH_VARARGS},
	{ "lcdDrawString", py_lcd_draw_string, METH_VARARGS},
	{NULL, NULL}
};



void initlcdmodule() {
	(void) Py_InitModule("lcdmodule",LcdMethods);
}
 

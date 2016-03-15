#ifndef KEDEI_HEADER
#define KEDEI_HEADER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <bcm2835.h>
#include <time.h>

#define LCD_CS 1
#define TOUCH_CS 0
#define LCD_WIDTH 480
#define LCD_HEIGHT 320

extern int delayus(int us);
extern int delayms(int ms);
extern int delays(int s);

extern int lcd_open(void);
extern int lcd_close(void);
extern void lcd_reset(void);
extern void lcd_data(uint16_t data);
extern void lcd_cmd(uint16_t cmd);
extern void lcd_clear(uint32_t clr);
extern void lcd_rectangle(uint32_t x, uint32_t y, uint32_t ex, uint32_t ey, uint32_t clr);
extern void lcd_rectangle_empty(uint32_t x, uint32_t y, uint32_t ex, uint32_t ey,  uint32_t clr1, uint32_t clr2);
extern void lcd_matrix(uint32_t x, uint32_t y, uint32_t ex, uint32_t ey, uint32_t *matrix, uint32_t size, uint8_t transform, uint32_t transformer);
extern void lcd_load_chars(uint32_t cw, uint32_t ch, uint32_t cc, uint32_t* matrix);
extern void lcd_draw_symbol(uint32_t x, uint32_t y, uint32_t sym, uint32_t clr);
extern void lcd_draw_string(uint32_t x, uint32_t y, uint32_t base, uint32_t clr, const char *s);
extern void lcd_init(void);

#endif
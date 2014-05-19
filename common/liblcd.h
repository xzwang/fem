#ifndef _LIBLCD_H_
#define _LIBLCD_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "liblcd.h"
#include "syspicture.h"

#define LCD_DEVICE "/dev/fb0"
#define FONT_COLOR 0x207
#define HZ24_FONT_SIZE 72
#define HZ16_FONT_SIZE 32
#define ASC24_FONT_SIZE 36
#define ASC16_FONT_SIZE 16
#define FB_BPP  2
#define FB_XMAX 800
#define FB_YMAX 480
#define FB_XOFFSET 0
#define FB_YOFFSET 0
#define FB_LINE_LENGTH 1600
#define FB_BLACK 0xffff
#define HZK24_FILE "HZK24.bin"

enum {
	WAITBACKGD,
	WAITINFO,
	CDSWAP,
	WAITSYS,
	CDNOTUSE,
	USERINFO,
	NETERROR,
	SYSWRITECD,
	WRITESUCC,
	WRITEERR,
	YFB_DEMO,
};

extern int open_lcd(void);
extern void close_lcd(int fd, char *fb, int fbsize);
extern int init_lcd(int fd, struct fb_var_screeninfo *vinfo,
		struct fb_fix_screeninfo *finfo);
extern char *request_fbmem(int fd, int fbsize);
extern void ScrCls(char *fb);
extern int fill_background(char *fb, const unsigned short *Image);
extern void putc_lcd_asc16(char *fb, int x, int y, char ch);
extern void puts_lcd_asc16(char *fb, int x, int y, const char *fmt, ...);
extern int lcd_printf16(char *fb, int x, int y, unsigned char *font);
extern void putc_lcd_asc24(char *fb, int x, int y, char ch);
extern void puts_lcd_asc24(char *fb, int x, int y, const char *fmt, ...);
extern int lcd_printf24(char *fb, int x, int y, unsigned char *font);
extern int assign_picture(char *fb, int x, int y, int xoft, int yoft, const unsigned short *Image);
extern int display_def_picture(char *fb, int type);
extern int set_lcd_backlight(int brightness);
#endif

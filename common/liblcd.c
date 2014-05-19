#include "liblcd.h"
#include "asc1224.h"
extern unsigned char gImage_user[]; /* 480 X 272 */
extern unsigned char gImage_background[]; /* 480 X 272 */
extern unsigned char gImage_cdswap[]; /* 327 X 101 */
extern unsigned char gImage_neterror[]; /* 327 X 101 */
extern unsigned char gImage_notusecd[]; /* 327 X 101 */
extern unsigned char gImage_pleasecd[]; /* 277 X 32 */
extern unsigned char gImage_syswritecd[]; /* 327 X 101 */
extern unsigned char gImage_waitsys[]; /* 327 X 101 */
extern unsigned char gImage_writecder[]; /* 327 X 101 */
extern unsigned char gImage_writecdsc[]; /* 327 X 101 */

/* 480 X 272 ==> start point: x = 76, y = 85 */
/* 277 X 32 : x =101 , y =120 */
/*======================================================================
* framebuffer operation functions
*======================================================================*/

int open_lcd(void)
{
	int fd;

	fd = open(LCD_DEVICE, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Can't open %s devices\n", LCD_DEVICE);
		return -1;
	}
	
	return fd;
}

void close_lcd(int fd, char *fb, int fbsize)
{
	
	munmap(fb, fbsize);
	close(fd);
}

int init_lcd(int fd, struct fb_var_screeninfo *vinfo,
		struct fb_fix_screeninfo *finfo)
{
	int ret;
	
	if (ioctl(fd, FBIOGET_FSCREENINFO, finfo)) {
		fprintf(stderr, "Read framebuffer fixed information failed\n");
		return -1;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, vinfo)) {
		fprintf(stderr, "Read framebuffer variable information failed\n");
		return -2;
	}

	ret = vinfo->xres * vinfo->yres * vinfo->bits_per_pixel/8;

	return ret;
}

char *request_fbmem(int fd, int fbsize)
{
	char *fb;
	fb = (char *)mmap(0, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (!fb) {
		fprintf(stderr, "failed to map framebuffer device to memory\n");
	}

	return fb;
}

/*=============================================================================
 *  font display function 
 *===========================================================================*/

static int get_asc16_font(unsigned long ftoft, unsigned char *font)
{
	FILE *fp;

	if (!font)
		return -1;
	fp = fopen("ASC16.bin", "rb");
	if (fp == NULL) {
		fprintf(stderr, "open ASC16 libs failed\n");
		return -2;
	}

	fseek(fp, ftoft, SEEK_SET);
	fread(font, ASC16_FONT_SIZE, 1, fp);

	fclose(fp);

	return 0;
}

static int get_hzk16_font(unsigned long ftoft, unsigned char *font)
{
	FILE *fp;

	if (!font)
		return -1;
	fp = fopen("HZK16.bin", "rb");
	if (fp == NULL) {
		fprintf(stderr, "open HZK16 libs failed\n");
		return -2;
	}

	fseek(fp, ftoft, SEEK_SET);
	fread(font, HZ16_FONT_SIZE, 1, fp);

	fclose(fp);

	return 0;
}

static int get_hzk24_font(unsigned long ftoft, unsigned char *font)
{
	FILE *fp;

	if (!font)
		return -1;
	fp = fopen("HZK24.bin", "rb");
	if (fp == NULL) {
		fprintf(stderr, "open HZK24 libs failed\n");
		return -2;
	}

	fseek(fp, ftoft, SEEK_SET);
	fread(font, HZ24_FONT_SIZE, 1, fp);

	fclose(fp);

	return 0;
}

#if 0
static int get_asc24_font(unsigned long ftoft, unsigned char *font)
{
	return 0;
}

void DrawDot(char *fb, int x, int y)
{
	int xres, yres;

	xres = x * FB_BPP + y*FB_LINE_LENGTH;
	*((unsigned short *)(fb + xres)) = 0x0;
	
}
static void draw_asc24(char *fb, int x, int y, int addr)
{
	char dot;

	int adjx,adjy,i,j,k;



	adjx = x;adjy = y;

	for(k=0;k<3;k++){

		adjx = x;

		for(j=0;j<12;j++){



			dot = asc12x24[addr];

			for(i=0;i<8;i++){

				if (dot & 0x80) {
					DrawDot(fb, adjx,adjy+i);
			//		fprintf(stderr, "x=%d, y=%d\n", adjx, adjy+i);
				}
				dot <<= 1;

			}	

			adjx ++;

			addr ++;

		}	

		adjy += 8;

	}
}
#endif

static void drawch16(char *fb, int x, int y, unsigned char *asc16)
{
	char dot;
	int xres, yres;
	int i,x1,y1;
	int xoft, yoft;


	x1 = x;
	y1 = y; 
	yoft  = y + 16;
	xoft  = x + 8;

	for (y1 = y;y1 < yoft; y1++) {
		yres = y1 * FB_LINE_LENGTH;
		for (x1 = x; x1 < xoft; x1++) {
			xres = x1* FB_BPP + yres;
			if(asc16[(y1-y)+(x1-x)/8] & 0x80>>(x1-x)%8)
				*((unsigned short *)(fb + xres)) = FONT_COLOR;

		}

	}

}
static void drawch24(char *fb, int x, int y, int addr)
{
	char dot;
	int xres, yres;
	int i,x1,y1;
	int xoft, yoft;


	x1 = x;
	y1 = y; 
	yoft  = y + 24;
	xoft  = x + 12;

	for (y1 = y;y1 < yoft;) {
		yres = y1 * FB_LINE_LENGTH;
		for (x1 = x; x1 < xoft; x1++) {
			xres = x1* FB_BPP + yres;
			dot = asc12x24[addr];
			for (i = 0; i < 8; i++) {
				if(dot & 0x80) {
					*((unsigned short *)(fb + xres + i*FB_LINE_LENGTH)) = FONT_COLOR;
				}
				dot <<= 1;
			}
			addr ++;
		}
		y1 += 8;

	}



}

void putc_lcd_asc16(char *fb, int x, int y, char ch)
{
	unsigned long ftoft;
	unsigned char asc16[16];

	ftoft = (unsigned char)ch * 16;
	get_asc16_font(ftoft, asc16);

	drawch16(fb, x, y, asc16);
}

void putc_lcd_asc24(char *fb, int x, int y, char ch)
{
	int addr;
	addr = (ch - 0x20) * ASC24_FONT_SIZE;
//	draw_asc24(fb, x, y, addr);
	drawch24(fb, x, y, addr);
}

void puts_lcd_asc16(char *fb, int x, int y, const char *fmt, ...)
{
	
	va_list  ap;
	char string[64];
	int i;
	memset(string, 0, sizeof(string));
	va_start(ap, fmt);
	vsnprintf(string, sizeof(string), fmt, ap);
	va_end(ap);

	i = 0;
	while (string[i] != '\0') {
		putc_lcd_asc16(fb, x, y, string[i]);
		x += 8;
		i++;
	}

}

void puts_lcd_asc24(char *fb, int x, int y, const char *fmt, ...)
{
	
	va_list  ap;
	char string[64];
	int i;
	memset(string, 0, sizeof(string));
	va_start(ap, fmt);
	vsnprintf(string, sizeof(string), fmt, ap);
	va_end(ap);

	i = 0;
	while (string[i] != '\0') {
		putc_lcd_asc24(fb, x, y, string[i]);
		x += 12;
		i++;
	}

}

static void putc_lcd_hzk16(char *fb, int x, int y, unsigned char *mat)
{

	int x1, y1;
	int xoft, yoft;
	int xres, yres;

	x1 = x;
	y1 = y; 
	yoft  = y + 16;
	xoft  = x + 16;

	for (y1 = y;y1 < yoft; y1++) {
		yres = y1 * FB_LINE_LENGTH;
		for (x1 = x; x1 < xoft; x1++) {
			xres = x1* FB_BPP + yres;
			if(mat[(y1-y)*2+(x1-x)/8] & 0x80>>(x1-x)%8)
				*((unsigned short *)(fb + xres)) = FONT_COLOR;

		}

	}


}

static void putc_lcd_hzk24(char *fb, int x, int y, unsigned char *mat)
{

	int x1, y1;
	int xoft, yoft;
	int xres, yres;

	x1 = x;
	y1 = y; 
	yoft  = y + 24;
	xoft  = x + 24;

	for (y1 = y;y1 < yoft; y1++) {
		yres = y1 * FB_LINE_LENGTH;
		for (x1 = x; x1 < xoft; x1++) {
			xres = x1* FB_BPP + yres;
			if(mat[(y1-y)*3+(x1-x)/8] & 0x80>>(x1-x)%8)
				*((unsigned short *)(fb + xres)) = FONT_COLOR;

		}

	}


}

int lcd_printf16(char *fb, int x, int y, unsigned char *font)
{
	unsigned char inode[2];
	unsigned char *ft;
	unsigned long ftoft;
	unsigned char mat[16][2];
	int xnum, curx, line, xreg;
	
	if (!fb | !font)
		return -1;
	if (x > 480 || y > 272)
		return -2;
	ft = font;
	xreg = x;
	xnum = (480 - x)/8;
	curx = xnum;
	line = 2;

	while (*ft != '\0') {
		if (*ft & 0x80) {/* HZK */
			inode[0] = *ft++ - 0xa0; /* Q */
			if (*ft & 0x80)
				inode[1] = *ft++ - 0xa0; /* W */
			else
				return -4;
			ftoft = ((inode[0] - 1)*94 + (inode[1] -1)) * HZ16_FONT_SIZE;
			get_hzk16_font(ftoft, (unsigned char*)mat);
			putc_lcd_hzk16(fb, x, y, (unsigned char *)mat);
			x += 16;
			curx -= 2;
		}			
		else { /* ASCII 16 */
			putc_lcd_asc16(fb, x, y, *ft);
			ft++;
			x += 8;
			curx -= 1;
		}
		
		if (curx > 2)
			continue;
		else {
			line--;
			curx = xnum;
			y += 16;
			x = xreg;
		}
		if (line == 0)
			return -3;
	}

	return 0;
}

int lcd_printf24(char *fb, int x, int y, unsigned char *font)
{
	unsigned char inode[2];
	unsigned char *ft;
	unsigned long ftoft;
	unsigned char mat[24][3];
	int xnum, curx, line, xreg;

	if (!fb | !font)
		return -1;
	if (x > 480 || y > 272) {
		fprintf(stderr, "function:display_font X/Y error:x<480,y<272\n");
		return -2;
	}

	ft = font;
	xreg = x;
	xnum = (480 - x)/12;
	curx = xnum;
	line = 2;

	while (*ft != '\0') {
		if (*ft & 0x80) {/* HZK24 */

			inode[0] = *ft++ - 0xa0; 	/* q code */
			if(*ft & 0x80) {
				inode[1] = *ft++ - 0xa0; /* w code */
			}
			else 
				return -4;		/* font lib error */
			ftoft = ((inode[0] -1)*94 + (inode[1] -1)) * HZ24_FONT_SIZE;

			get_hzk24_font(ftoft, (unsigned char *)mat);
			putc_lcd_hzk24(fb, x, y, (unsigned char *)mat);
			x = x + 24;
			curx -= 2;

		}
		else { /* ASCII 24 */
			putc_lcd_asc24(fb, x, y, (char)*ft);
			ft++;
			x += 12;
			curx -= 1;
		}
		
		if (curx > 2)
			continue;
		else {
			line--;
			curx = xnum;
			y += 24;
			x = xreg;
		}
		if (line == 0)
			return -3;
	}

	return 0;
}

/*=============================================================================
 * LCD picture display
 *============================================================================*/

void ScrCls(char *fb)
{
	
	int xres, yres;
	int x, y;
	for (y = 0; y < FB_YMAX; y++) {
		yres = y * FB_LINE_LENGTH;
		for (x = 0; x < FB_XMAX; x++) {
				xres = x * FB_BPP + yres;
				*((unsigned short *)(fb + xres)) = FB_BLACK;
		}
	}
}

void Scrcls(char *fb, int start_l, int end_l)
{
	int xres, yres;
	int x;
	
	if(start_l >= FB_YMAX || start_l > end_l || end_l >= FB_YMAX)
		return;
	
	for (;start_l <= end_l; start_l++) {
		yres = start_l * FB_LINE_LENGTH;
		for (x = 0; x < FB_XMAX; x++) {
			xres = x * FB_BPP + yres;
			*((unsigned short *)(fb + xres)) = FB_BLACK;
		}
	}
}

int fill_background(char *fb, const unsigned short *Image)
{
	int xres;
	int yres;
	int x, y;

	if (!fb || !Image)
		return -1;

	for (y = 0; y < FB_YMAX; y++) {
		yres = y * FB_LINE_LENGTH;
		for (x = 0; x < FB_XMAX; x++) {
				xres = x * FB_BPP + yres;
				*((unsigned short *)(fb + xres)) = *Image++;
		}
	}

	return 0;
}

int assign_picture(char *fb, int x, int y, int xoft, int yoft, const unsigned short *Image)
{
	int x1,y1;
	int xres, yres;

	if (!fb || !Image)
		return -1;

	if(x > FB_XMAX || y > FB_YMAX)
		return -2;
	
	if(x + xoft > FB_XMAX || y + yoft > FB_YMAX)
		return -3;
	
	for (y1 = y; y1 < y+yoft; y1++) {
		yres = y1 * FB_LINE_LENGTH;
		for (x1 = x; x1 < x+xoft; x1++) {
			xres = x1 * FB_BPP + yres;
			*((unsigned short *)(fb + xres)) = *Image++;
		}
	}

	return 0;
}


int display_def_picture(char *fb, int type)
{
	int ret;

	if (!fb)
		return -1;

	switch(type) {

		case WAITBACKGD:
				/* ret = fill_background(fb, (unsigned short *)gImage_background); */
				ret = assign_picture(fb, 0, 0, 480, 272, (unsigned short *)gImage_background);
				break;
		case WAITINFO:  
				ret = assign_picture(fb, 101, 120, 277, 32, (unsigned short *)gImage_pleasecd);
				break;
		case CDSWAP:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_cdswap); 
				break;
		case WAITSYS:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_waitsys);
				break;
		case CDNOTUSE:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_notusecd);
				break;
		case USERINFO:
				/* ret = fill_background(fb, (unsigned short *)gImage_user); */
				ret = assign_picture(fb, 0, 0, 480, 272, (unsigned short *)gImage_user);
				break;
		case NETERROR:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_neterror);
				break;
		case SYSWRITECD:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_syswritecd);
				break;
		case WRITESUCC:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_writecdsc);
				break;
		case WRITEERR:
				ret = assign_picture(fb, 76, 85, 327, 101, (unsigned short *)gImage_writecder);
				break;
		case YFB_DEMO:
				ScrCls(fb);
				ret = assign_picture(fb, 27, 30, 746, 418, (unsigned short *)gImage_yfbdemo);
				break;
		default: 
			ret = 0xff;
			break;
	}

	return ret;
}

/*
 * LCD backlight control interface
 */
static int sysprintf(const char *fmt, ...)
{
	unsigned char cmdbuf[128];
	va_list args;

	va_start(args, fmt);
	vsnprintf(cmdbuf, sizeof(cmdbuf), fmt, args);
	va_end(args);
	return system(cmdbuf);
}

int set_lcd_backlight(int brightness)
{
	if (brightness < 0 || brightness > 100)
		return -1;

	sysprintf("echo %d > /sys/class/backlight/pwm-backlight/brightness", brightness);

	return 0;
}

#include "fem_fb.h"
void lcd_test(void)
{
        int fd;
        int fbsize;
        char *fb = NULL;
        struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        int ret;

        unsigned char name[16]="强军科技";
        unsigned char addr[128]="湖南省长沙市望城区大风雷锋大道黄金路口";
        unsigned char mon[16] ="10000KW/h";
        unsigned char cur[16]="1000KW/h";

        /* open lcd devices */
        fd = open_lcd();
        if (fd < 0)
                return;

        fbsize = init_lcd(fd, &vinfo, &finfo);
        if (fbsize < 0)
                return;
        fprintf(stderr, "framebuffer size:%d,%d X %d\n", fbsize, vinfo.xres, vinfo.yres);
        fprintf(stderr, "lcd bit per pixel:%dbpp\n", vinfo.bits_per_pixel);
        fprintf(stderr, "lcd line length:%d\n", finfo.line_length);

        fb = request_fbmem(fd, fbsize);
        if (!fb) {
                return;
        }
        ScrCls(fb);
#if 1
        sleep(1);
        ret = display_def_picture(fb, WAITBACKGD);
        ret = display_def_picture(fb, WAITINFO);

        sleep(1);
        ret = display_def_picture(fb, CDSWAP);
        sleep(1);
        ret = display_def_picture(fb, WAITBACKGD);
        ret = display_def_picture(fb, WAITSYS);
        sleep(1);
        ret = display_def_picture(fb, WAITBACKGD);
        ret = display_def_picture(fb, CDNOTUSE);
        sleep(1);
        ret = display_def_picture(fb, USERINFO);
        lcd_printf24(fb, 75, 58, name);
        lcd_printf16(fb, 75, 102, addr);
        lcd_printf24(fb, 75, 150, mon);
        lcd_printf16(fb, 108, 190, cur);
        puts_lcd_asc16(fb, 0, 0, cur);
        sleep(2);
        ret = display_def_picture(fb, USERINFO);
        ret = display_def_picture(fb, NETERROR);
        sleep(1);
        ret = display_def_picture(fb, USERINFO);
        ret = display_def_picture(fb, SYSWRITECD);
        sleep(1);
        ret = display_def_picture(fb, USERINFO);
        ret = display_def_picture(fb, WRITESUCC);
        sleep(1);
        ret = display_def_picture(fb, USERINFO);
        ret = display_def_picture(fb, WRITEERR);
#endif
        close_lcd(fd, fb, fbsize);
}


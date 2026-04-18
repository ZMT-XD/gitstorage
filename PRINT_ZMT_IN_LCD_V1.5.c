#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>

static int fd_fb;
static struct fb_var_screeninfo var;	/* Current var */
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;

/**********************************************************************
                     增加碰撞效果，遇到屏幕边缘即回弹

                                                         TIME:2026.4.15
                                                                 By ZMT
 ***********************************************************************/ 
void lcd_put_pixel(int x, int y, unsigned int color)
{
	unsigned char *pen_8 = fb_base+y*line_width+x*pixel_width;
	unsigned short *pen_16;	
	unsigned int *pen_32;	

	unsigned int red, green, blue;	

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue  = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	int i;
	
	fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0)
	{
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can't get var\n");
		return -1;
	}
	ioctl(fd_fb, FBIOBLANK, FB_BLANK_UNBLANK);
	line_width  = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
	fb_base = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if (fb_base == (unsigned char *)-1)
	{
		printf("can't mmap\n");
		return -1;
	}

	/* 清屏: 全部设为白色 */
	memset(fb_base, 0xff, screen_size);

	/* 屏幕1024*600
	打印出姓名 
	
	| 颜色         | R   | G   | B   | 十六进制     |
        | ----------  | --- | --- | --- | -------- |
        | 红 (Red)    | 255 | 0   | 0   | 0xFF0000 |
        | 橙 (Orange) | 255 | 127 | 0   | 0xFF7F00 |
        | 黄 (Yellow) | 255 | 255 | 0   | 0xFFFF00 |
        | 绿 (Green)  | 0   | 255 | 0   | 0x00FF00 |
        | 青 (Cyan)   | 0   | 255 | 255 | 0x00FFFF |
        | 蓝 (Blue)   | 0   | 0   | 255 | 0x0000FF |
        | 紫 (Purple) | 139 | 0   | 255 | 0x8B00FF |

	
	*/
	
	while(1){
	memset(fb_base, 0xff, screen_size);
	//static unsigned int count = 0; 
	/*switch(count%3){
	case 0 :color=0xFF0000;break;
	case 1 :color=0x00FF00;break;
	case 2 :color=0x0000FF;break;	
	}*/
	usleep(50000);   // 50ms
	
	static unsigned int count = 0;	
	static int reverse = 0;
	static bool add_minus=1; 
  	    
	
	static int color = 0;
	static char color_flag = 0;
	switch(color_flag%7){
	case 0 : color=0xFF0000;break;
	case 1 : color=0xFF7F00;break;
	case 2 : color=0xFFFF00;break;
	case 3 : color=0x00FF00;break;
	case 4 : color=0x00FFFF;break;
	case 5 : color=0x0000FF;break;
	case 6 : color=0x8B00FF;break;
	}
	
			  
        /*if(color1==0xFF0000){
        color2=color1>>8;
        color3=color2>>8;
        }
        else if(color1==0x00FF00){
        color2=color1>>8;
        color3=color1<<8;
        }
        else if(color1==0x0000FF){
        color3=color1<<8;
        color2=color3<<8;
        }*/
        
	for (i = 0; i < 100; i++){
	lcd_put_pixel(i+reverse, var.yres*2/3-52,     color);
	lcd_put_pixel(i+reverse, var.yres*2/3-51,     color);
        lcd_put_pixel(i+reverse, var.yres*2/3-50,     color);
        lcd_put_pixel(99-i+reverse, var.yres*1/3+49+i,   color);
        lcd_put_pixel(100-i+reverse, var.yres*1/3+49+i,   color);
        lcd_put_pixel(101-i+reverse, var.yres*1/3+49+i,   color);
        lcd_put_pixel(i+reverse, var.yres*1/3+49,     color); 
        lcd_put_pixel(i+reverse, var.yres*1/3+48,     color); 
        lcd_put_pixel(i+reverse, var.yres*1/3+47,     color);       
  	}
	
	for (i = 0; i < 50; i++){
	lcd_put_pixel(var.xres*1/3-100+i+reverse, var.yres*2/3-50-2*i,color);
	lcd_put_pixel(var.xres*1/3-101+i+reverse, var.yres*2/3-50-2*i,color);
	lcd_put_pixel(var.xres*1/3-102+i+reverse, var.yres*2/3-50-2*i,color);
	
        lcd_put_pixel(var.xres*1/3-50+i+reverse, var.yres*2/3-50-2*49+2*i,color);
        lcd_put_pixel(var.xres*1/3-51+i+reverse, var.yres*2/3-50-2*49+2*i,color);
        lcd_put_pixel(var.xres*1/3-52+i+reverse, var.yres*2/3-50-2*49+2*i,color);
        
	lcd_put_pixel(var.xres*1/3+i+reverse, var.yres*2/3-50-2*i,color);
	lcd_put_pixel(var.xres*1/3+i+1+reverse, var.yres*2/3-50-2*i,color);
	lcd_put_pixel(var.xres*1/3+i+2+reverse, var.yres*2/3-50-2*i,color);
	
	lcd_put_pixel(var.xres*1/3+50+i+reverse, var.yres*2/3-50-2*49+2*i,color);
	lcd_put_pixel(var.xres*1/3+51+i+reverse, var.yres*2/3-50-2*49+2*i,color);
	lcd_put_pixel(var.xres*1/3+52+i+reverse, var.yres*2/3-50-2*49+2*i,color);
	}
	
	
	for (i = 0; i < 100; i++){
	
        lcd_put_pixel(var.xres*2/3-100+i+reverse, var.yres*1/3+49, color);
        lcd_put_pixel(var.xres*2/3-100+i+reverse, var.yres*1/3+48, color);
        lcd_put_pixel(var.xres*2/3-100+i+reverse, var.yres*1/3+47, color);
        
        lcd_put_pixel(var.xres*2/3-50+reverse, var.yres*1/3+48+i, color);
        lcd_put_pixel(var.xres*2/3-51+reverse, var.yres*1/3+48+i, color);
        lcd_put_pixel(var.xres*2/3-52+reverse, var.yres*1/3+48+i, color);
             
  	}
  	
  	
  	count++;
  	
  	if(var.xres*2/3-100+99+reverse>=var.xres-19){
  	add_minus=0;
  	color_flag++;
  	}
  	else if(reverse==0&&(count!=1)){
  	add_minus=1;
  	color_flag++;
  	}

  	if(add_minus==1)
  	reverse+=20;
  	else
  	reverse-=20;

	usleep(50000);   // 50ms	
	
	}
	
	munmap(fb_base , screen_size);
	
	close(fd_fb);
	
	return 0;	
}


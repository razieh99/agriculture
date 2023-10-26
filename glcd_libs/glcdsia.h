#ifndef _GLCDSIA_INCLUDED_
#define _GLCDSIA_INCLUDED_

#include  <string.h>
#include <ff.h>
#include <stdio.h>.
#include <stdbool.h>
#include <font5x7.h>

#if defined _GLCD_CTRL_ILI9325_
#include <glcd_ili9325.h>
#elif defined _GLCD_CTRL_ILI9340_
#include <glcd_ili9340.h>
#elif defined _GLCD_CTRL_ILI9341_
#include <glcd_ili9341.h>
#elif defined _GLCD_CTRL_KS0108_
#include <glcd_ks0108.h>
#elif defined _GLCD_CTRL_PCD8544_
#include <glcd_pcd8544.h>
#elif defined _GLCD_CTRL_S1D13700_
#include <glcd_s1d13700.h>
#elif defined _GLCD_CTRL_S6D1121_
#include <glcd_s6d1121.h>
#elif defined _GLCD_CTRL_SED1335_
#include <glcd_sed1335.h>
#elif defined _GLCD_CTRL_SED1520_
#include <glcd_sed1520.h>
#elif defined _GLCD_CTRL_SED1530_
#include <glcd_sed1530.h>
#elif defined _GLCD_CTRL_SPLC501_
#include <glcd_splc501.h>
#elif defined _GLCD_CTRL_SSD1289_
#include <glcd_ssd1289.h>
#elif defined _GLCD_CTRL_SSD1303_
#include <glcd_ssd1303.h>
#elif defined _GLCD_CTRL_SSD1322_
#include <glcd_ssd1322.h>
#elif defined _GLCD_CTRL_SSD1963_
#include <glcd_ssd1963.h>
#elif defined _GLCD_CTRL_SSD2119_
#include <glcd_ssd2119.h>
#elif defined _GLCD_CTRL_ST7565_
#include <glcd_st7565.h>
#elif defined _GLCD_CTRL_ST7567_
#include <glcd_st7567.h>
#elif defined _GLCD_CTRL_ST7920_
#include <glcd_st7920.h>
#elif defined _GLCD_CTRL_T6963_
#include <glcd_t6963.h>
#elif defined _GLCD_CTRL_UC1608_
#include <glcd_uc1608.h>
#elif defined _GLCD_CTRL_UC1701_
#include <glcd_uc1701.h>
#elif defined _GLCD_CTRL_XG7100_
#include <glcd_xg7100.h>
#else
#error No graphic controller specified in the project configuration
#endif

#define XSIZE 240   //
#define YSIZE 320
#define MSB 0
#define LSB 1

#ifndef NULL
#define NULL 0
#endif




typedef struct  {
int x;
int y;
}POINT; 


void glcdsia_init(int backcolor,int forecolor,flash unsigned char *defaultfont,char isLandscape);
void SetAddrWindow(int x0, int y0, int x1, int y1);
//POINT PutCh2 (int ch, int x, int y );
void PutCh2 (int ch, int x, int y,char *fontH,char *fontW);

void glcdsia_PutJPG(int left,int top,flash unsigned char* img);
void glcdsia_setfont(flash unsigned char *font);
void glcdsia_outtext(char *text);
void glcdsia_gotoxy(int xpos,int ypos);
void glcdsia_clear();
void glcdsia_drawlineH(int xstart,int ystart,int xend ,unsigned char thickness);//horizontal line
void glcdsia_drawlineH2(int xstart,int ystart,int lenght ,unsigned char thickness);//horizontal line
void glcdsia_putpixel(int x , int y,int color);
int glcdsia_getforeclr();
int glcdsia_getbackclr();
void glcdsia_setforeclr(int color);
void glcdsia_setbackclr(int color);
void glcdsia_outtextxy(int x,int y,char *text); 
void glcdsia_putImgSdCard(int xpos,int ypos,char* path,char *result,char turnLCDoff);
#endif
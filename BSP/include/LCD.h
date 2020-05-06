/******************************************************************************/
/* LCD.h: Graphic LCD function prototypes and defines                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2009 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#ifndef _LCD_H
#define _LCD_H

/*---------------------- Graphic LCD size definitions ------------------------*/
#ifdef _LCD_PORTRAIT 
#define WIDTH       240                 /* Screen Width (in pixels)           */
#define HEIGHT      320                 /* Screen Hight (in pixels)           */
#endif

#ifdef _LCD_LANDSCAPE 
#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#endif

void LCD_Pin_Config(void);
/*------------------------------------------------------------------------------
  Color coding
  LCD is coded:   15..11 red, 10..5 green, 4..0 blue  (unsigned short)  LCD_R5, LCD_G6, LCD_B5   
  original coding: 17..12 red, 11..6 green, 5..0 blue                    ORG_R6,  ORG_G6,  ORG_B6

  ORG_R1..5 = LCD_R0..4,  ORG_R0 = LCD_R4
  ORG_G0..5 = LCD_G0..5,
  ORG_B1..5 = LCD_B0..4,  ORG_B0 = LCD_B4
 *----------------------------------------------------------------------------*/
                            
/* LCD RGB color definitions                                                 */
#define LCD_COLOR_BLACK           0x0000	/*   0,   0,   0 */
#define LCD_COLOR_NAVY            0x000F   	/*   0,   0, 128 */
#define LCD_COLOR_DARKGREEN       0x03E0      	/*   0, 128,   0 */
#define LCD_COLOR_DARKCYAN        0x03EF      	/*   0, 128, 128 */
#define LCD_COLOR_MARROON          0x7800      	/* 128,   0,   0 */
#define LCD_COLOR_PURPLE          0x780F      	/* 128,   0, 128 */
#define LCD_COLOR_OLIVER           0x7BE0      	/* 128, 128,   0 */
#define LCD_COLOR_LIGHTGREY       0xC618      	/* 192, 192, 192 */
#define LCD_COLOR_DARKGREY        0x7BEF      	/* 128, 128, 128 */
#define LCD_COLOR_BLUE            0x001F      	/*   0,   0, 255 */
#define LCD_COLOR_GREEN           0x07E0      	/*   0, 255,   0 */
#define LCD_COLOR_CYAN            0x07FF      	/*   0, 255, 255 */
#define LCD_COLOR_RED             0xF800      	/* 255,   0,   0 */
#define LCD_COLOR_Magenta         0xF81F      	/* 255,   0, 255 */
#define LCD_COLOR_YELLOW          0xFFE0      	/* 255, 255, 0   */
#define LCD_COLOR_WHITE           0xFFFF      	/* 255, 255, 255 */

typedef enum {//自己添加部分
	Home1,
	Time1,
	Note1,
	More1
} tyDOCK;
extern void LCD_Init  (void);
void LCD_SetWindow (unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end);
void LCD_PutPixel       (unsigned int x, unsigned int y);
void LCD_SetTextColor   (unsigned short color);
void LCD_SetBackColor   (unsigned short color);
void LCD_ClearScreen    (unsigned short color);
void LCD_DisplayChar (uint16_t ln, uint16_t col, unsigned char c);

void LCD_DisplayChar2 (uint16_t ln, uint16_t col, unsigned char c); //自己添加显示16*24字函数（字符）

void LCD_DisplayString  (unsigned int ln, unsigned int col, unsigned char *s);

void LCD_DisplayString2  (unsigned int ln, unsigned int col, unsigned char *s); //自己添加显示16*24字函数（字符串）



//部分清屏函数，输入起始坐标、长、宽、颜色
// void Clear( uint16_t x, uint16_t y, uint16_t len, uint16_t high, uint16_t color );
void Clear( void );

//画横线函数，输入起始坐标、长度、颜色
void LineX( int x, int y, int len, uint16_t color );

//画竖线函数，输入起始坐标、长度、颜色
void LineY( int x, int y, int len, uint16_t color );

//画矩形函数，输入起始坐标、长、宽、颜色
void Square( int x, int y, int len, int high, uint16_t color );

//以下为自己添加的部分清屏函数
void LCD_ClearScreen2 (unsigned short color);


//extern void LCD_ClearLn        (unsigned int ln);
//extern void LCD_Bargraph       (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val);
//extern void LCD_Bitmap         (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap);
extern void LCD_ShowBmp (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bmp);

void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos);

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);


#endif /* _LCD_H */

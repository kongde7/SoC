/******************************************************************************/
/* GLCD_SPI_LPC1700.c: LPC1700 low level Graphic LCD (320x240 pixels) driven  */
/*                     with SPI functions                                     */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2009 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/


#include <lpc17xx.h>
#include "stdint.h"
#include "LCD.h"
#include "bmboard.h"
#include "ssd1298.h"
#include "fonts_5x7.h"
#include "fonts_16x24.h"


static unsigned short DeviceCode;

/* LCD module is fixed */
#define LCD_WIDTH	240
#define LCD_HEIGHT	320

#define CHAR_H      8
#define CHAR_W      6


/*******************************************************************************
*                                                DEFINES
* 
* Note(s) (1): LCD orientation and GRAM configuration:
*
*                        | ------ 240 ------ |
*                   (0,0)+===================+  --->x 
*                        |                   |   |
*           GRAM         | ----------------> |   |
*        ADDRES UPDATE   | ----------------> |   |
*                        |                   |   |
*                        |                   |   |  
*                        |                   |  320
*                        |                   |   |  
*                        |                   |   |
*                        |                   |   |
*                        |                   |   |
*                        |                   |   |
*                        +===================+  ---
*                       \ /                    (239, 319)
*******************************************************************************/

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*--------------- Graphic LCD interface hardware definitions -----------------*/

/* Pin EN setting to 0 or 1                                                   */
#define SET_LCD_EN(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_EN) : (LPC_GPIO0->FIOCLR = LCD_PIN_EN));
/* Pin LE setting to 0 or 1                                                   */
#define SET_LCD_LE(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_LE) : (LPC_GPIO0->FIOCLR = LCD_PIN_LE));
/* Pin DIR setting to 0 or 1                                                   */
#define SET_LCD_DIR(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_DIR) : (LPC_GPIO0->FIOCLR = LCD_PIN_DIR));
/* Pin CS setting to 0 or 1                                                   */
#define SET_LCD_CS(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_CS) : (LPC_GPIO0->FIOCLR = LCD_PIN_CS));
/* Pin RS setting to 0 or 1                                                   */
#define SET_LCD_RS(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_RS) : (LPC_GPIO0->FIOCLR = LCD_PIN_RS));
/* Pin WR setting to 0 or 1                                                   */
#define SET_LCD_WR(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_WR) : (LPC_GPIO0->FIOCLR = LCD_PIN_WR));
/* Pin RD setting to 0 or 1                                                   */
#define SET_LCD_RD(x)   ((x) ? (LPC_GPIO0->FIOSET = LCD_PIN_RD) : (LPC_GPIO0->FIOCLR = LCD_PIN_RD));
 
/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
//static
volatile unsigned short TextColor = LCD_COLOR_BLACK, BackColor = LCD_COLOR_WHITE;

/************************ Local auxiliary functions ***************************/
void wait_delay(int count)
{
  while(count--);
}

void wait()
{
}

static __inline void lcd_write (uint16_t word) {

	LPC_GPIO2->FIODIR |= 0x000000FF;  	//P2.0...P2.7 Output
	SET_LCD_DIR(1);		   		//Interface A->B(CPU->LCD)
	wait();
	SET_LCD_EN(0);	                        //Enable 2A->2B
	LPC_GPIO2->FIOPIN =  (word);   		//Write D0..D7
	SET_LCD_LE(1);
	wait();                           
	SET_LCD_LE(0);				//latch D0..D7
	LPC_GPIO2->FIOPIN =  (word >> 8);   	//Write D8..D15 
	wait();
}


/*******************************************************************************
* read 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline uint16_t lcd_read (void) {
	uint16_t id;
  
	LPC_GPIO2->FIODIR &= 0xFFFFFF00;          //P2.0...P2.7 as Input
	SET_LCD_DIR(0);		   		  //Interface B->A
	SET_LCD_EN(0);	                          //Enable 2B->2A
	wait_delay(50);				  //delay some times
	id = LPC_GPIO2->FIOPIN & 0x00FF;                //Read D8..D15                         
	SET_LCD_EN(1);	                                //Enable 1B->1A
	wait_delay(50);					//delay some times
	id = (id << 8) | (LPC_GPIO2->FIOPIN & 0x00ff);  //Read D0..D7                         
	SET_LCD_DIR(1);						
	return(id); 
}

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    cmd:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static __inline void SSD1298_write_command (uint8_t cmd) {

	SET_LCD_RS(0);
	SET_LCD_RD(1);
	lcd_write(cmd);
	SET_LCD_WR(0);
	wait_delay(50);
	SET_LCD_WR(1);
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    data:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void SSD1298_write_data (uint16_t data) {

	SET_LCD_RS(1);
	SET_LCD_RD(1);
	lcd_write(data);
	SET_LCD_WR(0);
	wait();
	SET_LCD_WR(1);
}

/*******************************************************************************
* Read data from LCD controller                                                *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

static __inline uint16_t SSD1298_read_data (void) {
	uint16_t val = 0;

	SET_LCD_RS(1);
	SET_LCD_WR(1);
	SET_LCD_RD(0)
	val = lcd_read();
	SET_LCD_RD(1);
	return val;
}

/*******************************************************************************
* Start of data writing to LCD controller                                      *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_start (void) {

	SET_LCD_RS(1);
}


/*******************************************************************************
* Stop of data writing to LCD controller                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) {

	SET_LCD_CS(1);
}


/*******************************************************************************
* Data writing to LCD controller                                               *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_only (uint16_t w) {

	lcd_write(w);
	SET_LCD_WR(0);
	wait();
	SET_LCD_WR(1);
}


/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

//static __inline void wr_reg (unsigned char reg, unsigned short val) {
static __inline void BSP_LCD_CtrlRegWr (unsigned char reg, unsigned short val) {

	SET_LCD_CS(0);
	SSD1298_write_command(reg);
	SSD1298_write_data(val);
	SET_LCD_CS(1);
}


/*******************************************************************************
* Read from LCD register                                                       *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from register                             *
*******************************************************************************/

static unsigned short BSP_LCD_CtrlRegRd (unsigned short reg) {
	unsigned short val = 0;

	SET_LCD_CS(0);
	SSD1298_write_command(reg);
	val = SSD1298_read_data(); 
	SET_LCD_CS(1);
	return (val);
}

/* Coordination Transform */
void swapXY(uint16_t* x, uint16_t* y)
{
	uint16_t t;
	t = *x;
	*x = *y;
	*y = LCD_HEIGHT-1 - t;
}


/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Init (void) { 
	LCD_Pin_Config();
	//delay(5);                            				 	/* Delay 50 ms           		*/

	DeviceCode = BSP_LCD_CtrlRegRd(0x00);

	if(DeviceCode==0x8999)	   //对应的驱动IC为SSD1298
	{
	//************* Start Initial Sequence **********	//values of POR
	BSP_LCD_CtrlRegWr(SSD1298_REG_OSC_ENABLE,     0x0001);	// Start internal OSC.
	BSP_LCD_CtrlRegWr(SSD1298_REG_DRIVER_OUTPUT,  			//0x3B3F);	
					SSD1298_DRIVER_OUTPUT_NOR |	// Normal Display (not reversed)
					SSD1298_DRIVER_OUTPUT_GD_G0 |	// G0,G1,..
					SSD1298_DRIVER_OUTPUT_BGR |	// BGR, not RGB
					SSD1298_DRIVER_OUTPUT_RL_SOURCE_NOSHIFT |	//both Gate and Source no shift
					SSD1298_DRIVER_OUTPUT_TB_GATE_NOSHIFT |
					SSD1298_DRIVER_OUTPUT_SM_PROGRASSIVE |		// progressive
					320-1				// to maxium display range.
					); 
	BSP_LCD_CtrlRegWr(SSD1298_REG_WAVEFORM_CTL,   0x0600);
/*
	//************* Power control setup ***********
	BSP_LCD_CtrlRegWr(SSD1298_REG_POWER_CTRL2, 0x0007); 	// Adjust VCIX2 output voltage. VRC[2:0]=0b111?
	BSP_LCD_CtrlRegWr(SSD1298_REG_POWER_CTRL3, 0x0006); 	// Set amplitude magnification of VLCD63; VRH[3:0]=0b0110
	BSP_LCD_CtrlRegWr(SSD1298_REG_POWER_CTRL4, 0x3200); 	// Set alternating amplitude of VCOM; 
	BSP_LCD_CtrlRegWr(SSD1298_REG_POWER_CTRL5, 0x00BB); 	// Set VcomH voltage; VCM[5:0]=0xBB
	BSP_LCD_CtrlRegWr(SSD1298_REG_POWER_CTRL1, 0x6A64); 	// Step-up factor/cycle setting  *BT[2:0]=0b101

	// ----------- Adjust the Gamma Curve ----------//
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL01, 0x0000);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL02, 0x0706);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL03, 0x0206);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL04, 0x0300);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL05, 0x0002);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL06, 0x0000);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL07, 0x0707);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL08, 0x0200);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL09, 0x0908);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GAMMA_CTRL10, 0x0F0D);

		//************* Special command ************** // values of all POR
	BSP_LCD_CtrlRegWr(SSD1298_REG_VCOM_OTP1, 0x0006); 	// 0x28, Enable test command
	BSP_LCD_CtrlRegWr(SSD1298_REG_RAM_SPEED, 0x12EB); 	// 0x2F, RAM speed tuning
	BSP_LCD_CtrlRegWr(SSD1298_REG_BANDGAP, 0x7000); 	// 0x26, Internal Bandgap strength
	BSP_LCD_CtrlRegWr(SSD1298_REG_VCOM_STRENGTH, 0xB0E3); 	// 0x20, Internal Vcom strength
	BSP_LCD_CtrlRegWr(SSD1298_REG_VCOMHL_TIME, 0x0044); 	// 0x27, Internal Vcomh/VcomL timing
	BSP_LCD_CtrlRegWr(SSD1298_REG_VCOM_CHARGING, 0x7E45); 	// 0x2E, VCOM charge sharing time  
*/

		//************ RAM position control ********** // Values of POR
	BSP_LCD_CtrlRegWr(SSD1298_REG_GATE_SCAN, 0x0000); 	// Gate scan position start at G0.
	BSP_LCD_CtrlRegWr(SSD1298_REG_H_RAM_START, 0xEF00); 	// Horizontal RAM address position
	BSP_LCD_CtrlRegWr(SSD1298_REG_V_RAM_START, 0x0000); 	// Vertical RAM address start position
	BSP_LCD_CtrlRegWr(SSD1298_REG_V_RAM_END, 0x013F); 	// Vertical RAM address end position

//************* Turn On display ******************/
	BSP_LCD_CtrlRegWr(SSD1298_REG_SLEEP_MODE, 0x0000); 	// Sleep mode off.
	
	// Entry mode setup. 262K type B, take care on the data bus with 16bit only . 0x6878 

#ifdef _LCD_PORTRAIT
	BSP_LCD_CtrlRegWr(SSD1298_REG_ENTRY_MODE, (0x0200 | 
					SSD1298_DISP_CTRL_DFM_65K | 
					SSD1298_DISP_CTRL_TYPE_A | 	// no effect in 65K mode 
					SSD1298_DISP_CTRL_ID1_V_INC | SSD1298_DISP_CTRL_ID0_H_INC | 
					SSD1298_DISP_CTRL_AM_H) ); 	
#endif
#ifdef _LCD_LANDSCAPE
	BSP_LCD_CtrlRegWr(SSD1298_REG_ENTRY_MODE, (0x0200 | 
	                                SSD1298_DISP_CTRL_DFM_65K | 
					SSD1298_DISP_CTRL_TYPE_A | 	// no effect in 65K mode 
					SSD1298_DISP_CTRL_ID1_V_DEC | SSD1298_DISP_CTRL_ID0_H_INC | 
					SSD1298_DISP_CTRL_AM_V) ); 	
#endif

	BSP_LCD_CtrlRegWr(SSD1298_REG_DISPLAY_CTRL, 0x0033); 	// Display ON	*/
	}
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
void LCD_SetWindow (unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end) {
	BSP_LCD_CtrlRegWr(SSD1298_REG_H_RAM_START, x|((x_end-1)<<8));
	BSP_LCD_CtrlRegWr(SSD1298_REG_V_RAM_START, y);
	BSP_LCD_CtrlRegWr(SSD1298_REG_V_RAM_END, y_end-1);
}

/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetTextColor (unsigned short color) {
	TextColor = color;
}

/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetBackColor (unsigned short color) {
	BackColor = color;
}

/*
*********************************************************************************************************
* Description: 	Setting the coordinate of cursor
* Arguments  : 	(Xpos , Ypos) the coordinate
* Returns    : 	None
*********************************************************************************************************
*/
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{
	BSP_LCD_CtrlRegWr(SSD1298_REG_GDDRAM_X, Xpos);
	BSP_LCD_CtrlRegWr(SSD1298_REG_GDDRAM_Y, Ypos);
}

/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void LCD_ClearScreen (unsigned short color) {
	unsigned int   i;

	LCD_SetCursor(0,0);
	SET_LCD_CS(0);
	SSD1298_write_command(SSD1298_REG_RAM_DATA);
	wr_dat_start();
	for(i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i++)
		wr_dat_only(color);
	wr_dat_stop();
}

/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color){
#ifdef _LCD_LANDSCAPE
	swapXY(&x, &y);
#endif
	LCD_SetCursor(x, y);
	SET_LCD_CS(0)
	SSD1298_write_command(SSD1298_REG_RAM_DATA);
	wr_dat_start();
	wr_dat_only(color);
	wr_dat_stop();	
}

/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:      ln:       line number, in character unit                   *
*                   col:      column number,in character unit                  *
*                   c:        ascii character                                  *
*   Return:                                                                    *
*******************************************************************************/

void LCD_DisplayChar (uint16_t ln, uint16_t col, unsigned char c) {

	int i, j;
	unsigned char *p = (unsigned char*) fonts + (c - ' ')*7;
	uint16_t b;
/*	
	if(ln > HEIGHT / FONT_HEIGHT || col > WIDTH / FONT_WIDTH) return;
	ln *= FONT_HEIGHT;
	col *= FONT_WIDTH;
*/	
#ifdef _LCD_LANDSCAPE
	swapXY((uint16_t*)&col, (uint16_t*) &ln);
#endif

	for (i=0; i<FONT_HEIGHT-1; i++){

#ifdef _LCD_LANDSCAPE
		LCD_SetCursor(col++, ln);
#endif
		
#ifdef _LCD_PORTRAIT
		LCD_SetCursor(col, ln++);
#endif		
		SET_LCD_CS(0)
		SSD1298_write_command(SSD1298_REG_RAM_DATA);
		wr_dat_start();
		for (j=0; j<FONT_WIDTH; j++){
			
			if (*p & ((0x80 >> j)))
				wr_dat_only(TextColor);
			else{
				//b = SSD1298_read_data(); how to display on background ?
				wr_dat_only(BackColor);
			}
		}
		p++;
		wr_dat_stop();
	}
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   s:        pointer to string                                *
*   Return:                                                                    *
*******************************************************************************/

void LCD_DisplayString (unsigned int line, unsigned int column, unsigned char *s) {
	while (*s) {
		LCD_DisplayChar(line, column, *s++);
		column += FONT_WIDTH;
	}
}

void LCD_ShowBmp (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bmp) {
	unsigned int    j;
	uint16_t *ptr = bmp;

#ifdef _LCD_PORTRAIT
	LCD_SetWindow(x, y, x+w, y+h);  
#endif
	
#ifdef _LCD_LANDSCAPE
	swapXY(&x, &y);
	LCD_SetWindow(x, y-w, x+h, y);  
#endif

	LCD_SetCursor(x, y);
	
	SET_LCD_CS(0)
	SSD1298_write_command(SSD1298_REG_RAM_DATA);
	wr_dat_start();

	for (j = 0;  j< w*h; j++)
		wr_dat_only(*ptr++);
	
	wr_dat_stop();
	LCD_SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);  
}

/******************************************************************************/

//以下为自己添加的显示16*24字函数（字符）
void LCD_DisplayChar2 (uint16_t ln, uint16_t col, unsigned char c) {

	int i, j;
	//unsigned char *p = (unsigned char*) fonts + (c - ' ')*7;
	uint16_t *p = (uint16_t *) Font_24x16 + (c - ' ') * (FONT_HEIGHT2);
	uint16_t b;
/*	
	if(ln > HEIGHT / FONT_HEIGHT2 || col > WIDTH / FONT_WIDTH2) return;
	ln *= FONT_HEIGHT2;
	col *= FONT_WIDTH2;
*/	
#ifdef _LCD_LANDSCAPE
	swapXY((uint16_t*)&col, (uint16_t*) &ln);
#endif

	for (i=0; i<FONT_HEIGHT2; i++){

#ifdef _LCD_LANDSCAPE
		LCD_SetCursor(col++, ln);
#endif
		
#ifdef _LCD_PORTRAIT
		LCD_SetCursor(col, ln++);
#endif		
		SET_LCD_CS(0)
		SSD1298_write_command(SSD1298_REG_RAM_DATA);
		wr_dat_start();
		for (j=0; j<FONT_WIDTH2; j++){
			if (*p & (0x01 << j) )
			//if (*p & ((0x80 >> j)))
				wr_dat_only(TextColor);
			else{
				//b = SSD1298_read_data(); how to display on background ?
				wr_dat_only(BackColor);
			}
		}
		p++;
		wr_dat_stop();
	}
}

//以下为自己添加的显示16*24字函数（字符串）
void LCD_DisplayString2 (unsigned int line, unsigned int column, unsigned char *s) {
	while (*s) {
		LCD_DisplayChar2(line, column, *s++);
		column += FONT_WIDTH2;
	}
}









//部分清屏函数，用于清空日历部分，以便重新打印
void Clear( void )
{
	int i, j;
	for( i=84; i<319; i++ )
	{
		for( j=40; j<207; j++ )
		{
			LCD_DrawPixel( i, j, BackColor );
		}
	}
}


//部分清屏函数，输入起始坐标、长、宽、颜色
// void Clear( uint16_t x, uint16_t y, uint16_t len, uint16_t high, uint16_t color )
// {
// 	int i, j;
// 	for( i=x; i<x+len; i++ )
// 	{
// 		for( j=y; j<y+high; j++ )
// 		{
// 			LCD_DrawPixel( i, j, color );
// 		}
// 	}
// }

//画横线函数，输入起始坐标、长度、颜色
void LineX( int x, int y, int len, uint16_t color )
{
	int i;
	for( i=x; i<x+len; i++ )
		LCD_DrawPixel( i, y, color );
}

//画竖线函数，输入起始坐标、长度、颜色
void LineY( int x, int y, int len, uint16_t color )
{
	int i;
	for( i=y; i<y+len; i++ )
		LCD_DrawPixel( x, i, color );
}

//画矩形函数，输入起始坐标、长、宽、颜色
void Square( int x, int y, int len, int high, uint16_t color )
{
	int i;
	if( len==0 || high==0 )
		return;
	for( i=x; i<x+len; i++ ) //画两条竖线
	{
		LCD_DrawPixel( i, y, color );
		LCD_DrawPixel( i, y+high-1, color );
	}
	for( i=y; i<y+high; i++ ) //画两条横线
	{
		LCD_DrawPixel( x, i, color );
		LCD_DrawPixel( x+len-1, i, color );
	}
}

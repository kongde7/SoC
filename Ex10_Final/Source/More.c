#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "lpc1768_rtc.h"
#include "joystick.h"
#include "24LC02.h"
#include "LCD.h"
#include "User.h"

extern volatile unsigned short TextColor, BackColor;
extern volatile uint16_t X, Y, XL, YL;
extern char *LOCATION;
extern char *DOCK;
static uint8_t ON;

void More( uint8_t value ) //More页有限状态机
{
	switch( value )
	{
		case 0: //输入0，初始化More页
		{
			XL = 0;
			DOCK = "More";
			LOCATION = "More";
			
			LCD_ClearScreen( BackColor ); //清屏
			Always(); //常显部分
			break;
		}
		case KEY_VALUE_UP: //按上键，移动光标
		{
			if( strcmp(LOCATION,"More")==0 )
			{
				LOCATION = "mode4";
			}
			else if( strcmp(LOCATION,"mode4")==0 )
			{
				LOCATION = "mode3";
			}
			else if( strcmp(LOCATION,"mode3")==0 )
			{
				LOCATION = "mode2";
			}
			else if( strcmp(LOCATION,"mode2")==0 )
			{
				LOCATION = "mode1";
			}
			break;
		}
		case KEY_VALUE_DOWN: //按下键，移动光标
		{
			if( strcmp(LOCATION,"mode1")==0 )
			{
				LOCATION = "mode2";
			}
			else if( strcmp(LOCATION,"mode2")==0 )
			{
				LOCATION = "mode3";
			}
			else if( strcmp(LOCATION,"mode3")==0 )
			{
				LOCATION = "mode4";
			}
			else if( strcmp(LOCATION,"mode4")==0 )
			{
				LOCATION = "More";
			}
			break;
		}
		case KEY_VALUE_LEFT: //按左键，换至Time页
		{
			if( strcmp(LOCATION,"More")==0 )
			{
				Time(0);
				return;
			}
		}
		case KEY_VALUE_RIGHT: //按右键，无动作
		{
			break;
		}
		case KEY_VALUE_ENTER: //按确认键
		{
			if( strcmp(LOCATION,"mode1")==0 ) //夜间模式按钮
			{
				ON = Night(ON);
				LCD_ClearScreen( BackColor ); //清屏
				Always(); //常显部分
			}
			else if( strcmp(LOCATION,"mode2")==0 ) //标记明日按钮
			{
				Nextday();
			}
			else if( strcmp(LOCATION,"mode3")==0 ) //恢复上个日期按钮
			{
				Recover();
			}
			else if( strcmp(LOCATION,"mode4")==0 ) //恢复出厂设置按钮
			{
				Factory();
			}
			break;
		}
	}
	Gui_More(); //绘制界面
}

void Gui_More( void ) //More页界面
{
	if( ON==1 )
		LCD_DisplayString2( 48, 32, "Night Mode    On" ); //显示夜间模式开
	else LCD_DisplayString2( 48, 32, "Night Mode   Off" ); //显示夜间模式关
	LCD_DisplayString2( 88, 32, "Nextday     Mark" ); //显示标记明日按钮文字
	LCD_DisplayString2( 128, 32, "Recover     Date" ); //显示恢复上个日期按钮文字
	LCD_DisplayString2( 168, 32, "Factory    Reset" ); //显示恢复出厂设置按钮文字
	
	Square( 32-2, 48-2-1, 256+4, 24+4, TextColor ); //显示夜间模式外框
	Square( 32-2, 88-2-1, 256+4, 24+4, TextColor ); //显示标记明日按钮外框
	Square( 32-2, 128-2-1, 256+4, 24+4, TextColor ); //显示恢复上个日期按钮外框
	Square( 32-2, 168-2-1, 256+4, 24+4, TextColor ); //显示恢复出厂设置按钮外框
	
	if( strcmp(LOCATION,"mode1")==0 )
	{
		X = 32-2;
		Y = 48-2-1;
		XL = 256+4;
		YL = 24+4;
	}
	else if( strcmp(LOCATION,"mode2")==0 )
	{
		X = 32-2;
		Y = 88-2-1;
		XL = 256+4;
		YL = 24+4;
	}
	else if( strcmp(LOCATION,"mode3")==0 )
	{
		X = 32-2;
		Y = 128-2-1;
		XL = 256+4;
		YL = 24+4;
	}
	else if( strcmp(LOCATION,"mode4")==0 )
	{
		X = 32-2;
		Y = 168-2-1;
		XL = 256+4;
		YL = 24+4;
	}
	else XL=0;
}

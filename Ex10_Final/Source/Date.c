#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "24LC02.h"
#include "lpc1768_rtc.h"
#include "joystick.h"
#include "LCD.h"
#include "User.h"

extern volatile unsigned short TextColor, BackColor;
volatile uint32_t YEAR, MON, DAY;
extern volatile uint16_t X, Y, XL, YL;
extern char *LOCATION;
extern char *DOCK;

void Date( uint8_t value ) //Date页有限状态机
{
	char word[20];
	RTCTime LocalTime;
	
	switch( value )
	{
		case 0: //输入0，初始化Date页
		{
			XL = 0;
			DOCK = "Date";
			LOCATION = "Date";
			
			LCD_ClearScreen( BackColor ); //清屏
			Always(); //常显部分
			
			LocalTime = RTC_GetTime();
			YEAR = LocalTime.RTC_Year; //读取当前年、月、日
			MON = LocalTime.RTC_Mon;
			DAY = LocalTime.RTC_Mday;
			
			LCD_DisplayString2( 88, 256, "OK" ); //显示OK按钮
			LCD_DisplayString2( 128, 256, "NO" ); //显示取消按钮
			break;
		}
		case KEY_VALUE_UP: //按上键
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				LOCATION = "year";
			}
			else if( strcmp(LOCATION,"year")==0 )
			{
				YEAR = ( YEAR + 1 ) % 2100; //年份有效值1901~2099
				if( YEAR==0 )
					YEAR = 1901; //防止年份小于1901
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				MON = ( MON + 1 ) % 13; //月份有效值1~12
				if( MON==0 )
					MON = 1; //防止月份为0
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				DAY = ( DAY + 1 ) % 32; //日期有效值1~31
				if( DAY==0 )
					DAY = 1; //防止日期为0
				if( DAY > HowMany(YEAR,MON) )
					DAY = 1; //防止日期超过当月最大值
			}
			else if( strcmp(LOCATION,"no")==0 )
			{
				LOCATION = "ok";
			}
			break;
		}
		case KEY_VALUE_DOWN: //按下键
		{
			if( strcmp(LOCATION,"year")==0 )
			{
				YEAR-- ;
				if( YEAR==1900 ) //防止年份小于1901
					YEAR = 2099;
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				MON--;
				if( MON==0 ) //防止月份为0
					MON = 12;
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				DAY--;
				if( DAY==0 ) //防止日期为0
					DAY = HowMany(YEAR,MON);
			}
			else if( LOCATION=="ok" )
			{
				LOCATION = "no";
			}
			break;
		}
		case KEY_VALUE_LEFT: //按左键
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				Home(0); //Date页面左边为Home页面
				return;
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				LOCATION = "year"; //月左边为年
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				LOCATION = "mon"; //日左边为月
			}
			else if( LOCATION=="ok" )
			{
				LOCATION = "day"; //OK按钮左边为日
			}
			break;
		}
		case KEY_VALUE_RIGHT: //按右键
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				Time(0); //Date页面右边为Time页面
				return;
			}
			else if( strcmp(LOCATION,"year")==0 )
			{
				LOCATION = "mon"; //年右边为月
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				LOCATION = "day"; //月右边为日
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				LOCATION = "ok"; //日右边为OK按钮
			}
			break;
		}
		case KEY_VALUE_ENTER: //按确认键
		{
			if( strcmp(LOCATION,"ok")==0 ) //按OK按钮
			{
				LocalTime = RTC_GetTime();
				sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
				E2PROM_WriteSeq(0x0,word, 8);
				LocalTime.RTC_Year = YEAR; //写入当前年、月、日
				LocalTime.RTC_Mon = MON;
				LocalTime.RTC_Mday = DAY;
				RTC_SetTime( LocalTime );
				Home(0); //返回主页
			}
			else if( strcmp(LOCATION,"no")==0 ) //按取消按钮
			{
				Date(0); //返回当前页
			}
			return;
		}
	}
	while( DAY > HowMany(YEAR,MON) ) //防止调整年、月后，日溢出
		DAY--;
	
	Gui_Date(); //绘制界面
}

void Gui_Date( void ) //Date页界面
{
	char word[20];
	sprintf( word, "%04d-%02d-%02d", YEAR, MON, DAY );
	LCD_DisplayString2( 88, 32, word ); //显示调整后的年、月、日
	
	Square( 32-2, 88-1-2, 64+2+2, 24+2, TextColor ); //显示年外框
	Square( 112-2, 88-1-2, 32+2+2, 24+2, TextColor ); //显示月外框
	Square( 160-2, 88-1-2, 32+2+2, 24+2, TextColor ); //显示日外框
	Square( 256-2, 88-1-2, 32+2+2, 24+2, TextColor ); //显示OK按钮外框
	Square( 256-2, 128-1-2, 32+2+2, 24+2, TextColor ); //显示取消按钮外框
	
	if( strcmp(LOCATION,"year")==0 ) //年闪烁光标
	{
		X = 32-2;
		Y = 88-1-2;
		XL = 64+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"mon")==0 ) //月闪烁光标
	{
		X = 112-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"day")==0 ) //日闪烁光标
	{
		X = 160-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"ok")==0 ) //确认键闪烁光标
	{
		X = 256-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"no")==0 ) //取消键闪烁光标
	{
		X = 256-2;
		Y = 128-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else XL=0;
}

//输入年和月，计算该月有几天
uint8_t HowMany( uint32_t year, uint32_t mon )
{
	uint8_t num[]={31,28,31,30,31,30,31,31,30,31,30,31};
	if( year%4==0 || year%100 && year%400==0 ) //判断闰年
		num[1] = 29;
	return num[mon-1];
}

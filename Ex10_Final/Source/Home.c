#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "lpc1768_rtc.h"
#include "joystick.h"
#include "24LC02.h"
#include "LCD.h"
#include "User.h"

extern volatile unsigned short TextColor, BackColor;
volatile uint32_t YEARS, MONS;
extern volatile uint16_t X, Y, XL, YL;
extern char *LOCATION;
extern char *DOCK;

void Home( uint8_t value ) //Home页有限状态机
{
	RTCTime LocalTime;
	
	switch( value )
	{
		case 0: //输入0，初始化Home页
		{
			XL = 0;
			DOCK = "Home";
			LOCATION = "Home";
			
			LCD_ClearScreen( BackColor ); //清屏
			Always(); //常显部分
			
			LocalTime = RTC_GetTime();
			YEARS = LocalTime.RTC_Year; //获取当前年、月
			MONS = LocalTime.RTC_Mon;
			Calendar( YEARS, MONS ); //打印日历
			break;
		}
		case KEY_VALUE_UP: //按上键，移动光标
		{
			if( strcmp(LOCATION,"Home")==0 )
			{
				LOCATION = "next";
			}
			else if( strcmp(LOCATION,"next")==0 )
			{
				LOCATION = "last";
			}
			else if( strcmp(LOCATION,"last")==0 )
			{
				LOCATION = "curr";
			}
			break;
		}
		case KEY_VALUE_DOWN: //按下键，移动光标
		{
			if( strcmp(LOCATION,"curr")==0 )
			{
				LOCATION = "last";
			}
			else if( strcmp(LOCATION,"last")==0 )
			{
				LOCATION = "next";
			}
			else if( strcmp(LOCATION,"next")==0 )
			{
				LOCATION = "Home";
			}
			break;
		}
		case KEY_VALUE_LEFT: //按左键，无动作
		{
			break;
		}
		case KEY_VALUE_RIGHT: //按右键，换至Date页
		{
			if( strcmp(LOCATION,"Home")==0 )
				Date(0);
			return;
		}
		case KEY_VALUE_ENTER: //按确认键
		{
			Clear(); //清屏
			if( strcmp(LOCATION,"curr")==0 ) //当前月按钮
			{
				LocalTime = RTC_GetTime();
				YEARS = LocalTime.RTC_Year;
				MONS = LocalTime.RTC_Mon;
			}
			else if( strcmp(LOCATION,"last")==0 ) //上一月按钮
			{
				MONS--;
				if( MONS==0 )
				{
					YEARS--; //减至0月，即上一年
					MONS = 12;
				}
			}
			else if( strcmp(LOCATION,"next")==0 ) //下一月按钮
			{
				MONS++;
				if( MONS==13 )
				{
					YEARS++; //加至13月，即下一年
					MONS = 1;
				}
			}
			Calendar( YEARS, MONS ); //刷新日历
			break;
		}
	}
	Gui_Home(); //绘制界面
}

void Gui_Home( void ) //Home页界面
{
	char word[20];
	
	if( strcmp(LOCATION,"curr")==0 )
	{
		X = 18-3;
		Y = 136-3;
		XL = 60+6;
		YL = 8+6;
	}
	else if( strcmp(LOCATION,"last")==0 )
	{
		X = 18-3;
		Y = 160-3;
		XL = 60+6;
		YL = 8+6;
	}
	else if( strcmp(LOCATION,"next")==0 )
	{
		X = 18-3;
		Y = 184-3;
		XL = 60+6;
		YL = 8+6;
	}
	else XL=0;
	
	sprintf( word, "Year : %04d", YEARS );
	LCD_DisplayString( 88, 18, word ); //显示当前日历是几年
	
	sprintf( word, "Month : %02d", MONS );
	LCD_DisplayString( 112, 18, word ); //显示当前日历是几月
	
	LCD_DisplayString( 136, 18, "Curr Month" ); //显示当前月按钮文字
	LCD_DisplayString( 160, 18, "Last Month" ); //显示上月按钮文字
	LCD_DisplayString( 184, 18, "Next Month" ); //显示下月按钮文字
	Square( 18-3, 136-3, 60+6, 8+6, TextColor ); //显示当前月按钮外框
	Square( 18-3, 160-3, 60+6, 8+6, TextColor ); //显示上月按钮外框
	Square( 18-3, 184-3, 60+6, 8+6, TextColor ); //显示下月按钮外框
}

uint32_t Mark( uint32_t year, uint32_t month ) //输入年、月，检测是否有重要的日子
{
	uint32_t y, m, d;
	char word[20];
	
	E2PROM_ReadSeq( 0x01, word, 8 ); //读取E2PROM里的储存的重要年、月、日
	sscanf( word, "%04d%02d%02d", &y, &m, &d );
	
	if( year==y && month==m ) //如果年、月一致
		return d; //返回当月重要的日子
	else return 0; //否则返回0
}

void Calendar( uint32_t year, uint32_t month ) //输入年、月，打印日历
{
	uint32_t day, temp, first;
	uint16_t i, x, y, trim=0, num[]={31,28,31,30,31,30,31,31,30,31,30,31};
	char word[8];
	unsigned short tempcolor;
	
	day = Mark( year, month ); //是否有重要的日子需要标记
	
	if( year%4==0 || year%100 && year%400==0 ) //判断闰年
		num[1] = 29;
	
	first = weekday( year, month, 1); //计算该月1号是星期几
	
	x = 110 + 30 * first; //1号起始横坐标
	y = 40 + 24; //1号起始纵坐标
	
	tempcolor = TextColor;
	LCD_SetTextColor( LCD_COLOR_BLUE );
	LCD_DisplayString( 40, 104, "                              Sat" ); //蓝色显示星期六
	LCD_SetTextColor( tempcolor );
	LCD_DisplayString( 40, 104, "     Mon  Tur  Wed  Thu  Fri" ); //黑色显示星期一到五
	LCD_SetTextColor( LCD_COLOR_RED );
	LCD_DisplayString( 40, 104, "Sun" ); //红色显示星期日
	
	for( temp=first, i=1; i<=num[month-1]; i++ )
	{
		LCD_SetTextColor( tempcolor );
		if( i==10 ) //当数字超过两位数时，微调距离，使得居中显示
		{
			trim = 3;
		}
		if( i==day && i<10 )
			Square( x-trim-13, y-5, 32, 8+10, TextColor ); //在重要日子上标记小方框
		else if( i==day && i>=10 )
			Square( x-trim-12, y-5, 37+trim, 8+10, TextColor ); //在重要日子上标记大方框
		
		switch( temp )
		{
			case 0: //星期日
				sprintf( word, "%d  ", i );
				LCD_SetTextColor( LCD_COLOR_RED );
				LCD_DisplayString( y, x, word ); //红色显示
				x = 104 - trim + 36; //横坐标右移1个单位
				temp++;
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5: //星期一到五
				if( i==10 )
					x = x - trim;
				sprintf( word, "%d  ", i );
				LCD_SetTextColor( tempcolor );
				LCD_DisplayString( y, x, word ); //黑色显示
				x = x + 30; //横坐标右移1个单位
				temp++;
				break;
			case 6: //星期六
				if( i==10 )
					x = x - trim;
				sprintf( word, "%d", i );
				LCD_SetTextColor( LCD_COLOR_BLUE );
				LCD_DisplayString( y, x, word ); //蓝色显示
				x = 110 - trim ; //横坐标回到最左端
				y = y + 24; //纵坐标下移1个单位
				temp = 0;
				break;
		}
	}
	LCD_SetTextColor( tempcolor );
}

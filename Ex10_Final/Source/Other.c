#include <stdio.h>
#include "lpc17xx.h"
#include "24LC02.h"
#include "lpc1768_rtc.h"
#include "LCD.h"
#include "User.h"

uint8_t Night( uint8_t on ) //夜间模式
{
	if( on==1 )
	{
		LCD_SetTextColor (LCD_COLOR_BLACK);
		LCD_SetBackColor (LCD_COLOR_WHITE);
		return 0;
	}
	else
	{
		LCD_SetTextColor (LCD_COLOR_WHITE);
		LCD_SetBackColor (LCD_COLOR_BLACK);
		return 1;
	}
}

void Factory( void ) //恢复出厂设置
{
	char word[20];
	RTCTime LocalTime;
	
	LocalTime.RTC_Year = 2019;
	LocalTime.RTC_Mon = 5;
	LocalTime.RTC_Mday = 31;
	LocalTime.RTC_Hour = 23;
	LocalTime.RTC_Min = 59;
	LocalTime.RTC_Sec = 50;
	
	E2PROM_WriteSeq( 0x01, word, 8 ); //将修改前日期写入E2PROM备份
	sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	RTC_SetTime( LocalTime ); //设置新日期
}

void Recover( void ) //恢复到上一个日期
{
	char word[20], temp[20];
	RTCTime LocalTime;
	
	E2PROM_ReadSeq( 0x00, word, 8 ); //读取E2PROM里的年、月、日
	LocalTime = RTC_GetTime(); //获取当前时间
	sprintf( temp, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	sscanf( word, "%04d%02d%02d", &LocalTime.RTC_Year, &LocalTime.RTC_Mon, &LocalTime.RTC_Mday );
	RTC_SetTime( LocalTime ); //设置新日期
	E2PROM_WriteSeq( 0x00, temp, 8 ); //将修改前日期写入E2PROM备份
}

void Nextday( void ) //标记明天是重要日子
{
	char word[20];
	RTCTime LocalTime;
	
	LocalTime = RTC_GetTime(); //获取当前时间
	
	if( LocalTime.RTC_Mday+1 > HowMany( LocalTime.RTC_Year, LocalTime.RTC_Mon ) )
	{
		if( LocalTime.RTC_Mon+1 > 12)
			LocalTime.RTC_Year++; //若在12月31日进行标记，则应标记明年1月1日
		else LocalTime.RTC_Mon++;
	}
	else LocalTime.RTC_Mday++;
	
	sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	E2PROM_WriteSeq( 0x01, word, 8 ); //将要标记的日期写入E2PROM
}

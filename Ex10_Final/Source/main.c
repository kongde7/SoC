#include <stdio.h>
#include <string.h>
#include "lpc17xx.h"
#include "bmboard.h"
#include "lpc1768_i2c.h"
#include "lpc1768_rtc.h"
#include "lpc1768_timer.h"
#include "joystick.h"
#include "LCD.h"
#include "User.h"

extern volatile unsigned short TextColor, BackColor;
extern volatile uint32_t DelayCounter;
volatile uint16_t X=0, Y=0, XL=0, YL=0;
char *LOCATION="Home";
char *DOCK="Home";

int main( void )
{
	uint8_t temp=0;
	uint32_t i, mon;
	char time[10], date[12];
	
	RTCTime LocalTime; //定义时间结构体
	KEY_TypeDef key; //定义五向按键结构体
	I2C_InitTypeDef I2C_InitStruct; //定义I2C结构体
	
	DelayCounter = 300;
	Timer_Init(0); //初始化定时器
	
	RTC_Init(); //初始化实时时钟
	
	Joystick_Config(); //初始化有限状态机
	
	I2C_InitStruct.Mode = I2C_MASTER;
	I2C_InitStruct.ClockRate = 400000;
	I2C_InitStruct.InterruptMode = I2C_INTERRUPT_MODE;
	I2C0_Init( &I2C_InitStruct ); //初始化I2C
	
	LCD_Init(); //初始化显示屏
	
	LocalTime = RTC_GetTime();
	mon = LocalTime.RTC_Mon; //获取当前月份
	Home(0); //默认进入主页
	
	while(1)
	{
		i = 10000;
		while( i-- ) //五向按键读取要延迟一段时间
		{
			if( DelayCounter>300 ) //光标每300ms黑白交替闪
			{
				if( temp==0 )
				{
					Square( X, Y, XL, YL, TextColor ); //白色光标
					temp++;
				}
				else if( temp==1 )
				{
					Square( X, Y, XL, YL, BackColor ); //黑色光标
					temp = 0;
				}
				DelayCounter = 300;
			}

			if( SecondInt ) //发生秒中断
			{
				LocalTime = RTC_GetTime();
				if( mon!=LocalTime.RTC_Mon && strcmp(DOCK,"Home")==0 ) //如果月份发生变动，刷新日历
					Home(0);
				mon = LocalTime.RTC_Mon;
				
				sprintf( date, "%04d-%02d-%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
				LCD_DisplayString2( 6, 10, date ); //顶栏常显日期
				
				sprintf( time, "%02d:%02d:%02d", LocalTime.RTC_Hour, LocalTime.RTC_Min, LocalTime.RTC_Sec );
				LCD_DisplayString2( 6, 182, time ); //顶栏常显时间
				
				SecondInt = 0;
			}
		}

		key = GetKey(); //获取五向按键状态

		if( key.KeyValue && key.Pressed ) //当按键被按下时
		{
			if( strcmp(DOCK,"Home")==0 ) //若在Home页
				Home( key.KeyValue ); //向Home页传入键值
			
			else if( strcmp(DOCK,"Date")==0 ) //若在Date页
				Date( key.KeyValue ); //向Date页传入键值
			
			else if( strcmp(DOCK,"Time")==0 ) //若在Time页
				Time( key.KeyValue ); //向Time页传入键值
			
			else if( strcmp(DOCK,"More")==0 ) //若在More页
				More( key.KeyValue ); //向More页传入键值
		}
	}
}

#include "lpc17xx.h"
#include "lpc1768_rtc.h"
#include "lpc1768_uart.h"

// 自行添加2个
#include <stdio.h>
#include "LCD.h"

volatile uint32_t Alarm_Ring = 0;
volatile uint32_t SecondInt = 0;

#define PCONP_PCRTC (0x01UL << 9)

void RTC_Init( void )
{
	LPC_SC->PCONP |= PCONP_PCRTC;
	if ( LPC_RTC->RTC_AUX & (0x1<<4) )
	{
		LPC_RTC->RTC_AUX |= (0x1<<4);
	}

	LPC_RTC->AMR = 0xF8;
	LPC_RTC->CIIR = CIIR_IMSEC;
	LPC_RTC->CCR = 0;
	NVIC_EnableIRQ(RTC_IRQn);
	RTC_Start();
	return;
}

void RTC_Start( void )
{
	LPC_RTC->CCR |= CCR_CLKEN;
	LPC_RTC->ILR = ILR_RTCCIF;
	return;
}

void RTC_Stop( void )
{
	LPC_RTC->CCR &= ~CCR_CLKEN;
	return;
}

RTCTime RTC_GetTime( void )
{
	RTCTime LocalTime;
	LocalTime.RTC_Sec = LPC_RTC->SEC;
	LocalTime.RTC_Min = LPC_RTC->MIN;
	LocalTime.RTC_Hour = LPC_RTC->HOUR;
	LocalTime.RTC_Mday = LPC_RTC->DOM;
	LocalTime.RTC_Wday = LPC_RTC->DOW;
	LocalTime.RTC_Yday = LPC_RTC->DOY;
	LocalTime.RTC_Mon = LPC_RTC->MONTH;
	LocalTime.RTC_Year = LPC_RTC->YEAR;
	return( LocalTime );
}

void RTC_IRQHandler( void )
{
	uint32_t ilr = LPC_RTC->ILR;
	if (ilr & ILR_RTCCIF)
	{
		LPC_RTC->ILR |= ILR_RTCCIF;
		SecondInt = 1;
	}
	else if( ilr & ILR_RTCALF )
	{
		LPC_RTC->ILR |= ILR_RTCALF;
		Alarm_Ring = 1;
	}
	return;
}

void RTC_SetTime( RTCTime Time )
{
	LPC_RTC->SEC = Time.RTC_Sec;
	LPC_RTC->MIN = Time.RTC_Min;
	LPC_RTC->HOUR = Time.RTC_Hour;
	LPC_RTC->DOM = Time.RTC_Mday;
	LPC_RTC->DOW = weekday(Time.RTC_Year, Time.RTC_Mon, Time.RTC_Mday);
	LPC_RTC->DOY = yearday(Time.RTC_Year, Time.RTC_Mon, Time.RTC_Mday);
	LPC_RTC->MONTH = Time.RTC_Mon;
	LPC_RTC->YEAR = Time.RTC_Year;
	return;
}





/* calculate the week day */
/* input: y-m-d
   return: weekday, val from 0 to 6 for Mon to Sun
*/
// int weekday(int y, int m, int d)
// {
// 	const char mon_base[][12]={ {0, 3, 3, 6, 1, 4, 0, 3, 5, 0, 3, 5}, \
//                                     {0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6} }; //1char改int
// 	char wd; //1char改int

// 	if(y % 4)		// Normal year
// 	{
// 		wd = y + y/4 + y/400 - y/100 - 1 + mon_base[0][m-1] + d;
// 	}
// 	else		// normal year
// 	{
// 		wd = y + y/4 + y/400 - y/100 - 2 + mon_base[1][m-1] + d;
// 	}
// 	return (wd % 7);
// }

int weekday( int y, int m, int d ) //输入年月日，返回0~6对应星期日~六
{
	if( m==1 || m==2 )
	{
		y--;
		m = m + 12;
	}
	return ( d + 1 + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400 ) % 7;
}

/* calculate the year day */
/* input: y-m-d
   return: year day, val from 1.
*/
int yearday(int y, int m, int d)
{
	const char mon_base[]={31,28,31,30,31,30,31,31,30,31,30,31}; //1月30天改31天 //1char改int
	char day=0, i; //1char改int

	for(i=0;i<m-1;i++)
		day += mon_base[i];
	day += d;

	if(!(y % 4) && m>2) return day+1;
	else return day;	
}

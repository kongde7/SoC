#include <stdio.h>
#include "lpc17xx.h"
#include "24LC02.h"
#include "lpc1768_rtc.h"
#include "LCD.h"
#include "User.h"

uint8_t Night( uint8_t on ) //ҹ��ģʽ
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

void Factory( void ) //�ָ���������
{
	char word[20];
	RTCTime LocalTime;
	
	LocalTime.RTC_Year = 2019;
	LocalTime.RTC_Mon = 5;
	LocalTime.RTC_Mday = 31;
	LocalTime.RTC_Hour = 23;
	LocalTime.RTC_Min = 59;
	LocalTime.RTC_Sec = 50;
	
	E2PROM_WriteSeq( 0x01, word, 8 ); //���޸�ǰ����д��E2PROM����
	sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	RTC_SetTime( LocalTime ); //����������
}

void Recover( void ) //�ָ�����һ������
{
	char word[20], temp[20];
	RTCTime LocalTime;
	
	E2PROM_ReadSeq( 0x00, word, 8 ); //��ȡE2PROM����ꡢ�¡���
	LocalTime = RTC_GetTime(); //��ȡ��ǰʱ��
	sprintf( temp, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	sscanf( word, "%04d%02d%02d", &LocalTime.RTC_Year, &LocalTime.RTC_Mon, &LocalTime.RTC_Mday );
	RTC_SetTime( LocalTime ); //����������
	E2PROM_WriteSeq( 0x00, temp, 8 ); //���޸�ǰ����д��E2PROM����
}

void Nextday( void ) //�����������Ҫ����
{
	char word[20];
	RTCTime LocalTime;
	
	LocalTime = RTC_GetTime(); //��ȡ��ǰʱ��
	
	if( LocalTime.RTC_Mday+1 > HowMany( LocalTime.RTC_Year, LocalTime.RTC_Mon ) )
	{
		if( LocalTime.RTC_Mon+1 > 12)
			LocalTime.RTC_Year++; //����12��31�ս��б�ǣ���Ӧ�������1��1��
		else LocalTime.RTC_Mon++;
	}
	else LocalTime.RTC_Mday++;
	
	sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
	E2PROM_WriteSeq( 0x01, word, 8 ); //��Ҫ��ǵ�����д��E2PROM
}

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

void Date( uint8_t value ) //Dateҳ����״̬��
{
	char word[20];
	RTCTime LocalTime;
	
	switch( value )
	{
		case 0: //����0����ʼ��Dateҳ
		{
			XL = 0;
			DOCK = "Date";
			LOCATION = "Date";
			
			LCD_ClearScreen( BackColor ); //����
			Always(); //���Բ���
			
			LocalTime = RTC_GetTime();
			YEAR = LocalTime.RTC_Year; //��ȡ��ǰ�ꡢ�¡���
			MON = LocalTime.RTC_Mon;
			DAY = LocalTime.RTC_Mday;
			
			LCD_DisplayString2( 88, 256, "OK" ); //��ʾOK��ť
			LCD_DisplayString2( 128, 256, "NO" ); //��ʾȡ����ť
			break;
		}
		case KEY_VALUE_UP: //���ϼ�
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				LOCATION = "year";
			}
			else if( strcmp(LOCATION,"year")==0 )
			{
				YEAR = ( YEAR + 1 ) % 2100; //�����Чֵ1901~2099
				if( YEAR==0 )
					YEAR = 1901; //��ֹ���С��1901
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				MON = ( MON + 1 ) % 13; //�·���Чֵ1~12
				if( MON==0 )
					MON = 1; //��ֹ�·�Ϊ0
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				DAY = ( DAY + 1 ) % 32; //������Чֵ1~31
				if( DAY==0 )
					DAY = 1; //��ֹ����Ϊ0
				if( DAY > HowMany(YEAR,MON) )
					DAY = 1; //��ֹ���ڳ����������ֵ
			}
			else if( strcmp(LOCATION,"no")==0 )
			{
				LOCATION = "ok";
			}
			break;
		}
		case KEY_VALUE_DOWN: //���¼�
		{
			if( strcmp(LOCATION,"year")==0 )
			{
				YEAR-- ;
				if( YEAR==1900 ) //��ֹ���С��1901
					YEAR = 2099;
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				MON--;
				if( MON==0 ) //��ֹ�·�Ϊ0
					MON = 12;
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				DAY--;
				if( DAY==0 ) //��ֹ����Ϊ0
					DAY = HowMany(YEAR,MON);
			}
			else if( LOCATION=="ok" )
			{
				LOCATION = "no";
			}
			break;
		}
		case KEY_VALUE_LEFT: //�����
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				Home(0); //Dateҳ�����ΪHomeҳ��
				return;
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				LOCATION = "year"; //�����Ϊ��
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				LOCATION = "mon"; //�����Ϊ��
			}
			else if( LOCATION=="ok" )
			{
				LOCATION = "day"; //OK��ť���Ϊ��
			}
			break;
		}
		case KEY_VALUE_RIGHT: //���Ҽ�
		{
			if( strcmp(LOCATION,"Date")==0 )
			{
				Time(0); //Dateҳ���ұ�ΪTimeҳ��
				return;
			}
			else if( strcmp(LOCATION,"year")==0 )
			{
				LOCATION = "mon"; //���ұ�Ϊ��
			}
			else if( strcmp(LOCATION,"mon")==0 )
			{
				LOCATION = "day"; //���ұ�Ϊ��
			}
			else if( strcmp(LOCATION,"day")==0 )
			{
				LOCATION = "ok"; //���ұ�ΪOK��ť
			}
			break;
		}
		case KEY_VALUE_ENTER: //��ȷ�ϼ�
		{
			if( strcmp(LOCATION,"ok")==0 ) //��OK��ť
			{
				LocalTime = RTC_GetTime();
				sprintf( word, "%04d%02d%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
				E2PROM_WriteSeq(0x0,word, 8);
				LocalTime.RTC_Year = YEAR; //д�뵱ǰ�ꡢ�¡���
				LocalTime.RTC_Mon = MON;
				LocalTime.RTC_Mday = DAY;
				RTC_SetTime( LocalTime );
				Home(0); //������ҳ
			}
			else if( strcmp(LOCATION,"no")==0 ) //��ȡ����ť
			{
				Date(0); //���ص�ǰҳ
			}
			return;
		}
	}
	while( DAY > HowMany(YEAR,MON) ) //��ֹ�����ꡢ�º������
		DAY--;
	
	Gui_Date(); //���ƽ���
}

void Gui_Date( void ) //Dateҳ����
{
	char word[20];
	sprintf( word, "%04d-%02d-%02d", YEAR, MON, DAY );
	LCD_DisplayString2( 88, 32, word ); //��ʾ��������ꡢ�¡���
	
	Square( 32-2, 88-1-2, 64+2+2, 24+2, TextColor ); //��ʾ�����
	Square( 112-2, 88-1-2, 32+2+2, 24+2, TextColor ); //��ʾ�����
	Square( 160-2, 88-1-2, 32+2+2, 24+2, TextColor ); //��ʾ�����
	Square( 256-2, 88-1-2, 32+2+2, 24+2, TextColor ); //��ʾOK��ť���
	Square( 256-2, 128-1-2, 32+2+2, 24+2, TextColor ); //��ʾȡ����ť���
	
	if( strcmp(LOCATION,"year")==0 ) //����˸���
	{
		X = 32-2;
		Y = 88-1-2;
		XL = 64+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"mon")==0 ) //����˸���
	{
		X = 112-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"day")==0 ) //����˸���
	{
		X = 160-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"ok")==0 ) //ȷ�ϼ���˸���
	{
		X = 256-2;
		Y = 88-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else if( strcmp(LOCATION,"no")==0 ) //ȡ������˸���
	{
		X = 256-2;
		Y = 128-1-2;
		XL = 32+2+2;
		YL = 24+2;
	}
	else XL=0;
}

//��������£���������м���
uint8_t HowMany( uint32_t year, uint32_t mon )
{
	uint8_t num[]={31,28,31,30,31,30,31,31,30,31,30,31};
	if( year%4==0 || year%100 && year%400==0 ) //�ж�����
		num[1] = 29;
	return num[mon-1];
}

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

void Home( uint8_t value ) //Homeҳ����״̬��
{
	RTCTime LocalTime;
	
	switch( value )
	{
		case 0: //����0����ʼ��Homeҳ
		{
			XL = 0;
			DOCK = "Home";
			LOCATION = "Home";
			
			LCD_ClearScreen( BackColor ); //����
			Always(); //���Բ���
			
			LocalTime = RTC_GetTime();
			YEARS = LocalTime.RTC_Year; //��ȡ��ǰ�ꡢ��
			MONS = LocalTime.RTC_Mon;
			Calendar( YEARS, MONS ); //��ӡ����
			break;
		}
		case KEY_VALUE_UP: //���ϼ����ƶ����
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
		case KEY_VALUE_DOWN: //���¼����ƶ����
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
		case KEY_VALUE_LEFT: //��������޶���
		{
			break;
		}
		case KEY_VALUE_RIGHT: //���Ҽ�������Dateҳ
		{
			if( strcmp(LOCATION,"Home")==0 )
				Date(0);
			return;
		}
		case KEY_VALUE_ENTER: //��ȷ�ϼ�
		{
			Clear(); //����
			if( strcmp(LOCATION,"curr")==0 ) //��ǰ�°�ť
			{
				LocalTime = RTC_GetTime();
				YEARS = LocalTime.RTC_Year;
				MONS = LocalTime.RTC_Mon;
			}
			else if( strcmp(LOCATION,"last")==0 ) //��һ�°�ť
			{
				MONS--;
				if( MONS==0 )
				{
					YEARS--; //����0�£�����һ��
					MONS = 12;
				}
			}
			else if( strcmp(LOCATION,"next")==0 ) //��һ�°�ť
			{
				MONS++;
				if( MONS==13 )
				{
					YEARS++; //����13�£�����һ��
					MONS = 1;
				}
			}
			Calendar( YEARS, MONS ); //ˢ������
			break;
		}
	}
	Gui_Home(); //���ƽ���
}

void Gui_Home( void ) //Homeҳ����
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
	LCD_DisplayString( 88, 18, word ); //��ʾ��ǰ�����Ǽ���
	
	sprintf( word, "Month : %02d", MONS );
	LCD_DisplayString( 112, 18, word ); //��ʾ��ǰ�����Ǽ���
	
	LCD_DisplayString( 136, 18, "Curr Month" ); //��ʾ��ǰ�°�ť����
	LCD_DisplayString( 160, 18, "Last Month" ); //��ʾ���°�ť����
	LCD_DisplayString( 184, 18, "Next Month" ); //��ʾ���°�ť����
	Square( 18-3, 136-3, 60+6, 8+6, TextColor ); //��ʾ��ǰ�°�ť���
	Square( 18-3, 160-3, 60+6, 8+6, TextColor ); //��ʾ���°�ť���
	Square( 18-3, 184-3, 60+6, 8+6, TextColor ); //��ʾ���°�ť���
}

uint32_t Mark( uint32_t year, uint32_t month ) //�����ꡢ�£�����Ƿ�����Ҫ������
{
	uint32_t y, m, d;
	char word[20];
	
	E2PROM_ReadSeq( 0x01, word, 8 ); //��ȡE2PROM��Ĵ������Ҫ�ꡢ�¡���
	sscanf( word, "%04d%02d%02d", &y, &m, &d );
	
	if( year==y && month==m ) //����ꡢ��һ��
		return d; //���ص�����Ҫ������
	else return 0; //���򷵻�0
}

void Calendar( uint32_t year, uint32_t month ) //�����ꡢ�£���ӡ����
{
	uint32_t day, temp, first;
	uint16_t i, x, y, trim=0, num[]={31,28,31,30,31,30,31,31,30,31,30,31};
	char word[8];
	unsigned short tempcolor;
	
	day = Mark( year, month ); //�Ƿ�����Ҫ��������Ҫ���
	
	if( year%4==0 || year%100 && year%400==0 ) //�ж�����
		num[1] = 29;
	
	first = weekday( year, month, 1); //�������1�������ڼ�
	
	x = 110 + 30 * first; //1����ʼ������
	y = 40 + 24; //1����ʼ������
	
	tempcolor = TextColor;
	LCD_SetTextColor( LCD_COLOR_BLUE );
	LCD_DisplayString( 40, 104, "                              Sat" ); //��ɫ��ʾ������
	LCD_SetTextColor( tempcolor );
	LCD_DisplayString( 40, 104, "     Mon  Tur  Wed  Thu  Fri" ); //��ɫ��ʾ����һ����
	LCD_SetTextColor( LCD_COLOR_RED );
	LCD_DisplayString( 40, 104, "Sun" ); //��ɫ��ʾ������
	
	for( temp=first, i=1; i<=num[month-1]; i++ )
	{
		LCD_SetTextColor( tempcolor );
		if( i==10 ) //�����ֳ�����λ��ʱ��΢�����룬ʹ�þ�����ʾ
		{
			trim = 3;
		}
		if( i==day && i<10 )
			Square( x-trim-13, y-5, 32, 8+10, TextColor ); //����Ҫ�����ϱ��С����
		else if( i==day && i>=10 )
			Square( x-trim-12, y-5, 37+trim, 8+10, TextColor ); //����Ҫ�����ϱ�Ǵ󷽿�
		
		switch( temp )
		{
			case 0: //������
				sprintf( word, "%d  ", i );
				LCD_SetTextColor( LCD_COLOR_RED );
				LCD_DisplayString( y, x, word ); //��ɫ��ʾ
				x = 104 - trim + 36; //����������1����λ
				temp++;
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5: //����һ����
				if( i==10 )
					x = x - trim;
				sprintf( word, "%d  ", i );
				LCD_SetTextColor( tempcolor );
				LCD_DisplayString( y, x, word ); //��ɫ��ʾ
				x = x + 30; //����������1����λ
				temp++;
				break;
			case 6: //������
				if( i==10 )
					x = x - trim;
				sprintf( word, "%d", i );
				LCD_SetTextColor( LCD_COLOR_BLUE );
				LCD_DisplayString( y, x, word ); //��ɫ��ʾ
				x = 110 - trim ; //������ص������
				y = y + 24; //����������1����λ
				temp = 0;
				break;
		}
	}
	LCD_SetTextColor( tempcolor );
}

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

void More( uint8_t value ) //Moreҳ����״̬��
{
	switch( value )
	{
		case 0: //����0����ʼ��Moreҳ
		{
			XL = 0;
			DOCK = "More";
			LOCATION = "More";
			
			LCD_ClearScreen( BackColor ); //����
			Always(); //���Բ���
			break;
		}
		case KEY_VALUE_UP: //���ϼ����ƶ����
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
		case KEY_VALUE_DOWN: //���¼����ƶ����
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
		case KEY_VALUE_LEFT: //�����������Timeҳ
		{
			if( strcmp(LOCATION,"More")==0 )
			{
				Time(0);
				return;
			}
		}
		case KEY_VALUE_RIGHT: //���Ҽ����޶���
		{
			break;
		}
		case KEY_VALUE_ENTER: //��ȷ�ϼ�
		{
			if( strcmp(LOCATION,"mode1")==0 ) //ҹ��ģʽ��ť
			{
				ON = Night(ON);
				LCD_ClearScreen( BackColor ); //����
				Always(); //���Բ���
			}
			else if( strcmp(LOCATION,"mode2")==0 ) //������հ�ť
			{
				Nextday();
			}
			else if( strcmp(LOCATION,"mode3")==0 ) //�ָ��ϸ����ڰ�ť
			{
				Recover();
			}
			else if( strcmp(LOCATION,"mode4")==0 ) //�ָ��������ð�ť
			{
				Factory();
			}
			break;
		}
	}
	Gui_More(); //���ƽ���
}

void Gui_More( void ) //Moreҳ����
{
	if( ON==1 )
		LCD_DisplayString2( 48, 32, "Night Mode    On" ); //��ʾҹ��ģʽ��
	else LCD_DisplayString2( 48, 32, "Night Mode   Off" ); //��ʾҹ��ģʽ��
	LCD_DisplayString2( 88, 32, "Nextday     Mark" ); //��ʾ������հ�ť����
	LCD_DisplayString2( 128, 32, "Recover     Date" ); //��ʾ�ָ��ϸ����ڰ�ť����
	LCD_DisplayString2( 168, 32, "Factory    Reset" ); //��ʾ�ָ��������ð�ť����
	
	Square( 32-2, 48-2-1, 256+4, 24+4, TextColor ); //��ʾҹ��ģʽ���
	Square( 32-2, 88-2-1, 256+4, 24+4, TextColor ); //��ʾ������հ�ť���
	Square( 32-2, 128-2-1, 256+4, 24+4, TextColor ); //��ʾ�ָ��ϸ����ڰ�ť���
	Square( 32-2, 168-2-1, 256+4, 24+4, TextColor ); //��ʾ�ָ��������ð�ť���
	
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

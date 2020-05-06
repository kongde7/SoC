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
	
	RTCTime LocalTime; //����ʱ��ṹ��
	KEY_TypeDef key; //�������򰴼��ṹ��
	I2C_InitTypeDef I2C_InitStruct; //����I2C�ṹ��
	
	DelayCounter = 300;
	Timer_Init(0); //��ʼ����ʱ��
	
	RTC_Init(); //��ʼ��ʵʱʱ��
	
	Joystick_Config(); //��ʼ������״̬��
	
	I2C_InitStruct.Mode = I2C_MASTER;
	I2C_InitStruct.ClockRate = 400000;
	I2C_InitStruct.InterruptMode = I2C_INTERRUPT_MODE;
	I2C0_Init( &I2C_InitStruct ); //��ʼ��I2C
	
	LCD_Init(); //��ʼ����ʾ��
	
	LocalTime = RTC_GetTime();
	mon = LocalTime.RTC_Mon; //��ȡ��ǰ�·�
	Home(0); //Ĭ�Ͻ�����ҳ
	
	while(1)
	{
		i = 10000;
		while( i-- ) //���򰴼���ȡҪ�ӳ�һ��ʱ��
		{
			if( DelayCounter>300 ) //���ÿ300ms�ڰ׽�����
			{
				if( temp==0 )
				{
					Square( X, Y, XL, YL, TextColor ); //��ɫ���
					temp++;
				}
				else if( temp==1 )
				{
					Square( X, Y, XL, YL, BackColor ); //��ɫ���
					temp = 0;
				}
				DelayCounter = 300;
			}

			if( SecondInt ) //�������ж�
			{
				LocalTime = RTC_GetTime();
				if( mon!=LocalTime.RTC_Mon && strcmp(DOCK,"Home")==0 ) //����·ݷ����䶯��ˢ������
					Home(0);
				mon = LocalTime.RTC_Mon;
				
				sprintf( date, "%04d-%02d-%02d", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday );
				LCD_DisplayString2( 6, 10, date ); //������������
				
				sprintf( time, "%02d:%02d:%02d", LocalTime.RTC_Hour, LocalTime.RTC_Min, LocalTime.RTC_Sec );
				LCD_DisplayString2( 6, 182, time ); //��������ʱ��
				
				SecondInt = 0;
			}
		}

		key = GetKey(); //��ȡ���򰴼�״̬

		if( key.KeyValue && key.Pressed ) //������������ʱ
		{
			if( strcmp(DOCK,"Home")==0 ) //����Homeҳ
				Home( key.KeyValue ); //��Homeҳ�����ֵ
			
			else if( strcmp(DOCK,"Date")==0 ) //����Dateҳ
				Date( key.KeyValue ); //��Dateҳ�����ֵ
			
			else if( strcmp(DOCK,"Time")==0 ) //����Timeҳ
				Time( key.KeyValue ); //��Timeҳ�����ֵ
			
			else if( strcmp(DOCK,"More")==0 ) //����Moreҳ
				More( key.KeyValue ); //��Moreҳ�����ֵ
		}
	}
}

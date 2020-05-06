#include <string.h>
#include "lpc17xx.h"
#include "LCD.h"

extern volatile unsigned short TextColor, BackColor;
extern char *DOCK;

void Always( void ) //���Բ���
{
	LCD_DisplayString2( 214, 8, "Home Date Time More" ); //��ʾ�ײ�����
	
	LineY( 79, 208, 32, TextColor );
	LineY( 159, 208, 32, TextColor );
	LineY( 239, 208, 32, TextColor ); //��ʾ�ײ���������
	
	LineX( 0, 31, 320, TextColor ); //��ʾ��������
	
	if( strcmp(DOCK,"Home")==0 ) //Homeҳ��ײ�����
	{
		LineX( 79, 208, 240, TextColor );
	}
	else if( strcmp(DOCK,"Date")==0 ) //Dateҳ��ײ�����
	{
		LineX( 0, 208, 80, TextColor );
		LineX( 159, 208, 160, TextColor );
	}
	else if( strcmp(DOCK,"Time")==0 ) //Timeҳ��ײ�����
	{
		LineX( 0, 208, 160, TextColor );
		LineX( 239, 208, 80, TextColor );
	}
	else if( strcmp(DOCK,"More")==0 ) //Moreҳ��ײ�����
	{
		LineX( 0, 208, 240, TextColor );
	}
}

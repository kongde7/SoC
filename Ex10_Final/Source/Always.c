#include <string.h>
#include "lpc17xx.h"
#include "LCD.h"

extern volatile unsigned short TextColor, BackColor;
extern char *DOCK;

void Always( void ) //常显部分
{
	LCD_DisplayString2( 214, 8, "Home Date Time More" ); //显示底部文字
	
	LineY( 79, 208, 32, TextColor );
	LineY( 159, 208, 32, TextColor );
	LineY( 239, 208, 32, TextColor ); //显示底部三条竖线
	
	LineX( 0, 31, 320, TextColor ); //显示顶部横线
	
	if( strcmp(DOCK,"Home")==0 ) //Home页面底部横线
	{
		LineX( 79, 208, 240, TextColor );
	}
	else if( strcmp(DOCK,"Date")==0 ) //Date页面底部横线
	{
		LineX( 0, 208, 80, TextColor );
		LineX( 159, 208, 160, TextColor );
	}
	else if( strcmp(DOCK,"Time")==0 ) //Time页面底部横线
	{
		LineX( 0, 208, 160, TextColor );
		LineX( 239, 208, 80, TextColor );
	}
	else if( strcmp(DOCK,"More")==0 ) //More页面底部横线
	{
		LineX( 0, 208, 240, TextColor );
	}
}

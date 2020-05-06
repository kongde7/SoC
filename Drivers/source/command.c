#include <string.h>
#include <stdint.h>
#include "lpc17xx.h"
#include "monitor.h"
#include "led.h"
#include "lpc1768_uart.h"
#include "command.h"
#include "lpc1768_rtc.h"

uint8_t *pOptions[4];
uint8_t *pOptions2[4];

uint8_t cmd_line_parser(uint8_t *pCmd, uint8_t** options)
{
	char *pstr;
	unsigned i= 1;
	if (*pCmd== NULL)
		return 0;
	pstr= strtok((char*) pCmd, " ");
	if (pstr!= NULL)
		options[0] = (uint8_t*) pstr;
	while (pstr!= NULL)
	{
		pstr= strtok(NULL, " ");
		if (pstr!= NULL)
			options[i++] = pstr;
	}
	return (i);
}

uint8_t cmd_line_parser2(uint8_t *pCmd, uint8_t** options)
{
	char *pstr;
	unsigned i= 1;
	if (*pCmd== NULL)
		return 0;
	pstr= strtok((char*) pCmd, ":");
	if (pstr!= NULL)
		options[0] = (uint8_t*) pstr;
	while (pstr!= NULL)
	{
		pstr= strtok(NULL, ":");
		if (pstr!= NULL)
			options[i++] = pstr;
	}
	return (i);
}



void led_process()
{
	if(strcmp(pOptions[1], "on") == 0)
	{
		LED_On(0x01 << (atoi(pOptions[2]) & 0x07) );
	}
else if(strcmp(pOptions[1], "off") == 0)
{
	LED_Off(0x01 << (atoi(pOptions[2]) & 0x07) );
}
else if(strcmp(pOptions[1], "all") == 0)
{
	LED_On( 0x0FF );
}

}


void Command_Process(uint8_t *pCommand)
{
	uint8_t Option_Count = cmd_line_parser(pCommand, pOptions);
	
	if (!Option_Count)
		return;
	
	if (pOptions[0]==NULL||*pOptions[0]==NULL)
		return;
	
	if (strcmp((const char*)pOptions[0], "led")==0)
	{
		if(Option_Count!= 3)
			xputs("Command Error.\n");
		led_process();
	}
	else if(strcmp((const char*)pOptions[0], "date")==0)
		date_process();
	
	else if(strcmp((const char*)pOptions[0], "time")==0)
		time_process();
	
	else xputs("Unknown Command.\n");
}

void date_process()
{
	RTCTime LocalTime;

	if( pOptions[1]==NULL )
	{
		LocalTime = RTC_GetTime();
		xprintf( "%04d:%02d:%02d\n", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday);
	}
	else
	{
		uint8_t Option_Count2 = cmd_line_parser2(pOptions[1], pOptions2);
		
		RTCTime Time = RTC_GetTime();
		
		Time.RTC_Mday = atoi(pOptions2[2]);
		Time.RTC_Mon = atoi(pOptions2[1]);
		Time.RTC_Year = atoi(pOptions2[0]);
		RTC_SetTime( Time );
		LocalTime = RTC_GetTime();
		xprintf( "%04d:%02d:%02d\n", LocalTime.RTC_Year, LocalTime.RTC_Mon, LocalTime.RTC_Mday);
		pOptions[1]=NULL;
	}
}

void time_process()
{
	RTCTime LocalTime;

	if( pOptions[1]==NULL )
	{
		LocalTime = RTC_GetTime();
		xprintf( "%02d:%02d:%02d\n", LocalTime.RTC_Hour, LocalTime.RTC_Min, LocalTime.RTC_Sec);
	}
	else
	{
		uint8_t Option_Count2 = cmd_line_parser2(pOptions[1], pOptions2);
		
		RTCTime Time = RTC_GetTime();
		
		Time.RTC_Sec = atoi(pOptions2[2]);
		Time.RTC_Min = atoi(pOptions2[1]);
		Time.RTC_Hour = atoi(pOptions2[0]);
		RTC_SetTime( Time );
		LocalTime = RTC_GetTime();
		xprintf( "%02d:%02d:%02d\n", LocalTime.RTC_Hour, LocalTime.RTC_Min, LocalTime.RTC_Sec);
		pOptions[1]=NULL;
	}
	return;
}

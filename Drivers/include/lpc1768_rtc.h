/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-07-01
** Version:             V1.00
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#ifndef __RTC_H 
#define __RTC_H

extern volatile uint32_t Alarm_Ring;
extern volatile uint32_t SecondInt;

typedef struct {
        uint32_t RTC_Sec;     /* Second value - [0,59] */
        uint32_t RTC_Min;     /* Minute value - [0,59] */
        uint32_t RTC_Hour;    /* Hour value - [0,23] */
        uint32_t RTC_Mday;    /* Day of the month value - [1,31] */
        uint32_t RTC_Mon;     /* Month value - [1,12] */
        uint32_t RTC_Year;    /* Year value - [0,4095] */
        uint32_t RTC_Wday;    /* Day of week value - [0,6] */
        uint32_t RTC_Yday;    /* Day of year value - [1,365] */
} RTCTime;

#define CIIR_IMSEC		(0x01 << 0)
#define CIIR_IMMIN		(0x01 << 1)
#define CIIR_IMHOUR		(0x01 << 2)
#define CIIR_IMDOM		(0x01 << 3)
#define CIIR_IMDOW		(0x01 << 4)
#define CIIR_IMDOY		(0x01 << 5)
#define CIIR_IMMON		(0x01 << 6)
#define CIIR_IMYEAR		(0x01 << 7)

#define AMR_SEC		        (0x01 << 0)  /* Alarm mask for Seconds */
#define AMR_MIN		        (0x01 << 1)  /* Alarm mask for Minutes */
#define AMR_HOUR		(0x01 << 2)  /* Alarm mask for Hours */
#define AMR_DOM		        (0x01 << 3)  /* Alarm mask for Day of Month */
#define AMR_DOW		        (0x01 << 4)  /* Alarm mask for Day of Week */
#define AMR_DOY		        (0x01 << 5)  /* Alarm mask for Day of Year */
#define AMR_MON		        (0x01 << 6)  /* Alarm mask for Month */
#define AMR_YEAR		(0x01 << 7)  /* Alarm mask for Year */

#define CCR_CLKEN		(0x01 << 0)
#define CCR_CTCRST		(0x01 << 1)
#define CCR_CCALEN		(0x01 << 4)

//#define PREINT_RTC	0x000001C8  /* Prescaler value, integer portion, 
//				    PCLK = 15Mhz */
//#define PREFRAC_RTC	0x000061C0  /* Prescaler value, fraction portion, 
//				    PCLK = 15Mhz */
#define ILR_RTCCIF	0x01
#define ILR_RTCALF	0x02

void RTC_IRQHandler (void);
void RTC_Init( void );
void RTC_Start( void );
void RTC_Stop( void );
void RTC_CTCReset( void );
void RTC_SetTime( RTCTime );
RTCTime RTC_GetTime( void );
void RTC_SetAlarm( RTCTime );
RTCTime RTC_GetAlarm( void );
void RTC_SetAlarmMask( uint32_t AlarmMask );

int weekday(int y, int m, int d); //自己添加的
int yearday(int y, int m, int d); //自己添加的

#endif /* end __RTC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

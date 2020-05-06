#ifndef __BMBOARD_H
#define __BMBOARD_H

/* 8 LEDs */
#define LED04   0x01UL << 0
#define LED05   0x01UL << 1
#define LED06   0x01UL << 2
#define LED07   0x01UL << 3
#define LED08   0x01UL << 4
#define LED09   0x01UL << 5
#define LED10   0x01UL << 6
#define LED11   0x01UL << 7

//#define LED (LED04 | LED05 | LED06 | LED07 | \
                 LED08 | LED09 | LED10 | LED11)
#define LED_ALL (LED04 | LED05 | LED06 | LED07 | \
                 LED08 | LED09 | LED10 | LED11)

/* User Keys  and Joy Stick*/
#define KEY0            (0x01UL << 10)            // Port 2
#define KEY1            (0x01UL << 11)             
#define KEY2            (0x01UL << 12)

// 原先的配置，现按照左为下，右为上，上为左，下为右改动
// #define JOY_ENTER       (0x01UL << 25)            // Port 1
// #define JOY_DOWN        (0x01UL << 26)
// #define JOY_LEFT        (0x01UL << 27)
// #define JOY_RIGHT       (0x01UL << 28)
// #define JOY_UP          (0x01UL << 29)
#define JOY_ENTER       (0x01UL << 25)            // Port 1
#define JOY_DOWN        (0x01UL << 28)
#define JOY_LEFT        (0x01UL << 26)
#define JOY_RIGHT       (0x01UL << 29)
#define JOY_UP          (0x01UL << 27)

/* LCD interface */
/* 8bit to 16bit LCD Interface 
   
   PINS: 
   - EN      = P0.19
   - LE      = P0.20
   - DIR     = P0.21
   - CS      = P0.22
   - RS      = P0.23 
   - WR      = P0.24 
   - RD      = P0.25
   - DB[0.7] = P2.0...P2.7
   - DB[8.15]= P2.0...P2.7                                                     */
#define LCD_PIN_EN          (0x01UL << 19)
#define LCD_PIN_LE          (0x01UL << 20)
#define LCD_PIN_DIR         (0x01UL << 21)
#define LCD_PIN_CS          (0x01UL << 22)
#define LCD_PIN_RS          (0x01UL << 23)
#define LCD_PIN_WR          (0x01UL << 24)
#define LCD_PIN_RD	    (0x01UL << 25)

/* Touch Panel */
#define TP_INT          (0x01UL << 13)    // port 2
#define TP_CS           (0x01UL << 6)

/* Serial Data FLASH */
#define DATA_FLASH_CS   (0x01UL << 16) 	// Port 0.16

/* SD Card */
#define SD_CD           (0x01UL << 25)    // Port 3.25
#define SD_PWR          (0x01UL << 26)    // Port 3.26
#define SD_CS           (0x01UL << 21)    // Port 0.21

void SD_Pin_Config(void);
void SD_Power(uint8_t OnOff);

#endif

#ifndef __MORE_H
#define __MORE_H

void Always( void );

void Home( uint8_t value );
void Gui_Home( void );

uint32_t Mark( uint32_t year, uint32_t month );
void Calendar( uint32_t year, uint32_t month );

void Date( uint8_t value );
void Gui_Date( void );

uint8_t HowMany( uint32_t year, uint32_t mon );

void Time( uint8_t value );
void Gui_Time( void );

void More( uint8_t value );
void Gui_More( void );

void Gui_More( void );
void More( uint8_t value );

uint8_t Night( uint8_t on );
void Nextday( void );
void Recover( void );
void Factory( void );

#endif

/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-07-01
** Version:             V1.00
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#define KEY_VALUE_NOKEY		0x00
#define KEY_VALUE_UP		0x01
#define KEY_VALUE_DOWN		0x02
#define KEY_VALUE_LEFT		0x03
#define KEY_VALUE_RIGHT		0x04
#define KEY_VALUE_ENTER		0x05

typedef enum {//自己添加部分
	KEY_IDLE,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_PRESS_CONFIRMED,
	KEY_RELEASE_CONFIRMED
} KEY_FSM_STATUS;

/* Key structure for keyboard applications 
    KeyVaule = KEY_NOKEY, mease there is no key activity
    when Keyalue is not KEY_NOKEY, var Pressed and Released indicate
    the activity of that key, 1 means one of the acitivities occured
    sometime.
    If both Pressed and Released is set to 1, that means the key is 
    pressed and released before application get it
*/
    
typedef struct {
	uint8_t KeyValue;
	uint8_t Pressed;
	uint8_t Released;
} KEY_TypeDef;

void Joystick_Config(void);
KEY_TypeDef GetKey(void);
void KeyboardScan(void);

#endif

#include "lpc17xx.h"
#include "bmboard.h"
#include "lpc1768_timer.h"
#include "joystick.h"

void (*oldcb) (void); //定义一个回调函数指针
KEY_FSM_STATUS status; //定义全局变量：有限状态机的状态
KEY_TypeDef Key; //定义五向按键结构体

static void KeyboardScan()
{
	static uint8_t counter=0; //定义计数器
	uint32_t key;
	
	if( counter++ < 15 ) //还没有到15ms
	{
		oldcb();
		return; //返回
	}
	else
	{
		counter = 0; //到15ms，计数器清零
		key = ~(LPC_GPIO1->FIOPIN) & 0x3E000000; //检测按键端口
		
		if( key & JOY_UP ) //如果检测到上，赋值给key
			key = KEY_VALUE_UP;
		else if( key & JOY_DOWN ) //如果检测到下，赋对应值给key
			key = KEY_VALUE_DOWN;
		else if( key & JOY_LEFT ) //如果检测到左，赋对应值给key
			key = KEY_VALUE_LEFT;
		else if( key & JOY_RIGHT ) //如果检测到右，赋对应值给key
			key = KEY_VALUE_RIGHT;
		else if( key & JOY_ENTER ) //如果检测到中，赋对应值给key
			key = KEY_VALUE_ENTER;
		else
			key = KEY_VALUE_NOKEY; //没有检测到，赋对应值给key
	}
	
	switch( status )
	{
		case KEY_IDLE: //空闲态情况下
			if(key)
				status = KEY_PRESSED; //有按键，进入按下态
		break;
		
		case KEY_PRESSED: //按下态情况下
			if(key)
			{
				status = KEY_PRESS_CONFIRMED; //还有按键，进入确认态
				Key.KeyValue = key; //传送键值
				Key.Pressed = 1;
			}
			else
				status = KEY_IDLE; //无按键，返回空闲态
		break;
			
		case KEY_PRESS_CONFIRMED: //按下确认态情况下
			if(!key)
				status = KEY_RELEASED; //有松开，进入释放态
		break;
			
		case KEY_RELEASED: //释放态情况下
			if(key)
				status = KEY_PRESS_CONFIRMED; //又按下，返回按键确认态
			else
			{
			status = KEY_RELEASE_CONFIRMED; //还是松开，进入释放确认态
			Key.Released = 1;
			}
		break;
		
		case KEY_RELEASE_CONFIRMED: //释放确认态情况下
			if(key)
			{
				status = KEY_PRESSED; //又按下，进入下一个按键态
				Key.KeyValue = key; //传送键值
			}
		break;
	}
	oldcb();
}

void Joystick_Config()
{
	LPC_PINCON->PINSEL3 = 0; //设置引脚
	LPC_GPIO1->FIODIR = ~( JOY_ENTER | JOY_LEFT | JOY_RIGHT | JOY_UP | JOY_DOWN );
	
	oldcb = pTimer0Callback; //将状态机和定时器联系起来
	pTimer0Callback = KeyboardScan; //定时器回调函数指向状态机的KeyboardScan()函数
	
	status = KEY_IDLE; //初始化状态为IDLE
}

KEY_TypeDef GetKey()
{
	KEY_TypeDef CurKey; //定义五向按键结构体
	CurKey.KeyValue = Key.KeyValue;
	CurKey.Pressed = Key.Pressed;
	CurKey.Released = Key.Released; //将当前按键状态设为初值
	
	if( status == KEY_RELEASE_CONFIRMED ) //如果已经被读取完毕
	{
		Key.KeyValue = KEY_VALUE_NOKEY; //置为空
		Key.Pressed = 0;
		Key.Released = 0; //清空按键状态
		status = KEY_IDLE; //返回IDLE状态
	}
	else if( status == KEY_PRESS_CONFIRMED || status == KEY_RELEASED ) //如果按下状态被读取
	{
		Key.Pressed = 0; //清零
	}
	return CurKey; //返回五向按键结构体
}

#include "LPC17xx.h"
#include "bmboard.h"
#include "led.h"
#include "key.h"

void Key_init( void ) //按键初始化程序
{
	uint32_t pincon;
	pincon = LPC_PINCON->PINSEL4; //PINSEL4设置成EINT0
	pincon &= ~(0x03 << 20);
	pincon |= (0x01 << 20);
	LPC_PINCON->PINSEL4 = pincon;
	
	LPC_SC->EXTMODE |= (1UL << 0); //设置外部中断
	LPC_SC->EXTPOLAR &= ~(1UL << 0);
	
	NVIC_EnableIRQ( EINT0_IRQn );
	
	LPC_GPIO2->FIODIR      &= ~(KEY1); //设置GPIO中断
	LPC_GPIOINT->IO2IntEnF |=  (KEY1); //KEY1设置双沿触发
	LPC_GPIOINT->IO2IntEnR |=  (KEY1);
	LPC_GPIO2->FIODIR      &= ~(KEY2);
	LPC_GPIOINT->IO2IntEnF |=  (KEY2); //KEY2设置双沿触发
	LPC_GPIOINT->IO2IntEnR |=  (KEY2);
	
	NVIC_EnableIRQ( EINT3_IRQn );
}

void EINT0_IRQHandler() //外部中断服务程序
{
	static char OnOff = 0;
	
	LPC_SC->EXTINT |= (1UL << 0); //清零
	
	if (OnOff == 1)
		{
			LED_Off(LED04); //按下KEY0，LED4熄灭
			OnOff = 0;
		}
	else
		{
			LED_On (LED04); //再次按下KEY0，LED4亮灯
			OnOff = 1;
		}
}

void EINT3_IRQHandler() //GPIO中断服务程序
{
	static char OnOff1 = 0, OnOff2 = 0;
	uint32_t status, status2, led;
	
	status = LPC_GPIOINT->IO2IntStatF; //读到下降沿触发的中断
	status2 = LPC_GPIOINT->IO2IntStatR; //读到上降沿触发的中断
	
	LPC_GPIOINT->IO2IntClr |= KEY1; //清零
	LPC_GPIOINT->IO2IntClr |= KEY2; //清零
	
	if (status & KEY1) //按住KEY1，LED6亮灯
		LED_On (LED06);
	
	if (status2 & KEY1) //松开KEY1，LED6熄灭
		LED_Off(LED06);
		
	if (status & KEY2) //按住KEY2，LED5亮灯
		LED_On (LED05);
	
	if (status2 & KEY2) //松开KEY2，LED5熄灭
		LED_Off(LED05);
}

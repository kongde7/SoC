#include <stdarg.h>
#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "lpc1768_timer.h"
#include "lpc17xx.h"
#include "lpc1768_uart.h"
#include "bmboard.h"

void(*pTimer0Callback)(void); //定义一个回调函数指针

static void nop(void)
{
    return; //定义一个空函数
}

uint32_t Timer_Init( uint8_t timer_num )
{
    if( timer_num == 0 )
    {
        NVIC_EnableIRQ( TIMER0_IRQn ); //运行中断服务程序
        LPC_TIM0->CTCR = 0; //设置定时器模式
        LPC_TIM0->PR = (25000000UL/2000-1); //设置分频2KHz
        LPC_TIM0->MR0 = 2-1; //设置每1ms发生一次中断
        LPC_TIM0->MCR = 3; //设置计数器复位产生中断
        LPC_TIM0->EMR = (0x3<<4); //设置电平翻转
        LPC_TIM0->TCR = TCR_COUNTER_ENABLE; //启动计时器
        pTimer0Callback = nop; //指向空函数
        return(1);
    }
}

volatile uint32_t DelayCounter = 0; //减计数器全局变量
void DelayMs(uint32_t delayInMs)
{
    if( delayInMs == 0 ) //传入参数必须非0
		return;
	DelayCounter = delayInMs; //减计数器赋初值
    while( DelayCounter ); //等待减计数器工作至0
    return;
}

void TIMER0_IRQHandler(void)
{
    LPC_TIM0->IR = 1; //清除中断标志
    DelayCounter--; //计数器减1
    pTimer0Callback(); //回调函数
}

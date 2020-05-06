/*********************************************************************************************************/
#ifndef __LPC1768_TIMER_H 
#define __LPC1768_TIMER_H

#define TCR_COUNTER_ENABLE	(0x01UL << 0)
#define TCR_COUNTER_RESET	(0x01UL << 1)

#define MCR_INTERRUPT_ENABLE	(0x01UL)
#define MCR_RESET_TC		(0x02UL)
#define MCR_STOP_TC		(0x04UL)

uint32_t init_timer0(void);
void DelayMs(uint32_t delayInMs);
uint32_t Timer_Init( uint8_t timer_num );

void TIMER0_IRQHandler (void);


extern void (*pTimer0Callback)(void);

#endif /* __LPC1768_TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

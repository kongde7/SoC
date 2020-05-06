/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-10-01
** Version:             V1.10
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "lpc17xx.h"    
#include "lpc1768_uart.h"

#define UART_BAUD_115200     115200      

extern uint32_t SystemCoreClock;
static uint8_t uart_set_divisors(LPC_UART_TypeDef *pUart, uint32_t baud);

/*
	Calculate the Baud Divisors and the Fraction Divisors.
	Assume the UARTx use PCLK as their clock source.
	Please refer to 14.4.12 in UM10360 about the algorithm.
*/
static uint8_t uart_set_divisors(LPC_UART_TypeDef *pUart, uint32_t baud)
{
	uint32_t PClk = SystemCoreClock /4;
	
	uint32_t d, m, bestd, bestm, tmp;
	uint64_t best_divisor, divisor;
	uint32_t current_error, best_error;
	uint32_t recalcbaud;

	best_error = 0xFFFFFFFF; /* Worst case */
	bestd = 0;
	bestm = 0;
	best_divisor = 0;
	
	for (m = 1 ; m <= 15 ;m++)
	{
		for (d = 0 ; d < m ; d++)
		{
		  divisor = ((uint64_t)PClk<<28)*m/(baud*(m+d));
		  current_error = divisor & 0xFFFFFFFF;

		  tmp = divisor>>32;

		  /* Adjust error */
		  if(current_error > ((uint32_t)1<<31)){
			current_error = -current_error;
			tmp++;
			}

		  if(tmp<1 || tmp>65536) /* Out of range */
		  continue;

		  if( current_error < best_error){
			best_error = current_error;
			best_divisor = tmp;
			bestd = d;
			bestm = m;
			if(best_error == 0) break;
			}
		} /* end of inner for loop */

		if (best_error == 0)
		  break;
	} /* end of outer for loop  */

	if(best_divisor == 0) return 0; /* can not find best match */

	recalcbaud = (PClk>>4) * bestm / (best_divisor * (bestm + bestd));

	/* reuse best_error to evaluate baud error*/
	if(baud > recalcbaud) 
		best_error = baud - recalcbaud;
	else 
		best_error = recalcbaud - baud;

	best_error = best_error * 100 / baud;

	if (best_error < UART_ACCEPTED_BAUDRATE_ERROR){			
		pUart->LCR |= LCR_DLAB_EN;
		pUart->DLM = (best_divisor>>8) & 0xFF;
		pUart->DLL = best_divisor & 0xFF;
	
		/* Then reset DLAB bit */
		pUart->LCR &= (~LCR_DLAB_EN) & 0xFF;
		pUart->FDR = ( (bestm << 4)&0xF0 | (bestd & 0x0F)) & 0xFF;
	}
	else
		return 0;
	
	return 1;
}

void UART2_Init( uint32_t baud )
{
	LPC_UART_TypeDef *pUart;
	pUart = LPC_UART2;
	LPC_PINCON->PINSEL0 |= (1<<20); //定义管脚
	LPC_PINCON->PINSEL0 |= (1<<22);
	LPC_SC->PCONP = LPC_SC->PCONP | (1<<24); //为UART供电
	uart_set_divisors( pUart, baud ); //设置波特率
	pUart->LCR = LCR_LENGTH_8 | LCR_STOP_1; //设置8位字宽，1个停止位
	pUart->FCR = 0x00; //不使用缓冲区
}

void UART2_SendByte( uint8_t ucData )
{
	while( !( LPC_UART2->LSR & LSR_THRE ) ); //THRE如果为0执行空循环
	LPC_UART2->THR = ucData; //发送数据
}

uint8_t UART2_GetByte( void )
{
	while( !( LPC_UART2->LSR & LSR_RDR ) ); //RDR位如果为0执行空循环
	return( LPC_UART2->RBR ); //返回接收到的数据
}

unsigned char  UART2_GetByte_Unblock( void )
{
	if( !( LPC_UART2->LSR & LSR_RDR ) ) //RDR位如果为0函数结束
		return 0;
	return( LPC_UART2->RBR ); //返回接收到的数据
}

void UART2_SendString( unsigned char *s, uint32_t len )
{
	while(len--)
		UART2_SendByte(*s++); //循环逐一发送字符
}

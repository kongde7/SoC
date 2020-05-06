/****************************************Copyright (c)****************************************************
**    Nanjing University Jinling College
**
** Created by:          James Ho
** Created date:        2012-04-11
** Version:             V1.00
** Descriptions:        For Embedded System Design Course
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "lpc17xx.h"                              /* LPC17xx definitions    */
#include "lpc1768_uart.h"

#define UART_BAUD_115200     115200                                     /* 串口波特率             */
extern uint32_t SystemCoreClock;
static uint8_t uart_set_divisors(LPC_UART_TypeDef *pUart, uint32_t baud);

RX_BUF_TypeDef RxBuf;

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
	LPC_UART_TypeDef*pUart;
	pUart=LPC_UART2;
	
	RxBuf.available = 0; //清空
	RxBuf.len = 0;
	
	LPC_PINCON->PINSEL0 |= (1<<20); //定义管脚
	LPC_PINCON->PINSEL0 |= (1<<22);
	LPC_SC->PCONP = LPC_SC->PCONP | (1<<24); //为UART供电
	uart_set_divisors( pUart, baud ); //设置波特率
	
	LPC_UART2->LCR = LCR_LENGTH_8 | LCR_STOP_1; //设置8位字宽，1个停止位
	LPC_UART2->FCR = ( FCR_FIFO_ENABLE | FCR_RX_FIFO | FCR_TX_FIFO ); //使用缓冲区
	LPC_UART2->IER= ( IER_RBR_ENABLED | IER_THRE_ENABLED ); //允许收发中断
	NVIC_EnableIRQ( UART2_IRQn ); //运行中断服务程序
}

void UART2_SendByte( unsigned char data )
{
	LPC_UART2->THR = data; //发送数据
}

unsigned char UART2_GetByte( void )
{
	return( LPC_UART2->RBR ); //返回接收到的数据
}

uint32_t StrLen;		// local variable to store the length of data that will be sent.
unsigned char *pSendStr;	// pointer to the string that is still in buffer.

void UART2_SendString (unsigned char *s, uint32_t len) 
{
	int i;
	StrLen = len;
	LPC_UART2->IER |= IER_THRE_ENABLED;

	/* UART has a 16 bytes buffer, so max 16 bytes can be filled in every time */
	for(i=0; i<((len<=16) ? len : 16); i++){
		LPC_UART2->THR = *s++;
		pSendStr = s;
		StrLen -- ;
	}
}

UART2_IRQHandler(void){
	uint32_t int_status;
	uint32_t lsr_status;
	char ch;
	int i, left;
	
	int_status = LPC_UART2->IIR;
	
	switch(int_status & 0x0F){
		case IIR_RLS:  			// this is the first priority interrupt.
		lsr_status = LPC_UART2->LSR;
		break;

		case IIR_RDA:
		if( (ch = LPC_UART2->RBR ) != '\r'){
			//UART2_SendByte(RxBuf.RxData[RxBuf.len++] = ch);
			RxBuf.RxData[RxBuf.len++] = ch;
		}
		else{
			RxBuf.available = 1;
			UART2_SendByte('\n');
		}
		UART2_SendByte(ch);
		break;
		
		case IIR_THRE:
		LPC_UART2->IER &= ~IER_THRE_ENABLED;
		lsr_status = LPC_UART2->LSR;
		switch(lsr_status & 0x60){
			case LSR_THRE:
				left = StrLen;
				for(i=0; i<((left<=16) ? left : 16); i++){
					LPC_UART2->THR = *pSendStr++;
					StrLen--;
				}
			break;
			
			case LSR_TEMP:				
				/* all data has been sent, Tx could be disabled*/
				//LPC_UART2->IER &= ~IER_THRE_ENABLED;
			break;
		}
		break;
	}
}

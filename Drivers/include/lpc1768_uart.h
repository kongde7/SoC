/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-07-01
** Version:             V1.00
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#ifndef __LPC1768_UART_H
#define __LPC1768_UART_H
#include <stdint.h>
#include <string.h>

// Baud definition
#define UART_BAUD_38400	     38400
#define UART_BAUD_57600	     57600
#define UART_BAUD_115200     115200      


/* Part 2: the following definitions are for LPC1768 UART Controller */

// Definitin of Interrupt Enable Register
#define IER_RBR_ENABLED		(0x01UL << 0)		// Receive Data Available
#define IER_THRE_ENABLED	(0x01UL << 1)		// Transmit Hold Register Empty
#define IER_RXLINE_ENABLED	(0x01UL << 2)		// Receive Line Status Interrupt
#define IER_ABEO_ENABLED	(0x01UL << 8)		// End of Auto Baud
#define IER_ABTO_ENABLED	(0x01UL << 9)		// Timeour of Auto Baud

// Definition of Interrupt Identification Register
#define IIR_ISTATUS		(0x01UL << 0)		// Interrupt occurs
#define IIR_RLS			(0x03UL << 1)		// Receive Line Status
#define IIR_RDA			(0x02UL << 1)		// Receive Data Available
#define IIR_CTI			(0x06UL << 1)		// Characer Timeout
#define IIR_THRE		(0x01UL << 1)		// Transmit Hold Register Empty
#define IIR_FIFO		(0x03UL << 6)		// copies of FCR
#define IIR_ABEO		(0x01UL << 8)		// Auto Baud End
#define IIR_ABTO		(0x01UL << 9)		// Auto Baud Timeout

// Definition of FIFO Control Register (Write Only Register)
#define FCR_FIFO_ENABLE		(0x01UL << 0)
#define FCR_RX_FIFO		(0x01UL << 1)
#define FCR_TX_FIFO		(0x01UL << 2)
#define FCR_DMA_MODE		(0x01UL << 3)
#define FCR_RX_TRIGGER		(0x03UL << 6)

// Definition of Line Status Register
#define LSR_RDR			(0x01UL << 0)		// Receiver Data Ready
#define LSR_OE			(0x01UL << 1)		// Overrun Error
#define LSR_PE			(0x01UL << 2)		// Parity Error
#define LSR_FE			(0x01UL << 3)		// Frame Error
#define LSR_BI			(0x01UL << 4)		// Break Interrupt
#define LSR_THRE		(0x01UL << 5)		// THR Empty
#define LSR_TEMP		(0x01UL << 6) 		// THR and TSR are empty
#define LSR_RXFE		(0x01UL << 7)		// Rx FIFO Error

#define LCR_LENGTH_5		(0x00)
#define LCR_LENGTH_6		(0x01)
#define LCR_LENGTH_7		(0x02)
#define LCR_LENGTH_8		(0x03)
#define LCR_STOP_1		(0x00 << 2)
#define LCR_STOP_2		(0x01 << 2)
#define LCR_PARITY_OFF		(0x00 << 3)
#define LCR_PARITY_EN		(0x01 << 3)
#define LCR_PARITY_ODD		(0x00 << 4)
#define LCR_PARITY_EVEN		(0x01 << 4)
#define LCR_PARITY_1		(0x02 << 4)
#define LCR_PARITY_0		(0x03 << 4)
#define LCR_BREAK_EN		(0x01 << 6)
#define LCR_DLAB_EN		(0x01 << 7)

#define UART_ACCEPTED_BAUDRATE_ERROR	(3)			/*!< Acceptable UART baudrate error */


#define RX_BUFFER_LENGTH	32
typedef struct {
	uint32_t len;
	uint32_t available;
	char RxData[RX_BUFFER_LENGTH];
} RX_BUF_TypeDef;

extern RX_BUF_TypeDef RxBuf;



void UART2_Init (uint32_t baud);
void UART2_SendByte (unsigned char ucData);
unsigned char  UART2_GetByte (void);
unsigned char  UART2_GetByte_Unblock (void);
void UART2_SendString (unsigned char *s, uint32_t len);

#define show(str) UART2_SendString((unsigned char*)str, strlen((char*) str))

#endif

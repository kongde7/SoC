/**************************************************************************//**
 * $Id: lpc12xx_i2c.c 584 2010-09-25 13:49:30Z cnh20446 $
 *
 * @file     lpc12xx_i2c.c
 * @brief    Contains all functions support for I2C firmware library on LPC12xx.
 * @version  1.0
 * @date     26. Sep. 2010
 * @author   NXP MCU Team
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/
 
 /* Peripheral group ----------------------------------------------------------- */
/** @addtogroup I2C
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc17xx.h"
#include "lpc1768_i2c.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */

#define _I2C
#ifdef _I2C

/** @defgroup 	I2C_Private_Functions
 * @{
 */

/* Private variables ---------------------------------------------------------*/

/* In order to start the I2C, all the parameters 
must be set in advance, including I2CWriteLength, I2CReadLength,
and the I2cMasterBuffer which contains the stream
command/data to the I2c slave device. 
(1) If it's a I2C write only, the number of bytes to be written is 
I2CWriteLength, I2CReadLength is zero, the content will be filled 
in the I2CMasterBuffer. 
(2) If it's a I2C read only, the number of bytes to be read is 
I2CReadLength, I2CWriteLength is 0, the read value will be filled 
in the I2CSlaveBuffer. 
(3) If it's a I2C Write/Read with repeated start, specify the 
I2CWriteLength, fill the content of bytes to be written in 
I2CMasterBuffer, specify the I2CReadLength, after the repeated 
start and the device address with RD bit set, the content of the 
reading will be filled in I2CSlaveBuffer. 
	
e.g. Start, DevAddr(W), WRByte1...WRByteN, Repeated-Start, DevAddr(R), 
RDByte1...RDByteN Stop. The content of the reading will be filled 
in I2CSlaveBuffer. */

volatile uint8_t *pI2CMasterBuffer;
volatile uint8_t *pI2CSlaveBuffer;
volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t I2CReadLength, I2CWriteLength;
volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

/*********************************************************************/
/**
  * @brief  Get I2C Status Byte.
  *
  *	@param	
  * @retval The value of the status byte.  
  */
uint8_t I2C_GetI2CStatus( void )
{
	return (LPC_I2C0->I2STAT);
}

/**
  * @brief  Read the I2C_I2CONSET bit.
  *
  *	@param  I2C_I2CONSET: specifies the bits will be read.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *				@arg I2C_I2CONSET_STO: STOP flag
  *  			        @arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval The I2C_CONSET bit value.  
  */
uint8_t I2C_ReadFlag( uint8_t I2C_I2CONSET )
{
  	//CHECK_PARAM(PARAM_I2C_I2CONSET(I2C_I2CONSET));

	return(LPC_I2C0->I2CONSET & I2C_I2CONSET);      /* retuen flag */
}

/**
  * @brief  Set the I2C_I2CONSET bit.
  *
  *	@param  I2C_I2CONSET: specifies the bits will be set.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *				@arg I2C_I2CONSET_STO: STOP flag
  *  			        @arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval None  
  */
void I2C_SetFlag( uint8_t I2C_I2CONSET )
{
  	//CHECK_PARAM(PARAM_I2C_I2CONSET(I2C_I2CONSET));

	LPC_I2C0->I2CONSET = I2C_I2CONSET;      /* Set flag */
}

/**
  * @brief  Clear the I2C_I2CONCLR bit.
  *
  *	@param  I2C_I2CONCLR: specifies the bits will be clear.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *  			        @arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval  
  */
void I2C_ClearFlag( uint8_t I2C_I2CONCLR )
{
	//CHECK_PARAM(PARAM_I2C_I2CONCLR(I2C_I2CONCLR));

	LPC_I2C0->I2CONCLR = I2C_I2CONCLR;      /* Clear flag */
}

/**
  * @brief  
  *
  *	@param  DataByte: specifies the data byte will be sent.
  * @retval None 
  */
void I2C_SendByte( uint8_t DataByte )
{
	LPC_I2C0->I2DAT = DataByte; 
}
/**
  * @brief  
  *
  *	@param	
  * @retval The byte read from DAT register.  
  */

uint8_t I2C_GetByte( void )
{
	return(LPC_I2C0->I2DAT);
}

/**
  * @brief  Setup clock rate for I2C peripheral.
  *
  *	@param	TargetClock: speed of I2C bus(bps).
        The speed could be 100KHz, 400KHz
  * @retval None  
  */
void I2C0_SetClock (uint32_t TargetClock)
{
	uint32_t temp;

	temp = SystemCoreClock  / TargetClock;

	/* Set the I2C clock value to register */
	LPC_I2C0->I2SCLH = (uint32_t)(temp / 2);
	LPC_I2C0->I2SCLL = (uint32_t)(temp - LPC_I2C0->I2SCLH);
}

/**
  * @brief  De-initializes the I2C peripheral registers to their default reset values.
  *
  * @param  None
  * @retval None  
  */
void I2C_DeInit(void)
{
//	LPC_I2C0->I2CONCLR = I2C_I2CONCLR_I2ENC;		/*!< Disable I2C control */
//	LPC_SC->SYSAHBCLKCTRL  =  LPC_SC->SYSAHBCLKCTRL & (~(1<<5));	/*!< Disable power for I2C module */
}

/**
  * @brief  Initializes the i2c peripheral with specified parameter.
  *
  * @param  i2C_InitStruct: pointer to a I2C_InitStruct structure that 
  *         contains the configuration information for the I2C peripheral.
  * @retval None
  */
void I2C0_Init(I2C_InitTypeDef* I2C_InitStuct)
{
        LPC_SC->PCONP |= (1 << 7);
        
	/*  I2C0 I/O config, P0.27/P0.28 are used as I2C interface */    
        LPC_PINCON->PINSEL1 &= ~((0x03UL << 22) | (0x03UL << 24));
        LPC_PINCON->PINSEL1 |= ((0x01 << 22) | (0x01 << 24));
            
	/*--- Clear flags ---*/
	LPC_I2C0->I2CONCLR = I2C_I2CONCLR_AAC | 
	                     I2C_I2CONCLR_SIC | 
	                     I2C_I2CONCLR_STAC | 
	                     I2C_I2CONCLR_I2ENC;    

	/*--- Set Clock rate ---*/
	I2C0_SetClock( I2C_InitStuct -> ClockRate );

	if ( I2C_InitStuct -> Mode == I2C_SLAVE )
	{
		LPC_I2C0->I2ADR0 = I2C_InitStuct -> SlaveAddress;
	}    

	/* Enable the I2C Interrupt */
	if (I2C_InitStuct -> InterruptMode == I2C_INTERRUPT_MODE )
	{
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	else if (I2C_InitStuct -> InterruptMode == I2C_POLLING_MODE ) 	
	{
		NVIC_DisableIRQ(I2C0_IRQn);		/* Disable the I2C Interrupt */
	}

	if ( I2C_InitStuct -> Mode == I2C_MASTER )
	{
		LPC_I2C0->I2CONSET = I2C_I2CONSET_I2EN;
	} 
	else if (I2C_InitStuct -> Mode == I2C_SLAVE)
	{
		LPC_I2C0->I2CONSET = I2C_I2CONSET_I2EN | I2C_I2CONSET_SI;
	}
}

/**
  * @brief  Configures functionality in I2C monitor mode.
  *
  *	@param	I2C_I2MMCTRL: Monitor mode configuration type.
  *         This parameter can be one of the following values:
  * 			@arg I2C_I2MMCTRL_MM_ENA: Enable monitor mode.
  * 			@arg I2C_I2MMCTRL_ENA_SCL: I2C module can 'stretch'
  * 					the clock line (hold it low) until it has had time to
  * 					respond to an I2C interrupt.
  *			@arg I2C_I2MMCTRL_MATCH_ALL: When this bit is set to '1'
  * 					and the I2C is in monitor mode, an interrupt will be
  * 					generated on ANY address received.
  *	@param	NewState: New State of this function, should be:
  * 			@arg ENABLE: Enable this function.
  * 			@arg DISABLE: Disable this function.
  * @retval None  
  */
/*
void I2C_MonitorModeConfig(uint32_t I2C_I2MMCTRL, FunctionalState NewState)
{
//	CHECK_PARAM(PARAM_I2C_I2MMCTRL(I2C_I2MMCTRL));

	if (NewState == ENABLE)
	{
		LPC_I2C0->MMCTRL |= I2C_I2MMCTRL;
	}
	else
	{
		LPC_I2C0->MMCTRL &= (~I2C_I2MMCTRL);
	}
}
*/
/**
  * @brief  Get data from I2C data buffer in monitor mode.
  *
  * @param  None
  * @retval The value read from DATA_BUFFER.  
  *
  * Note:	In monitor mode, the I2C module may lose the ability to stretch
  * 		the clock (stall the bus) if the ENA_SCL bit is not set. This means that
  * 		the processor will have a limited amount of time to read the contents of
  * 		the data received on the bus. If the processor reads the I2DAT shift
  * 		register, as it ordinarily would, it could have only one bit-time to
  * 		respond to the interrupt before the received data is overwritten by
  * 		new data.
  */
uint8_t I2C_MonitorGetDatabuffer(void)
{
	return ((uint8_t)(LPC_I2C0->I2DATA_BUFFER));
}

/**
  * @brief  General Master Interrupt handler for I2C peripheral
  *
  * @param  None
  * @retval None
  */

void I2C0_IRQHandler(void) 
{
	uint8_t StatValue;
	
	StatValue = I2C_GetI2CStatus();
	switch ( StatValue )
	{
		/* 0x08: A Start condition is issued. */
		case I2C_I2STAT_M_TX_START:			
		WrIndex = 0;
		I2C_SendByte(pI2CMasterBuffer[WrIndex++]);
		I2C_ClearFlag(I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC);
		break;
		
		/* 0x10: A repeated started is issued */
		case I2C_I2STAT_M_TX_RESTART:			
		RdIndex = 0;
		I2C_SendByte(pI2CMasterBuffer[WrIndex++]); 	/* Send SLA with R bit set */
		I2C_ClearFlag(I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC);
		break;
		
		/* 0x18: Regardless, it's a ACK*/
		case I2C_I2STAT_M_TX_SLAW_ACK:			
		if ( I2CWriteLength == 1 )
		{
			I2C_SetFlag(I2C_I2CONSET_STO);      	/* Set Stop flag */
			I2CMasterState = I2C_NO_DATA;
		}
		else
		{
			I2C_SendByte(pI2CMasterBuffer[WrIndex++]);
		}
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/* 0x28: Data byte has been transmitted. */
		case I2C_I2STAT_M_TX_DAT_ACK:			
		if ( WrIndex < I2CWriteLength )
		{   
			/* this should be the last one */
			I2C_SendByte(pI2CMasterBuffer[WrIndex++]);	
		}
		else
		{
			if ( I2CReadLength != 0 )	  
			{
				I2C_SetFlag(I2C_I2CONSET_STA);	/* Set Repeated-start flag */
			}
			else
			{
				I2C_SetFlag(I2C_I2CONSET_STO);	/* Set Stop flag */
				I2CMasterState = I2C_OK;
			}
		}
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/* 0x30: Data has been transmitted, NACK has been received */ 
		case I2C_I2STAT_M_TX_DAT_NACK:			
		I2C_SetFlag(I2C_I2CONSET_STO);			/* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_DATA;
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/* 0x40: Master Receive, SLA_R has been sent */
		case I2C_I2STAT_M_RX_SLAR_ACK:			
                if ( (++RdIndex) < I2CReadLength )
		{
			/* Will go to State 0x50 */
			I2C_SetFlag(I2C_I2CONSET_AA);		/* assert ACK after data is received */
		}
		else
		{
			/* Will go to State 0x58 */
			I2C_ClearFlag(I2C_I2CONCLR_AAC);	/* assert NACK after data is received */
		}
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/* 0x50: Data byte has been received, regardless following ACK or NACK */
		case I2C_I2STAT_M_RX_DAT_ACK:			
		*pI2CSlaveBuffer++ = I2C_GetByte();
		if ( (++RdIndex) < I2CReadLength )
		{   
			I2C_SetFlag(I2C_I2CONSET_AA);		/* assert ACK after data is received */
		}
		else
		{
			I2C_ClearFlag(I2C_I2CONCLR_AAC);	/* assert NACK on last byte */
		}
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/*0x58: Data has been received, NACK has been return */
		case I2C_I2STAT_M_RX_DAT_NACK:			
		*pI2CSlaveBuffer++ = I2C_GetByte();
		I2CMasterState = I2C_OK;
		I2C_SetFlag(I2C_I2CONSET_STO);			/* Set Stop flag */ 
		I2C_ClearFlag(I2C_I2CONCLR_SIC);		/* Clear SI flag */
		break;
		
		/*0x20: SLA+W has been transmitted, NACK has been received */
		/*0x48: SLA+R has been transmitted, NACK has been received */
		case I2C_I2STAT_M_TX_SLAW_NACK:			
		case I2C_I2STAT_M_RX_SLAR_NACK:			
		I2C_SetFlag(I2C_I2CONSET_STO);			/* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_ADDRESS;
		I2C_ClearFlag(I2C_I2CONCLR_SIC);
		break;
		
		/*0x38: Arbitration lost, in this example, we don't deal with multiple master situation */
		case I2C_I2STAT_M_TX_ARB_LOST:			
		default:
		I2CMasterState = I2C_ARBITRATION_LOST;
		I2C_ClearFlag(I2C_I2CONCLR_SIC);	
		break;
	}
	return;
}

void I2C0_ReadWrite(uint8_t *pWrite, uint32_t WriteLen, uint8_t *pRead, uint32_t ReadLen)
{
        I2CWriteLength = WriteLen;
        pI2CMasterBuffer = pWrite;
        I2CReadLength = ReadLen;
        pI2CSlaveBuffer = pRead;
	RdIndex = 0;
	WrIndex = 0;
	I2CMasterState = I2C_IDLE;
	I2C_SetFlag(I2C_I2CONSET_STA);	        /* Set start condition*/

	while ( I2CMasterState != I2C_OK );        
}

#endif
/* --------------------------------- End Of File ------------------------------ */

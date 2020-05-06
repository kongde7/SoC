/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-07-01
** Version:             V1.00
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#ifndef __EEPROM_24LC02_H
#define __EEPROM_24LC02_H

#include "stdint.h"
#include "lpc1768_i2c.h"

#define I2C_SLA_24LC02          (0xA0)
#define LC02_PAGE_SIZE          (0x08)

typedef struct E2PROM_Data {
        uint8_t SLA;
        uint8_t Addr;
        uint8_t Data[LC02_PAGE_SIZE + 1];
} E2PROM_Data_Typedef;

/* write or read from current address */
/*
void E2PROM_Read(uint8_t *buf, uint8_t len);
void E2PROM_Write(uint8_t *buf, uint8_t len);
*/

/* write or read from specified address */
void E2PROM_ReadSeq(uint8_t addr, uint8_t *buf, uint8_t len);
void E2PROM_WriteSeq(uint8_t addr, uint8_t *buf, uint8_t len);

#endif

/****************************************Copyright (c)**************************************************** 
**    Nanjing University Jinling College
**
**--------------------------------------------------------------------------------------------------------
** Created by:          James Ho
** Created date:        2013-07-01
** Version:             V1.00
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc1768_i2c.h"
#include "24LC02.h"
/* 
input: 
	addr: E2PROM address that the data will be written to.
	*buf, data buffer that will be written to E2PROM.
	len, the data len that will be written to E2PROM.
*/

void E2PROM_WriteSeq(uint8_t addr, uint8_t *buf, uint8_t len){
        E2PROM_Data_Typedef EData;
        
        EData.SLA = I2C_SLA_24LC02;
        EData.Addr = addr;
        memcpy(EData.Data, buf, len);
        I2C0_ReadWrite((uint8_t *) &EData, len+2, 0, 0);
}

void E2PROM_ReadSeq(uint8_t addr, uint8_t *buf, uint8_t len) {
        E2PROM_Data_Typedef EData;
        
        EData.SLA = I2C_SLA_24LC02;
        EData.Addr = addr;
        EData.Data[0] = I2C_SLA_24LC02 | 0x01;  // SLA + R
        I2C0_ReadWrite((uint8_t *) &EData, 2, EData.Data, len);
        memcpy(buf, EData.Data, len);
}

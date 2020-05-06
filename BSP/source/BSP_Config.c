#include "lpc17xx.h"
#include "bmboard.h"

/* for Baoma & LandTiger Development Board */

/* Configure the LCD Control pins */
void LCD_Pin_Config(void){

	LPC_GPIO2->FIODIR   |= 0x000000FF;	/* P2.0 to P2/7 are for LEDs as well */
	
	LPC_GPIO0->FIODIR   |= 0x03F80000;	/*P0.19--P0.25           		*/
	LPC_GPIO0->FIOSET    = 0x03F80000;	/*ÉèÖÃP0.19--P0.25 ÎªÊä³ö, output 1	*/

}

void SD_Pin_Config(void)
{
    /* P3.26 is SD Card Power Supply Enable Pin */
    LPC_GPIO3->FIODIR   |=  (SD_PWR);            /* P3.26 is output             */
    /* P3.25 is SD Card Insert Dection Pin */
    LPC_GPIO3->FIODIR   &=  ~(SD_CD);            /* P3.25 is input             */
}

void SD_Power(uint8_t OnOff)
{
    if(OnOff == 1)
        LPC_GPIO3->FIOSET   &= ~(SD_PWR);            /* set P3.26 low(enable power) */
    else if (OnOff == 0)
        LPC_GPIO3->FIOCLR   |= (SD_PWR);		/* set P3.26 High (disable the power) */
}

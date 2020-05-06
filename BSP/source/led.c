#include "lpc17xx.h"
#include "bmboard.h"
#include "led.h"

/* on LT board, LED and LCD are both controled by P2 */
void Set_LCD_DIR_A2B() {
        LPC_GPIO0->FIODIR = (LCD_PIN_DIR | LCD_PIN_EN);     
        LPC_GPIO0->FIOSET =  LCD_PIN_DIR;
				LPC_GPIO0->FIOCLR = LCD_PIN_EN;
}

/*----------------------------------------------------------------------------
  configer LED pins
 *----------------------------------------------------------------------------*/
void LED_Config(void) {
        Set_LCD_DIR_A2B();
        LPC_PINCON->PINSEL4 = 0;	// function select: b00=GPIO
        LPC_GPIO2->FIODIR = LED_ALL; 
				LPC_GPIO2->FIOPIN &= ~(LED_ALL);
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *----------------------------------------------------------------------------*/
void LED_On (uint32_t led) {

        LPC_GPIO2->FIOSET =  led;                  /* Turn On  LED */
}

/*------------------------------------------------------------------------------
  Switch off LEDs
 *-----------------------------------------------------------------------------*/
void LED_Off (uint32_t led) {

        LPC_GPIO2->FIOCLR = led;                  /* Turn Off LED */
}

void Toggle(uint8_t position)
{
      LPC_GPIO2->FIOPIN = ( LPC_GPIO2->FIOPIN & ~position ) | ( ~LPC_GPIO2->FIOPIN & position );
	  /* 把原来的亮灯部分，加上现在要显示的亮灯部分 */
}

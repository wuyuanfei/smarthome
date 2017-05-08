/*******************************************************************************
* File:    led_spk_fan_adc.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   LED & Speaker & FAN & ADC program body
********************************************************************************
********************************************************************************
* History:
* 2011.05.10:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "adc.h"

#include "oled2864.h" 
#include "key.h"
#include "tools.h"
#include "menu.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : ADC_Test
* Description    : ADC test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Test(void)
{
  char 		buf[25];
  uint32_t	adv;
  float		vol;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "Voltage");
  printf("\r\n");

  ADCInit(ADC_CLK);

  while(KEY_Read() != KEY_ESC)
  {
    adv = ADCRead(0);
	vol = adv*3.3/1023;
	adv = vol*10;
	snprintf(buf, 21, "%d.%dV   ", adv/10, adv%10);
	OLED_DisStrLine(2, 0, (uint8_t *)buf);
	printf("%d.%dV  ", adv/10, adv%10);

//	adv = ADCRead(3);
//	snprintf(buf, 16, "ADC3: %d   ", adv);
//	OLED_DisStrLine(3, 0, (uint8_t *)buf);
//	printf("\r\nADC3: %d", adv);

	delay_ms(250);
  }
}
 

/**** End of File ****/

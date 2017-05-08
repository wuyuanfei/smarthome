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

#include "oled2864.h" 
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
* Function Name  : LED_Menu_01
* Description    : LED test Menu.
* Input          : - numb, Function number
* Output         : None
* Return         : None
*******************************************************************************/
void Dis_Menu(Menu_TypeDef menu)
{
  uint16_t	page, row, i;

  page = (menu.numb-1) / 6;
  i = (page+1) * 6;
  if(i < menu.max_numb)
    row = 5;
  else
    row = (menu.max_numb-1) % 6;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, (uint8_t *)menu.title);
  OLED_DisChar(0, 6*17, 0, 'P');
  OLED_DisChar(0, 6*18, 0, 'g');
  OLED_DisChar(0, 6*19, 0, ' ');
  OLED_DisChar(0, 6*20, 0, 0x30+1+page);

  for(i=0; i<=row; i++)
  {
    if(menu.numb == (page*6 + i + 1))
	  OLED_DisStrLine(2+ i, 1, (uint8_t *)menu.item[page*6+i]);
	else
	  OLED_DisStrLine(2+ i, 0, (uint8_t *)menu.item[page*6+i]);
  }
}
 

/**** End of File ****/

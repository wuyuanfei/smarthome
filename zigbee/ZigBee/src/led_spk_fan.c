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
* Function Name  : LED_Test
* Description    : LED test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "LED";
  menu.item[0] = "1.Turn on LED";
  menu.item[1] = "2.Turn off LED";

  Dis_Menu(menu);

  delay_ms(250);
  
  while(menu.numb)
  {
    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
	        GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2
		    break;

		  case 2:
		    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	        GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2		    
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	if((key!=KEY_NONE) && (menu.numb!=0))
	{
	  delay_ms(250);
	}
  }
}

/*******************************************************************************
* Function Name  : Speaker_Test
* Description    : Speaker test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Speaker_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "Speaker";
  menu.item[0] = "1.Turn on Speaker";
  menu.item[1] = "2.Turn off Speaker";

  Dis_Menu(menu);

  delay_ms(250);
  
  while(menu.numb)
  {
    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			GPIOSetValue(PORT1, 1, 0);		// PIO1_1 output 0, Turn on Speaker
		    break;

		  case 2:
	        GPIOSetValue(PORT1, 1, 1);		// PIO1_1 output 1, Turn off Speaker		    
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	if((key!=KEY_NONE) && (menu.numb!=0))
	{
	  delay_ms(250);
	}
  }
}

/*******************************************************************************
* Function Name  : FAN_Test
* Description    : FAN test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Fan_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "FAN";
  menu.item[0] = "1.Turn on FAN";
  menu.item[1] = "2.Turn off FAN";

  Dis_Menu(menu);

  delay_ms(250);
  
  while(menu.numb)
  {
    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 1, Turn on FAN
		    break;

		  case 2:
	        GPIOSetValue(PORT0, 2, 0);		// PIO0_2 output 0, Turn off FAN		    
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	if((key!=KEY_NONE) && (menu.numb!=0))
	{
	  delay_ms(250);
	}
  }
}
 

/**** End of File ****/

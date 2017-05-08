/*******************************************************************************
* File Name          : tools.h
* Author             : Farsight Design Team
* Version            : V1.00
* Date               : 2011.06.21
* Description        : Endpoint CTR (Low and High) interrupt's service routines
*                      prototypes
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/
#include "lpc11xx.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint16_t	max_numb;
  uint16_t	numb;
  char 		*title;
  char		*item[32];
} Menu_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Dis_Menu(Menu_TypeDef menu);

/* External variables --------------------------------------------------------*/

#endif /* __MENU_H */

/**** End of File ****/



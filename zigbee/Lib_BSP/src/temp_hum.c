/*******************************************************************************
* File:    temp_hum.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Temperature & humidity program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "gpio.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "temp_hum.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern volatile uint32_t p3_2_counter;
extern volatile uint32_t p3_2_tc;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Read_Temp_Hum
* Description    : Read Temp & Hum.
* Input          : - temp : temp buf pointer.
*                  - hum : hum buf pointer.
* Output         : - temp[0] : temp x. part
*                  - temp[1] : temp .x part
*                  - hum[0] : hum x. part
*                  - hum[1] : hum .x part
* Return         : - 1, checksum ok
*                  - 0, checksum fail
*******************************************************************************/
uint32_t Read_Temp_Hum(uint8_t *temp, uint8_t *hum)
{
  uint32_t	cnt_last;
  uint8_t	hum_10, hum_01, temp_10, temp_01, chksum, chk;
  uint32_t	tc1, tc;
  uint32_t	i;

  p3_2_counter = 0;
  cnt_last = p3_2_counter;

  GPIOSetDir(PORT1, 5, 1);			// Set PIO1_5 to output
  GPIOSetValue(PORT1, 5, 0);
  delay_ms(30);
  GPIOSetValue(PORT1, 5, 1);

  GPIOSetDir(PORT1, 5, 0);
  GPIOSetInterrupt(PORT1, 5, 0, 0, 0);
  GPIOIntEnable(PORT1, 5);

  for(i=0; i<3; i++)
  {
    GPIOSetInterrupt(PORT1, 5, 0, 0, i&0x01);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;
  }

  for(i=0; i<40; i++)
  {
	GPIOSetInterrupt(PORT1, 5, 0, 0, 1);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;
	tc1 = p3_2_tc;

	GPIOSetInterrupt(PORT1, 5, 0, 0, 0);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;

	if(p3_2_tc < tc1)
	{
	  tc = tc1 - p3_2_tc;
	}
	else
	{
	  tc = 48000 - (p3_2_tc - tc1);
	}

	if(i < 8)
	{
	  hum_10 <<= 1;
	  if(tc >= 2328)
	    hum_10 |= 0x01;
	}
	else if(i < 16)
	{
	  hum_01 <<= 1;
	  if(tc >= 2328)
	    hum_01 |= 0x01;
	}
	else if(i < 24)
	{
	  temp_10 <<= 1;
	  if(tc >= 2328)
	    temp_10 |= 0x01;
	}
	else if(i < 32)
	{
	  temp_01 <<= 1;
	  if(tc >= 2328)
	    temp_01 |= 0x01;
	}
	else
	{
	  chksum <<= 1;
	  if(tc >= 2328)
	    chksum |= 0x01;
	}
  }

  GPIOSetInterrupt(PORT1, 5, 0, 0, 1);
  while(p3_2_counter == cnt_last);

  *temp = temp_10;
  *(temp+1) = temp_01;

  *hum = hum_10;
  *(hum+1) = hum_01;

  chk = hum_10;
  chk += hum_01;
  chk += temp_10;
  chk += temp_01;

  if(chk == chksum)
    return 1;
  else
    return 0;
}

/*******************************************************************************
* Function Name  : Temp_Hum_Test
* Description    : Temp & Hum func test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Temp_Hum_Test(void)
{
  uint8_t	temp[2], hum[2];
  char		buf[20];
  uint32_t	i, fan_on=0;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "Temp & Hum");

  while(1)
  {
    i = Read_Temp_Hum(temp, hum);

	if(i)
	{
	  if(temp[0]&0x80)
	  {
	    temp[0] &= 0x7f;
        snprintf(buf, 16, "Temp: -%d.%d  ", temp[0], temp[1]);
	  }
	  else
	  {
	    snprintf(buf, 16, "Temp: %d.%d  ", temp[0], temp[1]);
	  }
	  OLED_DisStrLine(2, 0, (uint8_t *)buf);
	  snprintf(buf, 16, "Hum:  %d.%d  ", hum[0], hum[1]);
	  OLED_DisStrLine(3, 0, (uint8_t *)buf);

	  printf("\r\nTemp: %d.%d   Hum:  %d.%d", temp[0], temp[1], hum[0], hum[1]);
    }
    else
    {
      printf("\r\nfail");
    }

	if((temp[0]>50)	&& (fan_on==0))
	{
	  GPIOSetValue(PORT0, 2, 1);		// Turn on Fan
	  OLED_DisStrLine(2-1 + 6, 0, (uint8_t *)"Temp>50, Turn on FAN");
      printf("\r\nTemp>50, Turn on FAN");
	  fan_on = 1;
	}

	if((temp[0]<45) && (fan_on!=0))
	{
	  GPIOSetValue(PORT0, 2, 0);		// Turn on Fan
	  OLED_DisStrLine(2-1 + 6, 0, (uint8_t *)"Temp<45, Turn off FAN");
      printf("\r\nTemp<45, Turn off FAN");
	  fan_on = 0;
	}

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;
  } 
}


/**** End of File ****/



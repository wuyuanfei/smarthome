/*******************************************************************************
* File:    main.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Main program body
********************************************************************************
********************************************************************************
* History:
* 2011.05.10:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "rom_drivers.h"
#include "light.h"

#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : SysTick interrupt Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{

}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	uint8_t	rd = 0;
	uint8_t rd_buf[20] = {0};//for saving each rd
	int c_count=0;//the count of rd_buf
	/*
	msg_type:	消息类型
	msg_devnum：设备号
	msg_switch：开关状态

	char msg_type = 0;
	char msg_devnum = 0;
	char msg_switch = 0;	
	*/

	
  SystemInit();
  GPIOInit();						// Enables clock for GPIO
  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */
	// LED
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1
  GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
  GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2
  // Fan
  GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
  GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 1, close FAN
  // beep
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;
  GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
  GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  //NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();
  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);
  printf("\r\n\r\nZigBee");
  delay_ms(250);
  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);
  SPI752_Init(1, 115200);
	/*
	I2CInit(I2CMASTER, 0);
	light_init();
	light_enable();
	light_setRange(LIGHT_RANGE_4000);
	*/


	while(1){				//while_start			
		if(ZigBee_GetChar(&rd)){
			UART0_PutChar(rd);
			rd_buf[c_count]=rd;
			c_count++;
			}	
			
		if(c_count>=4){
			printf("%s",rd_buf);
			if(rd_buf[0]==0xdd)
			{			
						switch(rd_buf[1]){
								case 0x00:
										if(rd_buf[2]==0x00){
											if(rd_buf[3]==0x00){
												send_temp_value();
												//memset(rd_buf,0,5);
												//c_count=0;
											}
										}
										break;
								
								case 0x01:
										if(rd_buf[2]==0x00){
											if(rd_buf[3]==0x00){
												send_hum_value();
												//memset(rd_buf,0,5);
												//c_count=0;
											}
										}
										break;
										
								case 0x02:		//request for light
										if(rd_buf[2]==0x00){
											if(rd_buf[3]==0x00){
												send_light_strength();
												//memset(rd_buf,0,5);
												//c_count=0;
												}
										}
										break;	

								case 0x03:
									if(rd_buf[2]==0x00){
										if(rd_buf[3]==0x00){
											send_xyz_value();
											//memset(rd_buf,0,5);
											//c_count=0;
										}
									}
									break;
							
								case 0x04:
									if(rd_buf[2] == 0x00)  //LED1
										{
											if(rd_buf[3] == 0x00){
												//开LED1	
												GPIOSetValue(PORT3, 0, 0);	
												//回发硬件状态消息
												ZigBee_PutChar(0xcc);
												ZigBee_PutChar(0x04);
												ZigBee_PutChar(0x00);
												ZigBee_PutChar(0x00);
												//memset(rd_buf,0,5);
												//c_count=0;
											}
											else if(rd_buf[3] == 0x01){
												//LED1 off
												GPIOSetValue(PORT3, 0, 1);
												//回发硬件状态消息
												ZigBee_PutChar(0xcc);
												ZigBee_PutChar(0x04);
												ZigBee_PutChar(0x00);
												ZigBee_PutChar(0x01);
												//memset(rd_buf,0,5);
												//c_count=0;
											}
										}			
									else if(rd_buf[2] == 0x01)  //LED2
									{
										if(rd_buf[3] == 0x00){
											//LED2 ON					
											GPIOSetValue(PORT3, 1, 0);
											//回发硬件状态消息
											ZigBee_PutChar(0xcc);
											ZigBee_PutChar(0x04);
											ZigBee_PutChar(0x01);
											ZigBee_PutChar(0x00);
											//memset(rd_buf,0,5);
											//c_count=0;
										}
										if(rd_buf[3] == 0x01){
											//LED2 OFF
											GPIOSetValue(PORT3, 1, 1);
											//回发硬件状态消息
											ZigBee_PutChar(0xcc);
											ZigBee_PutChar(0x04);
											ZigBee_PutChar(0x01);
											ZigBee_PutChar(0x01);
											//memset(rd_buf,0,5);
											//c_count=0;
										}
								 }			
								break;		
										
							case 0x05:   //Fan
									if(rd_buf[2] == 0x00)  
									{
										if(rd_buf[3] == 0x00){
											GPIOSetValue(PORT0, 2, 0);//fan on
											ZigBee_PutChar(0xcc);
											ZigBee_PutChar(0x05);
											ZigBee_PutChar(0x00);
											ZigBee_PutChar(0x00);
											//memset(rd_buf,0,5);
											//c_count=0;
										}
										if(rd_buf[3] == 0x01){
											GPIOSetValue(PORT0, 2, 1);//fan off
											ZigBee_PutChar(0xcc);
											ZigBee_PutChar(0x05);
											ZigBee_PutChar(0x00);
											ZigBee_PutChar(0x01);
											//memset(rd_buf,0,5);
											//c_count=0;
										}
									}			
								break;
										
							case 0x06:   //Beep
									if(rd_buf[2] == 0x00) 
									{
										if(rd_buf[3] == 0x00){
											//Beep On & Off	
											speaker_op(1);		
											delay_ms(1000);
											speaker_op(0);
											ZigBee_PutChar(0xcc);
											ZigBee_PutChar(0x06);
											ZigBee_PutChar(0x00);
											ZigBee_PutChar(0x00);
											//memset(rd_buf,0,5);
											//c_count=0;
										}
									}		
									break;
					
											

									
							default:
								//memset(rd_buf,0,5);
								//c_count=0;
								break;
					}			//end of switch
										
			}	else{
					
			}

			memset(rd_buf,0,sizeof(rd_buf));
			c_count=0;
	}//if_end
	

 }	//while_end
 
}//main_end
/**** End of File ****/

#include <stdio.h>
#include "lpc11xx.h"
#include "i2c.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "gpio.h"
#include "light.h"
#include <string.h>
#include "clkconfig.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "rom_drivers.h"
#include "spi_uart.h"

/*************************************************
        (LJQ)the function of beeping.
*************************************************/
volatile uint32_t period = 1500;
void speaker_op(uint8_t pwm_set)  //´«1Ïì ´«0²»Ïì
{
	switch(pwm_set)
	{
		case 1:
			init_timer32PWM(1, period, MATCH0);
			LPC_TMR32B1->PR = 0x7f;
			enable_timer32(1);

	   	break;
		case 0:
		//	init_timer32PWM(1, 0, MATCH0);
		//	delay_ms(50);
			disable_timer32(1);
		break;
		default:
			break;
	}
}
/*************************************************
        (LJQ)the function of beeping£®
*************************************************/


void send_light_strength(void)	//send light strength to zigbee
{
  char		buf[24];
  uint32_t	lux;
	//int count=0;
	//char light_strength[24];
	
	OLED_ClearScreen();

  I2CInit(I2CMASTER, 0);
  light_init();
  light_enable();
  light_setRange(LIGHT_RANGE_4000);

  //while(1)
  //{	
    lux = light_read();
    snprintf(buf, 20, "light : %d", lux);
    OLED_DisStrLine(2, 0, (uint8_t *)buf);
		//printf("light:%d\r\n", lux);
		//for(count=0;count<sizeof(light_strength);count++){
			ZigBee_PutChar(0xcc);
			ZigBee_PutChar(0x02);
			ZigBee_PutChar(0x00);
			ZigBee_PutChar(lux);
			//ZigBee_PutChar(light_strength[count]);
		//}
	//delay_ms(300);

  //}
}





void send_hum_value(){
	uint8_t	temp[2], hum[2];
  char		buf[20];
  uint32_t	i, fan_on=0;

  OLED_ClearScreen();
	
	i = Read_Temp_Hum(temp, hum);
	if(i){
		snprintf(buf, 16, "Hum:  %d.%d  ", hum[0], hum[1]);
		OLED_DisStrLine(1, 0, (uint8_t *)buf);
		ZigBee_PutChar(0xcc);
		ZigBee_PutChar(0x01);
		ZigBee_PutChar(0x00);
		ZigBee_PutChar(hum[0]);
		ZigBee_PutChar(hum[1]);
	}
}


void send_temp_value(){
	uint8_t	temp[2], hum[2];
  char		buf[20];
  uint32_t	i;

  OLED_ClearScreen();
	
	i = Read_Temp_Hum(temp, hum);
	if(i){
		if(temp[0]&0x80)
	  {
	    temp[0] &= 0x7f;
        snprintf(buf, 16, "Temp: -%d.%d  ", temp[0], temp[1]);
	  }
	  else
	  {
	    snprintf(buf, 16, "Temp: %d.%d  ", temp[0], temp[1]);
	  }
	  OLED_DisStrLine(0, 0, (uint8_t *)buf);
		
		ZigBee_PutChar(0xcc);
		ZigBee_PutChar(0x00);
		ZigBee_PutChar(0x00);
		ZigBee_PutChar(temp[0]);
		ZigBee_PutChar(temp[1]);
	}
}


void send_xyz_value(){
	char		buf[24];
  int32_t	xoff = 0;
  int32_t	yoff = 0;
  int32_t	zoff = 0;
  int8_t	x = 0;
  int8_t	y = 0;
  int8_t	z = 0;

   
  OLED_ClearScreen();  
  I2CInit(I2CMASTER, 0);
  acc_init();
  /* Assume base board in zero-g position when reading first value. */
  acc_read(&x, &y, &z);
  xoff = 0-x;
  yoff = 0-y;
  zoff = 0-z;


    /* Accelerometer */
    acc_read(&x, &y, &z);
    x = x+xoff;
    y = y+yoff;
    z = z+zoff;

    snprintf(buf, 20, "Acc x: %d  ", x);
    OLED_DisStrLine(3, 0, (uint8_t *)buf);
	printf("\r\nAcc x: %d,  ", x);

	snprintf(buf, 20, "Acc y: %d  ", y);
    OLED_DisStrLine(4, 0, (uint8_t *)buf);
	printf("Acc y: %d,  ", y);

	snprintf(buf, 20, "Acc z: %d  ", z);
    OLED_DisStrLine(5, 0, (uint8_t *)buf);
	printf("Acc z: %d. ", z);
	
	ZigBee_PutChar(0xcc);
	ZigBee_PutChar(0x03);
	ZigBee_PutChar(0x00);
	ZigBee_PutChar(x);
	ZigBee_PutChar(y);
	ZigBee_PutChar(z);
  
}


/*
void hardware_operations(uint8_t *rd_buf,int c_count){     //the operations for hardware
	int pd1;
	int pd2;
	int pd3;
	int pd4;
	int pd5;
	int pd6;
	int pd7;

	pd1=(rd_buf[0]=='l')&&(rd_buf[1]=='e')&&(rd_buf[2]=='d')&&(rd_buf[3]=='_')&&(rd_buf[4]=='o')&&(rd_buf[5]=='n');
	pd2=(rd_buf[0]=='L')&&(rd_buf[1]=='e')&&(rd_buf[2]=='d')&&(rd_buf[3]=='_')&&(rd_buf[4]=='o')&&(rd_buf[5]=='f')&&(rd_buf[6]=='f');
	pd3=(rd_buf[0]=='b')&&(rd_buf[1]=='e')&&(rd_buf[2]=='e')&&(rd_buf[3]=='p')&&(rd_buf[4]=='_')&&(rd_buf[5]=='o')&&(rd_buf[6]=='n');
	pd4=(rd_buf[0]=='B')&&(rd_buf[1]=='e')&&(rd_buf[2]=='e')&&(rd_buf[3]=='p')&&(rd_buf[4]=='_')&&(rd_buf[5]=='o')&&(rd_buf[6]=='f')&&(rd_buf[7]=='f');
	pd5=(rd_buf[0]=='f')&&(rd_buf[1]=='a')&&(rd_buf[2]=='n')&&(rd_buf[3]=='_')&&(rd_buf[4]=='o')&&(rd_buf[5]=='n');
	pd6=(rd_buf[0]=='F')&&(rd_buf[1]=='a')&&(rd_buf[2]=='n')&&(rd_buf[3]=='_')&&(rd_buf[4]=='o')&&(rd_buf[5]=='f')&&(rd_buf[6]=='f');
	pd7=(rd_buf[0]=='l')&&(rd_buf[1]=='i')&&(rd_buf[2]=='g')&&(rd_buf[3]=='h')&&(rd_buf[1]=='t');
//Turning on the led.
	if(pd1){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		GPIOSetValue(PORT3, 0, 0);
		printf("\r\n\r\nled_on");
}
	//Turning off the led.
	if(pd2){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		GPIOSetValue(PORT3, 0, 1);
		printf("\r\n\r\nled_off");
}
	//Turning on the beep.
	if(pd3){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		speaker_op(1);
		printf("\r\n\r\nbeep_on");
}
	//Turning off the beep.
	if(pd4){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		speaker_op(0);
		printf("\r\n\r\nbeep_off");
}
	//Turning on the fan.
	if(pd5){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		GPIOSetValue(PORT0, 2, 0);
		printf("\r\n\r\nfan_on");
}
	//Turning off the fan.
	if(pd6){
		memset(rd_buf,0,sizeof(rd_buf));
		c_count=0;
		GPIOSetValue(PORT0, 2, 1);
		printf("\r\n\r\nfan_off");
}	
	if(pd7){
		send_light_strength();
	}
}
*/

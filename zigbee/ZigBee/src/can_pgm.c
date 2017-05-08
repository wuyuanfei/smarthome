/*****************************************************************************
 *   $Id:: can_pgm.c 5034 2010-09-24 23:48:37Z nxp12832                      
 *   Project: NXP LPC11xx CAN example
 *
 *   Description:
 *     This file is part of the CAN and CANopen on-chip driver examples.
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "type.h"
#include "gpio.h"
#include "uart.h"
#include "rom_drivers.h"
#include "can.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
ROM **rom = (ROM **)0x1fff1ff8;

/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
extern message_object can_buff[MSG_OBJ_MAX];
extern uint32_t CANRxDone[MSG_OBJ_MAX];

/* Private variables ---------------------------------------------------------*/
CAN_MSG_OBJ		msg_obj, msg_obj_t, msg_obj_r;
/* Initialize CAN Controller */
uint32_t ClkInitTable[2] = {
  0x00000000UL, // CANCLKDIV
  0x00001C57UL  // CAN_BTR
};

/* External functions --------------------------------------------------------*/ 
/* Private function prototypes -----------------------------------------------*/
/* Callback function prototypes */
void CAN_rx(uint8_t msg_obj_num);
void CAN_tx(uint8_t msg_obj_num);
void CAN_error(uint32_t error_info);

/* Publish CAN Callback Functions */
CAN_CALLBACKS callbacks = {
   CAN_rx,
   CAN_tx,
   CAN_error,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
};

/* Private functions ---------------------------------------------------------*/

/*	CAN receive callback */
/*	Function is executed by the Callback handler after
	a CAN message has been received */
void CAN_rx(uint8_t msg_obj_num){
  
  /* Determine which CAN message has been received */
  msg_obj.msgobj = msg_obj_num;

  /* Now load up the msg_obj structure with the CAN message */
  (*rom)->pCAND->can_receive(&msg_obj);

  if (msg_obj_num == 1)
  {
    /* Simply transmit CAN frame (echo) with with ID +0x100 via buffer 2 */
    msg_obj.msgobj = 2;
    msg_obj.mode_id += 0x100;
    (*rom)->pCAND->can_transmit(&msg_obj);
  }

  return;
}

/*	CAN transmit callback */
/*	Function is executed by the Callback handler after
	a CAN message has been transmitted */
void CAN_tx(uint8_t msg_obj_num){
  return;
}

/*	CAN error callback */
/*	Function is executed by the Callback handler after
	an error has occured on the CAN bus */
void CAN_error(uint32_t error_info){
  return;
}

/*	CAN interrupt handler */
/*	The CAN interrupt handler must be provided by the user application.
	It's function is to call the isr() API located in the ROM */
/*
void CAN_IRQHandler (void){
  (*rom)->pCAND->isr();
}
*/


/******************************************************************************
**   TRANSIT ONLY or LOOPBACK mode
******************************************************************************/
void TX_Only_test( void )
{
#if LOOPBACK_MODE
  /* If loopback mode, set the same ID for both RX and TX MSG */ 
  can_buff[0].id      = RX_MSG_ID;	/* standard frame */
#else
  can_buff[0].id      = TX_MSG_ID;	/* standard frame */
#endif
  can_buff[0].dlc     = 0x08;		/* Length = 8 */
  can_buff[0].data[0] = 0x0001;
  can_buff[0].data[1] = 0x0203;
  can_buff[0].data[2] = 0x0405;
  can_buff[0].data[3] = 0x0607;

  while ( 1 )
  {
	CAN_Send( 0, FALSE, (uint32_t *)&can_buff[0] );
#if LOOPBACK_MODE
	/* In the loopback mode, it seems that the Bosch CAN IP can only 
	work if the TX and RX uses the same message box. If TX and RX are
	different, the RX won't work. */
	while ( !(LPC_CAN->STAT & STAT_TXOK) );
	LPC_CAN->STAT &= ~STAT_TXOK;
	CAN_MessageProcess( 0 );
#endif
  }
}

/******************************************************************************
**   Neither TRANSIT ONLY nor LOOPBACK mode, whenever CAN receives a message,
**   it bounces back/transmits a message with diferent msg ID back to where
**   it sends.
******************************************************************************/
void RX_TX_Bouncing( void )
{
  uint32_t i;

  /* Clear all CAN RX done flag. */
  for ( i = 0; i < MSG_OBJ_MAX; i++ )
  {
	CANRxDone[i] = FALSE;
  }

  while ( 1 )
  {
#if POLLING
#if BASIC_MODE
	while ( !(LPC_CAN->STAT & STAT_RXOK) );
	LPC_CAN->STAT &= ~STAT_RXOK;
	/* In BASIC mode, the message Object number doesn't apply. */
	CAN_MessageProcess( 0 );
#else
	/* first half of message object(0~15) or message object 1 to 16 */
	for ( i = 0; i < MSG_OBJ_MAX/2; i++ )
	{
	  if ( LPC_CAN->ND1 & (0x01<<i) )
	  {
		LPC_CAN->STAT &= ~STAT_RXOK;
		CAN_MessageProcess( i );
		/* Use the received data as the TX data field, and the next 
		message object slot for TX. */ 
		CAN_Send(i, TRUE, (uint32_t *)&can_buff[i]);
	  }
	}
	/* second half of message obj(16~31), or message object 17 to 32 */
	for ( i = MSG_OBJ_MAX/2; i < MSG_OBJ_MAX; i++ )
	{
	  if ( LPC_CAN->ND2 & (0x01<<(i-MSG_OBJ_MAX/2)) )
	  {
		LPC_CAN->STAT &= ~STAT_RXOK;
		CAN_MessageProcess( i );
		/* Use the received data as the TX data field, and the next 
		message object slot for TX. */
		CAN_Send(i, TRUE, (uint32_t *)&can_buff[i]);
	  }
	}
#endif		/* Endif BASIC_MODE */
#else		/* Not POLLING, it's interrupt driven. */
	/* The CANRxDone[x] flag is set in the interrupt handler where x indicates
	which message object receives the message. */

	for ( i = 0; i < MSG_OBJ_MAX; i++ )
	{
	  if ( CANRxDone[i] == TRUE )
	  {
		CAN_Send(i, TRUE, (uint32_t *)&can_buff[i]);
		CANRxDone[i] = FALSE;
	  }
	}
#endif		/* End POLLING */
  }
}

/*******************************************************************************
* Function Name  : CAN_Test
* Description    : CAN test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Test_ROM(void)
{
  Menu_TypeDef	menu;
  uint16_t	key, x;

  /* Initialize the CAN controller */
  (*rom)->pCAND->init_can(&ClkInitTable[0], 1);

  /* Configure the CAN callback functions */
  //(*rom)->pCAND->config_calb(&callbacks);

  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);


  menu.max_numb = 3;
  menu.numb = 1;
  menu.title = "CAN";
  menu.item[0] = "1.Send Hello CAN!";
  menu.item[1] = "2.Turn on other FAN";
  menu.item[2] = "3.Turn off other FAN";
  
  Dis_Menu(menu);

  x = 0;

  delay_ms(250);
  
  while(menu.numb)
  {
    /* Configure message object 1 to receive all 11-bit messages 0x400-0x4FF */
	if(LPC_CAN->STAT & STAT_RXOK)
	{
	  LPC_CAN->STAT &= ~STAT_RXOK;
      msg_obj_r.msgobj = 1;
      msg_obj_r.mode_id = 0x400;
      msg_obj_r.mask = 0x700;
	  msg_obj_t.data[0] = ' ';
      msg_obj_t.data[1] = ' ';
      msg_obj_t.data[2] = ' ';
      msg_obj_t.data[3] = '-';
	  msg_obj_t.data[4] = '-';
      msg_obj_t.data[5] = '-';
	  msg_obj_t.data[6] = '-';
	  msg_obj_t.data[7] = 0;
      (*rom)->pCAND->config_rxmsgobj(&msg_obj_r);

	  //(*rom)->pCAND->can_receive(&msg_obj_r);
	}

	msg_obj_r.data[7] = 0;
	for(key=0; key<8; key++)
    {
	  OLED_DisChar(2-1 + 6, x, 0, msg_obj_r.data[key]);
	  x += 6;
	  if(x > 120)
	  {
	    x = 0;
	  }
	  UART0_PutChar(msg_obj_r.data[key]);
    }

	key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		x = 0;
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		x = 0;
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			/* Send a simple one time CAN message */
            msg_obj_t.msgobj  = 0;
            msg_obj_t.mode_id = 0x400;
            msg_obj_t.mask    = 0x0;
            msg_obj_t.dlc     = 8;
            msg_obj_t.data[0] = 'H';
            msg_obj_t.data[1] = 'i';
            msg_obj_t.data[2] = ' ';
            msg_obj_t.data[3] = 'C';
			msg_obj_t.data[4] = 'A';
            msg_obj_t.data[5] = 'N';
			msg_obj_t.data[6] = '!';
			msg_obj_t.data[7] = 0;
            (*rom)->pCAND->can_transmit(&msg_obj_t);

			printf("Send: Hi CAN!");
		    break;

		  case 2:		    
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
* Function Name  : CAN_Test
* Description    : CAN test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key, x;
  uint32_t	i, j;


  /* Clear all CAN RX done flag. */
  for(i = 0; i < MSG_OBJ_MAX; i++)
  {
	CANRxDone[i] = FALSE;
  }

  CAN_Init(BITRATE500K8MHZ);

  menu.max_numb = 3;
  menu.numb = 1;
  menu.title = "CAN";
  menu.item[0] = "1.Send Hello CAN!";
  menu.item[1] = "2.Turn on other FAN";
  menu.item[2] = "3.Turn off other FAN";
  
  Dis_Menu(menu);

  x = 0;

  delay_ms(250);
  
  while(menu.numb)
  {
	// The CANRxDone[x] flag is set in the interrupt handler where x indicates
	// which message object receives the message.
	for(i = 0; i < MSG_OBJ_MAX; i++)
	{
	  if(CANRxDone[i] == TRUE)
	  {
		for(j=0; j<4; j++)
        {
  	      OLED_DisChar(2-1 + 6, x, 0, can_buff[i].data[j]);
  	      x += 6;
  	      if(x > 120)
  	      {
  	        x = 0;
  	      }
  	      OLED_DisChar(2-1 + 6, x, 0, can_buff[i].data[j]>>8);
  	      x += 6;
  	      if(x > 120)
  	      {
  	        x = 0;
  	      }
  
  	      UART0_PutChar(can_buff[i].data[j]);
  	      UART0_PutChar(can_buff[i].data[j]>>8);
        }

		if((((can_buff[i].data[0]>>0)&0xff)=='F') && (((can_buff[i].data[0]>>8)&0xff)=='A') 
		   && (((can_buff[i].data[1]>>0)&0xff)=='N') && (((can_buff[i].data[1]>>8)&0xff)==' ')
		   && (((can_buff[i].data[2]>>0)&0xff)=='O') && (((can_buff[i].data[2]>>8)&0xff)=='n')
		   && (((can_buff[i].data[3]>>0)&0xff)==' ') && (((can_buff[i].data[3]>>8)&0xff)==' '))
		{
		  GPIOSetValue(PORT0, 2, 1);		// Turn on Fan
		  printf("\r\nTurn on Fan");
		}
		else if((((can_buff[i].data[0]>>0)&0xff)=='F') && (((can_buff[i].data[0]>>8)&0xff)=='A') 
		   && (((can_buff[i].data[1]>>0)&0xff)=='N') && (((can_buff[i].data[1]>>8)&0xff)==' ')
		   && (((can_buff[i].data[2]>>0)&0xff)=='O') && (((can_buff[i].data[2]>>8)&0xff)=='f')
		   && (((can_buff[i].data[3]>>0)&0xff)=='f') && (((can_buff[i].data[3]>>8)&0xff)==' '))
		{
		  GPIOSetValue(PORT0, 2, 0);		// Turn off Fan
		  printf("\r\nTurn off Fan");
		}

		CANRxDone[i] = FALSE;
	  }
	}

	key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		x = 0;
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		x = 0;
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			/* Send a simple one time CAN message */
			can_buff[0].id      = RX_MSG_ID+1;				// standard frame
  			can_buff[0].dlc     = 0x08;						// Length = 8
			can_buff[0].data[0] = ('i'<<8) | ('H'<<0);
			can_buff[0].data[1] = ('C'<<8) | (' '<<0);
			can_buff[0].data[2] = ('N'<<8) | ('A'<<0);
			can_buff[0].data[3] = (' '<<8) | ('!'<<0);

            CAN_Send(0, FALSE, (uint32_t *)&can_buff[0]);

			printf("Send: Hi CAN! ");
		    break;

		  case 2:
		    can_buff[0].id      = RX_MSG_ID+1;				// standard frame
  			can_buff[0].dlc     = 0x08;						// Length = 8
			can_buff[0].data[0] = ('A'<<8) | ('F'<<0);
			can_buff[0].data[1] = (' '<<8) | ('N'<<0);
			can_buff[0].data[2] = ('n'<<8) | ('O'<<0);
			can_buff[0].data[3] = (' '<<8) | (' '<<0);

            CAN_Send(0, FALSE, (uint32_t *)&can_buff[0]);

			printf("Send: FAN On  ");		    
		    break;

          case 3:
		    can_buff[0].id      = RX_MSG_ID+1;				// standard frame
  			can_buff[0].dlc     = 0x08;						// Length = 8
			can_buff[0].data[0] = ('A'<<8) | ('F'<<0);
			can_buff[0].data[1] = (' '<<8) | ('N'<<0);
			can_buff[0].data[2] = ('f'<<8) | ('O'<<0);
			can_buff[0].data[3] = (' '<<8) | ('f'<<0);

            CAN_Send(0, FALSE, (uint32_t *)&can_buff[0]);

			printf("Send: FAN Off ");		    
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





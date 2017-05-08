#include "stdio.h"
#include "LPC11xx.h"
#include "timer32.h"

extern volatile uint32_t timer32_0_counter;
volatile uint32_t period = 1500;

void speaker_op(uint8_t pwm_set)
{
	switch(pwm_set)
	{
		case 1:
			LPC_TMR32B1->PR = 0x7f;
			init_timer32PWM(1, period, MATCH0);
			enable_timer32(1);
	   	break;
		case 0:
			disable_timer32(1);
		break;
	}
}

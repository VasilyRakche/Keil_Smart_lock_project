#ifndef PWM_SETUP_H
#define PWM_SETUP_H

/*
	CK_CNT, which is enabled only when the counter enable bit (CEN) in TIMx_CR1 register is set
	enable prescale register 
	
	
	frequency -TIMx_ARR
	duty cycle - TIMx_CCRx
	pwm mode selection -TIMx_CCMRx

	
*/

#include "stm32f10x.h"

/*
	Initialize pwm and gpio PA8 
	This PWM is used to control servo motor SG90
*/
void pwm_setup();

#endif
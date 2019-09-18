#ifndef INTERRUPT_TIMERS_SETTING_H
#define INTERRUPT_TIMERS_SETTING_H

#include "stm32f10x.h"

/****
	Function for generating TIM2_IRQHandler interrupt
	after sec number of seconds
****/
void gen_interrupt(uint16_t sec);
/*****
	GPIO init for TIM3 CH2 PA7
	Initializarion of TIM3_IRQHandler interrupt
	on rising endge of input signal
****/
void tim3_interrupt_input_init();
/*****
	Init for TIM2 CH1
	Just initalization for TIM2_IRQHandler interrupt
	This interrupt is used in gen_interrupt function 
****/
void tim2_interrupt_counter_init();

#endif
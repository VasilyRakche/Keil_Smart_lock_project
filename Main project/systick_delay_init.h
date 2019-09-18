#ifndef SYSTICK_DELAY_INIT_H
#define SYSTICK_DELAY_INIT_H

#include "stm32f10x.h"
#include <stdbool.h>


//Just to clerify type of signal needed for send_signal function
typedef enum	
	{ LED=0,
		BUZZ,
		BUZZx2,
		BUZZx3,
		EMTY
	} signal_option;

/****
	Initalization routine	for Systick interrupts
	Set priority and set for assert exceptions 
****/
void systick_delay_init();
/****
	delays but returns right away
	If user wants to stop the program from going further
	In while loop check should be done with delayed_activated()
****/
void delay(double seconds);

/***
	Checking the delay flag
	because delay flag is initialized inside the file and is not accessible from outside
	Function is checking it and giving information about if the delay is active or not
***/	

bool delayed_activated();

/***
	Handling the interrupts of SysTick
	responsible for delay and for the LED and Buzzer indications
***/
void SysTick_Handler(void);
	
	
/*******************
	GPIO SIGNAL FUNCTIONS

******************/

/*
	Signal activities
	LED- PA4
	BUZZ-PA5
	*/
void send_sig_gpio_init();
/***
	Inside the delay is used to make a blinking LED
	or to make a Buzz sound once, twice or three times
***/
void send_signal(double seconds, signal_option signal);

#endif
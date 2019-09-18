#include "systick_delay_init.h"

#define AHB 40000000UL/8

static volatile bool delayed=false;
static uint32_t tick_load;

void systick_delay_init(){
	//Clock source is AHB/8 is CLKSOURCE =0
	SysTick->CTRL |= 	SysTick_CTRL_TICKINT_Msk ; //Asserts exception
	//processor implements only bits[7:4] of 8 bit register
	//SysTick is PRI_15, lower value->greater priority
	SCB->SHP[11] |=  0x2<<4; //PRI_15 is a last register in SHP, setting highest priority from programable interrupts
}
void delay(double seconds){
	if (!delayed){
		tick_load=AHB*seconds;
		delayed=true;
	}
	if (tick_load>0x00FFFFFF){
		tick_load-=0x00FFFFFF;
		SysTick->LOAD =0x00FFFFFF; //maximum is 0x00FF FFFF
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	} else {
		SysTick->LOAD=tick_load;
		tick_load=0;
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	}	
}

bool delayed_activated(){
	if (delayed)
		return true;
	else 
		return false;
}

/*******************
	GPIO SIGNAL FUNCTIONS

******************/

static volatile double gpio_sec=0;
static volatile bool buzz_delay_passed=false;
static volatile signal_option _signal;
uint32_t _gpio_buff=0;

void send_sig_gpio_init(){
	//GPIO initialization for buzzerPA5, diodPA6, transistorPA4
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	_gpio_buff= GPIOA->CRL;
	_gpio_buff &= ~GPIO_CRL_CNF5 & ~GPIO_CRL_CNF4 & ~GPIO_CRL_CNF6;
	_gpio_buff &= ~GPIO_CRL_MODE5 & ~GPIO_CRL_MODE4 & ~GPIO_CRL_MODE6;
	_gpio_buff |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE6_0; //01 Max speed 10MHz
	
	GPIOA->CRL = _gpio_buff;	//Setting all the GPIOs
	GPIOA->BSRR |= GPIO_BSRR_BS5; //PA5 is responsible for BUZZ it should be inactive high(buzz is activated on low)
}
void send_signal(double seconds, signal_option signal){
	_signal=signal;
	gpio_sec=seconds;
	switch(_signal){
		case LED:
		GPIOA->BSRR |= GPIO_BSRR_BS4;
		break;
		default:	//For all BUZZ options
		GPIOA->BSRR |= GPIO_BSRR_BR5;	//Buzzer is activated on giving signal low
		break;
		}
	delay(gpio_sec);
}


/*
	Function called by Systick handler in order to continue or exit the process 
*/
void gpio_flag_clear(){ 
	switch(_signal){
		case LED:
			GPIOA->BSRR |= GPIO_BSRR_BR4;
			_signal = EMTY;
			delayed=false; 
			break;
		
		case BUZZ:
			GPIOA->BSRR |= GPIO_BSRR_BS5;
			_signal = EMTY;
			delayed=false; 
			break;
		
		case BUZZx2:
			GPIOA->BSRR |= GPIO_BSRR_BS5;				
			delayed=false; 
			if (!buzz_delay_passed){
				buzz_delay_passed=true;
				delay(gpio_sec);
			}else{
				buzz_delay_passed=false;
				_signal=BUZZ;
				send_signal(gpio_sec,_signal);
			}
			break;
			
		case BUZZx3:
			GPIOA->BSRR |= GPIO_BSRR_BS5;
			delayed=false;
			if (!buzz_delay_passed){
				buzz_delay_passed=true;
				delay(gpio_sec);
			}else{
				buzz_delay_passed=false;
				_signal=BUZZx2;
				send_signal(gpio_sec,_signal);
			}
			break;
			
		case EMTY:
			delayed=false; //The systick process is finished
			break;
	}
}


/*******************
	GPIO SIGNAL FUNCTIONS END

******************/

/***
	Interrupt handler
*/

void SysTick_Handler(void){
	if (tick_load!=0){
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		SysTick->VAL=0; //Write of any value makes it 0 
		delay(0); //The seconds dont have any meaning, any value can be put, for continuation of delay
	} else {
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		SysTick->VAL=0; //Write of any value makes it 0 	
		gpio_flag_clear();  //The delay is really small, so no need to write in condition above
	}
}
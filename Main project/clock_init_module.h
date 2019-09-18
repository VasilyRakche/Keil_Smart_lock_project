#ifndef CLOCK_INIT_MODULE_H
#define CLOCK_INIT_MODULE_H

#include "stm32f10x.h" /* Standart headhers*/

/***
	Structure for passing the parameters to 
	clock init functions
	This way we can initialize clock different
	every time
***/
typedef struct {
	uint32_t PLLMUL_PRESCALE;
	uint32_t AHB_PRESCALE;
	uint32_t APB2_PRESCALE;
	uint32_t APB1_PRESCALE;
	uint32_t PLLXTPRE_PRESCALE;
} RCC_Config_TypeDef;


/* Initialize clock configuration
	HSE clock (8MHz)
	HSE divider for PLL entry -not devided
	PLL multiplication set 
	AHB prescaler-not devided
	APB1(low speed) devided 
	APB2(high speed) not devided	
*/
void clock_init(RCC_Config_TypeDef* config);

#endif

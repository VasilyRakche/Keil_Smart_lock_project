#include "clock_init_module.h"

void clock_init(RCC_Config_TypeDef* _config)
{
		//HSE start*****************************************************************************
	RCC->CR |=RCC_CR_HSEON;
	while((RCC->CR & RCC_CR_HSERDY)==0)
	{}
	//Flash prefetch buffer and latency
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	while((FLASH->ACR & FLASH_ACR_PRFTBS)==0)
	{}	
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
 	FLASH->ACR |= FLASH_ACR_LATENCY_2;
	//Clock starting from PCLK1 and PCLK2 to AHB
	RCC->CFGR &= ~RCC_CFGR_PPRE2; 
	RCC->CFGR |= _config->APB2_PRESCALE; 	//APB2 not devided
	RCC->CFGR &= ~RCC_CFGR_PPRE1;		
	RCC->CFGR |= _config->APB1_PRESCALE;	//APB1 devided by 2
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= _config->AHB_PRESCALE;		 //AHB prescaler not divided
	
	//Clock starting from HSE to SYSCLCK
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;
	RCC->CFGR |= _config->PLLXTPRE_PRESCALE;	//HSE divider for PLL entry -not devided
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= _config->PLLMUL_PRESCALE;
	
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLON)==0)
	{}
	
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS_PLL)==0)
	{}
}

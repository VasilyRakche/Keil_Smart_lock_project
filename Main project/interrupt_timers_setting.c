#include "interrupt_timers_setting.h"

#define _TIM_PSC 40000-1		//1 kHZ If APB1!=1 then APB1 clock is multiplied by 2

volatile uint32_t CNT_reg,CCR1_reg;
uint32_t _gpioa_buff=0;

void tim2_interrupt_counter_init(){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //Clock for TIM2_CH1
	TIM2->PSC = _TIM_PSC;
  TIM2->CR1 |= TIM_CR1_CEN; //Enable counter
	
	//TIM2 CH1 configuration for interrupt on counter 
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1PE; //CCR1 register can be written at any time, preload register dissabled
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;// output is frozen(doesnt depend on CCR register)
	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S; //Channel is configured as output
	
	//TIM2->CCER |= TIM_CCER_CC1E; //Enable channel 1 
	NVIC->ISER[0] |= NVIC_ISER_SETENA_28;   //TIM2 Interrupt
	NVIC->IP[28]=0x3<<4; //Lower priority then Systick
	while (!(TIM2->SR & TIM_SR_CC1IF)); //On initialization flag is set, waiting to init
	TIM2->SR &= ~TIM_SR_CC1IF;
	while(TIM2->SR & TIM_SR_CC1IF);
	
}

void gen_interrupt(uint16_t sec){
	CNT_reg=TIM2->CNT;
	CCR1_reg=CNT_reg+sec*1000;
	if( CCR1_reg> 0xFFFF)
		CCR1_reg=CCR1_reg-0xFFFF;
	TIM2->CCR1=CCR1_reg;
	TIM2->DIER |= TIM_DIER_CC1IE; //Enable interrupt on CCR register
}

void tim3_interrupt_input_init(){
	//GPIO init for TIM3 CH2 PA7
	_gpioa_buff= GPIOA->CRL;
	_gpioa_buff &= ~GPIO_CRL_CNF7 ;
	_gpioa_buff |= GPIO_CRL_CNF7_1;
	_gpioa_buff &= ~GPIO_CRL_MODE7;
	GPIOA->CRL=_gpioa_buff;	
	GPIOA->BSRR |= GPIO_BSRR_BR7; //Set to pull-down of ODR==0

	
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //Clock for TIM3_CH2
	TIM3->PSC = _TIM_PSC;
  TIM3->CR1 |= TIM_CR1_CEN; //Enable counter
	//TIM3 CH2 configuration for touch sensor
	TIM3->CCMR1 &= ~TIM_CCMR1_CC2S; //Configuration of channel 2, link to TI2 input 
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_0; //Bits set to 01
	TIM3->CCMR1 &= ~(TIM_CCMR1_IC2F | TIM_CCMR1_IC2PSC); //No filter and no prescaler
	TIM3->CCER &= ~TIM_CCER_CC2P; //Rising edge triger 
	TIM3->CCER |= TIM_CCER_CC2E; //Enable channel 2 
//  TIM2->DIER |= TIM_DIER_CC2DE;//DMA request enabled
	TIM3->DIER |= TIM_DIER_CC2IE; //Interrupt enable on low-high
	NVIC->ISER[0] |= NVIC_ISER_SETENA_29;   //TIM2 Interrupt
	NVIC->IP[29]=0x3<<4; //Lower priority then Systick
	
}
//
//void TIM2_IRQHandler(void){
//	TIM2->SR &= ~TIM_SR_CC1IF;
//	while(TIM2->SR & TIM_SR_CC1IF);
//}
//void TIM3_IRQHandler(void){
//	TIM3->SR &= ~TIM_SR_CC2IF;
//	while(TIM3->SR & TIM_SR_CC2IF);
//}
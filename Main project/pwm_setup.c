	#include "pwm_setup.h"
	
	void pwm_setup(){
		
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; //TIM1 clock enabled

		TIM1->PSC &= ~TIM_PSC_PSC;
		TIM1->PSC |= 0x190; //Prescaler set to 400
		TIM1->ARR &= ~TIM_ARR_ARR;
		TIM1->ARR |= 0x7C9; //auto reload register(setting frequency)
		
		TIM1->CCR1 |= 0x32;	//compare/ capture register(controling duty cycle), from 0x32 to 0xF7 so from 0.5014ms to 2,477
		TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;	//****00-CC1 channel is configured as output

		TIM1->CCMR1 |= TIM_CCMR1_OC1M;
		TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0; //PWM mod 1, active when CNT<CCR1(in upcounting)
		TIM1->CCMR1 |= TIM_CCMR1_OC1PE; //Preload register enabled-read/write access the preload and active register is updated on event	
		TIM1->CR1 |= TIM_CR1_ARPE; //Auto reload preload register is enabled
		TIM1->EGR |= TIM_EGR_UG; 	//Event generation(values of preload registers will go to shadow registers)
		
		TIM1->CCER |= TIM_CCER_CC1E; //Enable OC1 (OC1 output depends of MOE, OSSI, OSSR, OIS1, OIS1N, CC1NE)
		TIM1->CCER &= ~TIM_CCER_CC1P; //set to 0, OC1 active high 
		//Output control bits for complementary OCx and OCxN
		TIM1->CCER &= ~TIM_CCER_CC1NE; //CC1N not enabled
		TIM1->BDTR |= TIM_BDTR_MOE; //Main output enabled 
		while(!(TIM1->BDTR & TIM_BDTR_MOE)); //Waiting for MOE bit to be set, problems otherwise
		TIM1->BDTR &= ~TIM_BDTR_OSSR; 
		//TIM1->BDTR &= ~TIM_BDTR_OSSI; //x -whatever value for this mode		
		
		TIM1->CR1 &= ~TIM_CR1_DIR;	//Upcounter mode
		TIM1->CR1 |= TIM_CR1_CEN;  //Counter enabled(Fcn=F/psc)
		
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //GPIO RCC Setup, push pull, alternate function output
		uint32_t buff= GPIOA->CRH;
		buff &= ~GPIO_CRH_CNF8_0;
		buff |= GPIO_CRH_CNF8_1;
		buff |= GPIO_CRH_MODE8;
		GPIOA->CRH = buff;		
	}
	
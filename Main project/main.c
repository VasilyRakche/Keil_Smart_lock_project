#include "stm32f10x.h"
#include "clock_init_module.h"
#include "systick_delay_init.h"
#include "interrupt_timers_setting.h"
#include "pwm_setup.h"

int main(){
	systick_delay_init();
	
	// Clock configuration settings
	RCC_Config_TypeDef _RCC_CNFG_val;
	RCC_Config_TypeDef* _RCC_CNFG=&_RCC_CNFG_val;
	
	//HSE 8MHz clock is used
	//PLL clock is used as SYSCLK
	_RCC_CNFG->PLLXTPRE_PRESCALE=0;
	_RCC_CNFG->PLLMUL_PRESCALE=RCC_CFGR_PLLMULL5;
	_RCC_CNFG->AHB_PRESCALE=0;
	_RCC_CNFG->APB1_PRESCALE=RCC_CFGR_PPRE1_DIV2;
	_RCC_CNFG->APB2_PRESCALE=0;
	
	clock_init(_RCC_CNFG);
	send_sig_gpio_init();
	
	
	
	tim2_interrupt_counter_init();
	tim3_interrupt_input_init();
	pwm_setup();
	while(1);
	
}

/******************************************************************
	IRQ HANDLERS:
*/

#define STOR_SIZE 10

volatile int i=0,cnt=0,cnt_sav=0;
volatile bool pin_gen=false,code_checking=false,enter_code_mode=false,unlocked=false; 
volatile double code_saved[STOR_SIZE],code_buff[STOR_SIZE],T=0;
volatile uint16_t dma_ex=0;
volatile uint32_t buff_ccr2[2];

void diod(bool turn_on){		//Turn the LED on and off
	if(turn_on){
		GPIOA->BSRR |= GPIO_BSRR_BS4;
	}else
		GPIOA->BSRR |= GPIO_BSRR_BR4;
}

bool check_code(){			//compare new code with previously saved
	for (int k=1;k<cnt;k++){
		if(((code_saved[k-1]-0.22)<code_buff[k]) && (code_buff[k]<(code_saved[k-1]+0.22))){ //0.4s tolerance, first value is not checked
			if(k==cnt-1){	//All are checked and right
				return true;																							//unlocking
			}
		}else{
			return false;																											//false code
		}
	}
	return false;																												//false code
}

void lock_toggle(){			//just toggle the lock mehanism
	GPIOA->BSRR |= GPIO_BSRR_BS6;
	if (TIM1->CCR1 == 0x32){
		TIM1->CCR1 = (uint16_t) 0xF7;	//compare/ capture register(controling duty cycle), from 0x32 to 0xF7 so from 0.5014ms to 2,477
	}else 
		TIM1->CCR1 = (uint16_t) 0x32;
	delay(1);
	while(delayed_activated());
	GPIOA->BSRR |= GPIO_BSRR_BR6;	
}

void clear(volatile double* code,volatile int* count){		//delete all elements from code variable
	for(int k=0;k<STOR_SIZE;k++){
		*code=0;
		code++; //incrementing adrress
	}
	*count=0;
}

#define TURN_LED_ON true
#define TURN_LED_OFF false

/*****
	TIM2 is entered every time we enter something with touch sensor, 
	and then when 3 sec passes (3 sec since last time sensor was touched, 
	means that user is not going to enter new data,
	we go to this handler to check what was entered
*****/
void TIM2_IRQHandler(void){
	// In debug, while the execution is stoped, CNT is incremented ??
	TIM2->SR &= ~TIM_SR_CC1IF;
	while(TIM2->SR & TIM_SR_CC1IF);
	TIM2->DIER &= ~TIM_DIER_CC1IE; //Disable interrupt on CCR register
	
	if (cnt==1 && unlocked){			//OPEN, If already open, pressing once is suficient to lock
		unlocked=false; 
		diod(TURN_LED_OFF); //Clear diod
		send_signal(0.1,BUZZ); 	//beep
		while(delayed_activated());	//Beep should be turned off before proceding
		send_signal(0.5,LED); //LED turned on
		while(delayed_activated());	//Making sure that beep will be after light turns off
		lock_toggle();//LOCK
		send_signal(0.1,BUZZx2); 	//beep beep
		
	}else if(!unlocked && cnt == 2){	//TO_BE_SET_CODE_ENTER_MODE
		if(code_buff[cnt-1]<1){
			enter_code_mode=true;
			diod(TURN_LED_ON); 			//Diod to indicate that user must enter code
			send_signal(0.1,BUZZx2); 	//beep beep
		}
		gen_interrupt(3);

	}else if (cnt>4 && !code_checking && enter_code_mode){			//NEW_PIN_ENTER_MODE, Pin entered 
		diod(TURN_LED_OFF); 			//Turn off the diode
		if(code_saved[0] == 0){
			delay(0.5); 
			while(delayed_activated()); //Wait diode turn-off time
			for(int k=1;k<cnt;k++)
				code_saved[k-1]=code_buff[k];
			cnt_sav=cnt-1;
			diod(TURN_LED_ON); 			//Diod to indicate that code is being checked 
			code_checking='y';
			send_signal(0.1,BUZZx2); 	//beep beep 
			gen_interrupt(3);							//interrupt to check the code
		}else if(check_code()){
			clear(code_saved,&cnt_sav); //Clear for new data to be saved later, password reset	
			send_signal(0.1,BUZZx2); 	//beep beep 
			enter_code_mode=false;
		}else {							//check_code not passed, just exiting
			enter_code_mode=false;
			send_signal(1,BUZZ); 	//beeeeeep
		}
		
	}else if (code_checking && enter_code_mode && (cnt-1)==cnt_sav){ // Need to enter it once more
		diod(TURN_LED_OFF);	//clear the diode sign
		if(check_code()){
			pin_gen=true;
			code_checking=false;
			enter_code_mode=false;
			send_signal(0.1,BUZZx3); 	//beep beep beep!!
			diod(TURN_LED_ON);
			lock_toggle();//Unlock
			unlocked=true;
		}else{													//False code
			code_checking=false;
			enter_code_mode=false;
			clear(code_saved,&cnt_sav);
			send_signal(1,BUZZ); 	//beeeeeep
		}
	}else if (!unlocked && pin_gen && cnt>4 && !code_checking && !enter_code_mode){			//When there is saved code
		if(check_code()){
			lock_toggle();//Unlock
			diod(TURN_LED_ON);
			send_signal(0.1,BUZZx3); 	//beep beep beep
			unlocked=true;
		}else{												//Check is not passed
			send_signal(1,BUZZ); 	//beeeeeep
		}			
	}else{			//Meaningless data, should be deleted
		enter_code_mode=false;
		code_checking= false;
		send_signal(1,BUZZ); 	//beeeeeep
		if (!unlocked)
			diod(TURN_LED_OFF);	//clear the diode sign
	}
	clear(code_buff,&cnt);
}

/***
	Saving value which indicates the time passed from previous and current sensor touch
	Also setting interrupt to be generated in 3 seconds
	If the user dont press touch in 3 seconds it means that the entered data should be chacked
	And is done in TIM2_IRQHandler interrupt
	If user touches the sensor before 3 seconds passed, data is saved 
	and interrupt is reinitialized to be in 3 seconds from the last touch
**/
void store_value(volatile double* code_b,volatile double* single_code,volatile int *cnt){ 
	*(code_b+*cnt)=*single_code;
	(*cnt)++;
	gen_interrupt(3);
}
/****
	Every time we touch sensor, we go to this hadler
	touching the sensor generates this interrupt
***/
void TIM3_IRQHandler(void){
	TIM3->SR &= ~TIM_SR_CC2IF;
	while(TIM3->SR & TIM_SR_CC2IF);
	send_signal(0.1,BUZZ);											
	buff_ccr2[0]=buff_ccr2[1];
	buff_ccr2[1]=TIM3->CCR2; //Last value would always be on position 1
	T = (double)((buff_ccr2[1] > buff_ccr2[0]) ? (buff_ccr2[1] - buff_ccr2[0]) : (65535+ buff_ccr2[1] - buff_ccr2[0]))/1000;
	store_value(code_buff,&T,&cnt);
}
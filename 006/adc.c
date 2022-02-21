#include "adc.h"
#include "stm32l4xx.h"

// Analog Inputs: 
// PA2 (ADC12_IN7)
// These pins are not used: PA0 (ADC12_IN5), PA1 (ADC12_IN6), PA3 (ADC12_IN8)

//******************************************************************************************
// STM32L4x6xx Errata sheet
// When the delay between two consecutive ADC conversions is higher than 1 ms the result of 
// the second conversion might be incorrect. The same issue occurs when the delay between the 
// calibration and the first conversion is higher than 1 ms.
// Workaround
// When the delay between two ADC conversions is higher than the above limit, perform two ADC 
// consecutive conversions in single, scan or continuous mode: the first is a dummy conversion 
// of any ADC channel. This conversion should not be taken into account by the application.


void adc1Init() {
	
	int TADCVREG_STUP=SystemCoreClock/1000000*20;  //20 us
	
	//ADC Pin Initialization
	// Enable the clock of GPIO Port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  //configurazione PA2 input mode
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	//GPIOA->MODER |= GPIO_MODER_MODE2_Msk;
	//GPIOA->MODER |=  3U<<(2*1);  // PA1 (ADC12_IN6), Mode 11 = Analog
	GPIOA->MODER |=  3U<<(2*2);  // PA2 (ADC12_IN7), Mode 11 = Analog
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR &= ~(3U<<(2*2)); //PA2 No pull-up, no pull-down
	
	// GPIO port analog switch control register (ASCR)
	// 0: Disconnect analog switch to the ADC input (reset state)
	// 1: Connect analog switch to the ADC input
	GPIOA->ASCR |= GPIO_ASCR_EN_1;  //collegamento PA1 con ADC1
	GPIOA->ASCR |= GPIO_ASCR_EN_2;  //collegamento PA2 con ADC1
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;  //abilitazione clock ADC1
	ADC1->CR &= ~ ADC_CR_ADEN;  //disabilitazione ADC1

	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;  //enable I/O analog switches voltage booster
	ADC123_COMMON->CCR &= ~ ADC_CCR_VREFEN; //enable the conversion of internal channels
	ADC123_COMMON->CCR &= ~ ADC_CCR_PRESC;  //Configure the ADC prescaler to select the frequency of the clock to the ADC (set clock not divided)
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0; //set the ADC clock mode as synchronous clock mode (HCLK/1)
	ADC123_COMMON->CCR &= ~ ADC_CCR_DUAL;   //Configure all ADCs as independent
	//By default, the ADC is in deep-power-down mode where its supply is internally switched off to reduce the leakage currents.
	//Therefore, software needs to wait up ADC
	//adc1Wakeup(); 
	ADC1->CR &= ~ADC_CR_DEEPPWD;  //uscita Deep-power-down mode
	ADC1->CR |= ADC_CR_ADVREGEN;  //abilitazione voltage regulator
	while(TADCVREG_STUP) {        //attesa abilitazione regulator
		TADCVREG_STUP--;
	}

	//calibrazione
	ADC1->CR &= ~ ADC_CR_ADCALDIF; //calibrazione modalita' singola
	ADC1->CR |= ADC_CR_ADCAL;      //inizio calibrazione
	while(ADC1->CR&ADC_CR_ADCAL);  //attesa calibrazione
	
	ADC1->CFGR &= ~ADC_CFGR_RES;   //Configure the resolution as 12 bits 
	ADC1->CFGR &= ~ADC_CFGR_ALIGN; //Select right alignment
	ADC1->SQR1 &= ~ADC_SQR1_L_Msk;
	ADC1->SQR1 &= ~ADC_SQR1_SQ1_Msk;
	ADC1->SQR1 |= ADC_SQR1_SQ1_0|ADC_SQR1_SQ1_1|ADC_SQR1_SQ1_2;
	ADC1->DIFSEL &= ~ ADC_DIFSEL_DIFSEL_7;
	ADC1->SMPR1 |= ADC_SMPR1_SMP7_1;
	ADC1->CFGR &= ~  ADC_CFGR_CONT;
	ADC1->CFGR &= ~  ADC_CFGR_EXTEN;
	ADC1->CR |= ADC_CR_ADEN;
	while((ADC1->ISR & ADC_ISR_ADRDY)==0);
}

void adc1Trigger() {
	ADC1->CR |=  ADC_CR_ADSTART;
 }

uint8_t adc1Ready() {
	if((ADC123_COMMON->CSR & ADC_CSR_EOC_MST)==ADC_CSR_EOC_MST) {
		return 1;
	} else {
		return 0;
	}
}

uint16_t adc1Read() {
	return  ADC1->DR;
}

void adc1Calibration(uint8_t cal) {
	ADC1->CALFACT &= ~ ADC_CALFACT_CALFACT_S_Msk;
	ADC1->CALFACT |= cal << ADC_CALFACT_CALFACT_S_Pos;
}

void adc1Wakeup() {
	int wait_time;
	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
	ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state
	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launching a calibration or enabling the ADC,
	// the ADC voltage regulator must first be enabled and the software must wait for the
	// regulator start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;
	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator
	// (T_ADCVREG_STUP, i.e. 20 us) before launching a calibration or enabling the ADC.
	wait_time = 20 * (80000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}
}


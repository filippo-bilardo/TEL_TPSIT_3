#include "adc.h"
#include "stm32l4xx.h"

void adc1Init() {
	int TADCVREG_STUP=SystemCoreClock/1000000*20;  //20 us
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  //configurazione PA2 input mode
	GPIOA->MODER |= GPIO_MODER_MODE2_Msk;
	GPIOA->ASCR |= GPIO_ASCR_EN_2; //collegamento PA2 con ADC1
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;//abilitazione clock ADC1
	ADC1->CR &= ~ ADC_CR_ADEN;

	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	ADC123_COMMON->CCR &= ~ ADC_CCR_VREFEN;
	ADC123_COMMON->CCR &= ~ ADC_CCR_PRESC;
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;
	ADC123_COMMON->CCR &= ~ ADC_CCR_DUAL;
	//adc1Wakeup();
	ADC1->CR &= ~ADC_CR_DEEPPWD; //uscita Deep-power-down mode
	ADC1->CR |= ADC_CR_ADVREGEN; //abilitazione voltage regulator
	while(TADCVREG_STUP) {        //attesa abilitazione regulator
		TADCVREG_STUP--;
	}

	//calibrazione
	ADC1->CR &= ~ ADC_CR_ADCALDIF;//calibrazione modalita' singola
	ADC1->CR |= ADC_CR_ADCAL;     //inizio calibrazione
	while(ADC1->CR&ADC_CR_ADCAL); //attesa calibrazione
	
	ADC1->CFGR &= ~ ADC_CFGR_RES;
	ADC1->CFGR &= ~  ADC_CFGR_ALIGN;
	ADC1->SQR1 &= ~  ADC_SQR1_L_Msk;
	ADC1->SQR1 &= ~ ADC_SQR1_SQ1_Msk;
	ADC1->SQR1 |=  ADC_SQR1_SQ1_0|ADC_SQR1_SQ1_1|ADC_SQR1_SQ1_2;
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


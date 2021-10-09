#include "stm32l4xx.h"
#include "SysClock.h"

void SysClockInit(void){
	pllInit(0, 10, 0); // clock a 80Mhz		
}

void MSIInit(uint8_t range) {

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
  // must be correctly programmed according to the frequency of the CPU clock
  // (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;

	RCC->CR |=  RCC_CR_MSION_Msk;
	while((RCC->CR & RCC_CR_MSION_Msk)==0);
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_MSI;
	RCC->CR &= ~ RCC_CR_MSIRANGE_Msk;
	RCC->CR |=range<<RCC_CR_MSIRANGE_Pos;	
	RCC->CR |=RCC_CR_MSIRGSEL;
	
	//CMSIS Function per l'aggiornamento della variabile SystemCoreClock
	SystemCoreClockUpdate();
}

void pllInit(uint8_t M, uint8_t N, uint8_t R) {
	
	uint32_t HSITrim;

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
  // must be correctly programmed according to the frequency of the CPU clock
  // (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;

	// Enable the Internal High Speed oscillator (HSI)
	RCC->CR |= RCC_CR_HSION;
	RCC->CR |=  RCC_CR_HSION_Msk;
	while((RCC->CR & RCC_CR_HSION_Msk)==0);
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR	
	HSITrim = 16; // user-programmable trimming value that is added to HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << RCC_ICSCR_HSITRIM_Pos;

	RCC->CR &= ~ RCC_CR_PLLON; //disabilito PLL
	while((RCC->CR & RCC_CR_PLLRDY_Msk)==RCC_CR_PLLRDY_Msk) {;} //attendo

	// Select clock source to PLL
  RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLSRC_Msk; //
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; //Seleziono HSI come sorgente PLL 
	RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLM_Msk; //M
	RCC->PLLCFGR |= M<<RCC_PLLCFGR_PLLM_Pos;
	RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_Msk; //N
	RCC->PLLCFGR |= N<<RCC_PLLCFGR_PLLN_Pos;
	RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLR_Msk; //R
	RCC->PLLCFGR |= R<<RCC_PLLCFGR_PLLR_Pos;
  
	RCC->CR |= RCC_CR_PLLON; //attivo PLL
	while((RCC->CR & RCC_CR_PLLRDY_Msk)==0) {;}

  RCC->PLLCFGR |= 1<<RCC_PLLCFGR_PLLREN_Pos; //attivo uscita R
	RCC->CFGR &= ~ RCC_CFGR_SW;	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	//CMSIS Function per l'aggiornamento della variabile SystemCoreClock
	SystemCoreClockUpdate();
}

/** ****************************************************************************************
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*        _____   ____
*       |  ___| |  _ \
*       | |     | |_| |
*       |  _|   |  _ /
*       | |     | |_| |
*   www.|_|     |_____/-labs.blogspot.it, https://github.com/filippo-bilardo 
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* \mainpage STML476VG Framework
*
* @brief FRAMEWORK per l'utilizzo del microcontrollore ARM Cortex-M4 presente nelle
* schede STML476VG Discovery e supporto al kernel FreeRTOS
* 
* @author Filippo Bilardo
* @date 09/10/21 
* @version 1.0 09/10/21 Versione iniziale 
*
* @section LICENSE
* Open Source Licensing 
* This program is free software: you can redistribute it and/or modify it under the terms 
* of the GNU General Public License as published by the Free Software Foundation, either 
* version 3 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; 
* withouteven the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
* See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with this
* program. If not, see(http://www.gnu.org/licenses/).
*/
//------------------------------------------------------------------------------------------
//=== INCLUDES =============================================================================
//------------------------------------------------------------------------------------------
// Device Libs
#include "stm32l4xx.h"            // Device header
// Standard Libs
#include <stdint.h>               // Libreria per tipi aritmetici C99
#include <stdbool.h>              // Libreria per tipi booleani
// My Drivers
#include "SysClock.h"
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"

//------------------------------------------------------------------------------------------
//=== FUNCTION PROTOTYPES ==================================================================
//------------------------------------------------------------------------------------------
void vTask_ToggleGreenLED(void * pvParameters);
void vTask_ToggleRedLED(void * pvParameters);
void LedInit(void);
void LedRedToggle(void);
void LedGreenToggle(void);

//------------------------------------------------------------------------------------------
//=== MAIN PROGRAM =========================================================================
//------------------------------------------------------------------------------------------
int main(void){

	//Inizializzazioni
	SysClockInit();		//SET clock to 80MHz
	LedInit();				//Led Red and Green init

	
	/* Create one of the two tasks. */
	xTaskCreate(vTask_ToggleGreenLED, /* Pointer to the function that implements the task.              */
		"Task 1 (Toggle Green LED)",    /* Text name for the task.  This is to facilitate debugging only. */
		200,   /* Stack depth in words.                */
		NULL,  /* We are not using the task parameter. */
		1,     /* This task will run at priority 1.    */
		NULL); /* We are not using the task handle.    */

	xTaskCreate(vTask_ToggleRedLED, /* Pointer to the function that implements the task.              */
		"Task 2 (Toggle Red LED)",    /* Text name for the task.  This is to facilitate debugging only. */
		200,   /* Stack depth in words.                */
		NULL,  /* We are not using the task parameter. */
		1,     /* This task will run at priority 1.    */
		NULL); /* We are not using the task handle.    */
	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();


	// Main loop
	while(1);  // Never Leave Main
}

//------------------------------------------------------------------------------------------
//=== LED ==================================================================================
//------------------------------------------------------------------------------------------
void LedInit(void) {
	
	// Enable the peripheral clock of GPIO Port	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/// Configuring LD4 Red = PB2
	/// 1. Enable the peripheral clock of GPIO Port	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	/// 2. GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->MODER &= ~(3U<<(2*2));  
	GPIOB->MODER |= 1U<<(2*2);      //  Output(01)
	/// 3. GPIO Speed: Low speed (00), Medium speed (01), 
	///             Fast speed (10), High speed (11)
	GPIOB->OSPEEDR &= ~(3U<<(2*2));	// Azzero i due bit di PB2
	GPIOB->OSPEEDR |=   3U<<(2*2);  // Imposto a uno i due bit di PB2 -> High speed
	/// 4. GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOB->OTYPER &= ~(1U<<2);       // Push-pull
	/// 5. GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOB->PUPDR   &= ~(3U<<(2*2));  // No pull-up, no pull-down
	//////////////////////////////////////////////////////////////////////////////////////////
	/// Configuring LD5 Green = PE8
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOE->MODER &= ~(3U<<(2*8));  
	GPIOE->MODER |= 1U<<(2*8);      //  Output(01)
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOE->OSPEEDR &= ~(3U<<(2*8));
	GPIOE->OSPEEDR |=   3U<<(2*8);  // High speed
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOE->OTYPER &= ~(1U<<8);       // Push-pull
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOE->PUPDR   &= ~(3U<<(2*8));  // No pull-up, no pull-down
}
void LedRedToggle(void) {
	GPIOB->ODR ^= GPIO_ODR_ODR_2;
}
void LedGreenToggle(void) {
	GPIOE->ODR ^= GPIO_ODR_ODR_8;
}

//------------------------------------------------------------------------------------------
//=== FreeRTOS =============================================================================
//------------------------------------------------------------------------------------------
void vTask_ToggleGreenLED(void * pvParameters) {
	const TickType_t xDelay = 200 / portTICK_PERIOD_MS; // Convert milliseconds to ticks
	for(;;) {
		LedGreenToggle();  // Toggle PE8
		vTaskDelay(xDelay);  // Block for 500ms. 
	}
}

void vTask_ToggleRedLED(void * pvParameters) {
	const TickType_t xDelay = 800 / portTICK_PERIOD_MS; // Convert milliseconds to ticks
	for(;;) {
		LedRedToggle();    // Toggle PB2
		vTaskDelay(xDelay);  // Block for 1000ms. 
	}
}





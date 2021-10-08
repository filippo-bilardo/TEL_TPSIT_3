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
* @file main.c
* @brief External Interrupt example. Toggle red led light when PA3 button is pressed.
* 
* @author Filippo Bilardo
* @date 08/10/21
* @version 1.0 - 08/10/21 - Versione iniziale. 
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
#include "stm32l4xx.h"

void EXTI3_IRQHandler(void) {  
  if ((EXTI->PR1 & EXTI_PR1_PIF3) != 0) {
    // Toggle LED
		GPIOB->ODR ^= GPIO_ODR_ODR_2;
    // Cleared flag by writing 1
    EXTI->PR1 |= EXTI_PR1_PIF3;
  }
}

int main(void)
{
	/* ********************************************************************************************
	* Led red Init
	* LD4 Red = PB2
	******************************************************************************************** */
	///1. Enable the peripheral clock of GPIO Port	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	///2. GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->MODER &= ~(3U<<(2*2));  
	GPIOB->MODER |= 1U<<(2*2);      //  Output(01)
	///3. GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOB->OSPEEDR &= ~(3U<<(2*2));	// Azzero i due bit di PB2
	GPIOB->OSPEEDR |=   3U<<(2*2);  // Imposto a uno i due bit di PB2 -> High speed
	///4. GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOB->OTYPER &= ~(1U<<2);      // Push-pull
	///5. GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOB->PUPDR   &= ~(3U<<(2*2)); // No pull-up, no pull-down

	/* ********************************************************************************************
	* SW3 Init (PA3)
	* Joystick (MT-008A): 
	*   Right = PA2        Up   = PA3         Center = PA0
	*   Left  = PA1        Down = PA5
	* LD4 Red = PB2
	******************************************************************************************** */
  /// 1. Enable the peripheral clock of GPIO Port	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	
  /// 2. GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
  GPIOA->MODER &= ~3U << 6;
  /// 3. GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
  GPIOA->PUPDR &= ~3U << 6;
  GPIOA->PUPDR |= 2U << 6;    // Pull down
	
	/// 4. Enable Interrupt [opional]
  NVIC_EnableIRQ(EXTI3_IRQn); 
  // Connect External Line to the GPI
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3; 
  SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI3_PA; 
  // Interrupt Mask Register: 0 = marked, 1 = not masked (enabled)
  EXTI->IMR1  |= EXTI_IMR1_IM3;     
  // Rising trigger selection: 0 = trigger disabled, 1 = trigger enabled
  EXTI->RTSR1 |= EXTI_RTSR1_RT3;  
	
	// Main loop. Waiting for the interurpt.
	for(;;) {;}
}

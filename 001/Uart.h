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
* @file Uart.h
* @brief Uart driver
* 
* @author Team STM, Filippo Bilardo
* @date 08/10/19
* @version 1.0 - 08/10/19 - Versione iniziale.
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

#ifndef __STM32L476G_DISCOVERY_UART_H
#define __STM32L476G_DISCOVERY_UART_H

//------------------------------------------------------------------------------------------
//=== INCLUDES =============================================================================
//------------------------------------------------------------------------------------------
#include "stm32l476xx.h"

//------------------------------------------------------------------------------------------
//=== CONSTANTS ============================================================================
//------------------------------------------------------------------------------------------
#define BUFFER_SIZE 32

//------------------------------------------------------------------------------------------
//=== GLOBAL FUNCTION PROTOTYPES ===========================================================
//------------------------------------------------------------------------------------------
void USART_Init(USART_TypeDef * USARTx);
void UART2_Init(void);
void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes);
uint8_t USART_Read(USART_TypeDef * USARTx);

/** ****************************************************************************************
* @brief Configurazione dell'Uart2 utilizzabile per il comunicazione con il pc
*
* @version 1.0 - 02/10/21 - Versione iniziale
* @author Filippo Bilardo
*/
void UartInit(void);
void UartTest(uint8_t testnum);
#endif /* __STM32L476G_DISCOVERY_UART_H */

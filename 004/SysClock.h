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
* @file SysClock.h
* @brief Configurazione del clock
* 
* @author Team STM, Filippo Bilardo
* @date 30/09/21
* @version 1.0 - 30/09/21 - Versione iniziale. 
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
#include <stdint.h>

/** ****************************************************************************************
* @brief configurazione MSI
* \param range
* 0000: range 0 around 100 kHz \n
* 0001: range 1 around 200 kHz \n
* 0010: range 2 around 400 kHz \n
* 0011: range 3 around 800 kHz \n
* 0100: range 4 around 1M Hz \n
* 0101: range 5 around 2 MHz \n
* 0110: range 6 around 4 MHz (reset value) \n
* 0111: range 7 around 8 MHz \n
* 1000: range 8 around 16 MHz \n
* 1001: range 9 around 24 MHz \n
* 1010: range 10 around 32 MHz \n
* 1011: range 11 around 48 MHz \n
*
* @version 1.0 - 02/10/21 - Versione iniziale
* @author Team STM, Filippo Bilardo
*/
void MSIInit(uint8_t range);

/** ****************************************************************************************
* @brief configurazione PLL (fin freq di ingresso al PLL) \n
*  fpll=fin/M*N/R  fin=4-16MHz  fpllmax=80MHz \n
*  impostando M=0, N=10, R=0 con fin=16Mhz \n
*  fpll=16/1*10/2 = 80Mhz - pllInit(0, 10, 0); \n
*
* \param M - Divisore della fin \n	
* 0: PLLM = 1 \n
* 1: PLLM = 2 \n
* ... \n
* 7: PLLM = 8 \n
*
* \param N - Moltiplicatore di (fin/M) \n
* 8: PLLN = 8 \n
* 9: PLLN = 9 \n
* ... \n
* 85: PLLN = 85 \n
* 86: PLLN = 86 \n
*
* \param R - Divisore di ((fin/M)*N) \n
* 0: PLLR = 2 \n
* 1: PLLR = 4 \n
* 2: PLLR = 6 \n
* 3: PLLR = 8 \n
*
* @version 1.0 - 02/10/21 - Versione iniziale
* @author Team STM, Filippo Bilardo
*/
void pllInit(uint8_t M, uint8_t N, uint8_t R);

/** ****************************************************************************************
* @brief  Switch the PLL source from MSI to HSI, and select the PLL as SYSCLK source.
* @version 1.0 - 02/10/21 - Versione iniziale
* @author Team STM, Filippo Bilardo
*/
void SysClockInit(void);

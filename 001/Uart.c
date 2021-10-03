//------------------------------------------------------------------------------------------
//=== INCLUDES =============================================================================
//------------------------------------------------------------------------------------------
#include "Uart.h"
#include <string.h>
#include <stdio.h>
#if !defined (USE_LEDDOG)
#include "Led.h"
#define LedDogOn        LedGreenOn
#define LedDogOff       LedGreenOff
#endif


//------------------------------------------------------------------------------------------
//=== LOCAL FUNCTION PROTOTYPES ============================================================
//------------------------------------------------------------------------------------------
void UART2_GPIO_Init(void);
void USART_Delay(uint32_t us);
void USART_IRQHandler(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t * pRx_counter);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
/*
void UartInit(int8_t uart, int32_t baud, int32_t clock, int32_t configs);
int UartTest(void);
void UartEnable(int8_t uart);
void UartDisable(int8_t uart);
int8_t UartCharsAvail(int8_t uart);
int8_t UartGetChar(int8_t uart);
void UartPutChar(int8_t uart, char data);
void UartPutString(int8_t uart, char *ptr);
void UartGetString(int8_t uart, char *ptr, int32_t length_max);
*/
//------------------------------------------------------------------------------------------
//=== GLOBAL VARIABLES =====================================================================
//------------------------------------------------------------------------------------------
uint8_t buffer[BUFFER_SIZE];

//------------------------------------------------------------------------------------------
//=== GLOBAL FUNCTIONS =====================================================================
//------------------------------------------------------------------------------------------
void USART_Init(USART_TypeDef * USARTx) {
	
	/// UART Ports: \n
	/// =================================================== \n
	/// PA.0 = UART4_TX (AF8)   |  PA.1 = UART4_RX (AF8)    \n 
	/// PB.6 = USART1_TX (AF7)  |  PB.7 = USART1_RX (AF7)   \n
	/// PD.5 = USART2_TX (AF7)  |  PD.6 = USART2_RX (AF7)   \n
	
	// Default setting: 
	//     No hardware flow control, 8 data bits, no parity, 1 start bit and 1 stop bit		
	USARTx->CR1 &= ~USART_CR1_UE;  // Disable USART
	
	// Configure word length to 8 bit
	USARTx->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	
	// Configure oversampling mode: Oversampling by 16 
	USARTx->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8
	
	// Configure stop bits to 1 stop bit
	//   00: 1 Stop bit;      01: 0.5 Stop bit
	//   10: 2 Stop bits;     11: 1.5 Stop bit
	USARTx->CR2 &= ~USART_CR2_STOP;   
                                    
	// CSet Baudrate to 9600 using APB frequency (80,000,000 Hz)
	// If oversampling by 16, Tx/Rx baud = f_CK / USARTDIV,  
	// If oversampling by 8,  Tx/Rx baud = 2*f_CK / USARTDIV
  // When OVER8 = 0, BRR = USARTDIV
	// USARTDIV = 80MHz/9600 = 8333 = 0x208D
	USARTx->BRR  = 0x208D; // Limited to 16 bits

	USARTx->CR1  |= (USART_CR1_RE | USART_CR1_TE);  	// Transmitter and Receiver enable
	
  if (USARTx == UART4){	
		USARTx->CR1 |= USART_CR1_RXNEIE;  			// Received Data Ready to be Read Interrupt  
		USARTx->CR1 &= ~USART_CR1_TCIE;    			// Transmission Complete Interrupt 
		USARTx->CR1 &= ~USART_CR1_IDLEIE;  			// Idle Line Detected Interrupt 
		USARTx->CR1 &= ~USART_CR1_TXEIE;   			// Transmit Data Register Empty Interrupt 
		USARTx->CR1 &= ~USART_CR1_PEIE;    			// Parity Error Interrupt 
		USARTx->CR1 &= ~USART_CR2_LBDIE;				// LIN Break Detection Interrupt Enable
		USARTx->CR1 &= ~USART_CR3_EIE;					// Error Interrupt Enable (Frame error, noise error, overrun error) 
		//USARTx->CR3 &= ~USART_CR3_CTSIE;				// CTS Interrupt
	}

	if (USARTx == USART2){
		USARTx->ICR |= USART_ICR_TCCF;
		USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
	}
	
	USARTx->CR1  |= USART_CR1_UE; // USART enable                 
	
	while ( (USARTx->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
	while ( (USARTx->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
}

void UART2_Init(void) {

	/// 1. Enable the clock of USART 1 & 2
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;  // Enable USART 2 clock		
	
	/// 2. Select the USART1 clock source
	/// - 00: PCLK selected as USART2 clock
	/// - 01: System clock (SYSCLK) selected as USART2 clock
	/// - 10: HSI16 clock selected as USART2 clock
	/// - 11: LSE clock selected as USART2 clock
	//01: System clock (SYSCLK) selected as USART2 clock
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |=  RCC_CCIPR_USART2SEL_0;
	
	UART2_GPIO_Init();
	USART_Init(USART2);
	
	//NVIC_SetPriority(USART2_IRQn, 0);			// Set Priority to 1
	//NVIC_EnableIRQ(USART2_IRQn);					// Enable interrupt of USART1 peripheral
}

uint8_t USART_Read(USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   
 

void UartInit(void) {
	//UartInit(UART00, 115200, 50000000, UART_LCRH_WLEN_8|UART_LCRH_FEN);
	UART2_Init();
}
void UartTest(uint8_t testnum) {
	int8_t cc = 0x56; // (‘V’)
	int32_t xx = 100;
	int16_t yy = -100;
	float zz = 3.14159265;
	char rxByte;
	int a, n;
	float b;

	
	//UartInit(UART00, 115200, 50000000, UART_LCRH_WLEN_8|UART_LCRH_FEN);
	if(testnum==0) 
	{
		printf("\n\nHello world\r\n");	//Hello world
		printf("cc = %c %d %#x\r\n",cc,cc,cc);	//cc = V 86 0x56
		printf("xx = %c %d %#x\r\n",xx,xx,xx);	//xx = d 100 0x64
		printf("yy = %d %#x\r\n",yy,yy);	//yy = -100 0xffffff9c
		printf("zz = %e %E %f %g %3.2f\r\n",zz,zz,zz,zz,zz);	//zz = 3.14159 3.14	
	} 
	else if (testnum==1) 
	{
		for(;;) 
		{
			n = sprintf((char *)buffer, "a = %d\t", a);
			n += sprintf((char *)buffer + n, "b = %f\r\n", b);
			USART_Write(USART2, buffer, n);		
			a = a + 1;
			b = (float)a/100;

			//for (i = 0; i < 80000; i++); // Delay
			//TODO: bug - bisogna digitare due volte il carattere
			printf("Give Red LED control input (Y = On, N = off):\r\n");
			scanf ("%c", &rxByte);
			if (rxByte == 'N' || rxByte == 'n'){
				#if !defined (USE_LEDDOG)
				LedDogOff();
				#endif
				printf("LED is Off\r\n\r\n");
			}
			else if (rxByte == 'Y' || rxByte == 'y'){
				#if !defined (USE_LEDDOG)
				LedDogOn();
				#endif
				printf("LED is On\r\n\r\n");
			}
			else if (rxByte == 'q' || rxByte == 'Q'){
				printf("Exit Uart test.\r\n");
				break;
			}
		}
	}
	else if (testnum==2) 
	{
		char str[]="Invio della stringa 'Hello from UART'\n";
		printf("%s",str);
		
		//UartPutString(UART00, str);
		
		printf("Inserire una stringa:\n");
		scanf("%s",str);
		printf("\nStringa inserita: %s\n", str);
		
		//UartPutString(UART00, "Inserire una stringa:\n");
		//UartGetString(UART00, str, 100);
		//UartPutString(UART00, "Stringa inserita: ");
		//UartPutString(UART00, str);
	} 
}

//------------------------------------------------------------------------------------------
//=== LOCAL FUNCTION =======================================================================
//------------------------------------------------------------------------------------------
/// Implement a dummy __FILE struct, which is called with the FILE structure.
struct __FILE {
    int dummy;
};

// We have to define FILE if prinf is used
FILE __stdout;
FILE __stdin;
 
// Retarget printf() to USART2
int fputc(int ch, FILE *f) { 
  uint8_t c;
  c = ch & 0x00FF;
  USART_Write(USART2, (uint8_t *)&c, 1);
  return(ch);
}

// Retarget scanf() to USART2
int fgetc(FILE *f) {  
  uint8_t rxByte;
  rxByte = USART_Read(USART2);
  return rxByte;
}

void USART_IRQHandler(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t * pRx_counter) {
	
	if(USARTx->ISR & USART_ISR_RXNE) {						// Received data                         
		buffer[*pRx_counter] = USARTx->RDR;         // Reading USART_DR automatically clears the RXNE flag 
		(*pRx_counter)++;  
		if((*pRx_counter) >= BUFFER_SIZE )  {
			(*pRx_counter) = 0;
		}   
	} else if(USARTx->ISR & USART_ISR_TXE) {
 		//USARTx->ISR &= ~USART_ISR_TXE;            // clear interrupt 
		//Tx1_Counter++;
	} else if(USARTx->ISR & USART_ISR_ORE) {			// Overrun Error
		while(1);
	} else if(USARTx->ISR & USART_ISR_PE) {				// Parity Error
		while(1);
	} else if(USARTx->ISR & USART_ISR_PE) {				// USART_ISR_FE	
		while(1);
	} else if (USARTx->ISR & USART_ISR_NE){ 			// Noise Error Flag
		while(1);     
	}	
}

void UART2_GPIO_Init(void) {

	// Enable the peripheral clock of GPIO Port
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIODEN;
	
	// ********************** USART 2 ***************************
	// PD5 = USART2_TX (AF7)
	// PD6 = USART2_RX (AF7)
	// Alternate function, High Speed, Push pull, Pull up
	// **********************************************************
	// Input(00), Output(01), AlterFunc(10), Analog(11)
	GPIOD->MODER   &= ~(0xF << (2*5));	// Clear bits
	GPIOD->MODER   |=   0xA << (2*5);      		
	GPIOD->AFR[0]  |=   0x77<< (4*5);       	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOD->OSPEEDR |=   0xF<<(2*5); 					 	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOD->PUPDR   &= ~(0xF<<(2*5));
	GPIOD->PUPDR   |=   0x5<<(2*5);    				
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOD->OTYPER  &=  ~(0x3<<5) ;       	
}
void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}



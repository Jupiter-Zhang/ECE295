#define F_CPU 8000000UL // Clock speed


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void USART_init(void)
{
	// set baud rate
	unsigned int UBRRValue = 103;
	UBRR1H = UBRRValue >> 8;   // set baud rate [11:8]
	UBRR1L = UBRRValue;        // set baud rate [7:0]

	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);


	// set frame format: Asynchronous, No parity, 1 stop bit, 8 bit data
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}



void USART_TxChar(unsigned char Tchar)
{
	// Wait for empty transmit buffer
	while(!(UCSR1A & (1 << UDRE1))){}

	// Put a char into buffer, sends the char
	UDR1 = Tchar;
}



unsigned char USART_RxChar(void)
{
	// Wait for data to be received
	while (!(UCSR1A & (1 << RXC1))){}

	// Get and return received a char from buffer */
	return UDR1;
}


void USART_RxString(unsigned char* string)
{
	for(int i = 0; i < 12; i++){
		string[i] = USART_RxChar();
		if(string[i] == ';')
		break;
	}
}

void USART_TxString(unsigned char* string){
	for(int i = 0; i < 12; i++){
		if(string[i] == ';')
		break;
		USART_TxChar(string[i]);
	}
	return;
}

void USART_flush(){
	unsigned char strp;
	while(!(UCSR1A & (1 << RXC1))){
		strp = UDR1;
	}
}

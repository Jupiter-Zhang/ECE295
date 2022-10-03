/*
* Created: 3/25/2022
* Author : ECE295 Subsystem D Team_053
* Hanfeng Cai, Pengzhou Xu, Jupiter Zhang
*/

#define F_CPU 8000000UL // Clock speed
#include <avr/io.h>
#include <util/delay.h> //for delay function
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "rotary.h"
#include "i2c.h"
#include "screen_cmds.h"
#include "si5351a.h"
//#include "si5351_asserts.h"
//#include "si5351_errors.h"

//#include "Si5351A_PLL.h"
char pcstr[12];
long int frequency = 8000000; // current/initial frequency = 8M
long int temp = 1000000;
int status = 1; // status from 1 - 4
// 1 for adding 1,000,000
// 2 for adding 100,000
// 3 for adding 10,000
// 4 for adding 100
bool TX_RX_Mode = true; // High for RX mode, low for TX_mode

bool controlMode = false; // false for board control, true for PC

char InitialString[] = "            hello     world";
char PCStart[] = "Hello";
char PCstop[] = "Goodbye";
//prototypes//
long returnIncrement();
void checkRotary();
char *displayString();

int main(void)
{
	// set CLK to 8 Mhz
	CLKPR = 1 << CLKPCE;
	CLKPR = 0;

	// set PA1, PA2, PA3 as input
	DDRA &= ~(1 << DDA1);
	DDRA &= ~(1 << DDA2);
	DDRA &= ~(1 << DDA3);

	// set PD4 as output
	DDRD |= (1 << DDD4);
	DDRA |= (1 << DDA0); // PortA as output (only need PA0 for display)
	volatile uint8_t resetPin, user_PC, TX_RX;
	_delay_ms(5);
	PORTA = PORTA | (0 << PA0); // turn off
	_delay_ms(200);
	PORTA = PORTA | (1 << PA0); // turn on display
	_delay_ms(5);
	i2c_init();
	// Set up I2C
	USART_init();
	// Initialize display/encoder/clock

	RotaryInit();
	screen_init();
	// Write some data
	screen_write_string(InitialString);
	_delay_ms(1000);
	while (1)
	{
		// check all pin values
		// renew display
		resetPin = (PINA & (1 << PINA1)) >> PINA1;
		user_PC = (PINA & (1 << PINA2)) >> PINA2;
		TX_RX = (PINA & (1 << PINA3)) >> PINA3;

		if (!resetPin) // reset screen, freq
		{
			screen_write_string(InitialString);
			frequency = 8000000;
			status = 1;
			TX_RX_Mode = false;
			RotaryResetStatus();
			_delay_ms(1000);
		}
		else if (!user_PC)
		{
			controlMode =(!controlMode);
			_delay_ms(1000);
			continue;
		}
		else if (!TX_RX)
		{
			TX_RX_Mode =(!TX_RX_Mode);
			_delay_ms(1000);
			continue;
		}

		if(TX_RX_Mode)
		PORTD |= (1 << PORTD4); // drive the output pin high
		else
		PORTD &= (0 << PORTD4); // drive the output pin low
		if(temp != frequency)
		{
			temp = frequency;
			si5351aSetFrequency(frequency);
		}
		screen_write_string(displayString());
		if(!controlMode){
			checkRotary();
		}
		else{
			while(1){
				USART_RxString(pcstr);
				//USART_flush();
				if((pcstr[0] == 'T') && (pcstr[1] == 'X')&& (pcstr[2] == '0')){
					TX_RX_Mode = true; //receive mode
					_delay_ms(20);
					break;
					}
				else if((pcstr[0] == 'T') && (pcstr[1] == 'X')&& (pcstr[2] == '1')){
					TX_RX_Mode = false; //transmit mode
					_delay_ms(20);
					break;}
				else if((pcstr[0] == 'T') && (pcstr[1] == 'X')&& (pcstr[2] == ';')){
					if(TX_RX_Mode)
					{
						USART_TxChar('T');
						USART_TxChar('X');
						USART_TxChar('0');
					}
					else{
						USART_TxChar('T');
						USART_TxChar('X');
						USART_TxChar('1');
					}
				}
				else if((pcstr[0] == 'F') && (pcstr[1] == 'A'))
				{
					pcstr[0] = '0';
					pcstr[1] = '0';
					frequency = atol(pcstr);
					break;
				}
				else if((pcstr[0] == 'I') && (pcstr[1] == 'F'))
				{
					for(int i = 0; i < 3; i++)
					{
						USART_TxChar('0');
					}
					char str[9];
					ltoa(frequency, str, 10);
					int num = strlen(str);
					for(int i = 0; i < 9-num; i++)
					{
						USART_TxChar('0');
					}
					for(int i = 0; i < num; i++)
					{
						USART_TxChar(str[i]);
					}
					for(int i = 0; i < 13; i++)
					{
						USART_TxChar('0');
					}
				}
				else if((pcstr[0] == 'B') && (pcstr[1] == 'D'))
				{
					controlMode = false;
					for(int i = 0; i < 7; i++){
						USART_TxChar(PCstop[i]);
					}
					break;
				}
				
			}
		}
		_delay_ms(50);
		//set clock 0
	}
}
long returnIncrement()//get the value of each increment/decrement of rotary encoder
{
	long increment;
	switch(status){
		case 1:
		increment = 1000000; break;
		case 2:
		increment = 100000; break;
		case 3:
		increment = 10000; break;
		case 4:
		increment = 100; break;
	}
	return increment;
}
void checkRotary()//check and update rotary condition
{
	RotaryCheckStatus();
	if (RotaryGetStatus() == 3)
	{//button pressed
		if (status == 4) // status wrap around
		status = 1;
		else
		status++;
		RotaryResetStatus();
	}
	if (RotaryGetStatus() == 2)
	{ //turn left
		frequency -= returnIncrement();
		RotaryResetStatus();
	}
	if (RotaryGetStatus() == 1)
	{//turn right
		frequency += returnIncrement();
		RotaryResetStatus();
	}
}
// make the string to display
char *displayString()
{
	char str[40], str0[40], str1[40], str2[40];
	if (TX_RX_Mode)
	strcpy(str," Receive  ");
	else
	strcpy(str," Transmit ");//11 bits

	switch(status){
		case 1:
		strcpy(str0, "+/-  1 M"); break;
		case 2:
		strcpy(str0, "+/- 100k"); break;
		case 3:
		strcpy(str0, "+/-  10k"); break;//8 bits
		case 4:
		strcpy(str0, "+/-  100"); break;
	}
	ltoa(frequency, str1, 10);//change to int

	if (controlMode)
	{
		if(frequency >= 9999999)
		strcpy(str2,"Hz PC ctrl   ");
		else if(frequency >= 999999)
		strcpy(str2,"Hz  PC ctrl   ");
		else if(frequency >= 99999)
		strcpy(str2,"Hz   PC ctrl   ");
		else if(frequency >= 9999)
		strcpy(str2,"Hz    PC ctrl   ");
		else if(frequency >= 999)
		strcpy(str2,"Hz     PC ctrl   ");
		else strcpy(str2,"Hz      PC ctrl   ");
	}
	else
	{
		if(frequency >= 9999999)
		strcpy(str2,"Hz BD ctrl   ");
		else if(frequency >= 999999)
		strcpy(str2,"Hz  BD ctrl   ");
		else if(frequency >= 99999)
		strcpy(str2,"Hz   BD ctrl   ");
		else if(frequency >= 9999)
		strcpy(str2,"Hz    BD ctrl   ");
		else if(frequency >= 999)
		strcpy(str2,"Hz     BD ctrl   ");
		else
		strcpy(str2,"Hz      BD ctrl   ");
	}
	strcat(str1, str2);
	strcat(str, str1);
	return (strcat(str, str0)); // return full string
}


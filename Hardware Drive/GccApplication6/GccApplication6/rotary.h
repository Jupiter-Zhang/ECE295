//*****************************************************************************
//
// File Name	: 'rotary.h'
// Title		: Reading rotarry encoder functions
// Author		: Scienceprog.com - Copyright (C) 2011
// Created		: 2011-08-01
// Revised		: 2011-08-04
// Version		: 1.0
// Target MCU	: Atmel AVR series (Atmega32)
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef ROTARY_H
#define ROTARY_H
#include <avr/io.h>
//define port where encoder is connected
#define ROTPORT PORTA
#define ROTDDR DDRA
#define ROTPIN PINA
//define rotary encoder pins
#define ROTPA PA4
#define ROTPB PA6
#define ROTPBUTTON	PA5
//define macros to check status
#define ROTA !((1<<ROTPA)&ROTPIN)
#define ROTB !((1<<ROTPB)&ROTPIN)
#define ROTCLICK !((1<<ROTPBUTTON)&ROTPIN)
//prototypes
void RotaryInit(void);
void RotaryCheckStatus(void);
uint8_t RotaryGetStatus(void);
void RotaryResetStatus(void);
#endif

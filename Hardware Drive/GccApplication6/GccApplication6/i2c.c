/*
 * i2c.c - I2C library for avr-gcc for devices with TWI
 *
 * Copyright (C) 2014 Jason Milldrum <milldrum@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <util/twi.h>
#include <avr/power.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include "i2c.h"

void i2c_start(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
}

void i2c_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

void i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
}

uint8_t i2c_read_ack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

uint8_t i2c_read_nack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

uint8_t i2c_status(void)
{
	return TW_STATUS;
}


void i2c_init(void)
{
	TWSR = 0x01; //prescaler of 1
	TWBR = 0x01;

	//enable I2C
	TWCR = 1<<TWEN;
}

/// @brief Send an I2C START
///
/// @return none
///
void I2Cstart()
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // clear interrupt (TWINT), generate a start when possible (TWSTA), enable I2C (TWEN)
	while ((TWCR & (1<<TWINT))==0); // wait until interrupt actually clears
	uint8_t status = TWSR & 0XF8; //check if start transmitted
}


/// @brief Send an I2C STOP
///
/// @return none
///
void I2Cstop()
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //clear interrupt, do a stop and enable
}


/// @brief Send an I2C byte. Wait until byte is sent and
/// check the I2C status register
///
/// @param data - byte to be written to I2C bus
///
/// @return none
///
void I2Cwrite(uint8_t data)
{
	TWDR = data; //write data to data register
	TWCR =  (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	uint8_t status = TWSR & 0XF8; //check if start transmitted
}


/// @brief Write an I2C cmd to DOGS164N LCD display,
/// including start, stop and address operations
///
/// @param addr - Slave address of display
/// @param cmd  - "command" byte to be send
///
/// @return none
///
void I2Csendcmd(uint8_t addr, uint8_t cmd)
{
	I2Cstart();		//start transmission
	I2Cwrite(addr);	//display address
	I2Cwrite(0x00);	//control for command
	I2Cwrite(cmd);	//8 bit data length extension?
	I2Cstop();		//stop transmission
}

/// @brief Write a byte of data to DOGS164N LCD display,
/// including start, stop and address operations
///
/// @param addr - Slave address of display
/// @param data  - "data" byte to be send
///
/// @return none
///
void I2Csenddatum(uint8_t addr, uint8_t data)
{
	I2Cstart();		//start transmission
	I2Cwrite(addr); //display address
	I2Cwrite(0x40); //write data control bit
	I2Cwrite(data); //write the character
	I2Cstop();		//stop transmission
}


/// @brief Write several bytes of data to DOGS164N LCD display,
/// including start, stop and address operations
///
/// @param addr - Slave address of display
/// @param str  - string of "data" bytes to be send
///
/// @example: uint8_t str[10] = "PRINT THIS";
///			  I2Csenddata(0x78,str);
///
/// @return none
///
void I2Csenddata(uint8_t addr, uint8_t *str)
{
	I2Cstart();						//start transmission
	I2Cwrite(addr);					//display address
	I2Cwrite(0x40);					//write data control bit
	for(int i=0; str[i] != 0; i++)	//print string char-by-char until NULL is reached
	I2Cwrite(str[i]);
	I2Cstop();						//stop transmission
}

#define I2C_START 0x08
#define I2C_START_RPT 0x10
#define I2C_SLA_W_ACK 0x18
#define I2C_SLA_R_ACK 0x40
#define I2C_DATA_ACK 0x28
#define I2C_WRITE 0b11000000
#define I2C_READ  0b11000001

uint8_t i2cStart()
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT))) ;

	return (TWSR & 0xF8);
}

void i2cStop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	while ((TWCR & (1<<TWSTO))) ;
}

uint8_t i2cByteSend(uint8_t data)
{
	TWDR = data;

	TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT))) ;

	return (TWSR & 0xF8);
}

uint8_t i2cByteRead()
{
	TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT))) ;

	return (TWDR);
}

uint8_t i2cSendRegister(uint8_t reg, uint8_t data)
{
	uint8_t stts;
	
	stts = i2cStart();
	if (stts != I2C_START) return 1;

	stts = i2cByteSend(I2C_WRITE);
	if (stts != I2C_SLA_W_ACK) return 2;

	stts = i2cByteSend(reg);
	if (stts != I2C_DATA_ACK) return 3;

	stts = i2cByteSend(data);
	if (stts != I2C_DATA_ACK) return 4;

	i2cStop();

	return 0;
}

uint8_t i2cReadRegister(uint8_t reg, uint8_t *data)
{
	uint8_t stts;
	
	stts = i2cStart();
	if (stts != I2C_START) return 1;

	stts = i2cByteSend(I2C_WRITE);
	if (stts != I2C_SLA_W_ACK) return 2;
	
	stts = i2cByteSend(reg);
	if (stts != I2C_DATA_ACK) return 3;

	stts = i2cStart();
	if (stts != I2C_START_RPT) return 4;

	stts = i2cByteSend(I2C_READ);
	if (stts != I2C_SLA_R_ACK) return 5;

	*data = i2cByteRead();

	i2cStop();

	return 0;
}

/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch>
File:     $Id: test_i2cmaster.c,v 1.3 2015/09/16 09:29:24 peter Exp $
Software: AVR-GCC 4.x
Hardware: any AVR device can be used when using i2cmaster.S or any
          AVR device with hardware TWI interface when using twimaster.c

Description:
    This example shows how the I2C/TWI library i2cmaster.S or twimaster.c 
	can be used to access a serial eeprom.
 
*****************************************************************************/
#include <avr/io.h>
#include <utils/delay.h>
#include "i2cmaster.h"
#include "USART.h"


#define UNOSLAVE  0x10      // Adres van Atmega328p I2C slave


int main(void)
{
    unsigned char ret;
    

    DDRD  |= (1 << PIND0);                            
    PORTD &= ~(1 << PIND0);						//terugmeldingspin                         
	
	initUSART();
    i2c_init();                                

	while(1)
	{	
			/* write commando 0xAA naar slave */
			ret = i2c_start(UNOSLAVE+I2C_WRITE);       // set device address and write mode
			if ( ret ) 
			{
				/* failed to issue start condition, possibly no device found */
				i2c_stop();
				PORTD |= (1 << PIND0);                
			}
			else 
			{
				/* issuing start condition ok, device accessible */
				i2c_write(0xAA);                       // ret=0 -> Ok, ret=1 -> no ACK 
				i2c_stop();                            // set stop conditon = release bus

				i2c_start(UNOSLAVE+I2C_READ);          // set device address and read mode
				ret = i2c_readNak();                   // read one byte
				i2c_stop();
        
				void printString("waarde ontvangen : ");
				void transmitByte(ret);
		
				PORTD |= (1 << PIND0);        
			}
			
			
	}
    	
}

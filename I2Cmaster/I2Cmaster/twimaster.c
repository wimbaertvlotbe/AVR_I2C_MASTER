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
	
Opmerking : 
Datum : 20/02/2023
W.Baert
Code aangepast voor communicatie met Atmega328p slave. Integratie USART.h en 
CircularBuffer.h.Proof of Concept-code. Geen extra beveiligingen en/of validatie.
Niet voor commercieel gebruik geschikt.
 
*****************************************************************************/

#define F_CPU  16000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "CircularBuffer.h"
#include "USART.h"

static buffer_t* buffer;
volatile uint8_t usartdata;

int main(void)
{
    volatile unsigned char ret;                       
	
	initUSART();
    i2c_init();                                

	UCSR0B|=(1<<RXCIE0);						// RX-interrupt enable
	
	sei();

	const int SIZE=3;
	uint8_t *datablock = calloc(SIZE, sizeof(uint8_t));
	buffer = init(datablock, SIZE);
	buffer->overwriteProtection = true;
	char UNOSLAVE=0x20;
	uint8_t data[SIZE];

	printString("I2C-Master online...\r\n");
		
	while(true)
	{
		if(bufferIsFull(buffer)) 
		{
			printString(">>alle data ontvangen...\n\r");	
			
			int i=0;
			while(!bufferIsEmpty(buffer))
			{
				data[i] = bufferRead(buffer);							
				i++;						
			}
            
			//printBufferArray(data,SIZE);
			
			   if (data[0] == 'R')
			   {
					 printString("Leesverzoek versturen naar slave\r\n");
					 printString("slaveadres = ");
					 transmitByte(UNOSLAVE);
					 printString("\r\n");
			
					ret=i2c_start(UNOSLAVE+I2C_READ);
			
					if(ret)
					{
						i2c_stop();
						printString("Error\r\n");				
					}
					else
					{
						ret = i2c_readNak();                    
						i2c_stop();
						printString("waarde ontvangen : ");
						transmitByte(ret);
						printString("\r\n");
					}				   
			   }
			   
			   if (data[0] == 'W')
			   {
					 printString("Schrijven naar slave\r\n");
					 ret = i2c_start(UNOSLAVE+I2C_WRITE);	
					 i2c_write(data[1]);	 					      
					 i2c_write(data[2]);     
					 i2c_stop();                            		   
			   }
		}	
	}		
}


ISR(USART_RX_vect)
{
		usartdata = UDR0;	
		transmitByte(usartdata);
        int error = bufferWrite(buffer, usartdata);
		if (error == -1) 
		{
			printString("Pauzeer verzenden. Buffer = vol\n\r");	
		}
}
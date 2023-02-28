/****************************************************************************
Title:    implementatie van een circulair buffer
Date :    10/01/2023
Author:   W. Baert
Software: AVR-GCC 
Hardware: Atmega328p

Description:
    De library voorziet mogelijheden om een circulair buffer aan te maken met
	of zonder overschrijfbeveiliging. De toepassing is besproken in de les. 
	
Opmerking :
 
Proof of Concept-code. Geen extra beveiligingen en/of validatie. Niet voor 
commercieel gebruik. Moet nog voorzien worden van commentaar. 
*****************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CircularBuffer.h"

buffer_t* init(uint8_t * dataBlock, const int size)
{
	buffer_t* buffer = (buffer_t*)malloc(sizeof(buffer_t));
	
	buffer->dataBlock = dataBlock;
	buffer->size = size;
	buffer->readPointer = 0;
	buffer->writePointer = 0;
	buffer->full = false;
	buffer->overwriteProtection = true; //circular buffer. Overwrite is possible
	
	return buffer;
}

int bufferWrite(buffer_t* buf, uint8_t data)
{
	
	if((buf->overwriteProtection) && buf->full)
	{
		buf->error = true;
		return -1; //error : overwrite-protected and buffer is full!
	}
		
	buf->dataBlock[buf->writePointer] = data; 
	bufferWritePointerUpdate(buf);

	return 0; //no error
}

uint8_t bufferRead(buffer_t* buf)
{
	uint8_t data = buf->dataBlock[buf->readPointer];
    bufferReadPointerUpdate(buf);
	return data;		
}

int bufferWritePointerUpdate(buffer_t* buf)
{
	if(buf->full)
	{
		if(++(buf->readPointer) == buf->size)
		{
			buf->readPointer = 0;
		}
	}
	
	if(++(buf->writePointer) == buf->size)
	{
			buf->writePointer = 0;
	}
	
	buf->full = (buf->writePointer == buf->readPointer);
	
	return 0;
}

int bufferReadPointerUpdate(buffer_t* buf)
{
	buf->full = false;
	buf->error = false;
	
	if(++(buf->readPointer) == buf->size)
	{
			buf->readPointer = 0;
	}
	
	return 0;
}

int getBufferSize(buffer_t* buf)
{
	return buf->size;
}

bool getBufferError(buffer_t* buf)
{
	return buf->error;
}

bool bufferIsFull(buffer_t* buf)
{
	return buf->full;
}

bool bufferIsEmpty(buffer_t* buf)
{
	return ((buf->writePointer == buf->readPointer) && !buf->full);
}


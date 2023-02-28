/*
 * CircularBuffer.h
 *
 * Created: 30/01/2023 19:07:26
 *  Author: wim.baert
 */ 


#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_


//******************************************* buffer_t type *******************************************
//fields
//uint8_t* dataBlock : pointer to data that is managed by the buffer
//int readPointer : pointer to the next location in dataBlock that wil be written. Zero-based pointer
//int writePointer : pointer to the next location in dataBlock that wil be read. Zero-based pointer
//int size  : number if uint8_t that fit in the buffer
//bool full : flag indicating the full-state of the buffer : full = True.
//bool overwriteProtection : if 'True' no overwrite possible
//bool error : error is 'true' when overwriteProtection is true and buffer is full.
typedef struct
{
	uint8_t * dataBlock;
	int readPointer; //wijst naar volgende uit te lezen item
	int writePointer; //wijst naar volgende te beschrijven locatie
	int size;
	bool full;
	bool overwriteProtection;
	bool error;
	
} buffer_t;
//******************************************************************************************************

buffer_t* init(uint8_t * dataBlock, const int size);

int bufferWrite(buffer_t* buf, uint8_t data);

uint8_t bufferRead(buffer_t* buf);

int bufferWritePointerUpdate(buffer_t* buf);

int bufferReadPointerUpdate(buffer_t* buf);

int getBufferSize(buffer_t* buf);

bool getBufferError(buffer_t* buf);

bool bufferIsFull(buffer_t* buf);

bool bufferIsEmpty(buffer_t* buf);

#endif /* CIRCULARBUFFER_H_ */
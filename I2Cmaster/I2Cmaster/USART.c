
/*
  Hulpfuncties voor seriële communicatie. Deze oorspronkelijke routines zijn her en der op het 
  Internet te downloaden. De baudrate wordt met behulp van de preprocessor-directives
  in het setbaud.h bestand berekend.
  Voorwaarde is dat de F_CPU en BAUD macro's gedefinieerd zijn. Je moet dus in de onderstaande
  code de kloksnelheid en baudrate opgeven.
  
  De standaardwaarden voor baudrate : 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000 and 256000 bits per seconde
  De F_CPU waarde hangt af van de oscillatorinstellingen die je via de fuses hebt vastgelegd.
  
*/
#define F_CPU  16000000UL  
#define BAUD   9600
#include <stdint.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/setbaud.h>
#include "USART.h"

/* Instellen van de USART

   Bij seriële RS232 communicatie zijn volgende microcontrollerinstellingen belangrijk
   - De baudrate
   - het aantal databits
   - Het aantal stopbits
   - Het enablen van zend- en ontvangstmogelijkheden

*/
void initUSART(void) 
{                                
  UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
  
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif
                                  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit */
}

void transmitByte(uint8_t data) 
{
  /* Wachtlus tot het verzendbuffer (transmit buffer) leeg is */
  while ( ! ( (UCSR0A) & (1 << (UDRE0)) ) ) ;		// is equivalent met "loop_until_bit_is_set(UCSR0A, UDRE0);" macro die gedefinieerd is in avr/sfr_defs.h
  UDR0 = data;                                      // te verzenden data in transmit-buffer schrijven
}

uint8_t receiveByte(void) 
{
  /* Wachtlus tot er data in het ontvangstbuffer (receive buffer) zit */
  while ( ! ( (UCSR0A) & (1 << (RXC0)) ) ) ;		// is equivalent met "loop_until_bit_is_set(UCSR0A, RXC0);" macro die gedefinieerd is in avr/sfr_defs.h
  return UDR0;										// ontvangen karakter retourneren naar aanroepende functie
}


/* 
	Diverse C-functies om data naar de seriële poort te verzenden
	en data van de seriële poort te ontvangen. 
	Deze functies maken gebruik van de transmitByte(uint8_t data)  
	en receiveByte(void) basisroutines. 
	
	printString : verzendt een array van chars één na één naar de ontvanger
	readString : leest een vooraf opgegeven aantal chars in van de ontvanger
	printByte : zet een char om naar decimale representatie. vb. char = 235 -> 02, 03, 05 wordt na elkaar verzonden
	printWord : zet een word (16-bits) om naar decimale representatie. vb. byte = 2358 -> 2, 3, 5, 8 worden na elkaar verzonden
	printBinaryByte : zet een char om naar binaire representatie. vb char = 235 -> 0,1,1,1,0,1,0,1,1 worden na elkaar verzonden
	nibbleToHexCharacter : De hexacecimale waarde van een nibble (4-bits) wordt verzonden
	printHexByte : Het hexadecimale equivalent van een char wordt verzonden. vb. char = 235 -> 0E, 0B worden na elkaar verzonden
	
*/

void printString(const char myString[]) {
  uint8_t i = 0;
  while (myString[i]) {
    transmitByte(myString[i]);
    i++;
  }
}

void readString(char myString[], uint8_t maxLength) {
  char response;
  uint8_t i;
  i = 0;
  while (i < (maxLength - 1)) {                   /* prevent over-runs */
    response = receiveByte();
    transmitByte(response);                                    /* echo */
    if (response == '\r') {                     /* enter marks the end */
      break;
    }
    else {
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}

void printByte(uint8_t byte) {
              /* Converts a byte to a string of decimal text, sends it */
  transmitByte('0' + (byte / 100));                        /* Hundreds */
  transmitByte('0' + ((byte / 10) % 10));                      /* Tens */
  transmitByte('0' + (byte % 10));                             /* Ones */
}

void printWord(uint16_t word) {
  transmitByte('0' + (word / 10000));                 /* Ten-thousands */
  transmitByte('0' + ((word / 1000) % 10));               /* Thousands */
  transmitByte('0' + ((word / 100) % 10));                 /* Hundreds */
  transmitByte('0' + ((word / 10) % 10));                      /* Tens */
  transmitByte('0' + (word % 10));                             /* Ones */
}

void printBinaryByte(uint8_t byte) {
                       /* Prints out a byte as a series of 1's and 0's */
  uint8_t bit;
  for (bit = 7; bit < 255; bit--) {
    if (bit_is_set(byte, bit))			// bit_is_set(byte,bit) is een macro die gedefinieerd is in avr/sfr_defs.h
      transmitByte('1');
    else
      transmitByte('0');
  }
}

char nibbleToHexCharacter(uint8_t nibble) {
                                   /* Converts 4 bits into hexadecimal */
  if (nibble < 10) {
    return ('0' + nibble);
  }
  else {
    return ('A' + nibble - 10);
  }
}

void printHexByte(uint8_t byte) {
                        /* Prints a byte as its hexadecimal equivalent */
  uint8_t nibble;
  nibble = (byte & 0b11110000) >> 4;
  transmitByte(nibbleToHexCharacter(nibble));
  nibble = byte & 0b00001111;
  transmitByte(nibbleToHexCharacter(nibble));
}

uint8_t getNumber(void) {
  // Gets a numerical 0-255 from the serial port.
  // Converts from string to number.
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';
  do {                                                   /* shift over */
    hundreds = tens;
    tens = ones;
    ones = thisChar;
    thisChar = receiveByte();                   /* get a new character */
    transmitByte(thisChar);                                    /* echo */
  } while (thisChar != '\r');                     /* until type return */
  return (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

#include "serializer.h"
#include <string.h>
/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Timer 2
**/
byte init_serializer_UART1()
{
	
	/****** INIT UART1 *****/
	SCON1 = 0x50;
	
	// Réglage timer 4
	T4CON = 0x3D; // Baud Generator for UART1 + Enable TR4
	
	RCAP4L = 0xDC;	// Reload value for BaudRate 19200
	RCAP4H = 0xFF;
	
  return 0;
}

void serializer_receive(byte* read_byte)
{
  if( (SCON1 & 0x01) == 1)
	{
		*read_byte = SBUF1;
		SCON1 &= 0xFE;	// Remise à 0 de RI1
	}else{
    *read_byte = '*';
  }
}

void serializer_send(byte ch)
{
	SBUF1 = ch;
	
	while( (SCON1 & 0x02) == 0){} // Tant que TI1 == 0
	SCON1 &= 0xFD;	// Remise à 0 de TI1
}

void serializer_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    serializer_send(*(str+i));
  }
}
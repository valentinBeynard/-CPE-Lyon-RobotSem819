#include "serializer.h"

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
	
	/****** INIT UART0 *****/
	SM01 = 0;
	SM11 = 1;
	REN1 = 1;		
		
	/****** INIT PIN *****/

	// Init UART0 on Crossbar
	XBR2 = 0x04;
	// Push Pull mode
	P0MDOUT = 0xFF;
	
	// Réglage timer 1
	T2CON = 0x0D; // Capture Mode & Counter Mode & Enable & External Trig enable
	RCLK0 = 1;
	TCLK0 = 1;
	RCAP2L = 0xDC;
	RCAP2H = 0xFF;
	TR2 = 1;
	
  return 0;
}

void serializer_receive(byte* read_byte)
{
  if(RI0 == 1)
	{
		*read_byte = SBUF0;
		RI0 = 0;
	}else{
    *read_byte = '*';
  }
}

void serializer_send(byte ch)
{
	SBUF0 = ch;
	while(TI0 == 0){}
	TI0 = 0;
}

void serializer_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    USART_send(*(str+i));
  }
}
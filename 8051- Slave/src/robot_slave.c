#include "spi_slave.h"
#include "c8051F020.h"

sfr Reg =	0xFF;


void Init_External_clk()
{
	// Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Exern CLK
	OSCICN = 0x08;
		
	CKCON = 0x00;
}

void Enable_Crossbar()
{
	// Enable Crossbar
	XBR2 |= 0x40;
}

void Enable_general_Int()
{
	EA = 1; // Enable general interruption
}

int main (void)
{
	Reg = 0xDE;   // Dévalidation du watchdog 
  Reg = 0xAD;
	
	Init_External_clk();
	
	Init_SPI();
	
	Enable_Crossbar();

	Enable_general_Int();
	
	
	while(1)
	{
		
		
	}
	
}
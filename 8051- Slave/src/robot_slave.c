#include "spi_slave.h"
#include "c8051F020.h"

/*
#############################################################################
        PIN List
#############################################################################
*
*	
*
*	SPI = SCK -> P0.0 | MISO -> P0.1 | MOSI -> P0.2 | NSS -> P0.3
*				SCK : OUT
*				MISO : IN
*				MOSI : OUT
*				NSS : Vcc
*
*	
*
*	
*
*	
*
*	
*
*
*
*
*/

sfr Reg =	0xFF;

OUT_M2 commands = {ACQ_non,
										0,
										GEN_non,
										0,
										0,
										0,
										0,
										Lumiere_non,
										0,
										0,
										0,
										0,
										Servo_non,
										0,
										Photo_non,
										0,
										0};

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
	LED = 0;
	
	while(1)
	{
		if(spi_data_is_ready() == 1)
		{
			spi_parse_cmd(&commands);
			if(commands.Etat_Servo == Servo_oui)
			{
				LED = 1;
			}
		}
		
	}
	
}
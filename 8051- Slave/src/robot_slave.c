#include "spi_slave.h"
#include "light_beam.h"
#include "img_handler.h"
//#include "pointeur_lumineux.h"
#include "c8051F020.h"

/*
#############################################################################
        PIN List
#############################################################################
*
*	UART0 = TX0 -> P0.0 | RX0 -> P0.1
*
*	SPI = SCK -> P0.2 | MISO -> P0.3 | MOSI -> P0.4 | NSS -> P0.5
*				SCK : OUT
*				MISO : IN
*				MOSI : OUT
*				NSS : Vcc
*
*	
*	Light Beam -> P1.0
*	
*	Photo Trigger -> P1.1
*
*	Servo_H -> P1.2
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
										10,
										10,
										20,
										10,
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
	
	Init_uart0();
	
	Init_SPI();
	
	Init_light_beam();
	
	Init_Img_Handler();
	
	Enable_Crossbar();

	Enable_general_Int();
	
	//LED = 0;
	
	
	
	while(1)
	{
		spi_process(&commands);
		
		light_beam_process(&commands);
		
		//change_lumiere(1, 2, 10, 10, &commands.Etat_Lumiere);
		if(commands.Etat_Photo == Photo_1)
		{
			prendre_photo(commands.Photo_Nbre, commands.Photo_Duree);
			commands.Etat_Photo = Photo_non;
		}
		
	}
	
}
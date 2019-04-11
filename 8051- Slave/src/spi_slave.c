//------------------------------------------------------------------------------------
// master.c
//------------------------------------------------------------------------------------
// DATE: 20/03/2019
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//------------------------------------------------------------------------------------
#include "spi_slave.h"
#include "c8051F020.h"

#include <stdlib.h>
#include <string.h>


sbit slave_selector = P1^0;


byte spi_data_in[TRAM_SIZE];
byte spi_data_in_ptr = 0;

byte spi_data_out[TRAM_SIZE] = {0xAA,'a','b','c','d','e','f','d','e',0xBB};
byte spi_data_out_ptr = 0;

byte data_ready = 0;

/*
#############################################################################
        Interrupt Routine
#############################################################################
*/

//On remet à zero le bit d'interruption qui est mis a 1 a chaque fin de transfert de donnees
void spi_int() interrupt 6
{
	byte _delay = 0;
	
	// Reset Transmission Flag
	SPIF = 0;
	
	// Read data from MASTER
	spi_data_in[spi_data_in_ptr] = SPI0DAT;
	
	// Wait at least 2 CLK before update SPIODAT
	for(_delay = 0 ; _delay < 5 ; _delay++);
	
	// Prepare Register with new Data
	SPI0DAT = spi_data_out[spi_data_out_ptr];
	
	spi_data_in_ptr++;
	spi_data_out_ptr++;
	
	// When all Tram is read
	if(spi_data_in_ptr == TRAM_SIZE)
	{
		spi_data_in_ptr = 0;
		spi_data_out_ptr = 0;
		data_ready = 1;
	}
}

/*
#############################################################################
        Peripheric initialize
#############################################################################
*/

void Init_SPI() 
{
	
	//Interruptions
	EIE1 |= 0x01;
	
	// Active Crossbar
	XBR0 |= 0x02;	 //Route les 4 ports de la SPI et les 2 de l'UART0;
	
	
	// Activation des ports 
	P0MDOUT |= 0xFF;
	P0MDOUT &= 0xFD;
	P1MDOUT |= 0xFF;
	
	SPI0CFG = 0x87;//Configure CLK SPI (actif front montant) + transmission des 8 bits
	//SPI0CN |= 0x02;	//active mode master
	SPI0CKR = 0x48; //On definit la fréquence SCK a 150kHz (elle doit etre comprise entre 100 et 200kHz pour une bonne optimisation)
	
	slave_selector = SLAVE_ENABLE;
	
	SPI0CN |= 0x01;	//active la SPI
	
	SPI0DAT = SPI_START_BYTE;
}

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/
/*
void spi_send_char(char a){
		slave_selector = SLAVE_ENABLE;
		
		SPI0DAT = a;
		// Wait until the end of transmission
		while(TXBSY == 1) {}
		
		
		slave_selector = SLAVE_DISEABLE;
}

void spi_transmit(SPI_PACKET* spi_packet)
{
	byte ptr = 0;
	
	// Enable Interrupt
	EIE1 |= 0x01;
	
	while(ptr < TRAM_SIZE)
	{
		spi_send_char(spi_packet->send_data[ptr]);
		ptr++;
	}
	
	// DISEABLE INTERRUPT
	EIE1 &= 0xFE;
	
	spi_packet->ready = 1;
	
	// Check Data Validation TODO
}

*/
void spi_cmd(SPI_PACKET* spi_packet)
{
	spi_packet->send_data[0] = 0xAA;
	spi_packet->send_data[1] = 'A';
	spi_packet->send_data[2] = 'B';
	spi_packet->send_data[3] = 'C';
	spi_packet->send_data[4] = 'D';
	spi_packet->send_data[5] = 'E';
	spi_packet->send_data[6] = 'F';
	spi_packet->send_data[7] = 'G';
	spi_packet->send_data[8] = 'H';
	spi_packet->send_data[9] = 0xBB;

}

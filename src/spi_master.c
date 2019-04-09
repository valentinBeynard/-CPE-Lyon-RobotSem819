//------------------------------------------------------------------------------------
// master.c
//------------------------------------------------------------------------------------
// DATE: 20/03/2019
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//------------------------------------------------------------------------------------
#include "c8051F020.h"
#include "spi_master.h"
#include <stdlib.h>
#include <string.h>

/*
    FULL STATE MACHINE

    Liste des Etats de la machines d'état de la SPI associés à leur fonction
*/
const SPI_FSM_PROCESS spi_state_machine[3] = {
    {SPI_IDLE, &spi_idle},
    {SPI_GET_DATA, &spi_get_data},
    {SPI_SET_DATA, &spi_set_data}
};

SPI_PARSER_STATE spi_state = SPI_IDLE;

sbit slave_selector = P1^0;

byte spi_data_in[BUFFER_SIZE];
byte spi_data_in_ptr = 0;

byte spi_data_out[BUFFER_SIZE];
byte spi_data_out_ptr = 0;


/*
#############################################################################
        Interrupt Routine
#############################################################################
*/

//On remet à zero le bit d'interruption qui est mis a 1 a chaque fin de transfert de donnees
void spi_txrx() interrupt 6
{
	
	// Reset Transmission Flag
	SPIF = 0;
	
	// Read data 
	spi_data_in[spi_data_in_ptr] = SPI0DAT;
	
	
	slave_selector = SLAVE_ENABLE;
		
	SPI0DAT = a;
		// Wait until the end of transmission
	while(TXBSY == 1) {}
		
		
	slave_selector = SLAVE_DISEABLE;
	
	if(spi_data_in[spi_data_in_ptr] == SPI_STOP_BYTE)
	{
		// DISEABLE INTERRUPT
		EIE1 &= 0xFE;
		spi_state = SPI_GET_DATA;
	}
	spi_data_in_ptr++;
	
}

/*
#############################################################################
        Peripheric initialize
#############################################################################
*/

void Init_SPI() 
{
	
	//Interruptions
	//EIE1 |= 0x01;
	
	// Active Crossbar
	XBR0 |= 0x02;	 //Route les 4 ports de la SPI et les 2 de l'UART0;
	
	
	// Activation des ports 
	P0MDOUT |= 0xFF;
	P0MDOUT &= 0xFD;
	P1MDOUT |= 0xFF;
	
	SPI0CFG = 0x87;//Configure CLK SPI (actif front montant) + transmission des 8 bits
	SPI0CN |= 0x02;	//active mode master
	SPI0CKR = 0x48; //On definit la fréquence SCK a 150kHz (elle doit etre comprise entre 100 et 200kHz pour une bonne optimisation)
	
	slave_selector = 0;
	
	SPI0CN |= 0x01;	//active la SPI
}

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

void spi_send_char(char a){
		slave_selector = 0;
		
		SPI0DAT = a;
		// Wait until the end of transmission
		while(TXBSY == 1) {}
		
		
		slave_selector = SLAVE_ENABLE;
}

void spi_receive_char(byte* read_byte)
{
	if(WCOL == 1)
	{
		*read_byte = SPI0DAT;
		WCOL = 0;
	}else{
    *read_byte = 0;
  }
}


void spi_process(SPI_PACKET* spi_packet)
{
	spi_state_machine[spi_state].state_process(spi_packet);
}

void spi_idle(SPI_PACKET* spi_packet)
{
	// Si interrupt deseable
	if((EIE1 & 0x01) != 1 )
	{		
		if(spi_packet->commands->Etat_ACQ_Son == ACQ_oui
			|| spi_packet->commands->Etat_Lumiere != Lumiere_non
			|| spi_packet->commands->Etat_Servo == Servo_V
			|| spi_packet->commands->Etat_Photo != Photo_non)
		{
			// Enable Interrupt
			EIE1 |= 0x01;
		}
	}
	
}

void spi_get_data(SPI_PACKET* spi_packet)
{
	*(spi_packet->spi_data) = spi_data_in;

	memset(spi_data_in, 0, spi_data_in_ptr);
	spi_data_in_ptr = 0;
	
	spi_state = SPI_IDLE;
}

void spi_set_data(SPI_PACKET* spi_packet)
{
	// TODO 
	spi_data_out[0] = SPI_START_BYTE;
	spi_data_out[1] = 0xA1;
	spi_data_out[2] = 0xA2;
	spi_data_out[3] = 0xA3;
	spi_data_out[4] = 0xA4;
	spi_data_out[5] = 0xA5;
	spi_data_out[6] = 0xA6;
	spi_data_out[7] = 0xA7;
	spi_data_out[8] = 0xA8;
	spi_data_out[9] = SPI_STOP_BYTE;

}



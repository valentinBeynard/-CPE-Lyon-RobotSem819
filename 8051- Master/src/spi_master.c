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


sbit slave_selector = P1^0;

byte spi_data_in[TRAM_SIZE + 1];
byte spi_data_in_ptr = 0;

byte spi_data_out[TRAM_SIZE];

/*
#############################################################################
        Interrupt Routine
#############################################################################
*/

//On remet à zero le bit d'interruption qui est mis a 1 a chaque fin de transfert de donnees
void spi_int() interrupt 6
{
	// Reset Transmission Flag
	SPIF = 0;
	
	// Read 
	spi_data_in[spi_data_in_ptr] = SPI0DAT;
	
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
	
	slave_selector = SLAVE_ENABLE;
	
	SPI0CN |= 0x01;	//active la SPI
}

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

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
	byte _delay = 0;
	
	// Reset Buffer before transmission
	memset(spi_data_in, 0 , spi_data_in_ptr);
	spi_data_in_ptr = 0;
	
	// Enable Interrupt
	EIE1 |= 0x01;
	
	while(ptr < TRAM_SIZE)
	{
		spi_send_char(spi_packet->send_data[ptr]);
		ptr++;
		// Wait at least 2 CLK before update SPIODAT
		for(_delay = 0 ; _delay < 20 ; _delay++);
	}

	// DISEABLE INTERRUPT
	EIE1 &= 0xFE;

	spi_data_in[spi_data_in_ptr + 1] = '\0';
	
	// Get DATA
	spi_packet->received_data = spi_data_in;
	
	spi_packet->ready = 1;
	
	memset(spi_data_out, 0, ptr);
	
	// Check Data Validation TODO
}

void spi_process(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	if(cmd->Etat_ACQ_Son == ACQ_oui)
	{
		
	}
	else if(cmd->Etat_GEN_Son == GEN_oui)
	{
		
	}
	else if(cmd->Etat_Lumiere != Lumiere_non)
	{
		
	}
	else if(cmd->Etat_Servo == Servo_V)
	{
		spi_cmd_servo(cmd, spi_packet);
		cmd->Etat_Servo = Servo_non;
		spi_packet->ready = 1;
	}
	else if(cmd->Etat_Photo != Photo_non)
	{
		
	}
	else
	{
		
	}
}

void spi_cmd_servo(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	/*spi_packet->send_data[0] = SPI_SERVO_CMD;
	spi_packet->send_data[1] = (cmd->Servo_Angle);
	spi_packet->send_data[2] = (cmd->Servo_Angle) >> 8;
	spi_packet->send_data[3] = 0;
	spi_packet->send_data[4] = 0;*/
	
		spi_packet->send_data[0] = 'A';
	spi_packet->send_data[1] = 'B';
	spi_packet->send_data[2] = 'C';
	spi_packet->send_data[3] = 'D';
	spi_packet->send_data[4] = 'E';
}


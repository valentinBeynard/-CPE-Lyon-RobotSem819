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


byte spi_data_in[TRAM_SIZE];
byte spi_data_in_ptr = 0;

byte spi_data_out[TRAM_SIZE] = {'A', 'L','I','D','V'};
byte spi_data_out_ptr = 0;

byte spi_data_ready = 0;

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
		spi_data_ready = 1;
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
	
	// First Value that MASTER will receive
	SPI0DAT = 'V';
	
	SPI0CN |= 0x01;	//active la SPI
	
	//SPI0DAT = SPI_START_BYTE;
}

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

void spi_parse_cmd(OUT_M2* cmd)
{
	switch(spi_data_in[0])
	{
		case 0xA1:
			cmd->Etat_ACQ_Son = ACQ_oui;
			cmd->ACQ_Duree = spi_data_in[1];
			break;
		case 0xA2:
			cmd->Etat_GEN_Son = GEN_oui;
			cmd->GEN_freq_code = spi_data_in[1];
			cmd->GEN_son_Duree = spi_data_in[2];
			cmd->GEN_silence_Duree = spi_data_in[3];
			cmd->GEN_nbr_bip = spi_data_in[4];
			break;
		case 0xA3:
			cmd->Etat_Lumiere = Allumer;
			cmd->Lumiere_Intensite = spi_data_in[1];
			cmd->Lumiere_Duree = spi_data_in[2];
			cmd->Lumire_Extinction = spi_data_in[3];
			cmd->Lumiere_Nbre = spi_data_in[4];
			break;
		case 0xA4:
			cmd->Etat_Lumiere = Eteindre;
			break;
		case 0xA5:
			cmd->Etat_Servo = Servo_oui;
			cmd->Servo_Angle = (spi_data_in[2] << 8) + spi_data_in[1];
			break;
		case 0xA6:
			switch(spi_data_in[1])
			{
				case 1:
					cmd->Etat_Photo = Photo_1;
					break;
				case 2:
					cmd->Etat_Photo = Photo_Multiple;
					break;
				case 3:
					cmd->Etat_Photo = Photo_continue;
					break;
			}
			cmd->Photo_Duree = spi_data_in[2];
			cmd->Photo_Nbre = spi_data_in[3];
			break;
		case 0xA7:
			cmd->Etat_Photo = Photo_non;
			break;
		default:
			cmd->Etat_ACQ_Son = ACQ_non;
			break;
	}
	spi_data_ready = 0;
}

byte spi_data_is_ready()
{
	return spi_data_ready;
}

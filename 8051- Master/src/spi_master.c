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

byte spi_data_in[TRAM_SIZE];
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

	// Check if received _data are correct througth SPI Slave Flag 'VALID'
	if(spi_validate() == 0)
	{
		spi_error();
	}
	else{
		spi_packet->ready = 1;
	}
	
	// Get DATA
	spi_packet->received_data = spi_data_in;
	
	memset(spi_data_out, 0, ptr);
	
}

void spi_process(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	if(cmd->Etat_ACQ_Son == ACQ_oui)
	{
		spi_cmd_acq_sound(cmd, spi_packet);
		cmd->Etat_ACQ_Son = ACQ_non;
		spi_packet->ready = 1;
	}
	else if(cmd->Etat_GEN_Son == GEN_oui)
	{
		spi_cmd_generate_sound(cmd, spi_packet);
		cmd->Etat_GEN_Son = GEN_non;
		spi_packet->ready = 1;
	}
	else if(cmd->Etat_Lumiere != Lumiere_non)
	{
		if(cmd->Etat_Lumiere == Allumer)
		{
			spi_cmd_light_ON(cmd, spi_packet);
			cmd->Etat_Lumiere = Lumiere_non;
			spi_packet->ready = 1;
		}
		else
		{
			spi_cmd_light_OFF(cmd, spi_packet);
			cmd->Etat_Lumiere = Lumiere_non;
			spi_packet->ready = 1;
		}
	}
	else if(cmd->Etat_Servo == Servo_V)
	{
		spi_cmd_servo(cmd, spi_packet);
		cmd->Etat_Servo = Servo_non;
		spi_packet->ready = 1;
	}
	else if(cmd->Etat_Photo != Photo_non)
	{
		if(cmd->Etat_Photo != Photo_stop)
		{
			spi_cmd_photo_ON(cmd, spi_packet);
			cmd->Etat_Photo = Photo_non;
			spi_packet->ready = 1;
		}
		else
		{
			spi_cmd_photo_OFF(cmd, spi_packet);
			cmd->Etat_Photo = Photo_non;
			spi_packet->ready = 1;
		}
	}
	else
	{

	}
	
	
	if(spi_packet->ready == 1)
	{
		spi_transmit(spi_packet);
		spi_packet->ready = 0;
	}
	
}

void spi_cmd_servo(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	spi_packet->send_data[0] = SPI_SERVO_CMD;
	spi_packet->send_data[1] = (cmd->Servo_Angle);
	spi_packet->send_data[2] = (cmd->Servo_Angle) >> 8;
	spi_packet->send_data[3] = 0;
	spi_packet->send_data[4] = 0;
}

void spi_cmd_light_ON(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	spi_packet->send_data[0] = SPI_LIGHT_ON_CMD;
	spi_packet->send_data[1] = (cmd->Lumiere_Intensite);
	spi_packet->send_data[2] = (cmd->Lumiere_Duree);
	spi_packet->send_data[3] = (cmd->Lumire_Extinction);
	spi_packet->send_data[4] = (cmd->Lumiere_Nbre);
	
}

void spi_cmd_light_OFF(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	/* For Warning Suppression */
	cmd = cmd;
	
	spi_packet->send_data[0] = SPI_LIGHT_OFF_CMD;
	spi_packet->send_data[1] = 0;
	spi_packet->send_data[2] = 0;
	spi_packet->send_data[3] = 0;
	spi_packet->send_data[4] = 0;
	
}

void spi_cmd_generate_sound(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	spi_packet->send_data[0] = SPI_GENERATE_SOUND_CMD;
	spi_packet->send_data[1] = (cmd->GEN_freq_code);
	spi_packet->send_data[2] = (cmd->GEN_son_Duree);
	spi_packet->send_data[3] = (cmd->GEN_silence_Duree);
	spi_packet->send_data[4] = (cmd->GEN_nbr_bip);
}

void spi_cmd_acq_sound(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	spi_packet->send_data[0] = SPI_ACQ_SOUND_CMD;
	spi_packet->send_data[1] = (cmd->ACQ_Duree);
	spi_packet->send_data[2] = 0;
	spi_packet->send_data[3] = 0;
	spi_packet->send_data[4] = 0;
}

void spi_cmd_photo_ON(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	/* For Warning Suppression */
	cmd = cmd;
	
	spi_packet->send_data[0] = SPI_PHOTO_ON_CMD;
	spi_packet->send_data[1] = cmd->Etat_Photo;
	spi_packet->send_data[2] = cmd->Photo_Duree;
	spi_packet->send_data[3] = cmd->Photo_Nbre;
	spi_packet->send_data[4] = 0;
	
}

void spi_cmd_photo_OFF(OUT_M1 * cmd, SPI_PACKET* spi_packet)
{
	/* For Warning Suppression */
	cmd = cmd;
	
	spi_packet->send_data[0] = SPI_PHOTO_OFF_CMD;
	spi_packet->send_data[1] = 0;
	spi_packet->send_data[2] = 0;
	spi_packet->send_data[3] = 0;
	spi_packet->send_data[4] = 0;
}

byte spi_validate()
{
	int hash = 0, i =0;
	
	for(i = 0; i < 5 ; i++)
	{
		hash += spi_data_in[i];
	}
	
	if(hash != 368)
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}

void spi_error()
{
	spi_data_in[0] = 'E';
	spi_data_in[1] = 'R';
	spi_data_in[2] = 'R';
	spi_data_in[3] = 'O';
	spi_data_in[4] = 'R';
}


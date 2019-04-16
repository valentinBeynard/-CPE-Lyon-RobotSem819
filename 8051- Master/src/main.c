#include "debug.h"
#include "commands_parser.h"
#include "serializer.h"
#include "distance_detector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "spi_master.h"

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
*	UART1 : TX1 -> P0.6 | RX1 -> P0.7
*
*	Pin NSS_slave : P1.0
*
*	Pin Servo_H : P1.2
*
*	Pin ADC1 : AIN0
*
*
*
*
*/


sfr Reg =	0xFF;

	OUT_M1 commands = {Epreuve_non, // Numéro Epreuve
                      Mouvement_non,  // Etat mouvement
                      0,  //  Vitesse
                      0,  // Coord_X
                      0,  // Coord_Y
                      0,  // Angle
                      ACQ_non,//ACQ_non,  // Commande d'acquisition du son
                      0,  // Durée d'acquisition
											GEN_non,
											0,  //  Code fréquence
                      0,  // Duree son
                      0,  // Duree silence
                      0,  // Nombre de bip
                      DCT_non,  // Commande de détection d'obstacle
                      0,  // Résolution angulaire de la détection d'obstacle
                      Lumiere_non,  // Commande d'allumage du pointeur lumineux
                      0,  //Intensitée lumineuse
                      0,  // Durée d'allumage
                      0,  // Durée d'extinction
                      0,  // Nombre de cycles d'allumage
                      Servo_non,  // Commande de position du servo
                      0,  // Paramètre angle
                      Energie_non,  // Commande relevée courant
                      Position_non, // Commande de gestion de position
                      0,  // Coord X de position
                      0,  // Coord Y de position
                      0,  // Angle
                      Photo_non,  // Commande de prise de phot
                      0,  // Temps entre deux photo
                      0 // Nbr de photo
                      };
											
	IN_M1 informations = { Invite_non,	// Identifieur de l'etat d'Invite
													"Start Epreuve !\n",	// Ptr sur la chaine contenant le msg d'invite
													BUT_Atteint_non,	// Arrivée au point transmit
													BUT_Servo_non,	// Information position servomoteur
													DCT_Obst_non,	// Mode de détection d'obstacle
													0,	// ptr vers tableau des distances
													0,	// taille du tableau 
													RESULT_Courant_non,	// Identificateur pour la mesure de courant
													0,	// Valeur de courant relevée
													RESULT_Energie_non,	// Identificateur pour la mesure d'énergie
													0,	// Valeur d'énergie relevée
													RESULT_Position_non,	// Identificateur de position du robot
													0,	// Coord X
													0,	// Coord Y
													0,	// Angle
													Aux_non,	// Identificateur pour la commande auxiliare
													0	// ptr vers la chaîne de caractère auxiliare
											};
	
	
			
											
PARSER_RESULT parser_result = {1 , &commands, &informations};
DD_PACKET dd_packet = {&commands, &informations, 0.0, 0};
SPI_PACKET spi_packet = { 0, {0xA5,0x00,0x64,'F','G'}, 0};

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

void enable_general_Int()
{
	EA = 1; // Enable general interruption
}


int main (void)
{
  char mes[10];
	float f = 0.0;
	int angle = 80;
	int ij = 0;
  
	
	Reg = 0xDE;   // Dévalidation du watchdog 
  Reg = 0xAD;
	
	// µP candencé sur clk extern à 22 MHz						
	Init_External_clk();
	
	// Initialise l'UART0 et le Timer 2 pour le parser de commandes				
  init_parser();
		
	// Initialise l'UART1 utilisé pour communiquer avec le sérializer
	init_serializer_UART1();
							
	Init_distance_detector();
	
	Init_SPI();
	
	Enable_Crossbar();

	enable_general_Int();
	
	USART_print("Start Routine \n\n");
	USART_print("Waiting for Serializer Init Processing... \n\n");
	

					
	//serializer_init_serial();
							
	USART_print("\n\n#############################\n\n>");

	//serializer_print("mogo 1:50 2:50");
						
  while(1)
  {
    
    cmd_parser_process(&parser_result);
    
		if( parser_result.commands->Etat_Epreuve == Stop_Urgence)
    {
      USART_print("Quit");
			break;
    }
		
		if( parser_result.commands->Etat_Epreuve == epreuve1)
    {
      //USART_print(parser_result.informations.MSG_Invit);			
			
			serializer_process(&parser_result);
			
			
			distance_detector_process(&dd_packet);
			
			spi_process(&commands, &spi_packet);
			
			if(spi_packet.ready == 1)
			{
				USART_print("SPI START TS ! \n ");
				spi_transmit(&spi_packet);
				spi_packet.ready = 0;
				USART_print("RESULT : ");
				USART_print(spi_packet.received_data);
				USART_print("\n");
				//break;
			}
			
			//USART_print(spi_packet.spi_data);
			
			if(parser_result.commands->Etat_DCT_Obst == oui_180)
			{
				dd_packet.measure = dd_start_conversion();
				sprintf(mes, "%f", dd_packet.measure);
				USART_print(mes);
				memset(mes, 0, 10);
				parser_result.commands->Etat_DCT_Obst = DCT_non;
			}
			
			if(parser_result.informations->Etat_BUT_Mouvement == BUT_Atteint_oui)
			{
				USART_print("Target Reached !! Success !\n");
				parser_result.informations->Etat_BUT_Mouvement = BUT_Atteint_non;
			}
			
    }
		else{
			parser_result.commands->Etat_Mouvement = Mouvement_non;
		}
		
	}
	
	USART_print("Fin Soft");
	
	while(1){}
		
	/*spi_packet.ready = 1;
	while(1)
	{
		//spi_transmit(&spi_packet);
			//spi_process(&commands, &spi_packet);
			
		
				USART_print("SPI START TS ! \n ");
				spi_transmit(&spi_packet);
				spi_packet.ready = 0;
				USART_print("RESULT : ");
				USART_print(spi_packet.received_data);
				USART_print("\n");
	}*/
	
}



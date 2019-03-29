#include "debug.h"
#include "commands_parser.h"
#include "serializer.h"
#include "telemetres.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

sfr Reg =	0xFF;

	OUT_M1 commands = {Epreuve_non, // Numéro Epreuve
                      Mouvement_non,  // Etat mouvement
                      0,  //  Vitesse
                      0,  // Coord_X
                      0,  // Coord_Y
                      0,  // Angle
                      ACQ_non,  // Commande d'acquisition du son
                      0,  // Durée d'acquisition
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

void Init_External_clk()
{
	// Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Exern CLK
	OSCICN = 0x08;
}

void Init_Crossbar()
{
	// Init UART0 on Crossbar
	XBR0 = 0x04;
	
	// Init UART1 on Crossbar
	XBR2 |= 0x04;
	
	// Push Pull mode
	P0MDOUT = 0xFF;
	
	// Enable Crossbar
	XBR2 |= 0x40;
}


#ifdef ROBOT

int main (void)
{
  char mes[10];
	float f = 0.0;								
  
	
	Reg = 0xDE;   // Dévalidation du watchdog 
  Reg = 0xAD;
	
	// µP candencé sur clk extern à 22 MHz						
	Init_External_clk();
	
	// Initialise l'UART0 et le Timer 2 pour le parser de commandes				
  init_parser();
		
	// Initialise l'UART1 utilisé pour communiquer avec le sérializer
	init_serializer_UART1();
							
	Init_Crossbar();
	
	init_telemeter();

	USART_print("Start Routine \n\n");
	USART_print("Waiting for Serializer Init Processing... \n\n");
					
	serializer_init_serial();
							
	USART_print("\n\n#############################\n\n>");

	//serializer_print("mogo 1:50 2:50");
						
  while(1)
  {
    
		//parser_process(state, &parser_result);
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
			
			// Commande Télémètre
			/*
			if(parser_result.commands.Etat_DCT_Obst == oui_180)
			{
				f = start_conversion();
				sprintf(mes, "%f", f);
				USART_print(mes);
				memset(mes, 0, 10);
				parser_result.commands.Etat_DCT_Obst = DCT_non;
			}
			*/
			if(parser_result.informations->Etat_BUT_Mouvement == BUT_Atteint_oui)
			{
				USART_print("Target Reached !! Success !\n");
				parser_result.informations->Etat_BUT_Mouvement = BUT_Atteint_non;
			}
			
    }
		else{
			parser_result.commands->Etat_Mouvement = Mouvement_non;
		}
		
		//serializer_send('H');
    //USART_send('A');
    //printf("Commande lu : %u", (int)(parser_result.commands->Etat_Epreuve));
  
		
		//serializer_print("mogo 1:50 2:50\r");
		
	}
	
	USART_print("Fin Soft");
	
	while(1);
}

#endif

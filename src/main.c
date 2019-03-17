#include "debug.h"
#include "commands_parser.h"
#include "serializer.h"

sfr Reg =	0xFF;

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

  PARSER_RESULT parser_result = {1 , &commands};
  
	
	Reg = 0xDE;   // Dévalidation du watchdog 
  Reg = 0xAD;
	
	// µP candencé sur clk extern à 22 MHz						
	Init_External_clk();
	
	// Initialise l'UART0 et le Timer 2 pour le parser de commandes				
  init_parser();
		
	// Initialise l'UART1 utilisé pour communiquer avec le sérializer
	init_serializer_UART1();
							
	Init_Crossbar();

	USART_print("Start Routine \n\n");
	USART_print("Waiting for Serializer Init Processing... \n\n");
					
	serializer_init_serial();
							
	USART_print("\n\n#############################\n\n>");

	//serializer_print("mogo 1:50 2:50");
						
  while(1)
  {
    
		//parser_process(state, &parser_result);
    cmd_parser_process(&parser_result);
    
		if( parser_result.commands.Etat_Epreuve == Stop_Urgence)
    {
      USART_print("Quit");
			break;
    }
		
		if( parser_result.commands.Etat_Epreuve == epreuve1)
    {
      //USART_print("Epreuve 1 !");
			serializer_process(&(parser_result.commands));
    }
		else{
			parser_result.commands.Etat_Mouvement = Mouvement_non;
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

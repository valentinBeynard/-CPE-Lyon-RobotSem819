#include "commands_parser.h"
#include <string.h>
//#include <stdlib.h>

/*
    DISPATCH TABLE

    Tableau regroupant commande et fonction associée
    On utilise un tablleau de structure CMD_ afin de référencer toutes les
    commandes implémentées et les lier à une fonctionnalitée.
*/
const CMD_ dispatch_table [NUMBER_OF_COMMAND] = {
    {"D", start_test},
    {"E", default_process},
    {"Q", default_process},
    {"TV", default_process},
    {"A", default_process},
    {"B", default_process},
    {"S", default_process},
    {"RD", default_process},
    {"RG", default_process},
    {"RC", default_process},
    {"RA", default_process},
    {"G", default_process},
    {"ASS", default_process},
    {"MI", default_process},
    {"ME", default_process},
    {"IPO", default_process},
    {"POS", default_process},
    {"MOU", default_process},
    {"MOB", default_process},
    {"MOS", default_process},
    {"SD", default_process},
    {"L", default_process},
    {"LS", default_process},
    {"CS", default_process},
    {"PPH", default_process},
    {"SPH", default_process},
    {"AUX", default_process}
};

/*
    FULL STATE MACHINE

    Liste des Etats de la machines d'état du Parser associés à leur fonction
*/
const FSM_PROCESS full_state_machine[3] = {
    {WAIT, &wait},
    {GET_COMMAND, &get_command},
    {SEND_COMMAND, &wait}
};

/* Etat courant de la machine d'Etat */
COMMANDS_PARSER_STATE current_state = WAIT;

/* Buffer principal dans lesquel est stocké tout caractère reçu sur la liaison UART du µP */
byte raw_data[COMMAND_BUFFER_SIZE];

/* Pointer permettant de parcourir le Buffer Principal */
byte buffer_index = 0;

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Interrupt
**/
byte init_parser()
{
  // Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Exern CLK
	OSCICN = 0x08;
	
	
	/****** INIT UART0 *****/
	SM00 = 0;
	SM10 = 1;
	REN0 = 1;		
		
	/****** INIT PIN *****/
	
	/* PIN SYSCLK */

	// Enable Crossbar
	XBR2 = 0x40;
	// Init UART0 on Crossbar
	XBR0 = 0x04;
	// Push Pull mode
	P0MDOUT = 0xFF;
	
	/****** INIT TIMER ******/
	CKCON = 0x00; // Set CLK divider to 12 for timer 0 and 2
	
	
	// Réglage timer 2
	T2CON = 0x0D; // Capture Mode & Counter Mode & Enable & External Trig enable
	RCLK0 = 1;
	TCLK0 = 1;
	RCAP2L = 0xDC;
	RCAP2H = 0xFF;
	TR2 = 1;
	
  return 0;
}

void USART_receive(byte* read_byte)
{
  if(RI0 == 1)
	{
		*read_byte = SBUF0;
		RI0 = 0;
	}else{
    *read_byte = '*';
  }
}

void USART_send(byte ch)
{
	SBUF0 = ch;
	while(TI0 == 0){}
	TI0 = 0;
}



/*
#############################################################################
        Core Code of commands_parser.c
#############################################################################
*/

void USART_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    USART_send(*(str+i));
  }
}


//const FSM_PROCESS* cmd_parser_next_state()
void cmd_parser_process(PARSER_RESULT* result)
{
  //return &full_state_machine[current_state];
	/*
		En utilisant un pointer de function, un problème en mémoire apparait
		et le programme ne fonctionne plus comme prévu...
		Il semblerait que lorsqu'on arrive à l'appelle de wait par le biais
		de son ptr créer le problème. A ce moment, on vient écrire à l'adresse 0x000114
		de "result" la valeur du ptr de result...
	*/
  (full_state_machine[current_state]).state_process(result);
	
	/*
		Sans passer par les pointer de fonctions, plus de problème ...
	*/
	/*switch(current_state)
	{
		case WAIT:
			wait(result);
			break;
		case GET_COMMAND:
			get_command(result);
			break;
		default:
			wait(result);
		break;
		
	}*/
	//wait(result);
}


void wait(PARSER_RESULT* parser_result)
{
  byte read_byte = '*';

  // Scrutation sur l'UART0
  USART_receive(&read_byte);

  // Si on lit un caractère
  if(read_byte != '*'){
      if(read_byte == STOP_BYTE)
      {
        // On passe à l'Etat 'Get Command' après avoir terminé de préparer le Buffer
        //USART_print("\nCommand Mode !\n");
        raw_data[buffer_index] = read_byte;
        buffer_index++;
        // Tjrs ajouter un '\0' à la fin du buffer, sinon strlen ne marche pas...
        raw_data[buffer_index] = '\0';
        current_state = GET_COMMAND;
      }else{
        USART_send(read_byte);
        raw_data[buffer_index] = read_byte;
        buffer_index++;
      }
  }
}


void get_command(PARSER_RESULT* parser_result)
{
    // On analyse le buffer brute
    if (!parse(parser_result))
    {
      USART_print("Error decoding cmd !");
    }
    //USART_print("Fin");
    // Clear le buffer pour la prochaine lecture
    memset(raw_data, 0, strlen(raw_data));
    buffer_index = 0;
    // On revient dans l'Etat où on scrute l'UART0
    current_state = WAIT;
}


byte parse(PARSER_RESULT* parser_result)
{
  // Tableau dans lequel on va ranger : [Nom_Commande][Param1][Valeur1][...]
  char commands_data[MAX_COMMAND_WORD][ARGS_BUFFER_SIZE];
  // Pointers pour parcourir les différents buffers
  byte data_index = 0, ptr = 0, reading_ptr = 0;

  char c = 0, i = 0;
  char run = 1;
	
	CMD_PACKET cmd_packet;

  // Dispatches commands and arguments in commands_data buffer
  while(run)
  {
      c = *(raw_data + reading_ptr);

      switch (c)
      {
        case COMMAND_SEPARATOR:
          *( *(commands_data + data_index) + ptr ) = '\0';
          //USART_print(commands_data[data_index]);
          data_index++;
          ptr = 0;
          break;

        case ARG_VALUE_SEPARATOR:
          *( *(commands_data + data_index) + ptr ) = '\0';
          //USART_print(commands_data[data_index]);
          data_index++;
          ptr = 0;
          break;

          // Fin de l'analyse
        case STOP_BYTE:
          *( *(commands_data + data_index) + ptr ) = '\0';
          //USART_print(commands_data[data_index]);
          run = 0;
          break;

        default:
          *( *(commands_data + data_index) + ptr ) = c;
          //commands_data[data_index][ptr] = c;
          ptr++;
      }

      reading_ptr++;
  }
	
	cmd_packet.commands_data = commands_data;
	cmd_packet.cmd_size = data_index;
	cmd_packet.commands = &(parser_result->commands);

  // Looking for the command in the Command Dictionnary
  for(i = 0 ; i < NUMBER_OF_COMMAND; i++)
  {
    if(strcmp(*(commands_data), dispatch_table[i].name) == 0)
    {
      dispatch_table[i].process(&cmd_packet);
      USART_print("Find ");
      USART_print(dispatch_table[i].name);
      return 1;
    }
  }

  return 0;
}


void read_command(PARSER_RESULT* parser_result)
{
    // Nothing TODO
}

/*
char encode_data()
{


}*/

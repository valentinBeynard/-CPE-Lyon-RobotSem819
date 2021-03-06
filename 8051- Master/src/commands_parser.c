#include "commands_parser.h"
#include <string.h>
#include <stdio.h>

/*
    DISPATCH TABLE

    Tableau regroupant commande et fonction associée
    On utilise un tablleau de structure CMD_ afin de référencer toutes les
    commandes implémentées et les lier à une fonctionnalitée.
*/
const CMD_ dispatch_table [NUMBER_OF_COMMAND] = {
		/*{"Nom de la Commande", Nombre minimum d'arg à rentrer, nombre d'arg  possible, function},
	Exemple : 
	
	La commande "RA sens:valeur", est rentré comme suit : "RA D:90"
	
	Le cahier des Charges spécifie qu'on peut le sens de la rotation donné par l'arg sens peut
	ne pas être rentré. On a donc que l'arg 'valeur' obligatoire à rentrer.
	Donc min_arg_size = 1
	Les arg possibles sont D ou G et une valeur. On a donc au maximum 2 arg à rentrer.
	Donc max_arg_size = 2
	Les noms d'arg possible sont D, G, donc args_label = {"D", "G"}
	*/
		{"D", epreuve_cmd},
    {"E", epreuve_stop},
    {"Q", safety_break_cmd},
    {"TV", set_default_speed_cmd},
    {"A", move_forward_cmd},
    {"B", move_backward_cmd},
    {"S", move_stop_cmd},
    {"RD", rigth_rotation_cmd},
    {"RG", left_rotation_cmd},
    {"RC", complete_rotation_cmd},
    {"RA", angle_rotation_cmd},
    {"G", move_to_cmd},
    {"ASS", default_process},
    {"MI", default_process},
    {"ME", default_process},
    {"IPO", default_process},
    {"POS", default_process},
    {"MOU", detecte_obstacle},
    {"MOB", default_process},
    {"MOS", default_process},
    {"SD", generate_sound_cmd},
    {"L", light_beam_ON_cmd},
    {"LS", light_beam_OFF_cmd},
    {"CS", servo_move_cmd},
    {"PPH", photo_cmd},
    {"SPH", photo_OFF_cmd},
    {"AUX", aux_cmd}
};

/*
    FULL STATE MACHINE

    Liste des Etats de la machines d'état du Parser associés à leur fonction
*/
const FSM_PROCESS full_state_machine[2] = {
    {WAIT, &wait},
    {GET_COMMAND, &get_command}
};

/* Etat courant de la machine d'Etat */
COMMANDS_PARSER_STATE current_state = WAIT;

/* Buffer principal dans lesquel est stocké tout caractère reçu sur la liaison UART du µP */
byte raw_data[COMMAND_BUFFER_SIZE];

/* Pointer permettant de parcourir le Buffer Principal */
byte buffer_index = 0;

byte MSG_buffer[MSG_INFO_BUFFER_SIZE] = "Start Epreuve !\n";

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Timer 2
	UART0 = TX0 -> P0.0 | RX0 -> P0.1
**/
byte Init_parser(PARSER_RESULT* parser_result)
{
	
	/****** INIT UART0 *****/
	SM00 = 0;
	SM10 = 1;
	REN0 = 1;		
		
	/****** INIT PIN *****/
	P0MDOUT |= 0x03;
	
	/****** Init UART0 on Crossbar *****/
	XBR0 |= 0x04;

	
	/****** INIT TIMER ******/
	CKCON = 0x00; // Set CLK divider to 12 for timer 0 and 2
	
	
	// Réglage timer 2
	T2CON = 0x0D; // Capture Mode & Counter Mode & Enable & External Trig enable
	RCLK0 = 1;
	TCLK0 = 1;
	RCAP2L = 0xDC;
	RCAP2H = 0xFF;
	TR2 = 1;
	
	// Load MSG_invite ptr to MSG_buffer
	parser_result->informations->MSG_Invit = &MSG_buffer;
	
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
  //(full_state_machine[current_state]).state_process(result);
	
	/*
		Sans passer par les pointer de fonctions, plus de problème ...
	*/
	switch(current_state)
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
		
	}
	
	send_informations(result);
}

void wait(PARSER_RESULT* parser_result)
{
  byte read_byte = '*';

  // Scrutation sur l'UART0
  USART_receive(&read_byte);
	
	// For WARNING C280 supression
	parser_result = parser_result;

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
      //USART_print("Error decoding cmd !");
			error_cmd_flag();
    }else{
			valid_cmd_flag();
		}
    
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
	byte ret = 0;
  char c = 0, i = 0;
  char run = 1;
	/*char** ptra = 0;
	char** ptrb = 0;
	char* t = 0;
	char* v = 0;*/

	CMD_PACKET cmd_packet;

  // Dispatches commands and arguments in commands_data buffer
  while(run)
  {
      c = *(raw_data + reading_ptr);
			/*t = (commands_data + data_index);
			v = commands_data[data_index];*/
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
          ptr++;
      }

      reading_ptr++;
  }
	
	// Préparation du packet
	
	cmd_packet.commands_data = (char*)commands_data;
	cmd_packet.cmd_size = data_index;
	cmd_packet.commands = (parser_result->commands);

	
  // Looking for the command in the Command Dictionnary
  for(i = 0 ; i < NUMBER_OF_COMMAND; i++)
  {
    if(strcmp(*(commands_data), dispatch_table[i].name) == 0)
    {
			// On test la validité des args entré par l'user
			if(dispatch_table[i].process(&cmd_packet) != 0)
			{
				ret = 1;
			}else{
				USART_print("Cmd Erreur !!!!");
				ret = 0;
			}
			/*
      USART_print("Find Command : ");
      USART_print(dispatch_table[i].name);
			USART_send('\n');
			*/
      
    }
  }

	//clear_buffer(commands_data, data_index+1);
	memset(commands_data, 0 , MAX_COMMAND_WORD * ARGS_BUFFER_SIZE);
	memset(cmd_packet.commands_data, 0 , MAX_COMMAND_WORD * ARGS_BUFFER_SIZE);
	//clear_buffer(cmd_packet.commands_data, data_index+1);

  return ret;
}

void valid_cmd_flag()
{
	USART_send(0x0D);
	USART_send(0x0A);
	USART_send(COMMAND_CONFIRM_BYTE);
}

void error_cmd_flag()
{
	USART_send(0x0D);
	USART_send(0x0A);
	USART_send(COMMAND_ERROR_BYTE);	
}


void send_informations(PARSER_RESULT* parser_result)
{
	// If Information Msg to PC required
	if(parser_result->informations->Etat_Invite == Invite_oui)
	{
		// Target reached message Info
		if(parser_result->informations->Etat_BUT_Mouvement == BUT_Atteint_oui)
		{
			parser_result->informations->MSG_Invit = "\nB\n";
			parser_result->informations->Etat_BUT_Mouvement = BUT_Atteint_non;
		}
		
		else if(parser_result->informations->Etat_BUT_Servo == BUT_Servo_H)
		{
			parser_result->informations->MSG_Invit = "\nAS H\n";
			parser_result->informations->Etat_BUT_Servo = BUT_Servo_non;
		}
		
		else if(parser_result->informations->Etat_BUT_Servo == BUT_Servo_V)
		{
			parser_result->informations->MSG_Invit = "\nAS V\n";
			parser_result->informations->Etat_BUT_Servo = BUT_Servo_non;
		}
		
		// "KOB" return message after single detection operation "MOU"
		else if(parser_result->informations->Etat_DCT_Obst == DCT_Obst_single_oui)
		{
			send_KOB_MOU(parser_result);
			parser_result->informations->Etat_BUT_Servo = BUT_Servo_non;
		}
		
		else
		{
			parser_result->informations->MSG_Invit = "\nI \tStart Epreuve !\n";
		}
		
		parser_result->informations->Etat_Invite = Invite_non;
		USART_print(parser_result->informations->MSG_Invit);
		
	}

	// Nothing TODO
}

void send_KOB_MOU(PARSER_RESULT* parser_result)
{
	static byte msg[32];
		
	int angle_front = 0, angle_back = 0;
	int distance_front = 0, distance_back = 0;
	
	memset(msg, 0 , 32);

	if(parser_result->informations->Nbre_Val_obst > 2)
	{
		angle_front = parser_result->informations->Tab_Val_Obst[0];
		distance_front = parser_result->informations->Tab_Val_Obst[1];
		sprintf(msg, "\nKOB %d:%d\n", angle_front, distance_front);
	}
	else
	{
		angle_front = parser_result->informations->Tab_Val_Obst[0];
		distance_front = parser_result->informations->Tab_Val_Obst[1];
		angle_back = parser_result->informations->Tab_Val_Obst[2];
		distance_back = parser_result->informations->Tab_Val_Obst[3];
		sprintf(msg, "\nKOB %d:%d %d:%d\n", angle_front, distance_front, angle_back, distance_back);
	}

	
	parser_result->informations->MSG_Invit = msg;
	//strcpy(parser_result->informations->MSG_Invit, msg);
}

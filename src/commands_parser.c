#include "commands_parser.h"
#include <string.h>
#include <stdlib.h>

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
    {WAIT, wait},
    {GET_COMMAND, get_command},
    {SEND_COMMAND, wait}
};

/* Etat courant de la machine d'Etat */
COMMANDS_PARSER_STATE current_state;

/* Buffer principal dans lesquel est stocké tout caractère reçu sur la liaison UART du µP */
byte raw_data[COMMAND_BUFFER_SIZE];

/* Pointer permettant de parcourir le Buffer Principal */
byte buffer_index = 0;

/*
#############################################################################
        Pour µP ATmega328p
#############################################################################
*/

#ifdef ATMEGA


byte init_parser()
{
  // Set BAUD Rate
  UBRR0H = (byte) (UBRR_BAUD >> 8);
  UBRR0L = (byte) (UBRR_BAUD);

  //Enable Receiver and Transmetter
  UCSR0B = (1<<RXEN0)
            | (1<<TXEN0);

  // Set Frame Format : 8Data and 1 Stop Bit
  UCSR0C = (0<<USBS0)|(3<<UCSZ00);

  // Enable Interrupt
  //sei();

  return 0;
}

/*
ISR(USART_RX_vect)
{

  while ((UCSR0A & (1 << RXC0)) == 0) {};
  raw_data[buffer_index] = UDR0;

  if(raw_data[buffer_index] == STOP_BYTE)
  {
    current_state = GET_COMMAND;
  }else{
    buffer_index++;
  }
}*/


void USART_receive(byte* read_byte)
{
  if((UCSR0A & (1 << RXC0)))
  {
    *read_byte = UDR0;
  }else{
    *read_byte = '*';
  }
}

void USART_send(byte data)
{
	//while the transmit buffer is not empty loop
	while(!(UCSR0A & (1<<UDRE0)));

	//when the buffer is empty write data to the transmitted
	UDR0 = data;
}

/*
#############################################################################
        Pour µP 8051F020
#############################################################################
*/

#else
/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Interrupt
**/
byte init_parser()
{
  // TODO
  return 0;
}

void USART_receive(byte* read_byte)
{
  // TODO
  // *read_byte = UDR0;

}

void USART_send(byte data)
{
	// TODO
}

#endif

/*
#############################################################################
        Core Code of commands_parser.c
#############################################################################
*/

void USART_print(char* str)
{
  for(byte i = 0 ; i < strlen(str); i++)
  {
    USART_send(*(str+i));
  }
}


//const FSM_PROCESS* cmd_parser_next_state()
void cmd_parser_process(PARSER_RESULT* result)
{
  //return &full_state_machine[current_state];
  full_state_machine[current_state].state_process(result);
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

  char c = 0;
  char run = 1;

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

  // Looking for the command in the Command Dictionnary
  for(char i = 0 ; i < NUMBER_OF_COMMAND; i++)
  {
    if(strcmp(*(commands_data), dispatch_table[i].name) == 0)
    {
      dispatch_table[i].process(commands_data, data_index, (parser_result->commands));
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

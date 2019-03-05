#include "commands_parser.h"
#include <string.h>
#include <stdlib.h>

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

const FSM_PROCESS full_state_machine[3] = {
    {WAIT, wait},
    {GET_COMMAND, get_command},
    {SEND_COMMAND, wait}
};

COMMANDS_PARSER_STATE current_state;

byte raw_data[COMMAND_BUFFER_SIZE];// = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte buffer_index = 0;

#ifdef ATMEGA

/**
  Initialize devices for commands_parser : UART0 and Interrupt
**/
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

void USART_print(char* str)
{
  for(byte i = 0 ; i < strlen(str); i++)
  {
    USART_send(*(str+i));
  }
}


#else
/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Interrupt
**/
byte init_parser()
{
  // Set BAUD Rate




  return 0;
}

#endif

const FSM_PROCESS* cmd_parser_next_state()
{
  return &full_state_machine[current_state];
}

void wait(PARSER_RESULT* parser_result)
{
  byte read_byte = '*';

  USART_receive(&read_byte);
  if(read_byte != '*'){
      if(read_byte == STOP_BYTE)
      {
        //USART_print("\nCommand Mode !\n");
        raw_data[buffer_index] = read_byte;
        buffer_index++;
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
    //USART_print("Command State");
    //USART_print(raw_data);
    //if (!decode_data(parser_result))
    if (!parse(parser_result))
    {
      USART_print("Error in decoding cmd !");
    }
    USART_print("Fin");
    memset(raw_data, '0', strlen(raw_data));
    buffer_index = 0;
    current_state = WAIT;
}

void read_command(PARSER_RESULT* parser_result)
{
    // Nothing TODO
}

byte parse(PARSER_RESULT* parser_result)
{
  char commands_data[ARGS_MAX_NUMBER][ARGS_BUFFER_SIZE];
  char data_index = 0;
  char ptr = 0;
  char c = 0;
  char run = 1;

  // Dispatch commands and arguments in commands_data buffer
  while(run)
  {
      c = *(raw_data + ptr);

      switch (c)
      {
        case COMMAND_SEPARATOR:
          //*( *(commands_data + data_index) + ptr ) = '\0';
          commands_data[data_index][ptr] = '\0';
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

        case STOP_BYTE:
          //USART_print(*(commands_data + data_index) );
          //*( *(commands_data + data_index) + ptr ) = '\0';
          commands_data[data_index][ptr] = '\0';
          USART_print(commands_data[data_index]);
          run = 0;
          break;

        default:
          //*( *(commands_data + data_index) + ptr ) = c;
          commands_data[data_index][ptr] = c;
          ptr++;
      }
  }

  // Looking for the command in the Command Dictionnary
  for(char i = 0 ; i < NUMBER_OF_COMMAND; i++)
  {
    if(strcmp(*(commands_data), dispatch_table[i].name))
    {
      //dispatch_table[i].process(commands_data, (parser_result->commands));
      return 1;
    }
  }

  return 0;
}
/*
char decode_data(PARSER_RESULT* parser_result)
{
  char* command_token;
  char args[ARGS_MAX_NUMBER][ARGS_BUFFER_SIZE];
  byte values[ARGS_MAX_NUMBER];

  command_token = strtok(raw_data, COMMAND_SEPARATOR);
  if(command_token != NULL)
  {
    for(char i = 0 ; i < NUMBER_OF_COMMAND; i++)
    {
      if(strcmp(command_token, dispatch_table[i].name))
      {
        parse_args(raw_data, args, values);
        dispatch_table[i].process(args, values, (parser_result->commands));
        return 1;
      }
    }
  }else{
    USART_print("Error Decoding Command !");
    return -1;
  }

}

void parse_args(char * raw_args, char** args, byte* values)
{
  char * token;
  char * args_token;
  int i = 0;

  token = strtok(raw_args, COMMAND_SEPARATOR);
  while(token != NULL)
  {
    args_token = strtok(token, ARG_VALUE_SEPARATOR);
    if(args_token != NULL)  // Arguments complexe
    {
      strcpy(args[i], (const char*)args_token);
      values[i] = atoi(token);
      i++;
    }
    else{ // Argument simple (juste une valeur)
      USART_print("Bug");
      strcpy(args[i], "NULL");
      values[i] = atoi(token);
      i++;
    }
  }

  args[i] = '\0';
}
*/
/*
char encode_data()
{


}*/

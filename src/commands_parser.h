#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H
#endif

#include "commands.h"
#include <stdio.h>
#include <avr/io.h>

#include <avr/interrupt.h>

typedef unsigned char byte;

#define ATMEGA

#ifdef ATMEGA

#define CLK 16000000UL
#define BAUD  9600
#define UBRR_BAUD CLK/16/BAUD-1

#else

#endif

// Taille en octect que l'on alloue au buffer qui récupère la commande envoyé
#define COMMAND_BUFFER_SIZE 10

// Nombre max d'argmuents que l'on peut récupérer avec le buffer lors du parse
#define ARGS_MAX_NUMBER  5

// Taille max du buffer pour un seul nom d'argument
#define ARGS_BUFFER_SIZE  12

// Taille en octect que l'on alloue au buffer des params lors du décodage
#define PARAM_BUFFER_SIZE 16

// Nombre de commandes implémentée
#define NUMBER_OF_COMMAND 27

#define COMMAND_MAX_BYTES 6
#define MAX_PARAM_NUMBER  5

// Byte de stop à la fin de chaque commande, imposé par le CdC
#define STOP_BYTE 0x0D

// Byte séparateur entre les différents éléments d'une commande
#define COMMAND_SEPARATOR 0x20

// Byte séparateur entre un arguments complexe et sa valeur
#define ARG_VALUE_SEPARATOR 0x3A

typedef struct
{
  byte has_command;
  OUT_M1 * commands;
}PARSER_RESULT;

typedef enum
{
  WAIT,
  GET_COMMAND,
  SEND_COMMAND
}COMMANDS_PARSER_STATE;

typedef struct
{
  COMMANDS_PARSER_STATE state_name;
  void(*state_process)(PARSER_RESULT*);
}FSM_PROCESS;



typedef struct
{
  char* name;
  void(*process)(char**, OUT_M1*);
}CMD_;

/*
typedef enum
{
  D = 100,
  E = 10,
  Q = 9,
  TV = 220,
  A = 91,
  B = 219,
  S = 204,
  RD = 37,
  RG = 133,
  RC = 131,
  RA = 21,
  G = 109,
  ASS = 107,
  MI = 166,
  ME = 243,
  IPO = 0,
  POS = 177,
  MOU = 135,
  MOB = 72,
  MOS = 213,
  SD = 168,
  L = 29,
  LS = 244,
  CS = 41,
  PPH = 172,
  SPH = 120,
  AUX = 167,
}HASH_IDENTIFIER;
*/

byte init_parser();
const FSM_PROCESS* cmd_parser_next_state();
void wait(PARSER_RESULT* parser_result);
void get_command(PARSER_RESULT* parser_result);
void read_command(PARSER_RESULT* parser_result);

//char decode_data(PARSER_RESULT* parser_result);
//void parse_args(char * raw_args, char** args, byte* values);
byte parse(PARSER_RESULT* parser_result);

void USART_send(byte data);
void USART_receive(byte* read_byte);
void USART_print(char* str);

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
#define COMMAND_BUFFER_SIZE 100

// Nombre max de mots composants une commande que l'on peut récupérer avec le buffer lors du parse
#define MAX_COMMAND_WORD  8

// Taille max du buffer pour un seul mot de commande/argument
#define ARGS_BUFFER_SIZE  12

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
  void(*process)(char**, byte, OUT_M1*);
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

/*
#############################################################################
        µP Related Code
#############################################################################
*/

/**
  Initialize devices for commands_parser : UART0 and Interrupt
**/
byte init_parser();

/*
  Envoie un caractere sur la liaison UART0
*/
void USART_send(byte data);

/*
  Récupère un caractere sur la liaison UART0 par scrutation
*/
void USART_receive(byte* read_byte);

/*
#############################################################################
        Core Code of commands_parser.c
#############################################################################
*/

/*
  Envoie une chaine de caractères sur la liaison UART0
*/
void USART_print(char* str);

/*

*/
const FSM_PROCESS* cmd_parser_next_state();

/*
  Execute un cycle de la machine d'Etat commands_parser.c
*/
void cmd_parser_process(PARSER_RESULT*);

/*
  Etat d'attente de la FSM. A chaque exécution, on scrute l'UART0 pour voir si
  un nouveau caractère est présent, et on l'ajoute dans le buffer raw_data
*/
void wait(PARSER_RESULT* parser_result);

/*
  Analyse le buffer brute afin d'en extraire une commande existante
*/
void get_command(PARSER_RESULT* parser_result);

/*
  TODO
*/
void read_command(PARSER_RESULT* parser_result);

/*
  Découpe le buffer brute afin d'en extraire le nom de la commande, les paramètres,
  et leurs valeurs
*/
byte parse(PARSER_RESULT* parser_result);

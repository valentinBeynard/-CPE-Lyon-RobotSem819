#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H
#endif

#include "c8051F020.h"
#include "commands.h"


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
  OUT_M1 commands;
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
void USART_send(byte ch);

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

#ifndef UART_H

#define UART_H

#include "main.h"

#define UART_BUFFER_SIZE	16

// Byte de stop à la fin de chaque commande
#define STOP_BYTE 0x0D

typedef unsigned char byte;

enum GEN_son {GEN_non, GEN_oui};
enum ACQ_son {ACQ_non, ACQ_oui};


typedef struct
{
	enum GEN_son Etat_GEN;
	
	byte frequency_code;
	byte delay_ON;
	byte delay_OFF;
	byte nbr_sound;
	
	enum ACQ_son Etat_ACQ;
	
}CMD_PCK;

void uart_cmd_process(CMD_PCK * cmd);

#endif
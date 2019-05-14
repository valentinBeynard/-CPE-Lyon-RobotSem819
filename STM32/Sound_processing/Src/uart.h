#ifndef UART_H

#define UART_H

#include "main.h"
#include "cmd_pck.h" 

#define UART_BUFFER_SIZE	16

#define UART1_BAUDRATE	19200

// Byte de stop à la fin de chaque commande
#define STOP_BYTE 0x0D

#define SD_GEN_GENERATE_CMD	'G'
#define ASS_ACQUIRE_SOUND_CMD	'A'


typedef unsigned char byte;

void uart_init(UART_HandleTypeDef *huart);
void uart_cmd_process(CMD_PCK * cmd);
void clear_raw_buffer();
#endif
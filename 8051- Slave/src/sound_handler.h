#ifndef SOUND_HANDLER_H

#define SOUND_HANDLER_H

#include "c8051F020.h"
#include "slave_cmd.h"

#define byte unsigned char
	

// Byte de stop à la fin de chaque commande
#define STOP_BYTE 0x0D

// Byte séparateur entre les différents éléments d'une commande
#define COMMAND_SEPARATOR 0x20

#define SD_GEN_BUFFER_SIZE	16

#define SD_GEN_GENERATE_CMD	'G'
	
typedef enum
{
  SD_GEN_IDLE,
	SD_GEN_PARSE_CMD,
  SD_GEN_SEND_CMD
}SOUND_HANDLER_STATE;


byte Init_uart0();
void UART0_receive(byte* read_byte);
void UART0_send(byte ch);
void UART0_print(char* str);

void sound_handler_process(OUT_M2* cmd);

void sd_gen_idle(OUT_M2* cmd);
void sd_gen_send(OUT_M2* cmd);
void sd_gen_parse(OUT_M2* cmd);

#endif
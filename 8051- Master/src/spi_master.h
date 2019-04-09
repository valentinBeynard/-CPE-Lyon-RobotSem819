#ifndef SPI_MASTER_H

#define SPI_MASTER_H

#include "pc_cmd.h"

#define BUFFER_SIZE	16

#define SLAVE_ENABLE	0

#define SLAVE_DISEABLE	1

#define SPI_START_BYTE	0xAA

#define SPI_STOP_BYTE	0xBB


#define byte unsigned char



typedef struct
{
  OUT_M1 * commands;
	IN_M1 * informations;
	byte spi_data[BUFFER_SIZE];
	// Info sur l'obstacle ?
}SPI_PACKET;

typedef enum
{
  SPI_IDLE,
  SPI_GET_DATA,
  SPI_SET_DATA
}SPI_PARSER_STATE;

typedef struct
{
  SPI_PARSER_STATE state_name;
  void(*state_process)(SPI_PACKET*);
}SPI_FSM_PROCESS;


void Init_SPI();
void spi_send_char(char a);


void spi_process(SPI_PACKET* spi_packet);

void spi_idle(SPI_PACKET* spi_packet);
void spi_get_data(SPI_PACKET* spi_packet);
void spi_set_data(SPI_PACKET* spi_packet);


#endif
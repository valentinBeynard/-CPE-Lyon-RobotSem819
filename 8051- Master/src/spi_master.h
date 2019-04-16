#ifndef SPI_MASTER_H

#define SPI_MASTER_H

#include "pc_cmd.h"

#define TRAM_SIZE	5

#define SLAVE_ENABLE	0

#define SLAVE_DISEABLE	1

/*
#############################################################################
					Protocol SPI : Commands ID
#############################################################################
*/
#define SPI_SERVO_CMD	0xA1


#define byte unsigned char



typedef struct
{
  byte * received_data;
	byte send_data[TRAM_SIZE];
	byte ready;
	// Info sur l'obstacle ?
}SPI_PACKET;


void Init_SPI();
void spi_send_char(char a);

void spi_transmit(SPI_PACKET* spi_packet);

void spi_process(OUT_M1 * cmd, SPI_PACKET* spi_packet);

void spi_cmd_servo(OUT_M1 * cmd, SPI_PACKET* spi_packet);

#endif
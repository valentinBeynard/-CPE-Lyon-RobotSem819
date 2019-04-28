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
#define SPI_GENERATE_SOUND_CMD	0xA2
#define SPI_LIGHT_ON_CMD	0xA3
#define SPI_LIGHT_OFF_CMD	0xA4
#define SPI_SERVO_CMD	0xA5
#define SPI_PHOTO_ON_CMD	0xA6
#define SPI_PHOTO_OFF_CMD	0xA7

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
void spi_cmd_light_ON(OUT_M1 * cmd, SPI_PACKET* spi_packet);
void spi_cmd_light_OFF(OUT_M1 * cmd, SPI_PACKET* spi_packet);
void spi_cmd_generate_sound(OUT_M1 * cmd, SPI_PACKET* spi_packet);
void spi_cmd_photo_ON(OUT_M1 * cmd, SPI_PACKET* spi_packet);
void spi_cmd_photo_OFF(OUT_M1 * cmd, SPI_PACKET* spi_packet);

byte spi_validate();
void spi_error();

#endif
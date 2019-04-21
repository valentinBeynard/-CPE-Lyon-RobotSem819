#ifndef SPI_SLAVE_H

#define SPI_SLAVE_H

#include "slave_cmd.h"

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

#define byte unsigned char


void Init_SPI();

void spi_process(OUT_M2* cmd);
void spi_parse_cmd(OUT_M2* cmd);


#endif
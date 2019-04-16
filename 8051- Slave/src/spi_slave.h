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
#define SPI_SERVO_CMD	0xA1


#define byte unsigned char


void Init_SPI();

void spi_process(OUT_M2* cmd);
void spi_parse_cmd(OUT_M2* cmd);


#endif
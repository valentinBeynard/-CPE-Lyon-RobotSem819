#ifndef SPI_MASTER_H

#define SPI_MASTER_H

#define TRAM_SIZE	10

#define SLAVE_ENABLE	0

#define SLAVE_DISEABLE	1

#define SPI_START_BYTE	0xAA

#define SPI_STOP_BYTE	0xBB


#define byte unsigned char



typedef struct
{
  byte received_data[TRAM_SIZE];
	byte send_data[TRAM_SIZE];
	byte ready;
	// Info sur l'obstacle ?
}SPI_PACKET;


void Init_SPI();
void spi_send_char(char a);

void spi_transmit(SPI_PACKET* spi_packet);

void spi_cmd(SPI_PACKET* spi_packet);


#endif
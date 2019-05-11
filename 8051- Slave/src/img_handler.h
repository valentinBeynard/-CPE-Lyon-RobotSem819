#ifndef IMG_HANDLER_H

#define IMG_HANDLER_H

#include "c8051F020.h"
#include "slave_cmd.h"

#define byte unsigned char

typedef enum
{
	IMGH_IDLE = 0,
	IMGH_SIMPLE,
	IMGH_SEQ,
	IMGH_CONTINUE
}IMGH_STATE;



void Init_Img_Handler();

void trigger_photo(int nbr_photo, int time_step);
void sequence_trig(int nbr_photo, int time_step);
void infinite_trig(int time_step);


void img_handler_process(OUT_M2 * cmd);

#endif
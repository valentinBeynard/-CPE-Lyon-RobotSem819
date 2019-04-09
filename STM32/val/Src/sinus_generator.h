#ifndef SINUS_GENERATOR_H

#define SINUS_GENERATOR_H

#include "main.h"

/*
*		####################################################
*					PINOUT : Sinus sur PA4
*		####################################################
*/

typedef struct
{
	TIM_HandleTypeDef * htim3;
	TIM_HandleTypeDef * htim6;
	DAC_HandleTypeDef * hdac;
	unsigned char nbr_bip;
	unsigned int time_H;
	unsigned int time_L;
	unsigned int frequence;
	unsigned char start;
}SINUS_PCK;


void init_sinus_generator(SINUS_PCK*);

void freq_calculator(SINUS_PCK* pck);

void init_sinus(SINUS_PCK*);
unsigned char send_signal(SINUS_PCK* pck);

void sinus_generator_process(SINUS_PCK*);



#endif
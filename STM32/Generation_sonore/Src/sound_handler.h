#ifndef SOUND_HANDLER_H

#define SOUND_HANDLER_H

#include "main.h"
#include "cmd_pck.h" 

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
	ADC_HandleTypeDef * hadc1;
	ADC_HandleTypeDef * hadc2;
	CMD_PCK * cmd_pck;
}SOUND_PCK;


void init_sinus_generator(SOUND_PCK*);

void freq_calculator(SOUND_PCK* pck);

void init_sinus(SOUND_PCK*);
void init_sound_handler(SOUND_PCK* pck);

unsigned char send_signal(SOUND_PCK* pck);

void sinus_generator_process(SOUND_PCK*);

void detector_process();

#endif
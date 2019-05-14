#ifndef SOUND_HANDLER_H

#define SOUND_HANDLER_H

#include "main.h"
#include "cmd_pck.h" 

#define THRESHOLD	0x00F

#define VDETECT	0xF000

#define ADC1_TIME_STEP	50.125

#define ADC_BUFFER_SIZE	20

/*
*		####################################################
*					PINOUT : Sinus sur PA4
*		####################################################
*/

typedef enum
{
	SH_IDLE = 0,
	SH_GENE,
	SH_ACQ
}SH_STATE;

typedef struct
{
	TIM_HandleTypeDef * htim3;
	TIM_HandleTypeDef * htim2;
	TIM_HandleTypeDef * htim6;
	DAC_HandleTypeDef * hdac;
	ADC_HandleTypeDef * hadc1;
	ADC_HandleTypeDef * hadc2;
	UART_HandleTypeDef * huart4;
	CMD_PCK * cmd_pck;
}SOUND_PCK;


void init_sinus_generator(SOUND_PCK*);

void freq_calculator(SOUND_PCK* pck);

void init_sinus(SOUND_PCK*);
void init_sound_handler(SOUND_PCK* pck);

unsigned char send_signal(SOUND_PCK* pck);

void sinus_generate(SOUND_PCK*);
void sound_handler_process(SOUND_PCK * pck);

void sound_acquisition(SOUND_PCK*);

void signal_recopieur();

#endif
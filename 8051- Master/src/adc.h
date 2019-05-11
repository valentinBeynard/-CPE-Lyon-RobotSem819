#ifndef ADC_H

#define ADC_H

#include "c8051F020.h"

#define byte unsigned char

typedef enum
{
	AIN0 = 0,
	AIN1,
	CURRENT_SHUNT
}ADC_INPUT;

void Init_ADC0();

int _read_ADC0();
int read_ADC0();

void choose_ADC_input(ADC_INPUT adc_input);


#endif
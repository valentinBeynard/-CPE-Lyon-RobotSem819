#ifndef ADC_H

#define ADC_H

#include "c8051F020.h"

typedef enum
{
	AIN0 = 0,
	AIN1,
	AIN6
}ADC_INPUT;

void Init_ADC0();

int read_ADC0();

void choose_ADC_input(ADC_INPUT adc_input);


#endif
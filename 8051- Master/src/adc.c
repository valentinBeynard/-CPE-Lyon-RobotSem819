#include "adc.h"

sfr16 ADC0 = 0xbe;

void Init_ADC0()
{

	ADC0CN |=0x81;
	ADC0CN &= ~0x4C;
	ADC0CF &= 0x00;
	//AMX0SL&=0xF0;
	//AMX0CF&=0xF0;

	REF0CN&=0xEB;
	REF0CN|=0x03;
	
	DAC0CN |=0x80;
	DAC0CN &=0xE0;
}

int _read_ADC0()
{
	return ADC0;
}

int read_ADC0()
{
	byte adc0 = 0;
	
	AD0INT=0;
	AD0BUSY=1;
	
	// Wait Until the END of Conversion
	while (AD0INT ==0) {};
	
	adc0 = ADC0;
	
	AD0BUSY=0;
	
	return adc0;
}

void choose_ADC_input(ADC_INPUT adc_input)
{
	if(adc_input == AIN0)
	{
		AMX0SL = 0x00;
	}
	else if(adc_input == AIN1)
	{
		AMX0SL = 0x01;
	}
	else
	{
		AMX0SL = 0x06;
	}
}


// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//
//------------------------------------------------------------------------------------
#include "c8051F020.h"

#define POW 4096.0


sbit sig = P1^1;
sfr16 ADC0 = 0xbe;

// Configuration du crossbar
void config_XBAR();
void config_DAC_ADC();
float mesure();
void init_telemeter();
float start_conversion();
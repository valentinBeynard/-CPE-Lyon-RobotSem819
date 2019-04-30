#include "power_sensor.h"
#include <math.h>

sbit sig = P1^1;
sfr16 ADC0 = 0xbe;
float Vref=2.40; //2,40 V (datasheet)
float tension=0;
float intensite=0;
float Rshunt = 0.05; //50mOhms


int Mesure_Courant(char* commande)
{
		config_ADC();
		
	  AMX0SL = 0x14; // sélection des ports d'entrées AIN1
	  AD0BUSY = 0x1; //ADC0 conversion is in progress
	/*
		unsigned int lecture_high;
		unsigned int lecture_low;
	*/
	
	
		if(strcmp(commande,"MI")==0 {
	  
		while (AD0INT ==0) {}; //tant que la conversion est en cours
		AD0INT = 0x00; // Flag marquant la fin de la conversion 
		
		tension= (ADC0*Vref)/(pow(2,10));
			
		intensite = (1000*tension)/Rshunt;
			
		/*
		lecture_low <<4; //on décale pour obtenir les bits de poids faible
		lecture_high>>4; //on décale pour obtenir les bits de poids fort
			
		tension = lecture_high + lecture_low;
			*/
		
			return intensite;
			

			
		}
}

void power_sensor_process(OUT_M1 * cmd)
{
	
	
}


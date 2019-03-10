// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//
//------------------------------------------------------------------------------------
#include "c8051F020.h"

#define POW 4096.0


sbit sig = P1^1;
sfr16 ADC0 = 0xbe;


//configuration du watchdog
void config_watchdog()
{
	WDTCN = 0xDE;   // D�validation du watchdog 
  WDTCN = 0xAD;
}


// Configuration du crossbar
void config_XBAR()
{
	/* Le crossbar permet d'activer les ports entr�es/sorties du C8051F020. 
	C'est une fonction de configuration des ports entr�es/sorties de la carte du microcontroleur. 
	Pas d'arguments ni en entr�e ni en sortie. */
	
	XBR0 = 0x04;
	XBR1 = 0x80;
	XBR2 = 0x40;

}


// Configuration de l'horloge 
void sysclk()
{
	/* L'horloge permet de synchroniser le syst�me � une fr�quence de 22 MHz
	Le r�le de cette fonction est de cr�er une horloge.
	C'est une fonction de configuration du syst�me d'horloge du microcontroleur. 
	Pas de param�tres d'entr�e ou de sortie.
	Pour tester, on configure l'horloge sur un port 
	puis on branche une sonde sur ce port et on observe le r�sultat sur l'oscilloscope.*/
	
	unsigned int i;
	OSCXCN |= 0x67; 
	OSCXCN = OSCXCN & ~0x10; //On met � 0 le bit 4
	for(i=0;i<1000;i++); //On temporise pour atteindre la stabilit�
	
	OSCICN |= 0x08; // Mise � 1 du bit 3
	
	
}


void config_ADC() {
/* -role: cette fonction permet de configurer le convertisseur ADC
	 -fonction de configuration du convertisseur.
	 -arguments d'entree: none.
	 -argument de sortie: none.
*/
	ADC0CN |=0x81;
	ADC0CN &= ~0x4C;
	ADC0CF &= 0x00;

	REF0CN&=0xEB;
	REF0CN|=0x03;
	
	DAC0CN |=0x80;
	DAC0CN &=0xE0;
}


float mesure(){
/* -role: cette fonction permet de mesurer la distance entre le servomoteur et l'obstacle.
	 -fonction d'utilisation.
	 -arguments d'entree: none.
	 -argument de sortie: distance.
*/	
	float d;
	float V_mes;
	
	//lecture de la tension mesur�e sur AIN2
	AMX0SL&=0x00;
	AMX0SL|=0x02;
	V_mes=ADC0/(POW*5.6);
	
	//Calcul des distances
	d=-16.669*V_mes+67.367; //�quation tir� de l'�talonnage

	return d;
	
}


void init(){
	config_watchdog();
	config_XBAR();
	sysclk();
	config_ADC();
}

#ifdef UNITY_TELEMETER

void main()
{
	init();
	while(1){
		AD0INT=0;
		AD0BUSY=1;
		mesure();
		AD0BUSY=0;
	}
	
}

#endif
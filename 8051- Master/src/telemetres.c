
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//  NOM: METEYER

//
//------------------------------------------------------------------------------------
#include "c8051F020.h"
#include "telemetres.h"

#define POW 4096.0

//sbit LED = P1^6;
//char is_sending = 0;
char received_byte = 0;



// Configuration du crossbar
void config_XBAR()
{
	/* Le crossbar permet d'activer les ports entrées/sorties du C8051F020. 
	C'est une fonction de configuration des ports entrées/sorties de la carte du microcontroleur. 
	Pas d'arguments ni en entrée ni en sortie. */
	
	/* On va modifier les paramètres suivants: XBR0, XBR1, XBR2. */
	
	XBR1 |= 0x80;
	XBR2 |= 0x40;

}
/*
// Configuration de l'UART
void config_UART()
{
	/* L'UART permet la réception et la transmission de données via un port série.
	C'est une fonction de gestion de périphérique. 
	Pas de paramètres d'entrée et de sortie. 
	On teste à l'aide des fonctions getchar et putchar et on observe sur PuTTY la bonne reception et transmission des données.
	*/
	
	/* On va modifier les paramètres suivants: SCON0, PCON. *//*
	SCON0 |= 0x70;
	SCON0 &= 0x7F;
	
	PCON |= 0x80;
	PCON &= ~0x40;
}
*/


void config_DAC_ADC() {
/* -role: cette fonction permet de configurer les convertisseurs digital <-> analogique. Le convertisseur DAC nous permet d'interpréter le signal de sortie du télémètre en le convertissant en signal analogique. Le convertisseur ADC nous permet de vérifier le bon fonctionnement du DAC en visualisant le signal reconverti à l'oscillateur. Le signal de référence est imposé à 3.3V.
	 -fonction de configuration des convertisseurs.
	 -arguments d'entree: none.
	 -argument de sortie: none.
	 	 1. Configuration de la tension de référence : tension externe VREF0=3V
2. Configuration des voies Unipolaires : AIN0.0=Tension en sortie du telemetre infrarouge; AIN1=Tension consigne min; AIN2=Tension consigne max
3. Gain: 1
4. Horloge CLK SAR d'apres SYSCLK=22MHZ
5. Mode de déclenchement de la conversion: interruption sur ADC0
6. Effacer les drapeaux signalant une fin de conversion ou un dépassement
comparateur

• Injecter un signal périodique à l’aide d’un générateur sur AIN0.0
• Lancer la conversion en continue sur ADC0 et réinjecter sur DAC0
• Contrôler sur oscilloscope la correspondance entre entrée ADC et sortie DAC
*/
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
/*
void uart_0_int() interrupt 4
{
	if(TI0 == 1)
		TI0 = 0;
	if(RI0 == 1)
	{
		received_byte = SBUF0;
		RI0 = 0;
	}
}*/
/*

void putchar(char c){
/* -role: cette fonction permet d'afficher la distance et l'angle de mesure sur Putty.
	 -fonction d'utilisation du périphérique UART0.
	 -arguments d'entree: angle de mesure demandé par l'utilisateur et distance de l'obstacle.
	 -argument de sortie: none.
	 -Test: Appel de la fonction avec 2 valeurs numériques et vérification de leur affichage sur Putty.
	*/
	/*
	TI0=0;
	SBUF0=c;
	// si TI0 == 1, alors transmission finie sinon on attend
	while(TI0!=1);
}
*/

float mesure(){
/* -role: cette fonction permet de mesurer la distance entre le servomoteur et l'obstacle.
	 -fonction d'utilisation.
	 -arguments d'entree: none.
	 -argument de sortie: distance.
	 -Test: 

	*/	
	float d;
	float V_mes;
	
	//lecture de la tension mesurée sur AIN2
	AMX0SL&=0x00;
	AMX0SL|=0x01;
	
	V_mes=ADC0/(POW*5.6);
	
	//Calcul des distances
	d=-16.669*V_mes+67.367; 
	//putchar((char)d);
	
	return d;
	
}


void init_telemeter(){
	config_XBAR();
	//config_UART();
//	config_timer2();
	config_DAC_ADC();
	//init_interrupt();
}


float start_conversion(){
	float conv;
	unsigned int j;
	AD0INT=0;
	AD0BUSY=1;
	mesure();
	AD0BUSY=0;
	
	for(j=0;j<1000;j++); //On temporise pour atteindre la stabilité
	
	AD0INT=0;
	AD0BUSY=1;
	conv =mesure();
	AD0BUSY=0;
	
	return conv;
}
/*
float m;

void main()
{
	init();
	m=start_conversion();
	
	//read_command();
	//mesure();
}*/
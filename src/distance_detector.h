// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//
//------------------------------------------------------------------------------------
#ifndef DISTANCE_DETECTOR_H

#define DISTANCE_DETECTOR_H

#include "c8051F020.h"
#include "pc_cmd.h"


#define byte unsigned char


#define POW 4096.0

typedef struct
{
  OUT_M1 * commands;
	IN_M1 * informations;
	float measure;
	byte obs_detected;
	// Info sur l'obstacle ?
}DD_PACKET;

typedef enum
{
	DD_IDLE = 0,
	MOVE_SERVO_H,
	SINGLE_MEASURE,
	SLEW_DETECTION,
	OBS_DETECTION
}DD_STATE;


typedef struct
{
  DD_STATE state_name;
  void(*state_process)(DD_PACKET*);
}DD_FSM_PROCESS;




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
void dd_config_DAC_ADC();

/*
	Init timer 0 for servomoteur control
*/
void dd_init_timer0();

/*
	Initialize distance detector entity (servomotor + telemeter )
*/
void Init_distance_detector();

/* -role: cette fonction permet de mesurer la distance entre le servomoteur et l'obstacle.
	 -fonction d'utilisation.
	 -arguments d'entree: none.
	 -argument de sortie: distance.
	 -Test: 

*/


float dd_mesure();
float dd_start_conversion();

void dd_set_angle(char * angle);

void distance_detector_process(DD_PACKET * dd_packet);

void dd_idle(DD_PACKET * dd_packet);
void dd_move_servo_h(DD_PACKET * dd_packet);
void dd_single_measure(DD_PACKET * dd_packet);
void dd_slew_detection(DD_PACKET * dd_packet);
void dd_obs_detection(DD_PACKET * dd_packet);


#endif


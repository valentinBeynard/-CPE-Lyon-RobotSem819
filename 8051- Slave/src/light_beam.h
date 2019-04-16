// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//
//------------------------------------------------------------------------------------
#ifndef LIGHT_BEAM_H

#define LIGHT_BEAM_H

#include "c8051F020.h"
#include "slave_cmd.h"

#define byte unsigned char

#define SERVO_PWN_PERIOD	20000

#define SERVO_PWN_ANGLE_COEF	11

/*
	Init timer 0 for servomoteur control
*/
void lb_init_timer0();

/*
	Initialize distance detector entity (servomotor + telemeter )
*/
void Init_light_beam();

/* -role: cette fonction permet de mesurer la distance entre le servomoteur et l'obstacle.
	 -fonction d'utilisation.
	 -arguments d'entree: none.
	 -argument de sortie: distance.
	 -Test: 

*/

void lb_set_angle(int angle);

void light_beam_process(OUT_M2 * cmd);

void light_beam_move(OUT_M2 * cmd);

#endif


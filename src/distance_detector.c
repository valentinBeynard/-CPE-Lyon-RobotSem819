//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//  NOM: METEYER

//
//------------------------------------------------------------------------------------
#include "distance_detector.h"

//port de sortie
sbit P1_2 = P1^2;

sfr16 ADC0 = 0xbe;

volatile int servo_angle_H = 0;

byte pwn_over = 0;

DD_STATE dd_current_state = DD_IDLE;

/*
    FULL STATE MACHINE

    Liste des Etats de la machines d'état associés à leur fonction
*/
const DD_FSM_PROCESS dd_full_state_machine[5] = {
    {DD_IDLE, &dd_idle},
		{MOVE_SERVO_H, &dd_move_servo_h},
    {SINGLE_MEASURE, &dd_single_measure},
    {SLEW_DETECTION, &dd_slew_detection},
		{OBS_DETECTION, &dd_obs_detection}
};

/*
#############################################################################
        Interrupt Routine For Timer 0
#############################################################################
*/
	
void timer_0_int() interrupt 1
{	
	static byte nbr_interrupt = 0;
	static char high = 0;
	
	int duree_imp = 0;
	int reload_value = 0;

	duree_imp = 10*(servo_angle_H+90) + 600 ;// 600us pour 90°  (cf doc technique)
	
	P1_2 = !P1_2;
	
	if (high == 0)
	{
		reload_value = 0xFFFF - (2*6000 - duree_imp );
		high=1;
	} else {
		
		reload_value = 0xFFFF - (2 * duree_imp );
		high=0;
	}
	TL0= reload_value;
	TH0= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	if(nbr_interrupt >= 150)
	{
		ET0 = 0;	// Enable timer0 interuption
		nbr_interrupt = 0;
		pwn_over = 1;
	}
	else
	{
		nbr_interrupt++;
	}
}

/*
#############################################################################
        End of Interrupt Routine For Timer 0
#############################################################################
*/

/*
#############################################################################
        Peripheric initialize
#############################################################################
*/


void Init_distance_detector()
{
	dd_config_DAC_ADC();
	
	
	// XBAR for servomoteur H
	XBR1 |=0x02; 
	
	/* XBAR for telemeter */
	XBR1 |= 0x80;
	
	// Push-Pull mode
	P1MDOUT = 0xFF;
	
	dd_init_timer0();
	
	ET0 = 1;	// Enable timer0 interuption
	EA = 1; // Enable general interruption
}

void dd_init_timer0()
{	
	//Reglages Timer 0
	TMOD=0x01; //mode 1
	TCON= 0x11;	// Enable Timer0 (bit4) and enable interupt on edge (bit0)
	TL0=0xA8;
	TH0=0xFB;
	
	TR0 = 1;
}

void dd_config_DAC_ADC() {

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
#############################################################################
        Distance Detector Core
#############################################################################
*/

//const FSM_PROCESS* cmd_parser_next_state()
void distance_detector_process(DD_PACKET * dd_packet)
{
	dd_full_state_machine[dd_current_state].state_process(dd_packet);
}


void dd_idle(DD_PACKET * dd_packet)
{
	if(dd_packet->commands->Etat_DCT_Obst != DCT_non)
	{
		switch(dd_packet->commands->Etat_DCT_Obst)
		{
			case oui_180:
				dd_current_state = SLEW_DETECTION;
				break;
			
			case oui_360:
				
				break;
			
			default:
				dd_current_state = DD_IDLE;
		}
	}
	else
	{
		if(dd_packet->commands->Etat_Servo == Servo_H)
		{
			dd_current_state = MOVE_SERVO_H;
			dd_packet->commands->Etat_Servo = Servo_non;
		}
	}

}

void dd_move_servo_h(DD_PACKET * dd_packet)
{
	dd_set_angle(dd_packet->commands->Servo_Angle);
	pwn_over = 0;
	ET0 = 1;	// Enable timer0 interuption
	dd_current_state = DD_IDLE;
}

void dd_slew_detection(DD_PACKET * dd_packet)
{
	static byte step = 0;
	static char delta_angle = -90;
	static float smallest_measure = 70.0;
	static char angle_obs = 0
	
	switch(step)
	{
		case 0:
			dd_set_angle(-90);
			dd_packet->commands->DCT_Obst_Resolution = 5;
			step++;
			break;
		case 1:
			delta_angle += dd_packet->commands->DCT_Obst_Resolution;
			dd_set_angle(delta_angle);
			if(delta_angle == 90)
			{
				step++;
			}
			dd_current_state = SINGLE_MEASURE;
			break;
		default:
			delta_angle = -90;
			step = 0;
			dd_current_state = DD_IDLE;
			break;
	}
}

void dd_set_angle(int angle)
{
	servo_angle_H = angle;
}


float dd_mesure(){
	
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


float dd_start_conversion(){
	float conv;
	unsigned int j;
	AD0INT=0;
	AD0BUSY=1;
	dd_mesure();
	AD0BUSY=0;
	
	for(j=0;j<1000;j++); //On temporise pour atteindre la stabilité
	
	AD0INT=0;
	AD0BUSY=1;
	conv =dd_mesure();
	AD0BUSY=0;
	
	return conv;
}
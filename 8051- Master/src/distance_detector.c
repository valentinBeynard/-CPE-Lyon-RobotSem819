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

PWN_STATE pwn_state = PWN_IDDLE;

DD_STATE dd_current_state = DD_IDLE;
int nbr_interrupt = 0;

int val_obs_buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte val_obs_buffer_size = 0;

/*
    FULL STATE MACHINE

    Liste des Etats de la machines d'�tat associ�s � leur fonction
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
	static char high = 0;
	
	int duree_imp = 0;
	int reload_value = 0;

	duree_imp = 10*(servo_angle_H+90) + 600 ;// 600us pour 90�  (cf doc technique)
	
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
	TH0= reload_value >> 8; //on d�cale pour obtenir les bits de poids fort
	
	if(nbr_interrupt == 1000)
	{
		//ET0 = 0;	// Enable timer0 interuption
		nbr_interrupt++;
		pwn_state = PWN_FINISH;
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
	//XBR1 |=0x02; 
	
	/* XBAR for telemeter */
	//XBR1 |= 0x80;
	
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
			
			case oui_single:
				dd_current_state = SINGLE_MEASURE;
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
	if(pwn_state == PWN_IDDLE)
	{
		ET0 = 0;  // Stop last PWN Interrupt routine
		dd_set_angle(dd_packet->commands->Servo_Angle);
		nbr_interrupt = 0; // Reset tempo flag
		pwn_state = PWN_RUN;
		ET0 = 1;	// Enable timer0 interuption
		
	}
	else if (pwn_state == PWN_FINISH)
	{
		dd_packet->informations->Etat_BUT_Servo = BUT_Servo_H;
		dd_packet->informations->Etat_Invite = Invite_oui;
		pwn_state = PWN_IDDLE;
		dd_current_state = DD_IDLE;
	}
	else{
		// Nothing to do
	}
	

}

void dd_slew_detection(DD_PACKET * dd_packet)
{
	static byte step = 0;
	static char delta_angle = -90;
	static float smallest_measure = 70.0;
	static char angle_obs = 0;
	
	switch(step)
	{
		case 0:
			dd_set_angle(-90);
			dd_packet->commands->DCT_Obst_Resolution = 5;
			step++;
			break;
		case 1:
			delta_angle += dd_packet->commands->DCT_Obst_Resolution;
			// TODO
			/**dd_set_angle(delta_angle);
			if(delta_angle == 90)
			{
				step++;
			}
			dd_current_state = SINGLE_MEASURE;*/
			break;
		default:
			delta_angle = -90;
			step = 0;
			dd_current_state = DD_IDLE;
			break;
	}
}

void dd_single_measure(DD_PACKET * dd_packet)
{
	clear_val_obs_buffer();
	
	// Front telemeter
	choose_ADC_input(AIN0);
	dd_packet->measure = dd_start_conversion();

	if(dd_packet->measure <= MAX_DISTANCE)
	{
		val_obs_buffer[0] = (int)(10 * dd_packet->measure);
		val_obs_buffer[1] = servo_angle_H;
		dd_packet->obs_detected_front = 1;
	}
	else
	{
		dd_packet->obs_detected_front = 0;
	}
	
	// Back telemeter
	choose_ADC_input(AIN1);
	dd_packet->measure = dd_start_conversion();

	if(dd_packet->measure <= MAX_DISTANCE)
	{
		val_obs_buffer[2] = (int)(10 * dd_packet->measure);
		val_obs_buffer[3] = servo_angle_H;
		dd_packet->obs_detected_back = 1;
	}
	else
	{
		dd_packet->obs_detected_back = 0;
	}
	
	dd_packet->informations->Tab_Val_Obst = val_obs_buffer;
	dd_packet->informations->Nbre_Val_obst = val_obs_buffer_size;
	
	// Switch ON "KOB" info return
	dd_packet->informations->Etat_DCT_Obst = DCT_Obst_single_oui;
	dd_packet->informations->Etat_Invite = Invite_oui;
	
	dd_packet->commands->Etat_DCT_Obst = DCT_non;
	
	dd_current_state = DD_IDLE;
}

void dd_set_angle(int angle)
{
	servo_angle_H = angle;
}

void clear_val_obs_buffer()
{
	byte i = 0;
	
	for(i = 0 ; i < VAL_OBS_BUFFER_SIZE ; i++)
	{
		val_obs_buffer[i] = 0;
	}
	val_obs_buffer_size = 0;
}

void choose_ADC_input(ADC_INPUT adc_input)
{
	if(adc_input == AIN0)
	{
		AMX0SL = 0x00;
	}
	else
	{
		AMX0SL = 0x01;
	}
}

float dd_mesure(){
	
	float d;
	float V_mes;
	
	//lecture de la tension mesur�e sur AIN2
	AMX0SL&=0x00;
	AMX0SL|=0x01;
	
	V_mes=ADC0/(POW*5.6);
	
	//Calcul des distances
	d=-16.669*V_mes+MAX_DISTANCE; 
	
	return d;
	
}	


float dd_start_conversion(){
	float conv;
	unsigned int j;
	AD0INT=0;
	AD0BUSY=1;
	dd_mesure();
	AD0BUSY=0;
	
	for(j=0;j<1000;j++); //On temporise pour atteindre la stabilit�
	
	AD0INT=0;
	AD0BUSY=1;
	conv =dd_mesure();
	AD0BUSY=0;
	
	return conv;
}
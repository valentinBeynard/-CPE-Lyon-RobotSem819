#include "light_beam.h"

//port de sortie
sbit PWN_pin = P1^0;

volatile int servo_angle_V = -90;

byte pwn_over = 0;

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

	//duree_imp = 10*(servo_angle_V+90) + 600 ;// 600us pour 90°  (cf doc technique)
	
	duree_imp = SERVO_PWN_ANGLE_COEF *(servo_angle_V+90) + 2000 ;// 1ms pour -90°  (cf doc technique)

	PWN_pin = !PWN_pin;
	
	if (high == 0)
	{
		reload_value = 0xFFFF - (2 * SERVO_PWN_PERIOD - duree_imp );
		high=1;
	} else {
		
		reload_value = 0xFFFF - duree_imp;
		high=0;
	}
	TL0= reload_value;
	TH0= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	/*
	if(nbr_interrupt >= 150)
	{
		ET0 = 0;	// Enable timer0 interuption
		nbr_interrupt = 0;
		pwn_over = 1;
	}
	else
	{
		nbr_interrupt++;
	}*/
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


void Init_light_beam()
{
	
	// Push-Pull mode
	P1MDOUT |= 0x01;
	
	lb_init_timer0();
	
	ET0 = 1;	// Enable timer0 interuption
	
}

void lb_init_timer0()
{	
	//Reglages Timer 0 (a SYSCLK/12 selon CKCON, soit un incrément tous les 0.545us)
	TMOD=0x01; //mode 1
	TCON= 0x11;	// Enable Timer0 (bit4) and enable interupt on edge (bit0)
	TL0=0xA8;
	TH0=0xFB;
	
	TR0 = 1;
}

/*
#############################################################################
        Light Beam Emetter Core
#############################################################################
*/

void light_beam_process(OUT_M2 * cmd)
{
	if(cmd->Etat_Servo == Servo_oui)
	{
		light_beam_move(cmd);
	}
	
}

void light_beam_move(OUT_M2 * cmd)
{
	// Set angle
	if(cmd->Servo_Angle != 0)
	{
		lb_set_angle(cmd->Servo_Angle);
	}
	else
	{
		cmd->Servo_Angle = 90;
		lb_set_angle(cmd->Servo_Angle);
	}
	
	// Re-launch PWN
	pwn_over = 0;
	ET0 = 1;
	
	// Clear CMD Flag
	cmd->Etat_Servo = Servo_non;
	
}

void lb_set_angle(int angle)
{
	servo_angle_V = angle;
}

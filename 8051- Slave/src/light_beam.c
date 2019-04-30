#include "light_beam.h"

//port de sortie
sbit PWN_pin = P1^2;
sbit FLASH_pin = P1^0;

volatile int servo_angle_V = 0;

int duree_flash = 10;
int duree_no_flash = 10;
byte nbr_flash = 10;
byte pwn_duty = 10;

int prescaler_counter = -1;
byte nbr_seq = 0;
LIGHT_STATE light = LIGHT_ON;
LIGHT_GENERATOR light_genrator_state = STOP;


/*
#############################################################################
        Interrupt Routine For Timer 0
#############################################################################
*/


void timer_0_int() interrupt 1
{	
	//static int nbr_interrupt = 0;
	static char high = 0;
	
	int duree_imp = 0;
	int reload_value = 0;
	
	duree_imp = 18 * servo_angle_V + 2768;// (cf excel table 3)
	
	//duree_imp = 13*(servo_angle_V+90) + 1800 ;// 900us pour 90°  (cf doc technique)
	
	//duree_imp = SERVO_PWN_ANGLE_COEF *(servo_angle_V+90) + 2000 ;// 1ms pour -90°  (cf doc technique)

	PWN_pin = !PWN_pin;
	
	if (high == 1)
	{
		reload_value = 0xFFFF - (36666 - duree_imp );
		high=0;
	} else {
		
		reload_value = 0xFFFF - duree_imp;
		high=1;
	}
	TL0= reload_value;
	TH0= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	/*
	if(nbr_interrupt >= 500)
	{
		ET0 = 0;	// Diseable timer0 interuption
		PWN_pin = 0;
		high = 0;
		nbr_interrupt = 0;
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
        Interrupt Routine For Timer 1
#############################################################################
*/

void timer_1_int() interrupt 3
{	
	static char high = 1;
	
	int reload_value = 0;
	
	if (high == 1)
	{
		reload_value = 0xFFFF - (LIGHT_PWN_PERIOD - pwn_duty * LIGHT_PWN_DUTY_1 );
		FLASH_pin = 0;
		high=0;
	}
	else
	{
		reload_value = 0xFFFF - pwn_duty * LIGHT_PWN_DUTY_1;
		prescaler_counter++;
		( FLASH_pin = (light == LIGHT_ON) ? 1 : 0);
		high=1;
	}
	
	
	TL1= reload_value;
	TH1= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	light_beam_flash_sequence();
	
	if(light_genrator_state == FINISH)
	{
		ET1 = 0;
		high = 0;
	}
}

void light_beam_flash_sequence()
{

	if(light_genrator_state == FINISH)
	{
		nbr_seq = 0;
	}
	
	if(light == LIGHT_ON)
	{
		if(prescaler_counter >= 10 * duree_flash)
		{
			prescaler_counter = 0;
			light = LIGHT_OFF;
		}
	}
	else
	{
		if(prescaler_counter >= 10 * duree_no_flash)
		{
			light = LIGHT_ON;
			prescaler_counter = 0;
			nbr_seq++;
			if(nbr_seq >= nbr_flash)
			{
				nbr_seq = 0;
				light_genrator_state = FINISH;
			}
		}
	}
	
}
/*
#############################################################################
        End of Interrupt Routine For Timer 1
#############################################################################
*/

/*
#############################################################################
        Peripheric initialize
#############################################################################
*/


void Init_light_beam()
{
	
	// Push-Pull mode for P0.0 and P0.1
	P1MDOUT |= 0x0F;
	PWN_pin = 0;
	FLASH_pin = 0;
	
	lb_init_timer0();
	
	lb_init_timer1();
	
	ET0 = 0;	// Diseable timer0 interuption
	ET1 = 0;	// Diseable timer1 interuption

}


void lb_init_timer0()
{	
	//Reglages Timer 0 (a SYSCLK/12 selon CKCON, soit un incrément tous les 0.545us)
	TMOD |=0x01; //mode 1
	TCON |= 0x11;	// Enable Timer0 (bit4) and enable interupt on edge (bit0)
	TL0=0xA8;
	TH0=0xFB;
	
}

void lb_init_timer1()
{	
	int init_reload_value = 0;
	
	//Reglages Timer 0 (a SYSCLK/12 selon CKCON, soit un incrément tous les 0.545us)
	TMOD |= 0x10; // Timer 1 Mode 1
	TCON |= 0x44;	// Enable Timer1 (bit6) and enable interupt on edge (bit2)
	
	init_reload_value = 0xFFFF - pwn_duty * LIGHT_PWN_DUTY_1;
	
	TL1= init_reload_value;
	TH1= init_reload_value >> 8;
	
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
	
	if(cmd->Etat_Lumiere == Allumer)
	{
		switch(light_genrator_state)
		{
			case STOP:
				light_beam_switch_ON(cmd);
				break;
			case FINISH:
				cmd->Etat_Lumiere = Lumiere_non;
				light_genrator_state = STOP;
				break;
			default:
				break;		
		}	
	}
	else if(cmd->Etat_Lumiere == Eteindre)
	{
		// TODO
		cmd->Etat_Lumiere = Lumiere_non;
		light_genrator_state = STOP;
		nbr_seq = 0;
		light_beam_switch_OFF(cmd);
	}
	
}

void light_beam_move(OUT_M2 * cmd)
{
	ET0 = 0;
	
	// Set angle
	lb_set_angle(cmd->Servo_Angle);
	
	// Re-launch PWN
	//pwn_over = 0;
	ET0 = 1;
	
	// Clear CMD Flag
	cmd->Etat_Servo = Servo_non;
	
	LED = 1;
	
}

void light_beam_switch_ON(OUT_M2 * cmd)
{
	pwn_duty = cmd->Lumiere_Intensite;
	duree_flash = cmd->Lumiere_Duree;
	duree_no_flash = cmd->Lumire_Extinction;
	nbr_flash = cmd->Lumiere_Nbre;
	
	light_genrator_state = GENERATE;
	
	FLASH_pin = 1;
	
	// Re-launch PWN
	ET1 = 1;
	
	LED = 1;

}

void light_beam_switch_OFF(OUT_M2 * cmd)
{
	FLASH_pin = 0;
	
	ET1 = 0;
	
	LED = 0;
}




void lb_set_angle(int angle)
{
	servo_angle_V = angle;
}

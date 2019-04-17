#include "light_beam.h"

//port de sortie
sbit PWN_pin = P1^0;
sbit FLASH_pin = P1^1;

volatile int servo_angle_V = -90;

volatile int duree_flash = 0;
volatile int duree_no_flash = 0;
volatile byte nbr_flash = 0;
volatile byte pwn_duty = 0;

byte light_ON = 0;


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
	
	if (high == 1)
	{
		reload_value = 0xFFFF - (2 * SERVO_PWN_PERIOD - duree_imp );
		high=0;
	} else {
		
		reload_value = 0xFFFF - duree_imp;
		high=1;
	}
	TL0= reload_value;
	TH0= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	
	if(nbr_interrupt >= 150)
	{
		ET0 = 0;	// Diseable timer0 interuption
		PWN_pin = 0;
		high = 0;
		nbr_interrupt = 0;
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
        Interrupt Routine For Timer 1
#############################################################################
*/

void timer_1_int() interrupt 3
{	
	static byte nbr_int = 0;
	static char prescaler_counter = 0;
	static char high = 0;
	
	int reload_value = 0;
	
	if (high == 1)
	{
		if(prescaler_counter < 10 * duree_flash)
		{
			FLASH_pin = !FLASH_pin;
			reload_value = 0xFFFF - (LIGHT_PWN_PERIOD - pwn_duty * LIGHT_PWN_DUTY_1 );
			prescaler_counter++;
			high=0;
		}
		else
		{
			FLASH_pin = 0;
			reload_value = 0xFFFF - LIGHT_PWN_PERIOD;
			prescaler_counter = 0;
			high=3;
		}	
	}
	else if (high == 0)
	{
		FLASH_pin = !FLASH_pin;
		reload_value = 0xFFFF - pwn_duty * LIGHT_PWN_DUTY_1;
		prescaler_counter++;
		high=1;
	}
	else
	{
		reload_value = 0xFFFF - LIGHT_PWN_PERIOD;
		if(prescaler_counter < 10 * duree_no_flash)
		{
			prescaler_counter++;
		}
		else{
			prescaler_counter = 0;
			high = 0;
		}
	}
	
	TL1= reload_value;
	TH1= reload_value >> 8; //on décale pour obtenir les bits de poids fort
	
	if(nbr_int >= nbr_flash)
	{
		ET1 = 0;	// Diseable timer1 interuption
		FLASH_pin = 0;
		prescaler_counter = 0;
		high = 0;
		nbr_int = 0;
		light_ON = 0;
	}
	else
	{
		nbr_int++;
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
	
	// Push-Pull mode
	P1MDOUT |= 0x01;
	PWN_pin = 0;
	
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
	//Reglages Timer 0 (a SYSCLK/12 selon CKCON, soit un incrément tous les 0.545us)
	TMOD |= 0x10; // Timer 1 Mode 1
	TCON |= 0x44;	// Enable Timer1 (bit6) and enable interupt on edge (bit2)
	
	TL1=0xA8;
	TH1=0xFB;
	
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
		if(light_ON != 1)
		{
			light_beam_switch_ON(cmd);
		}
		else{
			// Clear CMD Flag
			cmd->Etat_Lumiere = Eteindre;
		}		
	}
	else if(cmd->Etat_Lumiere == Eteindre)
	{
		// TODO
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
	
	light_ON = 1;
	
	// Re-launch PWN
	ET1 = 1;
	
	LED = 1;

}

void lb_set_angle(int angle)
{
	servo_angle_V = angle;
}

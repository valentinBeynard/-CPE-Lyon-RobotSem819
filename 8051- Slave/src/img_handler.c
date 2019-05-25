
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//  NOM: METEYER

//
//------------------------------------------------------------------------------------
#include "img_handler.h"

sbit photo_trigger = P1^1;

volatile int IH_Nombre_photos = 1;
volatile int IH_duree_photos = 1;
volatile int IH_Compt_Nombre_photos = 0;
volatile int IH_Compt_duree_photos = 0;
volatile int IH_Compt_nombre_iterations_timer = 0;
volatile int IH_reload = 0xFFFF - 0xB330;

IMGH_STATE imgh_current_state = IMGH_IDLE;

/* Busy Flag. Set to one when a photo process already processing */
byte imgh_busy = 0;

/*
#############################################################################
        Interrupt Routine For Timer 4
#############################################################################
*/
void timer_4_init() interrupt 16 {
	
	IH_Compt_nombre_iterations_timer++;
	if(IH_Compt_nombre_iterations_timer == 4)
	{
		IH_Compt_nombre_iterations_timer = 0;
		IH_Compt_duree_photos++;
		if (IH_Compt_duree_photos == IH_duree_photos)
		{
			IH_Compt_Nombre_photos++;
			photo_trigger = !photo_trigger;
			IH_Compt_duree_photos = 0;
			if (IH_Compt_Nombre_photos == IH_Nombre_photos)
			{
				T4CON  &= ~0x04;
				imgh_busy = 0;
			}
		}
	}
	T4CON  &= ~0x80;
}

//initialisation du timer0
void Init_Img_Handler()
{	
	//paramétrage timer 4	
	RCAP4L = 0x00; //valeur de recharge lowbite
	RCAP4H = 0x00; //valeur de recharge highbite
		
	// Push Pull mode
	P1MDOUT |= 0xFF;
	
	photo_trigger = 0;
	
	EIE2 = 0x04;	// Enable timer4 interuption
}


void img_handler_process(OUT_M2 * cmd)
{
	switch(imgh_current_state)
	{
		/* IDLE State */
		case IMGH_IDLE:
			switch(cmd->Etat_Photo)
			{
				case Photo_1:
					if(!imgh_busy) imgh_current_state = IMGH_SIMPLE;
					cmd->Etat_Photo = Photo_non;
					break;
				
				case Photo_Multiple:
					if(!imgh_busy) imgh_current_state = IMGH_SEQ;
					cmd->Etat_Photo = Photo_non;
					break;
				
				case Photo_continue:
					if(!imgh_busy) imgh_current_state = IMGH_CONTINUE;
					cmd->Etat_Photo = Photo_non;
					break;
				
				default:
					break;
			}
			break;
			
		case IMGH_SIMPLE:
			imgh_busy = 1;
			trigger_photo(1, 1);
			imgh_current_state = IMGH_IDLE;
			break;
		
		case IMGH_SEQ:
			imgh_busy = 1;
			sequence_trig(cmd->Photo_Nbre, cmd->Photo_Duree);
			break;
		
		case IMGH_CONTINUE:
			imgh_busy = 1;
			infinite_trig(cmd->Photo_Duree);
			break;
	}
}

void trigger_photo(int nbr_photo, int time_step)
{
	IH_Nombre_photos = nbr_photo;
	IH_duree_photos = time_step;
	RCAP4L = 0xCF; //valeur de recharge lowbite
	RCAP4H = 0x4C; //valeur de recharge highbite
	T4CON = 0x04;
}

void sequence_trig(int nbr_photo, int time_step)
{
	static byte init_f = 0;
	
	/* Init Sequence */
	if(init_f == 0)
	{
		trigger_photo(nbr_photo, time_step);
		init_f++;
	}
	/* Stop Sequence State when good amount of picture has been taken */
	else if(IH_Compt_Nombre_photos == IH_Nombre_photos)
	{
		imgh_current_state = IMGH_IDLE;
		IH_Compt_Nombre_photos = 0;
		init_f = 0;
	}
	else
	{
		// Nothing to do
	}
}

void infinite_trig(int time_step)
{
		/* Init Sequence */
	if(IH_Compt_Nombre_photos == 0)
	{
		trigger_photo(10, time_step);
	}
	else /* Reset photo counter flag check in Interrupt Timer to never reach end condition */
	{
		IH_Compt_Nombre_photos = 1;
	}
	
}
#include "c8051F020.h"
//#include <string.h>
//#include <stdio.h>
#include "pointeur_lumineux.h"
//#define STATE_WAITING	0
//#define PWN_PERIOD	4000
sfr Reg    =	 0xFF; 

int E;
int D;
float I;
int up;
int down;
int N;

char high = 1;
char angle = 0;
int reload = 0;
int int_servo_OK = 0;
int param_ok;
int is_ended;
// int i=0;
double essai;
int N_cycle;
int cpt_lum;
int cpt_cycle;

enum Lumiere * f_lum = Eteindre;

sbit output_lum = P1^0;

//initialisation du timer0
void init_timer4()
{	
	// Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Exern CLK
	OSCICN = 0x08;
		
	/****** INIT TIMER ******/
	CKCON = 0x40; // Set CLK divider to 12 for timer 0 and 2
	// Réglage timer 0
	
//paramétrage timer 4
	T4CON &= 0x4a;
/*	T4CON &= 0xFE;
	T4CON &= 0xFB;
	T4CON &= 0xEF;
	T4CON &= 0xDF;
	T4CON &= 0x7F;
	T4CON &= 0xFD;
	T4CON &= ~1<<7;
		*/

	
	RCAP4L = 0x00; //valeur de recharge lowbite
	RCAP4H = 0x00; //valeur de recharge highbite
	TH4=0x00;
	TL4=0x00;

	// Pin
	XBR1 = 0x02;
	// Enable Crossbar
	XBR2 = 0x40;
		
	// Push Pull mode
	P1MDOUT = 0xFF;
			
}

void timer_4_int() interrupt 16
{
	if (cpt_cycle >= N_cycle)
	{
		output_lum = 0;
		*f_lum = Eteindre;
		T4CON &= 0xFB;
	}
	else if (high == 0)
	{
		if (cpt_lum <= 4000)
		{
			cpt_lum++;
		} else {
			cpt_lum = 0;
			//essai = E;
			reload = E;
			high = 1;
		}
	} else {
		if (cpt_lum <= 4000)
		{
			if (output_lum == 1)
			{
				//essai = D * (1 - I);
				reload = up;
				output_lum = 0;
				cpt_lum++;
			} 
			else {
				//essai = D * I;
				reload = down;
				output_lum = 1;
			}
		} else {
			high = 0;
			cpt_lum = 0;
			output_lum = 0;
			cpt_cycle++;
			//essai = E;
			reload = E;
			
		}
	}
	
	RCAP4L = reload;
	RCAP4H = reload >> 8;
	//T4CON|=0x08;
	//T4CON&=0xF7;
	//IE7 &= ~1<<5;
	T4CON  &= 0x7f;
	//TH4=RCAP4H;
	//TL4=RCAP4L;
	/*TL4	= reload;
	TH4 = reload >> 8;*/
}


void init_interrupt()
{
	EA = 1; // Enable general interruption
	EIE2 |= 0x04;	// Enable timer4 interuption
}

void init()
{
	init_timer4();
	init_interrupt();
	output_lum = 0;

}


int change_lumiere(double t_allume, double t_eteint, int intensite, int N, enum Lumiere * flag_lum)
{	
	static unsigned char init = 0;
	
	if(init == 0)
	{
		if(*flag_lum == Allumer)
		{
			f_lum = flag_lum;
			if((t_allume <= 9.9 && t_allume >= 0.1) && (t_eteint <= 9.9 && t_eteint >= 0.1) && (intensite >= 1 && intensite <= 100) && (N >= 1) && (N <= 99))
			{
				is_ended = 0;
				
				E = 0xffff - (int) (t_eteint * 5529.6);	
				D = (int) (t_allume * 5529.6);
				I = (float) intensite / 100;
				N_cycle = N;
				cpt_lum = 0;
				cpt_cycle = 0;
				down = (0xffff -  (int) (D * (1 - I)));
				up =  (0xffff - (int)( D * I));
				T4CON |= 0x04;
				init = 1;
				return 1;
			}
		}
		else{
			init = 0;
			return 0;
		}
	}
	else
	{
		return 0;
	}

}

/*
void main (void) 
{	
	Reg = 0xDE; // Ne pas enlever ces 2 lignes de code
  Reg = 0xAD;	// Les laisser au tout d?but du main
	
	init();
	change_lumiere(1, 5, 50, 3);
	//T4CON |= 0x04;
	while(1){}
}*/

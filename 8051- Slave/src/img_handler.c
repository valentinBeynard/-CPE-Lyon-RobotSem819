
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//  NOM: METEYER

//
//------------------------------------------------------------------------------------
#include "c8051F020.h"

sbit sig = P1^1;
int IH_Nombre_photos = 1;
int IH_duree_photos = 1;
int IH_Compt_Nombre_photos = 0;
int IH_Compt_duree_photos = 0;
int IH_Compt_nombre_iterations_timer = 0;
int IH_reload = 0xFFFF - 0xB330;

void WatchDog()
{
	WDTCN = 0xDE;
	WDTCN = 0xAD;
}

//initialisation du timer0
void Init_timer4()
{	
	/*// Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Extern CLK
	OSCICN = 0x08;*/
		
	/****** INIT TIMER ******/
	//CKCON &= 0xBF;; // Set SYSTEM CLK to TIMER 4 /12
	
	
//paramétrage timer 4	
	/*RCAP4L = 0x00; //valeur de recharge lowbite
	RCAP4H = 0x00; //valeur de recharge highbite
	TH4=0x00;
	TL4=0x00;

	// Enable Crossbar
	//XBR2 = 0x40;
		
	// Push Pull mode
	P1MDOUT |= 0x40;
	P3MDOUT |= 0x02;
	
	// 
	EIE2 = 0x04;	// Enable timer4 interuption
	
	/// Enable Timer 4 
	T4CON |= 0x0C;*/
}


void timer_4_init() interrupt 16 {
	
	IH_Compt_nombre_iterations_timer = IH_Compt_nombre_iterations_timer + 1;
	if (IH_Compt_nombre_iterations_timer == 4) {
		IH_Compt_nombre_iterations_timer = 0;
		IH_Compt_duree_photos++;
		if (IH_Compt_duree_photos == IH_duree_photos) {
			IH_Compt_duree_photos = 0;
			sig = !sig;
			IH_Compt_Nombre_photos = IH_Compt_Nombre_photos + 1;
			if (IH_Compt_Nombre_photos == IH_Nombre_photos) {
				T4CON &= 0x48;
		
	RCAP4L = IH_reload; //valeur de recharge lowbite
	RCAP4H = IH_reload >> 8;
	T4CON  &= 0x7f;
			}
		}
	}
}

void prendre_photo(n, e) {
	IH_Nombre_photos = n;
	IH_duree_photos = e;
	T4CON |= 0x04;
}

/*
void main(void) {
	init_timer4();
	init_interrupt();
	WatchDog();
	prendre_photo(2,2);
}*/
/*
nt photo(char etat)
{	
	switch(etat)
	{
	case 'O':
		photo_O(E, 1)
	case 'C':
		photo_C(E, 10000000)
	case 'S':
		photo_S(E,N)
		
	}	
	return 0;
}*/
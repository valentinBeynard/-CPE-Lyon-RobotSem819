
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//  NOM: METEYER

//
//------------------------------------------------------------------------------------
#include "c8051F020.h"

sbit sig = P1^1;
volatile int IH_Nombre_photos = 1;
volatile int IH_duree_photos = 1;
volatile int IH_Compt_Nombre_photos = 0;
volatile int IH_Compt_duree_photos = 0;
volatile int IH_Compt_nombre_iterations_timer = 0;
volatile int IH_reload = 0xFFFF - 0xB330;

void WatchDog()
{
	WDTCN = 0xDE;
	WDTCN = 0xAD;
}

//initialisation du timer0
void Init_Img_Handler()
{	
	/*// Config External Osci
	OSCXCN = 0x67;
	while(OSCXCN != 0xE7) {}
	
	// Use Extern CLK
	OSCICN = 0x08;*/
		
	/****** INIT TIMER ******/
	//CKCON &= 0xBF;; // Set SYSTEM CLK to TIMER 4 /12
	
	
//paramétrage timer 4	
	RCAP4L = 0x00; //valeur de recharge lowbite
	RCAP4H = 0x00; //valeur de recharge highbite
	//TH4=0x00;
	//TL4=0x00;

	// Enable Crossbar
	//XBR2 = 0x40;
		
	// Push Pull mode
	P1MDOUT |= 0xFF;
	//P3MDOUT |= 0x02;
	
	/// Enable Timer 4 
	//T4CON |= 0x08;
	
	sig = 0;
	
	EIE2 = 0x04;	// Enable timer4 interuption
}


void timer_4_init() interrupt 16 {
	
	IH_Compt_nombre_iterations_timer++;
	if(IH_Compt_nombre_iterations_timer == 4)
	{
		IH_Compt_nombre_iterations_timer = 0;
		IH_Compt_duree_photos++;
		if (IH_Compt_duree_photos == IH_duree_photos)
		{
			IH_Compt_Nombre_photos++;
			sig = !sig;
			IH_Compt_duree_photos = 0;
			if (IH_Compt_Nombre_photos == IH_Nombre_photos)
			{
				T4CON  &= ~0x04;
				IH_Compt_Nombre_photos = 0;
			}
		}
	}
	T4CON  &= ~0x80;
}

void prendre_photo(n, e) {
	IH_Nombre_photos = n;
	IH_duree_photos = e;
	RCAP4L = 0xCF; //valeur de recharge lowbite
	RCAP4H = 0x4C; //valeur de recharge highbite
	T4CON = 0x04;
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
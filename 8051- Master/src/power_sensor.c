#include "power_sensor.h"
#include <math.h>

float intensite=0;
float Rshunt = 0.05; //50mOhms

/* Total amount of energy consumption since Robot Initialisation */
unsigned int global_energy = 0;
		
int last_current_measure = 0;
		
/*
#############################################################################
        Interrupt Routine For Timer 1
#############################################################################
*/
	
void timer_1_int() interrupt 3
{	
	// Set ADC Channel
	choose_ADC_input(CURRENT_SHUNT);

	// Read Shunt Value on ADC Channel
	last_current_measure = shunt_measure();
	
	/* Compute amount of energy consums by robot : E (W) = P * t = 0.001 * U(V) * I(mA) * (s) t */
	global_energy += (int)(0.001 * ROBOT_BATTERY_VOLTAGE * last_current_measure * PS_TIME_STEP);
	
	// Reset Value for 10ms Interrupt
	TL1= 0x62;
	TH1= 0xB8;
}

int shunt_measure()
{
	float v_adc = 0;
	
	// Converte ADC0 to Voltage
	v_adc = (read_ADC0() * CONVERSION_RATE);
			
	// Convert and return mA current throught shunt
	return (int)(v_adc * VOLTAGE_2_CURRENT_RATE);
			
}

void Init_power_sensor()
{
	ps_init_timer1();
	
	ET1 = 1;
}

void ps_init_timer1()
{	
	//Reglages Timer 1 (a SYSCLK/12 selon CKCON, soit un incrément tous les 0.545us)
	TMOD |= 0x10; // Timer 1 Mode 1
	TCON |= 0x44;	// Enable Timer1 (bit6) and enable interupt on edge (bit2)
	
	// Reset Value for 10ms Interrupt
	TL1= 0x62;
	TH1= 0xB8;
	
	
}

void power_sensor_process(OUT_M1 * cmd, IN_M1 * info)
{
	
	if(cmd->Etat_Energie == Mesure_I)
	{
		info->Mesure_Courant = last_current_measure;
		cmd->Etat_Energie = Energie_non;
		info->Etat_RESULT_Courant = RESULT_Courant_oui;
	}
	else if(cmd->Etat_Energie == Mesure_E)
	{
		info->Mesure_Energie = global_energy;
		cmd->Etat_Energie = Energie_non;
		info->Etat_RESULT_Energie = RESULT_Energie_oui;
	}

	
}


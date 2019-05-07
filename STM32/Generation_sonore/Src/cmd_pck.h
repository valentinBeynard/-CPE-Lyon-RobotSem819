#ifndef CMD_PCK_H

#define CMD_PCK_H

typedef unsigned char byte;

enum GEN_son {GEN_non, GEN_oui};
enum ACQ_son {ACQ_non, ACQ_oui};

typedef struct
{
	enum GEN_son Etat_GEN;
	
	int frequency_code;
	int delay_ON;
	int delay_OFF;
	int nbr_sound;
	
	enum ACQ_son Etat_ACQ;
	
}CMD_PCK;

#endif
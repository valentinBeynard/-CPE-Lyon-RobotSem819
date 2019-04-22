#ifndef CMD_PCK_H

#define CMD_PCK_H

typedef unsigned char byte;

enum GEN_son {GEN_non, GEN_oui};
enum ACQ_son {ACQ_non, ACQ_oui};

typedef struct
{
	enum GEN_son Etat_GEN;
	
	byte frequency_code;
	byte delay_ON;
	byte delay_OFF;
	byte nbr_sound;
	
	enum ACQ_son Etat_ACQ;
	
}CMD_PCK;

#endif
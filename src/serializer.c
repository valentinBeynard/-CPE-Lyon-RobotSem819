#include "serializer.h"
#include <string.h>
#include <stdio.h>
/*
const SERIALIZER_FSM_PROCESS full_state_machine[3] = {
    {WAIT, &wait},
    {GET_COMMAND, &get_command},
    {SEND_COMMAND, &wait}
};*/

/* Etat courant de la machine d'Etat */
//enum Mouvement serializer_state = Mouvement_non;


/*
#############################################################################
        UART1 related function for µP 8051F020
#############################################################################
*/

/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Timer 2
**/
byte init_serializer_UART1()
{
	
	/****** INIT UART1 *****/
	SCON1 = 0x50;
	
	// Réglage timer 4
	T4CON = 0x3D; // Baud Generator for UART1 + Enable TR4
	
	RCAP4L = 0xDC;	// Reload value for BaudRate 19200
	RCAP4H = 0xFF;
	
  return 0;
}

void serializer_receive(byte* read_byte)
{
  if( (SCON1 & 0x01) == 1)
	{
		*read_byte = SBUF1;
		SCON1 &= 0xFE;	// Remise à 0 de RI1
	}else{
    *read_byte = '*';
  }
}

void serializer_send(byte ch)
{
	SBUF1 = ch;
	while( (SCON1 & 0x02) == 0){} // Tant que TI1 == 0
	SCON1 &= 0xFD;	// Remise à 0 de TI1
}

void serializer_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    serializer_send(*(str+i));
  }
}

/*
#############################################################################
        Serializer related functions
#############################################################################
*/

void serializer_process(OUT_M1* cmd)
{
	if(cmd->Etat_Mouvement != Mouvement_non)
	{
		simple_mvt(cmd);

	}else{
		//serializer_print(cmd);
	}
	
}

byte simple_mvt(OUT_M1* cmd)
{
	int speed = 10;
	
	if(cmd->Vitesse == 0)
	{
		return 0;
	}
	
	speed = cmd->Vitesse;
	
	switch(cmd->Etat_Mouvement)
	{
		case Avancer:
			setMotors(speed, speed);
			cmd->Etat_Mouvement = Mouvement_non;
			break;
		
		case Reculer:
			setMotors((-1) * speed, (-1) * speed);
			cmd->Etat_Mouvement = Mouvement_non;
			break;
		
		case Stopper:
			stopMotors();
			cmd->Etat_Mouvement = Mouvement_non;
			break;

		case Rot_90D:
			
			break;
		
		case Rot_90G:
			
			break;
		
		case Rot_180D:
			
			break;
		
		case Rot_180G:
			
			break;

		case Rot_AngD:
			
			break;

		case Rot_AngG:
			
			break;*/
	}
}

void setMotors(int mtr_speed_1, int mtr_speed_2)
{
	char cmd[MOGO_CMD_SIZE] = "mogo 1:";
	
	sprintf(cmd, "mogo 1:%u 2:%u\n", mtr_speed_1, mtr_speed_2);

	serializer_print(cmd);
}

void stopMotors()
{
	serializer_print("stop\n");
}
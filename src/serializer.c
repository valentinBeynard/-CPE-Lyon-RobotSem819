#include "serializer.h"
#include <string.h>
#include <stdio.h>

const SERIALIZER_FSM_PROCESS serializer_state_machine[4] = {
    {IDLE, NULL},
    {TRANSLATE, NULL},
    {ROTATE, NULL},
		{NAVIGATE, NULL}
};

/* Etat courant de la machine d'Etat */
SERIALIZER_STATE serializer_state = IDLE;

byte is_processing = 0;

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

void UART_send(byte ch)
{
	SBUF0 = ch;
	while(TI0 == 0){}
	TI0 = 0;
}

void serializer_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    UART_send(*(str+i));
		serializer_send(*(str+i));
  }
	
	serializer_send(0x0D);
}

void serializer_init_serial()
{
	char c = 0;
	do
	{
		serializer_receive(&c);
		UART_send(c);
	}while(c != END_RSLT_BYTE);
	
}

void serializer_clear_serial()
{
	char c = 0;
	//UART_send('!');
	do
	{
		serializer_receive(&c);
		UART_send(c);
	}while(c != END_RSLT_BYTE && c != '*');
	
}

/*
#############################################################################
        Serializer related functions
#############################################################################
*/

void serializer_process(OUT_M1* cmd)
{
	PTS_2DA pts = {1, 0, 0, 0};
	
	if(cmd->Etat_Mouvement != Mouvement_non)
	{
		if(serializer_state != IDLE)
		{
			// TO DO
		}else{
			
			pts.speed = cmd->Vitesse;
			
			switch(cmd->Etat_Mouvement)
			{
				case Avancer:
					serializer_state = TRANSLATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
				
				case Reculer:
					serializer_state = TRANSLATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
				
				case Stopper:
					serializer_state = IDLE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;

				case Rot_90D:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
				
				case Rot_90G:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
				
				case Rot_180D:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
				
				case Rot_180G:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;

				case Rot_AngD:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;

				case Rot_AngG:
					serializer_state = ROTATE;
					cmd->Etat_Mouvement = Mouvement_non;
					break;
			}
				//serializer_state_machine[serializer_state].state_process(&pts);
			
			if(serializer_state == TRANSLATE)
			{				
				translate(&pts);
				serializer_state = IDLE;
			}
			else//(serializer_state = ROTATE)
			{
				rotate(&pts);
				serializer_state = IDLE;
			}
		}
		
	}else{
		serializer_state = IDLE;
	}
	
	
	
}

byte translate(PTS_2DA* pts)
{
	char speed = 10;
	
	if(pts->speed != 0)
	{
		speed = pts->speed;
	}
	
	setMotors(pts->x * speed, pts->x * speed);
	
}

byte rotate(PTS_2DA* pts)
{
	//static int angle_consigne = 0;
	char speed = TURN_SPEED;
	char sens = 0;
	
	if(is_processing)
	{
		// ANGLE CALCULATION : TO DO
		/*if()
		{
			
		}*/
	}
	else{
		sens = (pts->angle >= 0 ? 1 : -1 );
		setMotors((-1) * sens * speed, sens * speed);
	}
}

void setMotors(int mtr_speed_1, int mtr_speed_2)
{
	char cmd[MOGO_CMD_SIZE] = "mogo 1:";
	
	//sprintf(cmd, "mogo 1:%u 2:%u\n", mtr_speed_1, mtr_speed_2);

	serializer_print("mogo 1:50 2:50");
	//serializer_print(cmd);
	
	serializer_clear_serial();
}

void stopMotors()
{
	serializer_print("stop\n");
	serializer_clear_serial();
}

int getRawEncoders(ENCODER_ID encoder_id)
{
	int enc_value = 0;
	char result[ENC_RSLT_SIZE];
	char c = 0;
	byte ptr = 0;
	
	if(encoder_id == LEFT)
	{
		serializer_print("getenc 1\n");
	}
	else{
		serializer_print("getenc 2\n");
	}
	
	do
	{
		serializer_receive(&c);
		result[ptr] = c;
		ptr++;
	}while(c != END_RSLT_BYTE);
	
	sscanf(result, "%d", &enc_value);
	
	return enc_value;
}

int getEncoderDistance(ENCODER_ID encoder_id)
{
	int enc_value = 0;
	
	enc_value = getRawEncoders(encoder_id);
	
	return ENC_2_MM(enc_value);
}

int getEncorderAngle()
{
	
}
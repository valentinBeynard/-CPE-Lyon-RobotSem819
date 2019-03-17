#include "serializer.h"
#include <string.h>
#include <stdio.h>

const SERIALIZER_FSM_PROCESS serializer_state_machine[5] = {
    {IDLE, &idle},
    {TRANSLATE, &translate},
    {ROTATE, &rotate},
		{NAVIGATE, NULL},
		{STOP, &stop}
};

/* Etat courant de la machine d'Etat */
volatile SERIALIZER_STATE serializer_state = IDLE;

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
	UART_send('\n');
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
	}while(c != END_RSLT_BYTE);
	
}

/*
#############################################################################
        Serializer related functions
#############################################################################
*/

void serializer_process(OUT_M1* cmd)
{
	PTS_2DA pts = {1, 0, 0, 0};
	
	// Si une commande de mvt est demandé
	if(cmd->Etat_Mouvement == Stopper)
	{
		serializer_state = STOP;
		is_processing = 0;
		cmd->Etat_Mouvement = Mouvement_non;
	}
	else if(cmd->Etat_Mouvement != Mouvement_non)
	{
		// Transition d'état de la FSM
		switch(serializer_state)
		{
			case IDLE:
				idle_next_state(cmd, &pts);
				break;
			
			case TRANSLATE:
				if(is_processing == 1)
				{
					serializer_state = TRANSLATE;
				}
				else
				{
					serializer_state = STOP;
				}
				break;
				
			case ROTATE:
				if(is_processing == 1)
				{
					serializer_state = ROTATE;
				}
				else
				{
					serializer_state = STOP;
				}
				break;
				
			case STOP:
				serializer_state = IDLE;
				cmd->Etat_Mouvement = Mouvement_non;
				break;
			
			default:
				serializer_state = STOP;
				break;
			
		}
		
	}else{
		serializer_state = IDLE;
	}
	
	serializer_state_machine[serializer_state].state_process(&pts);
}

void idle_next_state(OUT_M1* cmd, PTS_2DA* pts)
{
	switch(cmd->Etat_Mouvement)
	{
			case Avancer:
				serializer_state = TRANSLATE;
				pts->x = 1;
				pts->speed = cmd->Vitesse;
				break;
			
			case Reculer:
				serializer_state = TRANSLATE;
				pts->x = -1;
				pts->speed = cmd->Vitesse;
				break;
			
			case Stopper:
				serializer_state = STOP;
				pts->x = 0;
				pts->speed = 0;
				//serializer_print("mogo 1:0 2:0");
				break;

			case Rot_90D:
				serializer_state = ROTATE;
				pts->angle = -90;
				pts->speed = 20;
				break;
			
			case Rot_90G:
				serializer_state = ROTATE;
				pts->angle = 90;
				pts->speed = 20;
				break;
			
			case Rot_180D:
				serializer_state = ROTATE;
				pts->angle = -180;
				pts->speed = 20;
				break;
			
			case Rot_180G:
				serializer_state = ROTATE;
				pts->angle = 180;
				pts->speed = 20;
				break;

			case Rot_AngD:
				serializer_state = ROTATE;
				pts->angle = -1 * cmd->Angle;
				pts->speed = 20;
				break;

			case Rot_AngG:
				serializer_state = ROTATE;
				pts->angle = 1 * cmd->Angle;
				pts->speed = 20;
				break;
			
			default:
				serializer_state = IDLE;
				break;
		}
	
}

void idle(PTS_2DA* pts)
{
	// Nothing to do
}


void translate(PTS_2DA* pts)
{
	char speed = 10;
	
	if(is_processing == 0)
	{
		if(pts->speed != 0)
		{
			speed = pts->speed;
		}
		
		setMotors(pts->x * speed, pts->x * speed);
		is_processing = 1;
	}
	
}

void rotate(PTS_2DA* pts)
{
	char sens = 0;
	
	if(is_processing == 0)
	{
		moveAngle(pts->angle);
		is_processing = 1;
	}
	else{
		is_processing = is_PID_processing();
	}
}

void setMotors(int mtr_speed_1, int mtr_speed_2)
{
	char cmd[MOGO_CMD_SIZE];
	
	sprintf(cmd, "mogo 1:%d 2:%d", mtr_speed_1, mtr_speed_2);

	serializer_print(cmd);
	
	serializer_clear_serial();
}

void moveAngle(float angle)
{
	char cmd[DIGO_CMD_SIZE];
	int distance_in_tick = 0;
	
	distance_in_tick = (int)(ANGLE_2_DIST(angle));
	
	sprintf(cmd, "digo 1 : %d : 28 2 : %d : 28", -1 * distance_in_tick, distance_in_tick);
	serializer_print(cmd);
	serializer_clear_serial();
}

void stop(PTS_2DA* pts)
{
	serializer_print("stop");
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
		serializer_print("getenc 1");
	}
	else{
		serializer_print("getenc 2");
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

char is_PID_processing()
{
	char result[PID_RSLT_SIZE];
	int pid_state = 0;
	char ptr = 0, c = 0;

	do
	{
		serializer_receive(&c);
		result[ptr] = c;
		ptr++;
	}while(c != END_RSLT_BYTE);
	
	sscanf(result, "%d", &pid_state);
	
	return (char)pid_state;
}


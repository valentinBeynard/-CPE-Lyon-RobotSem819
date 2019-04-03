#include "serializer.h"
#include <string.h>
#include <stdio.h>
#include "motion.h"


const SERIALIZER_FSM_PROCESS serializer_state_machine[6] = {
    {IDLE, &idle},
    {TRANSLATE, &translate},
		{MOVETO, &moveTo},
    {ROTATE, &rotate},
		{NAVIGATE, &navigate},
		{STOP, &stop}
};

/* Etat courant de la machine d'Etat */
volatile SERIALIZER_STATE serializer_state = IDLE;

byte is_processing = 0;

byte is_navigating = 0;

int pids_timer = 0;

PTS_2DA robot_position = {0, 0, 0, 0};

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
    //UART_send(*(str+i));
		serializer_send(*(str+i));
  }
	
	serializer_send(0x0D);
	//UART_send('\n');
}

void serializer_init_serial()
{
	char c = 0;
	do
	{
		serializer_receive(&c);
		//UART_send(c);
	}while(c != END_RSLT_BYTE);
	
}

void serializer_clear_serial()
{
	char c = 0;
	//UART_send('!');
	do
	{
		serializer_receive(&c);
		//UART_send(c);
	}while(c != END_RSLT_BYTE);
	//UART_send('\n');
}

/*
#############################################################################
        Serializer related functions
#############################################################################
*/

//void serializer_process(OUT_M1* cmd)
void serializer_process(PARSER_RESULT* parser)
{
	PTS_2DA pts = {1, 0, 0, 0};
	
	// Si une commande de mvt est demandé
	if(parser->commands->Etat_Mouvement == Stopper)
	{
		serializer_state = STOP;
		is_processing = 0;
		parser->commands->Etat_Mouvement = Mouvement_non;
	}
	else if(parser->commands->Etat_Mouvement != Mouvement_non)
	{
		// Transition d'état de la FSM
		switch(serializer_state)
		{
			case IDLE:
				idle_next_state(parser->commands, &pts);
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
				// If already rotating
				if(is_processing == 1)
				{
					serializer_state = ROTATE;
				}
				else
				{
					serializer_state = STOP;
				}
				break;
				
			case NAVIGATE:
				if(is_navigating == 1)
				{
					serializer_state = NAVIGATE;
				}
				else
				{
					parser->informations->Etat_BUT_Mouvement = BUT_Atteint_oui;
					parser->commands->Etat_Mouvement = Mouvement_non;
					robot_position.angle = 1 * parser->commands->Angle;
					robot_position.x = parser->commands->Coord_X;
					robot_position.y = parser->commands->Coord_Y;
					robot_position.speed = parser->commands->Vitesse;
					serializer_state = IDLE;
				}
				break;
				
			case MOVETO:
				if(is_processing == 1)
				{
					serializer_state = MOVETO;
				}
				else
				{
					serializer_state = STOP;
				}
				break;
				
			case STOP:
				if(is_navigating == 0)
				{
					serializer_state = IDLE;
					parser->commands->Etat_Mouvement = Mouvement_non;
				}
				else{
					serializer_state = NAVIGATE;
				}
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
				pts->angle = (-1) * 90;
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
			
			case Depl_Coord:
				serializer_state = NAVIGATE;
				pts->angle = 1 * cmd->Angle;
				pts->x = cmd->Coord_X;
				pts->y = cmd->Coord_Y;
				pts->speed = cmd->Vitesse;
				break;
			
			default:
				serializer_state = IDLE;
				break;
		}
	
}

void idle(PTS_2DA* pts)
{
	// For WARNING C280 supression
	pts = pts;
	
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
		//clear_encoder();
		//getEncoderDistance(LEFT);
		moveAngle(pts->angle);
		is_processing = 1;
	}
	else{
		
		if(pids_timer >= 5000)
		{
			is_processing = is_PID_processing();
			pids_timer = 0;
		}
		else{
			pids_timer++;
		}
	}
}

void navigate(PTS_2DA* pts)
{
	static byte navigation_step = 0;
	static int target_angle = 0;
	PTS_2DA temp_pts = {0, 0, 0, 0};
	
	switch(navigation_step)
	{
		case 0:
			// Start Navigation Flag
			is_navigating = 1;
			pts->x = 10;
			pts->y = 10;
			target_angle = angle_target(pts->x, pts->y);
			temp_pts.angle = delta_angle(&(robot_position.angle), &target_angle);
			//temp_pts.angle = delta_angle(&(pts->angle), &(robot_position.angle) );
			serializer_state = ROTATE;
			navigation_step++;
			rotate(&temp_pts);
			break;
		
		case 1:
			/*
			temp_pts.x = (robot_position.x - pts->x);
			temp_pts.y = (robot_position.y - pts->y);
			temp_pts.speed = pts->speed;
		*/
			temp_pts.x = 10;
			temp_pts.y = 10;
			temp_pts.speed = 20;
			serializer_state = MOVETO;
			navigation_step++;
			moveTo(&temp_pts);
			break;
		
		case 2:
			temp_pts.angle = delta_angle(&target_angle, &(robot_position.angle) );
			serializer_state = ROTATE;
			navigation_step++;
			rotate(&temp_pts);
			break;
		
		case 3:
			is_navigating = 0;
			navigation_step = 0;
			target_angle = 0;
			break;
		
	}
	
	
}

void setMotors(int mtr_speed_1, int mtr_speed_2)
{
	char cmd[MOGO_CMD_SIZE];
	
	sprintf(cmd, "mogo 1:%d 2:%d", mtr_speed_1, mtr_speed_2);

	serializer_print(cmd);
	
	serializer_clear_serial();
}

void moveTo(PTS_2DA* pts)
{
	char cmd[DIGO_CMD_SIZE];
	int distance_in_tick = 0;
	
	if(is_processing == 0)
	{
		distance_in_tick = distance(pts->x, pts->y);
		distance_in_tick = (int)(3.3 * 100 * distance_in_tick);
		
		// TODO : Pour le moment, la vitesse du digo est de 28%, donc pas la valeur set par TV
		// On est obligé car ce 28 est fixé par le DPID et le VPID du sérializer ...
		sprintf(cmd, "digo 1:%d:10 2:%d:10", distance_in_tick, distance_in_tick);
	
		serializer_print(cmd);
		serializer_clear_serial();
		
		is_processing = 1;
	} 
	else{
		
		if(pids_timer >= 5000)
		{
			is_processing = is_PID_processing();
			pids_timer = 0;
		}
		else{
			pids_timer++;
		}
	}
}

void moveAngle(int angle)
{
	char cmd[DIGO_CMD_SIZE];
	int distance_in_tick = 0, r_dist = 0;
	
	distance_in_tick = ANGLE_2_DIST(angle);
	
	r_dist = (-1) * distance_in_tick;
	
	sprintf(cmd, "digo 1:%d:20 2:%d:20", distance_in_tick, r_dist);
	serializer_print(cmd);
	serializer_clear_serial();
	
	//memset(cmd, 0, DIGO_CMD_SIZE);
}

void stop(PTS_2DA* pts)
{
	// For WARNING C280 supression
	pts = pts;
	
	serializer_print("stop");	
	serializer_clear_serial();
	//clear_encoder();
}

/*
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


void clear_encoder()
{
	char result[PID_RSLT_SIZE];
	char ptr = 0, c = 0;
	
	serializer_print("clrenc 1 2");

	do
	{
		serializer_receive(&c);
		result[ptr] = c;
		ptr++;
	}while(c != END_RSLT_BYTE);
	
	
}
*/

char is_PID_processing()
{
	char result[PID_RSLT_SIZE];
	int pid_state = 1;
	char ptr = 0;
	byte c = 0;
	
	serializer_print("pids");
		
	do
	{
		serializer_receive(&c);
		if(c != '*')
		{
			result[ptr] = c;
			ptr++;
			UART_send(c);
		}
	}while(c != END_RSLT_BYTE);
	
	sscanf(result, "%d", &pid_state);
	
	memset(result, 0, PID_RSLT_SIZE);
	
	return (char)pid_state;
}


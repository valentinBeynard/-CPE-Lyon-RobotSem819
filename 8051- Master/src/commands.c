#include "commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

byte default_process(CMD_PACKET* cmd_packet)
{
  //printf("Non-implemented command !!");
	cmd_packet->commands->Etat_Epreuve = (enum Epreuve)1;
	
	return 1;
}

/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
byte epreuve_cmd(CMD_PACKET* cmd_packet)
{
  int epreuve_index = 1;

	// Trop d'argument
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}

  // Si le paramètre correspondant au numéro d'épreuve est présent
  if(cmd_packet->cmd_size == 1)
  {
    // Si valeur non numérique
		if(sscanf((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "%u", &epreuve_index) != 1)
		{
			return 0;
		}

		// Si pas dans l'intervalle de valeur acceptées
		if(epreuve_index < 1 || epreuve_index > 9)
		{
			return 0;
		}
		
    cmd_packet->commands->Etat_Epreuve = (enum Epreuve)epreuve_index;
  }
  else
  {
    cmd_packet->commands->Etat_Epreuve = epreuve1;
  }
	return 1;
}

byte epreuve_stop(CMD_PACKET* cmd_packet)
{
  //printf("Non-implemented command !!");
	cmd_packet->commands->Etat_Epreuve = Epreuve_non;
	
	return 1;
}


byte safety_break_cmd(CMD_PACKET* cmd_packet)
{
	// Trop d'argument
	if(cmd_packet->cmd_size > 0)
	{
		return 0;
	}

	cmd_packet->commands->Etat_Epreuve = Stop_Urgence;
	
	return 1;
}

byte set_default_speed_cmd(CMD_PACKET* cmd_packet)
{
	int motor_speed = 20;
	
	// Seu la commande TV valeur est acceptée
	if(cmd_packet->cmd_size != 1)
	{
		return 0;
	}
	
	// Si valeur non numérique
	if(sscanf((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "%d", &motor_speed) != 1)
	{
		return 0;
	}
	
	// Si pas dans l'intervalle de valeur acceptées
	if(motor_speed < 0 || motor_speed > 100)
	{
		return 0;
	}
	
	cmd_packet->commands->Vitesse = motor_speed;

	return 1;
}

byte move_forward_cmd(CMD_PACKET* cmd_packet)
{
	int motor_speed = 20;
	
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}
		
	cmd_packet->commands->Etat_Mouvement = Avancer;

	if(cmd_packet->cmd_size == 1)
	{
		// Si valeur non numérique
		if(sscanf((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "%d", &motor_speed) == 0)
		{
			return 0;
		}
		
		// Si pas dans l'intervalle de valeur acceptées
		if(motor_speed < 0 || motor_speed > 100)
		{
			return 0;
		}
		
		cmd_packet->commands->Vitesse = motor_speed;
	}else{
		if(cmd_packet->commands->Vitesse == 0)
		{
			cmd_packet->commands->Vitesse = motor_speed;
		}
	}
	return 1;
}

byte move_backward_cmd(CMD_PACKET* cmd_packet)
{
	int motor_speed = 20;
	
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}
		
	cmd_packet->commands->Etat_Mouvement = Reculer;

	if(cmd_packet->cmd_size == 1)
	{
		// Si valeur non numérique
		if(sscanf((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "%d", &motor_speed) == 0)
		{
			return 0;
		}
		
		// Si pas dans l'intervalle de valeur acceptées
		if(motor_speed < 0 || motor_speed > 100)
		{
			return 0;
		}
		
		cmd_packet->commands->Vitesse = motor_speed;
	}else{
		if(cmd_packet->commands->Vitesse == 0)
		{
			cmd_packet->commands->Vitesse = motor_speed;
		}
	}
	return 1;
}

byte move_stop_cmd(CMD_PACKET* cmd_packet)
{	
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}
	cmd_packet->commands->Etat_Mouvement = Stopper;
	return 1;
}

byte rigth_rotation_cmd(CMD_PACKET* cmd_packet)
{
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}
	
	cmd_packet->commands->Etat_Mouvement = Rot_90D;
	return 1;
}

byte left_rotation_cmd(CMD_PACKET* cmd_packet)
{
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}
	
	cmd_packet->commands->Etat_Mouvement = Rot_90G;
	return 1;
}

byte complete_rotation_cmd(CMD_PACKET* cmd_packet)
{
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}

	if(cmd_packet->cmd_size == 1)
	{
		
		if(strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "D") == 0 
			|| strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "G") == 0)
		{
			switch( (*(cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE))) )
			{
				case 'D':
					cmd_packet->commands->Etat_Mouvement = Rot_180D;
					break;
				default :
					cmd_packet->commands->Etat_Mouvement = Rot_180G;
					break;
			}
		}else{
			return 0;
		}

	}else{
		cmd_packet->commands->Etat_Mouvement = Rot_180D;
	}
	return 1;
}

byte angle_rotation_cmd(CMD_PACKET* cmd_packet)
{
	unsigned int angle = 0;
	
	if(cmd_packet->cmd_size != 2)
	{
		return 0;
	}
	
	if(strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "D") == 0 
			|| strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "G") == 0)
	{
		switch( (*(cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE))) )
		{
			case 'D':
				cmd_packet->commands->Etat_Mouvement = Rot_AngD;
				break;
			default :
				cmd_packet->commands->Etat_Mouvement = Rot_AngG;
				break;
		}
		
		if(sscanf((cmd_packet->commands_data + (2 * ARGS_BUFFER_SIZE)), "%d", &angle) == 0)
		{
			return 0;
		}
		
		cmd_packet->commands->Angle = (int)(angle);
	}else{
		return 0;
	}
	return 1;
}

/*
	Move to (X,Y,alpha) coordinate

	Ex : "G X:10 Y:15 A:45"

	Function exclude from Data Overlaying in "linker_directives" : OVERLAY( * ! move_to_cmd )

*/
byte move_to_cmd(CMD_PACKET* cmd_packet)
{
	byte i = 0, j = 0;
	char params[3][5] = {"X", "Y", "A"};
	int angle = 0, x = 0, y = 0;
	byte param_find = 0;
	char * str = 0;
	
	
	// Not enougthl args
	if(cmd_packet->cmd_size != 6)
	{
		return 0;
	}
	
	// Analyse each param:value couple
	for(i = 0 ; i < 3 ; ++i)
	{
		str = (cmd_packet->commands_data + ( ((2*i) + 1) * ARGS_BUFFER_SIZE));
		
		// For one couple, identify the param
		for(j = 0 ; j < 3 ; j++)
		{
			if( strcmp(str, params[j]) == 0 )
			{
				switch(j)
				{
					case 0:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &x) == 0)
						{
							return 0;
						}				
						break;
					
					case 1:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &y) == 0)
						{
							return 0;
						}		
						break;
					
					case 2:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &angle) == 0)
						{
							return 0;
						}	
						break;
					
				}
				param_find = 1;
				break;
			}
		}
		
		if(param_find == 0)
		{
			return 0;
		}else{
			param_find = 0;
		}
		
	}
	
	cmd_packet->commands->Angle = angle;
	cmd_packet->commands->Coord_X = (byte)x;
	cmd_packet->commands->Coord_Y = (byte)y;
	cmd_packet->commands->Etat_Mouvement = Depl_Coord;
	return 1;
}

byte detecte_obstacle(CMD_PACKET* cmd_packet)
{
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}
	
	cmd_packet->commands->Etat_DCT_Obst = oui_single;
	return 1;
}

byte servo_move_cmd(CMD_PACKET* cmd_packet)
{
	int angle = 45;
	
	if(cmd_packet->cmd_size > 3)
	{
		return 0;
	}
	
	if(strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "H") == 0 
		|| strcmp((cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE)), "V") == 0)
	{
		switch( (*(cmd_packet->commands_data + (1 * ARGS_BUFFER_SIZE))) )
		{
			case 'V':
				cmd_packet->commands->Etat_Servo = Servo_V;
				break;
			default :
				cmd_packet->commands->Etat_Servo = Servo_H;
				break;
		}
		
	}
	
	if(strcmp((cmd_packet->commands_data + (2 * ARGS_BUFFER_SIZE)), "A") == 0 )
	{
		if(sscanf((cmd_packet->commands_data + (3 * ARGS_BUFFER_SIZE)), "%d", &angle) == 0)
		{
			return 0;
		}
	
		cmd_packet->commands->Servo_Angle = (char)(angle);
	}
	
	return 1;
}

byte light_beam_ON_cmd(CMD_PACKET* cmd_packet)
{
	byte i = 0, j = 0;
	char params[4][5] = {"I", "D", "E", "N"};
	int intensity = 100, delay_on = 99, delay_off = 0, nbr_flash = 1;
	byte param_find = 0;
	char * str = 0;
	
	// To much args
	if(cmd_packet->cmd_size > 8 || (cmd_packet->cmd_size % 2) != 0)
	{
		return 0;
	}
	
	// Analyse each param:value couple
	for(i = 0 ; i < (cmd_packet->cmd_size/2) ; ++i)
	{
		str = (cmd_packet->commands_data + ( ((2*i) + 1) * ARGS_BUFFER_SIZE));
		
		// For one couple, identify the param
		for(j = 0 ; j < 4 ; j++)
		{
			if( strcmp(str, params[j]) == 0 )
			{
				switch(j)
				{
					case 0:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &intensity) == 0)
						{
							return 0;
						}
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(intensity < 1 || intensity > 100)
							{
								return 0;
							}
						}
						break;
					
					case 1:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &delay_on) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(delay_on < 1 || delay_on > 99)
							{
								return 0;
							}
						}						
						break;
					
					case 2:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &delay_off) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(delay_off < 0 || delay_off > 99)
							{
								return 0;
							}
						}		
						break;
						
					case 3:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &nbr_flash) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(nbr_flash < 1 || nbr_flash > 99)
							{
								return 0;
							}
						}	
						break;
				}
				param_find = 1;
				break;
			}
		}
		
		if(param_find == 0)
		{
			return 0;
		}else{
			param_find = 0;
		}
		
	}
	
	cmd_packet->commands->Etat_Lumiere = Allumer;
	cmd_packet->commands->Lumiere_Intensite = intensity;
	cmd_packet->commands->Lumiere_Duree = delay_on;
	cmd_packet->commands->Lumire_Extinction = delay_off;
	cmd_packet->commands->Lumiere_Nbre = nbr_flash;
	
	return 1;
}

byte light_beam_OFF_cmd(CMD_PACKET* cmd_packet)
{
	// To much args
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}

	cmd_packet->commands->Etat_Lumiere = Eteindre;

	return 1;
}

byte generate_sound_cmd(CMD_PACKET* cmd_packet)
{
		byte i = 0, j = 0;
	char params[4][5] = {"F", "P", "W", "B"};
	int frequency_code = 100, delay_son = 25, delay_silence = 50, nbr_bip = 3;
	byte param_find = 0;
	char * str = 0;
	
	// To much args
	if(cmd_packet->cmd_size > 8 || (cmd_packet->cmd_size % 2) != 0)
	{
		return 0;
	}
	
	// Analyse each param:value couple
	for(i = 0 ; i < (cmd_packet->cmd_size/2) ; ++i)
	{
		str = (cmd_packet->commands_data + ( ((2*i) + 1) * ARGS_BUFFER_SIZE));
		
		// For one couple, identify the param
		for(j = 0 ; j < 4 ; j++)
		{
			if( strcmp(str, params[j]) == 0 )
			{
				switch(j)
				{
					case 0:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &frequency_code) == 0)
						{
							return 0;
						}
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(frequency_code < 1 || frequency_code > 99)
							{
								return 0;
							}
						}
						break;
					
					case 1:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &delay_son) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(delay_son < 1 || delay_son > 99)
							{
								return 0;
							}
						}						
						break;
					
					case 2:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &delay_silence) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(delay_silence < 1 || delay_silence > 99)
							{
								return 0;
							}
						}		
						break;
						
					case 3:
						if(sscanf((cmd_packet->commands_data + ((2 + i * 2) * ARGS_BUFFER_SIZE)), "%d", &nbr_bip) == 0)
						{
							return 0;
						}	
						else
						{
							// Si pas dans l'intervalle de valeur acceptées
							if(nbr_bip < 1 || nbr_bip > 99)
							{
								return 0;
							}
						}	
						break;
				}
				param_find = 1;
				break;
			}
		}
		
		if(param_find == 0)
		{
			return 0;
		}else{
			param_find = 0;
		}
		
	}
	
	cmd_packet->commands->Etat_GEN_Son = GEN_oui;
	cmd_packet->commands->GEN_freq_code = frequency_code;
	cmd_packet->commands->GEN_son_Duree = delay_son;
	cmd_packet->commands->GEN_silence_Duree = delay_silence;
	cmd_packet->commands->GEN_nbr_bip = nbr_bip;
	
	return 1;
	
}

byte photo_cmd(CMD_PACKET* cmd_packet)
{
	byte i = 1, j = 0;
	char params[5][3] = {"E", "N", "O", "C", "S"};
	int duree_photo = 1, nbr_photo = 1, mode = 1;
	byte param_find = 0;
	char * str = 0;
	
	
	// To much args
	if(cmd_packet->cmd_size > 5)
	{
		return 0;
	}
		
	
	// Analyse each param:value couple
	while(i <= cmd_packet->cmd_size)
	{
		str = (cmd_packet->commands_data + ( i * ARGS_BUFFER_SIZE));
		
		// For one couple, identify the param
		for(j = 0 ; j < 5 ; j++)
		{
			if( strcmp(str, params[j]) == 0 )
			{
				switch(j)
				{
					case 0:
						if(sscanf((cmd_packet->commands_data + ( (i+1) * ARGS_BUFFER_SIZE) ), "%d", &duree_photo) == 0)
						{
							return 0;
						}		
						i += 2;
						break;
					
					case 1:
						if(sscanf((cmd_packet->commands_data + ( (i+1) * ARGS_BUFFER_SIZE) ), "%d", &nbr_photo) == 0)
						{
							return 0;
						}
						i += 2;
						break;
					case 2:
						mode = 1;
						i++;
						break;
					case 3:
						mode = 2;
						i++;
						break;
					case 4:
						mode = 3;
						i++;
						break;
				}
				param_find = 1;
				break;
			}
		}
		
		if(param_find == 0)
		{
			return 0;
		}else{
			param_find = 0;
		}
		
	}
	
	cmd_packet->commands->Photo_Duree = (byte)duree_photo;
	cmd_packet->commands->Photo_Nbre = (byte)nbr_photo;
	( cmd_packet->commands->Etat_Photo =  (mode == 1) ? Photo_1 : ( (mode == 2) ? Photo_Multiple : Photo_continue) );
	return 1;
}

byte photo_OFF_cmd(CMD_PACKET* cmd_packet)
{
	// To much args
	if(cmd_packet->cmd_size != 0)
	{
		return 0;
	}

	cmd_packet->commands->Etat_Photo = Photo_stop;

	return 1;
}

byte aux_cmd(CMD_PACKET* cmd_packet)
{
	byte * str = 0;
	char params[4][3] = {"A", "B", "S", "L"};
	int ticks_value = 0;
	
	str = (cmd_packet->commands_data + ( 1 * ARGS_BUFFER_SIZE));
	
	// Command A [Tick] to rotate from [Tick] number of ticks 
	if( strcmp(str, params[0]) == 0 )
	{
		if(sscanf((cmd_packet->commands_data + ( 2 * ARGS_BUFFER_SIZE)), "%d", &ticks_value) == 0)
		{
			return 0;
		}
		cmd_packet->commands->Etat_Mouvement = Avancer;
		cmd_packet->commands->Pos_Angle = ticks_value;
		return 1;
	}
	
	// Command B to get back from last calibration position
	else if( strcmp(str, params[1]) == 0 )
	{
		cmd_packet->commands->Etat_Mouvement = Reculer;
		return 1;
	}
	
	// Command S to save last tick value and start new one
	else if( strcmp(str, params[2]) == 0 )
	{
		cmd_packet->commands->Etat_Mouvement = Stopper;
		return 1;
	}
	
	// Command to plot ALL ticks _data
	else if (strcmp(str, params[3]) == 0)
	{
		cmd_packet->commands->Etat_Mouvement = Depl_Coord;
		return 1;
	}
	
	return 0;
}

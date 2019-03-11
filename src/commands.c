#include "commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

byte default_process(CMD_PACKET* cmd_packet)
{
  //printf("Non-implemented command !!");
}

/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
byte epreuve_cmd(CMD_PACKET* cmd_packet)
{
  int epreuve_index = 1;
	int c = 0;
	char* str;
	char** str_2;
	// Trop d'argument
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}

  // Si le paramètre correspondant au numéro d'épreuve est présent
  if(cmd_packet->cmd_size == 1)
  {
    // Si valeur non numérique
		str = (cmd_packet->commands_data + 1);
		str_2 = (cmd_packet->commands_data + 1);
		c = sscanf(str, "%d", &epreuve_index);
		if(c != 1)
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
	byte motor_speed = 20;
	
	// Seu la commande TV valeur est acceptée
	if(cmd_packet->cmd_size != 1)
	{
		return 0;
	}
	
	// Si valeur non numérique
	if(sscanf(*(cmd_packet->commands_data + 1), "%d", motor_speed) == 0)
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
	byte motor_speed = 20;
	
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}
		
	cmd_packet->commands->Etat_Mouvement = Avancer;

	if(cmd_packet->cmd_size == 1)
	{
		// Si valeur non numérique
		if(sscanf(*(cmd_packet->commands_data + 1), "%d", motor_speed) == 0)
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
	byte motor_speed = 20;
	
	if(cmd_packet->cmd_size > 1)
	{
		return 0;
	}
		
	cmd_packet->commands->Etat_Mouvement = Reculer;

	if(cmd_packet->cmd_size == 1)
	{
		// Si valeur non numérique
		if(sscanf(*(cmd_packet->commands_data + 1), "%d", motor_speed) == 0)
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
	cmd_packet->commands->Vitesse = Stopper;
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
		
		if(strcmp(*(cmd_packet->commands_data + 1), "D") == 0 
			|| strcmp(*(cmd_packet->commands_data + 1), "G") == 0)
		{
			switch( *(*(cmd_packet->commands_data + 1)) )
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
	unsigned int angle = 90;
	
	if(cmd_packet->cmd_size != 2)
	{
		return 0;
	}
	
	if(strcmp(*(cmd_packet->commands_data + 1), "D") == 0 
			|| strcmp(*(cmd_packet->commands_data + 1), "G") == 0)
	{
		switch( *(*(cmd_packet->commands_data + 1)) )
		{
			case 'D':
				cmd_packet->commands->Etat_Mouvement = Rot_AngD;
				break;
			default :
				cmd_packet->commands->Etat_Mouvement = Rot_AngG;
				break;
		}
		
		if(sscanf(*(cmd_packet->commands_data + 2), "%d", angle) == 0)
		{
			return 0;
		}
		
		cmd_packet->commands->Angle = (int)(100 * angle);
	}else{
		return 0;
	}
	return 1;
}

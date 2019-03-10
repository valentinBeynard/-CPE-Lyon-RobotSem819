#include "commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

byte args_valid(CMD_PACKET* cmd_packet)
{
	byte cmd_data_ptr = 0, agrs_ptr = 0;
	byte arg_found = 0;
	
	if(cmd_packet->cmd_size == 0)
	{
		return 1;
	}
	
	// Lorsqu'une commande contient des args (ie sargs_size != 0), le nom des args
	// sont toujours sur un index impair dans command_data
	
	// On parcour tous les agrs entrés par l'user
	for(cmd_data_ptr = 1; cmd_data_ptr < cmd_packet->cmd_size; cmd_data_ptr+=2)
	{
		// On compare chaque arg avec la liste des args dispo avec la commande
		for(agrs_ptr = 0; agrs_ptr < cmd_packet->args_size ; agrs_ptr++)
		{
			// Si l'arg est valide, on test le suivant entré par l'user
			if(strcmp((cmd_packet->commands_data + cmd_data_ptr), (cmd_packet->args_label + agrs_ptr) ) == 0)
			{
				arg_found = 1;
				break;
			}
		}
		
		// Si arg pas dans la liste des arg disponnible, erreur de cmd
		if(arg_found == 0)
		{
			return 0;
		}else{
			arg_found = 0;
		}
	}
	
	return 1;
	
}

void default_process(CMD_PACKET* cmd_packet)
{
  //printf("Non-implemented command !!");
}

/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
void start_test(CMD_PACKET* cmd_packet)
{
  byte epreuve_index = 1;

  // Si le paramètre correspondant au numéro d'épreuve est présent
  if(cmd_packet->cmd_size == 1)
  {
    epreuve_index = atoi(*(cmd_packet->commands_data + 1));
    cmd_packet->commands->Etat_Epreuve = (enum Epreuve)epreuve_index;
  }
  else
  {
    cmd_packet->commands->Etat_Epreuve = epreuve1;
  }

}


void safety_break(CMD_PACKET* cmd_packet)
{
	cmd_packet->commands->Etat_Epreuve = Stop_Urgence;
}

void set_default_speed(CMD_PACKET* cmd_packet)
{
	byte motor_speed = 20;

	motor_speed = atoi(*(cmd_packet->commands_data + 1));
	motor_speed = (byte) max(5, min(motor_speed, 100));
	
	cmd_packet->commands->Vitesse = motor_speed;

	
}

void move_forward(CMD_PACKET* cmd_packet)
{
	byte motor_speed = 20;
		
	cmd_packet->commands->Etat_Mouvement = Avancer;

	if(cmd_packet->cmd_size == 1)
	{
		motor_speed = atoi(*(cmd_packet->commands_data + 1));
		motor_speed = max(5, min(motor_speed, 100));
		cmd_packet->commands->Vitesse = motor_speed;
	}else{
		if(cmd_packet->commands->Vitesse == 0)
		{
			cmd_packet->commands->Vitesse = motor_speed;
		}
	}
	
}

void move_backward(CMD_PACKET* cmd_packet)
{
	byte motor_speed = 20;
		
	cmd_packet->commands->Etat_Mouvement = Reculer;

	if(cmd_packet->cmd_size == 1)
	{
		motor_speed = atoi(*(cmd_packet->commands_data + 1));
		motor_speed = max(5, min(motor_speed, 100));
		cmd_packet->commands->Vitesse = motor_speed;
	}else{
		if(cmd_packet->commands->Vitesse == 0)
		{
			cmd_packet->commands->Vitesse = motor_speed;
		}
	}
	
}

void move_stop(CMD_PACKET* cmd_packet)
{	
	cmd_packet->commands->Vitesse = Stopper;

}

void rigth_rotation(CMD_PACKET* cmd_packet)
{
	cmd_packet->commands->Etat_Mouvement = Rot_90D;
	
}

void left_rotation(CMD_PACKET* cmd_packet)
{
	cmd_packet->commands->Etat_Mouvement = Rot_90G;
	
}

void complete_rotation(CMD_PACKET* cmd_packet)
{
	if(cmd_packet->cmd_size == 1)
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
		cmd_packet->commands->Etat_Mouvement = Rot_180D;
	}
		
}

void angle_rotation(CMD_PACKET* cmd_packet)
{
	unsigned int angle = 90;
	
	// A revoir. La commande RA 75 ne marche pas ....
	// Il faut soit RA, RA D , RA G, RA D:valeur, mais pas RA valeur....
	if(cmd_packet->cmd_size == 1)
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
		cmd_packet->commands->Angle = (100 * angle);
	}
	else if(cmd_packet->cmd_size == 2)
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
		
		sscanf(*(cmd_packet->commands_data + 2), "%d", angle);
		angle = max(0, min(angle, 180));
		cmd_packet->commands->Angle = (int)(100 * angle);
	}else{
		cmd_packet->commands->Etat_Mouvement = Rot_AngD;
		cmd_packet->commands->Angle = (100 * angle);
	}
}

#ifndef COMMAND_H

#define COMMAND_H

#include "pc_cmd.h"

#define byte unsigned char
#define signed_byte char

/*
typedef unsigned char byte;
typedef char signed_byte;
*/

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

// Taille max du buffer pour un seul mot de commande/argument
#define ARGS_BUFFER_SIZE  20

// Nombre max de mots composants une commande que l'on peut récupérer avec le buffer lors du parse
#define MAX_COMMAND_WORD  10



typedef struct
{
	char* commands_data;
	byte cmd_size;
	OUT_M1* commands;
}CMD_PACKET;

typedef struct
{
  const char* name;
  byte(*process)(CMD_PACKET*);
}CMD_;


byte default_process(CMD_PACKET* cmd_packet);


/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
byte epreuve_cmd(CMD_PACKET* cmd_packet);
byte epreuve_stop(CMD_PACKET* cmd_packet);

byte safety_break_cmd(CMD_PACKET* cmd_packet);
byte set_default_speed_cmd(CMD_PACKET* cmd_packet);
byte move_forward_cmd(CMD_PACKET* cmd_packet);
byte move_backward_cmd(CMD_PACKET* cmd_packet);
byte move_stop_cmd(CMD_PACKET* cmd_packet);
byte rigth_rotation_cmd(CMD_PACKET* cmd_packet);
byte left_rotation_cmd(CMD_PACKET* cmd_packet);
byte complete_rotation_cmd(CMD_PACKET* cmd_packet);
byte angle_rotation_cmd(CMD_PACKET* cmd_packet);
byte move_to_cmd(CMD_PACKET* cmd_packet);


byte detecte_obstacle(CMD_PACKET* cmd_packet);
byte servo_move_cmd(CMD_PACKET* cmd_packet);
byte light_beam_ON_cmd(CMD_PACKET* cmd_packet);
byte light_beam_OFF_cmd(CMD_PACKET* cmd_packet);
byte generate_sound_cmd(CMD_PACKET* cmd_packet);
byte photo_cmd(CMD_PACKET* cmd_packet);
byte photo_OFF_cmd(CMD_PACKET* cmd_packet);

#else

//#error Multiple define !

#endif
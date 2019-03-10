#include "commands.h"
#include <stdlib.h>

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

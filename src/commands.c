#include "commands.h"
#include <stdio.h>
#include <string.h>

void default_process(char** args, byte size, OUT_M1* commands)
{
  printf("Non-implemented command !!");
}

/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
void start_test(char** args, byte size, OUT_M1* commands)
{
  byte epreuve_index = 1;

  // Si le paramètre correspondant au numéro d'épreuve est présent
  if(size == 1)
  {
    epreuve_index = atoi(*(args + 1));
    commands->Etat_Epreuve = (enum Epreuve)epreuve_index;
  }
  else
  {
    commands->Etat_Epreuve = epreuve1;
  }

}

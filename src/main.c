#ifndef F_CPU

/*
  Par défaut, les fuses de l'ATmega328p sont configurés pour obtenir une clk de 1MHz
  Pour changer la clock du µP et utiliser le crital de 16MHz, il faut reprogrammer
  les fuses par le biais de avrdude. Comme j'utilise une BOARD Arduino, et que je
  deploy par USB et non par Sonde JTAG, le code est deploy par le bootloader Arduino,
  et donc les fuses sont programmés pour avoir la clock de 16MHz ...
*/
#define F_CPU 16000000UL // or whatever may be your frequency


#endif

//#include <avr/io.h>
//#include <util/delay.h>                // for _delay_ms()
#include "commands_parser.h"

/* Temps en ms écoulé entre deux états de la FSM des cmd */
#define CMD_TIMER 10


#define BLINK_DELAY_MS 1000

// int main (void)
// {
//  /* set pin 5 of PORTB for output*/
//  DDRB |= _BV(DDB5);
//  while(1)
//  {
//   /* set pin 5 high to turn led on */
//   PORTB |= _BV(PORTB5);
//   _delay_ms(BLINK_DELAY_MS);
//
//   /* set pin 5 low to turn led off */
//   PORTB &= ~_BV(PORTB5);
//   _delay_ms(BLINK_DELAY_MS);
//  }
// }

byte cmd_timer = 0;

int main (void)
{
  OUT_M1 commands = {Epreuve_non, // Numéro Epreuve
                      Mouvement_non,  // Etat mouvement
                      0,  //  Vitesse
                      0,  // Coord_X
                      0,  // Coord_Y
                      0,  // Angle
                      ACQ_non,  // Commande d'acquisition du son
                      0,  // Durée d'acquisition
                      DCT_non,  // Commande de détection d'obstacle
                      0,  // Résolution angulaire de la détection d'obstacle
                      Lumiere_non,  // Commande d'allumage du pointeur lumineux
                      0,  //Intensitée lumineuse
                      0,  // Durée d'allumage
                      0,  // Durée d'extinction
                      0,  // Nombre de cycles d'allumage
                      Servo_non,  // Commande de position du servo
                      0,  // Paramètre angle
                      Energie_non,  // Commande relevée courant
                      Position_non, // Commande de gestion de position
                      0,  // Coord X de position
                      0,  // Coord Y de position
                      0,  // Angle
                      Photo_non,  // Commande de prise de phot
                      0,  // Temps entre deux photo
                      0 // Nbr de photo
                      };
  PARSER_RESULT parser_result = {0, &commands};
  char* str;

  init_parser();

  while(1)
  {
    //parser_process(state, &parser_result);
    cmd_parser_next_state()->state_process(&parser_result);
    /*if((int)(parser_result.commands->Etat_Epreuve) == 1)
    {
      USART_print("Epreuve 1 !!!");
    }*/
    //USART_send(&c);
    //printf("Commande lu : %u", (int)(parser_result.commands->Etat_Epreuve));
  }
}

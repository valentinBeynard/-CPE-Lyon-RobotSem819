#include "sound_handler.h"
#include <string.h>


SOUND_HANDLER_STATE sd_gen_current_state = SD_GEN_IDLE;

/* Buffer principal dans lesquel est stocké tout caractère reçu sur la liaison UART du µP */
byte raw_data[SD_GEN_BUFFER_SIZE];

/* Pointer permettant de parcourir le Buffer Principal */
byte sd_gen_buffer_index = 0;

/**
  8051 µP :
  Initialize devices for commands_parser : UART0 and Timer 2
	UART0 = TX0 -> P0.0 | RX0 -> P0.1
**/
byte Init_uart0()
{
	
	/****** INIT UART0 *****/
	SM00 = 0;
	SM10 = 1;
	REN0 = 1;		
		
	/****** INIT PIN *****/
	P0MDOUT |= 0x03;
	
	/****** Init UART0 on Crossbar *****/
	XBR0 |= 0x04;

	
	// Réglage timer 2
	T2CON = 0x0D; // Capture Mode & Counter Mode & Enable & External Trig enable
	RCLK0 = 1;
	TCLK0 = 1;
	RCAP2L = 0xDC;
	RCAP2H = 0xFF;
	TR2 = 1;
	
  return 0;
}

void UART0_receive(byte* read_byte)
{
  if(RI0 == 1)
	{
		*read_byte = SBUF0;
		RI0 = 0;
	}else{
    *read_byte = '*';
  }
}

void UART0_send(byte ch)
{
	SBUF0 = ch;
	while(TI0 == 0){}
	TI0 = 0;
}

void UART0_print(char* str)
{
  byte i = 0;
	for(i = 0 ; i < strlen(str); i++)
  {
    UART0_send(*(str+i));
  }
}

/*
#############################################################################
        Core Code of sound_handler.c
#############################################################################
*/

void sound_handler_process(OUT_M2* cmd)
{
  switch(sd_gen_current_state)
	{
		case SD_GEN_IDLE:
			sd_gen_idle(cmd);
			break;
		case SD_ACQ_SEND_CMD:
			acq_send(cmd);
			break;
		default:
			sd_gen_send(cmd);
		break;
		
	}
	//wait(result);
}

void sd_gen_idle(OUT_M2* cmd)
{
	byte read_byte = '*';
	
	if(cmd->Etat_GEN_Son == GEN_oui || cmd->Etat_ACQ_Son == ACQ_oui)
	{
		(sd_gen_current_state = (cmd->Etat_GEN_Son == GEN_oui) ? SD_GEN_SEND_CMD : SD_ACQ_SEND_CMD);
	}
	else
	{
		// Scrutation sur l'UART0
		/*UART0_receive(&read_byte);

		// Si on lit un caractère
		if(read_byte != '*'){
				if(read_byte == STOP_BYTE)
				{
					// On passe à l'Etat 'Get Command' après avoir terminé de préparer le Buffer
					raw_data[sd_gen_buffer_index] = read_byte;
					sd_gen_buffer_index++;
					// Tjrs ajouter un '\0' à la fin du buffer, sinon strlen ne marche pas...
					raw_data[sd_gen_buffer_index] = '\0';
					sd_gen_current_state = SD_GEN_PARSE_CMD;
				}else{
					UART0_send(read_byte);
					raw_data[sd_gen_buffer_index] = read_byte;
					sd_gen_buffer_index++;
				}
		}*/
	}
	
}

void sd_gen_send(OUT_M2* cmd)
{
	UART0_send(SD_GEN_GENERATE_CMD);
	UART0_send(cmd->GEN_freq_code);
	UART0_send(cmd->GEN_son_Duree);
	UART0_send(cmd->GEN_silence_Duree);	
	UART0_send(cmd->GEN_nbr_bip);
	UART0_send(STOP_BYTE);

	sd_gen_current_state = SD_GEN_IDLE;
	cmd->Etat_GEN_Son = GEN_non;
}

void acq_send(OUT_M2* cmd)
{

	UART0_send(ACQ_GENERATE_CMD);
	UART0_send(STOP_BYTE);

	sd_gen_current_state = SD_GEN_IDLE;
	cmd->Etat_ACQ_Son = ACQ_non;
}

void sd_gen_parse(OUT_M2* cmd)
{
	if(strcmp(raw_data, "SD_F") == 0)
	{
		cmd->Etat_GEN_Son = GEN_non;
	}
	else if(strcmp(raw_data, "ASS_F") == 0)
	{
		cmd->Etat_ACQ_Son = ACQ_non;
	}
	else
	{
		
	}
	
	memset(raw_data, 0, sd_gen_buffer_index);
	sd_gen_buffer_index = 0;
	sd_gen_current_state = SD_GEN_IDLE;
}
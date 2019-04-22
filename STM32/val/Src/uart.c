#include "uart.h"


unsigned char raw_data[UART_BUFFER_SIZE];

uint16_t raw_data_buffer_ptr = 0;

unsigned char cmd_received = 0;

uint8_t Rx_byte = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	
	if(cmd_received == 0)
	{
		if(huart->Instance == USART1)
		{
			// Echo
			HAL_UART_Transmit(huart, &Rx_byte, 1, 100);
			
			if(Rx_byte != STOP_BYTE)
			{
				raw_data[raw_data_buffer_ptr] = Rx_byte;
				raw_data_buffer_ptr++;
			}
			else
			{
				cmd_received = 1;
			}	
			// Reactivate receive on Interrupt on 1 byte
			HAL_UART_Receive_IT(huart, &Rx_byte, 1);
		}
	}
	else
	{
		// Reactivate receive on Interrupt on 1 byte
		HAL_UART_Receive_IT(huart, &Rx_byte, 1);
	}	
}

void uart_init(UART_HandleTypeDef *huart)
{
	// Activate UART Receive Interrupt each 1 byte received
	HAL_UART_Receive_IT(huart, &Rx_byte, 1);
}

void uart_cmd_process(CMD_PCK * cmd)
{
	if(cmd_received)
	{
		switch(raw_data[0])
		{
			case SD_GEN_GENERATE_CMD:
				cmd->Etat_GEN = GEN_oui;
				/*cmd->frequency_code = raw_data[1];
				cmd->delay_ON = raw_data[2];
				cmd->delay_OFF = raw_data[3];
				cmd->nbr_sound = raw_data[4];*/
				break;
		}
		
		clear_raw_buffer();
		cmd_received = 0;
	}
	
}

void clear_raw_buffer()
{
	byte i = 0;
	
	for(i = 0 ; i < UART_BUFFER_SIZE; i++)
	{
		raw_data[i] = 0;
	}
}
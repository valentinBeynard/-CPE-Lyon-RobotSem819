#include "uart.h"

unsigned char raw_data[UART_BUFFER_SIZE];

uint16_t raw_data_buffer_ptr = 0;

unsigned char cmd_received = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t c = 0;
	
	if(cmd_received == 0)
	{
		if(huart->Instance == USART1)
		{
			HAL_UART_Receive_IT(huart, &c, 1);
			
			if(c != STOP_BYTE)
			{
				raw_data[raw_data_buffer_ptr] = c;
				raw_data_buffer_ptr++;
			}
			else
			{
				cmd_received = 1;
			}	
		}
	}
	else
	{
		HAL_UART_Receive_IT(huart, &c, 1);
	}	
}

void uart_cmd_process(CMD_PCK * cmd)
{
	if(cmd_received)
	{
		switch(raw_data[0])
		{
			
		}
		
	}
	
	
}
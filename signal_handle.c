#include "signal_handle.h"
static UART_HandleTypeDef* g_huart;
static uint8_t rx_byte;
static char rx_buffer[32];
static uint8_t rx_index = 0;
static char last_command[32];
static uint8_t command_ready = 0;

void Uart_control_init(UART_HandleTypeDef *huart)
{
	g_huart = huart;
	HAL_UART_Receive_IT(g_huart,&rx_byte,1);
};
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == g_huart->Instance){
		if (rx_byte == '\n' || rx_byte == '\r')
		{
			if (rx_index > 0 )
			{
				rx_buffer[rx_index]='\0';
				strcpy(last_command,rx_buffer);
				command_ready =1;
				rx_index=0;
			}
		}
		else{
			if (rx_index<31)
			{
				rx_buffer[rx_index++]=(char)rx_byte;
				
			}
		}
		HAL_UART_Receive_IT(g_huart, &rx_byte, 1);
	}
	
}
void uart_control_handle(remote_control *rc){
	if(command_ready)
	{
		char type = last_command[0];
		float value = atof(&last_command[1]);
		if(type == 'T'||type =='t')
		{
			rc->throttle=(int)value;
		}
		if(type == 'P'||type =='p')
		{
			rc->pitch=(int)value;
		}
		if(type == 'R'||type =='r')
		{
			rc->roll=(int)value;
		}
		
		rc->is_new_data = 1;
		command_ready=0;
	}
}

#include "include.h"



void HAL_AT_send_cmd(uint8_t* str, int str_len)
{
	USART3_SendArray(str, str_len);
}

void HAL_AT_recv(char* data, int timeout)
{
	USART3_Recv(data, timeout);
}

#include "include.h"
#include "hal_at.h"
#include "USART.h"


void HAL_AT_send_cmd(uint8_t* str, int str_len)
{
	USART3_SendArray(str, str_len);
}

void HAL_AT_recv(char *buf, int timeout)
{
	USART3_Recv(buf, timeout);
}

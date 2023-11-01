#include "include.h"



void HAL_AT_send_cmd(uint8_t* str, int str_len)
{
	USART3_SendArray(str, str_len);
}

#include "include.h"
#include "hal_at.h"
#include "USART.h"


void HAL_AT_send_cmd(uint8_t* str, int str_len)
{
	USART3_SendArray(str, str_len);
}

void HAL_AT_recv(uint8_t *buf, int timeout)
{
	USART3_Recv(buf, timeout);
}

int HAL_AT_read_buf(esp8266_obj_t       device, int offset, void* data_buf, int size)
{
	int ret; 
	ret = USART3_Recv_buf(device, offset, data_buf, size);
	return ret;
}


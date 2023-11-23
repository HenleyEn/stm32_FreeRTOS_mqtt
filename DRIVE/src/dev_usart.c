#include "include.h"
#include "dev_usart.h"
#include "ESP8266_AT.h"
#include "platform_mutex.h"
#include "ringbuf.h"

extern struct esp8266_obj esp8266_dev;

void USART3_SendByte(uint8_t Byte)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, Byte);
}

void USART3_SendArray(uint8_t arr[], uint16_t length)
{
	uint16_t i;
	for(i=0; i<length; i++)
	{
		USART3_SendByte(arr[i]);
//		ringbuf_write(&USART3_RingBuf, arr[i]);
	}
}

struct ringbuf test_buf;
int len = 0;
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
			// ringbuf_write(esp8266_dev.data_buffer, USART_ReceiveData(USART3));
			esp8266_dev.resp->buf[len++] = USART_ReceiveData(USART3);

			ringbuf_write(&test_buf, USART_ReceiveData(USART3));
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
//			platform_mutex_unlock_from_isr(esp8266_dev.mutex);
			platform_semphr_unlock_from_isr(esp8266_dev.rx_semphr);
			
	}
}

platform_mutex_t USART3_recv_mutex;

void USART3_Recv( uint8_t *buf, int timeout)
{
	while(1)
	{
		if(TRUE == ringbuf_read(buf, esp8266_dev.data_buffer))
		{
			return;
		}
		else
		{
			platform_mutex_lock_timeout(esp8266_dev.mutex, timeout);
		}
	}
}

int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size)
{

	int str_len;
	configASSERT(device);
	
	if(data_buf == NULL)
	{
		return NULL;
	}

	str_len = strlen(device->resp_buf);
	
	if(str_len < size)
	{
		return NULL;
	}
	else
	{
		strncpy(data_buf, device->resp_buf + offset, size);
	}
	
	return 1;
}

static void fifo_lock(void)
{
    taskENTER_CRITICAL();
}

/* fifo解锁函数 */
static void fifo_unlock(void)
{
    taskEXIT_CRITICAL();
}

void uart_fifo_init(void)
{
    fifo_create();
}

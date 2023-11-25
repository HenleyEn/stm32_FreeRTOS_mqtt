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
/*
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
*/

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


static void fifo_unlock(void)
{
    taskEXIT_CRITICAL();
}

int uart_fifo_init(dev_uart_t *dev)
{
	if(dev == NULL)
	{
		return NULL;
	}

    fifo_create(&(dev->rx_fifo), dev->rx_buf, dev->rx_buf_size, fifo_lock, fifo_unlock);
	fifo_create(&(dev->tx_fifo), dev->tx_buf, dev->tx_buf_size, fifo_lock, fifo_unlock);

	return 1;
}

/**
 * @brief uart read fifo
 * 
 * @param dev device uart
 * @param buf save fifo buffer addr
 * @param size read size
 * @return uint32_t 
 */
uint32_t uart_read(dev_uart_t *dev, uint8_t *buf, uint32_t size)
{
	return fifo_read(&(dev->rx_fifo), buf, size);
}

/**
 * @brief uart write fifo
 * 
 * @param dev device uart
 * @param buf need write buf
 * @param size write buf size
 * @return uint32_t 
 */
uint32_t uart_write(dev_uart_t *dev, uint8_t *buf, uint32_t size)
{
	return fifo_write(&(dev->tx_fifo), buf, size);
}

/**
 * @brief dma finish recevice buffer isr
 * 
 * @param dev 
 */
void uart_dma_rx_done_isr(dev_uart_t *dev)
{
	uint32_t recv_size;

	recv_size = dev->rx_buf_size - dev->last_dma_rx_size;

	fifo_write(&(dev->rx_fifo), &(dev->rx_buf[dev->last_dma_rx_size]), recv_size);
	dev->last_dma_rx_size = 0;
}

#include "include.h"
#include "dev_usart.h"
#include "bsp_usart_config.h"
//#include "ESP8266_AT.h"
#include "platform_mutex.h"
#include "ringbuf.h"

extern struct esp8266_obj esp8266_dev;

static fifo_t dma_rx_fifo;
static fifo_t dma_tx_fifo;

#ifdef USE_USART3_DMA

#define USART_RX_BUF_SIZE			128
#define USART_TX_BUF_SIZE			64
#define USART_DMA_RX_BUF_SIZE		256
#define USART_DMA_TX_BUF_SIZE		64

static uint8_t uart_rx_buf[USART_RX_BUF_SIZE];
static uint8_t uart_tx_buf[USART_TX_BUF_SIZE];
static uint8_t uart_dma_rx_buf[USART_DMA_RX_BUF_SIZE];
static uint8_t uart_dma_tx_buf[USART_DMA_TX_BUF_SIZE];
static platform_mutex_t dma_mutex;
#endif
struct _dev_uart uart3_dev =
{
	.uart_status = UART_INIT,
	.rx_fifo = &dma_rx_fifo,
	.tx_fifo = &dma_tx_fifo,
	
#ifdef USE_USART3_DMA
	.DMAy_Channelx = DMA1_Channel3,

    .dma_tx_buf = uart_tx_buf,
    .dma_tx_buf_size = USART_TX_BUF_SIZE,
    .dma_rx_buf = uart_rx_buf,
    .dma_rx_buf_size = USART_RX_BUF_SIZE,
	
    .last_dma_rx_size = 0,
    .dma_status = DMA_INIT,
    
#endif
};

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
	}
}




// void USART3_IRQHandler(void)
// {
// 	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
// 	{
// 			// ringbuf_write(esp8266_dev.data_buffer, USART_ReceiveData(USART3));
// 			esp8266_dev.resp->buf[len++] = USART_ReceiveData(USART3);

// 			ringbuf_write(&test_buf, USART_ReceiveData(USART3));
// 			USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
// //			platform_mutex_unlock_from_isr(esp8266_dev.mutex);
// 			platform_semphr_unlock_from_isr(esp8266_dev.rx_semphr);
			
// 	}
// }


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
			platform_mutex_lock_timeout(&esp8266_dev.mutex, timeout);
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


//extern uint8_t qwer[256];
static uint8_t qwer[256];
int uart_device_init(dev_uart_t *dev)
{
	if(dev == NULL)
	{
		return NULL;
	}
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART1_Config(115200);
	USART3_Config(115200);

    fifo_create((dev->rx_fifo), uart_rx_buf, sizeof(uart_rx_buf), fifo_lock, fifo_unlock);
	fifo_create((dev->tx_fifo), uart_tx_buf, sizeof(uart_tx_buf), fifo_lock, fifo_unlock);

	platform_mutex_init(&dev->dma_mutex);
	
	usart3_rx_DMA_config((uint32_t)(dev->dma_rx_buf), dev->dma_rx_buf_size); 
	
	return 1;
}

uint16_t get_dma_remain_cnt(DMA_Channel_TypeDef* DMAy_Channelx)
{
	uint16_t cnt;
	cnt = DMA_GetCurrDataCounter(DMAy_Channelx);
	return cnt;
}

uint16_t get_dma_recv_cnt(DMA_Channel_TypeDef* DMAy_Channelx)
{
	uint16_t cnt;
	cnt = USART_RX_BUF_SIZE - get_dma_remain_cnt(DMAy_Channelx);
	return cnt;
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
	return fifo_read((dev->rx_fifo), buf, size);
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
	return fifo_write((dev->tx_fifo), buf, size);
}

/**
 * @brief dma finish recevice buffer isr
 * 
 * @param dev 
 */
void uart_dma_rx_done_isr(dev_uart_t *dev)
{
	uint32_t recv_size;
	dev->uart_status = RECV_STATUS;
	recv_size = dev->dma_rx_buf_size - dev->last_dma_rx_size;

	fifo_write((dev->rx_fifo), (const uint8_t *)&(dev->dma_rx_buf[dev->last_dma_rx_size]), recv_size);
	dev->last_dma_rx_size = 0;
}

fifo_t test_fifo;
uint8_t recv_buffer_test[256];
void uart_dmarx_idle_isr(dev_uart_t *dev)
{
  	uint16_t recv_total_size;
  	uint16_t recv_size;
	

	recv_total_size = dev->dma_rx_buf_size - get_dma_remain_cnt(dev->DMAy_Channelx);

	recv_size = recv_total_size - dev->last_dma_rx_size;
	
//	s_UartTxRxCount[uart_id*2+1] += recv_size;

//	fifo_write(&(dev->rx_fifo), 
//				   (const uint8_t *)&(dev->dma_rx_buf[dev->last_dma_rx_size]), recv_size);
				   
	fifo_write(&test_fifo, (const uint8_t *)&qwer, recv_size);
	dev->last_dma_rx_size = recv_total_size;
}

void uart_dmarx_half_done_isr(dev_uart_t *dev)
{
  	uint16_t recv_total_size;
  	uint16_t recv_size;
	

	recv_total_size = dev->dma_rx_buf_size - get_dma_remain_cnt(dev->DMAy_Channelx);

	recv_size = recv_total_size - dev->last_dma_rx_size;
	
	fifo_write((dev->rx_fifo), 
				   (const uint8_t *)&(dev->dma_rx_buf[dev->last_dma_rx_size]), recv_size);
				   
	dev->last_dma_rx_size = recv_total_size;
}

int uart_dma_operate(dev_uart_t *dev)
{
	if(dev == NULL)
	{
		return ERROR;
	}

	uint8_t status = dev->uart_status;
	
	platform_mutex_lock(&dev->dma_mutex);
	
	switch(status)
	{
		case DMA_BUF_FULL:
			uart_dma_rx_done_isr(dev);
			break;
		case DMA_BUF_HAIF:
			uart_dmarx_half_done_isr(dev);
			break;
		case DMA_UART_IDLE:
			uart_dmarx_idle_isr(dev);
			break;
		default:
			break;
	}
	
//	platform_mutex_unlock(&dev->dma_mutex);
	
	return OK;
}

void uart_dma_task(dev_uart_t *dev)
{
	TickType_t waiting_time = 3;
	
	uart_device_init(dev);
	
	while(1)
	{
		
		uart_dma_operate(dev);
		vTaskDelay(waiting_time);
	}
	 
}


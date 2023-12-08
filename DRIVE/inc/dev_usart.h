#ifndef __DEV_USART_H
#define __DEV_USART_H

#include "ESP8266_AT.h"
#include "fifo.h"
#include "platform_mutex.h"

#define USART_DMA
#ifdef __cplusplus
extern "C" {
#endif

#define ERROR 	0
#define OK		1

#define RECV_STATUS 0x01
#define SEND_STATUS 0x02
#define IDLE_STATUS 0x04
#define UART_INIT   0x08

#define DMA_BUF_FULL 		0x01
#define DMA_BUF_HAIF		0x02
#define DMA_UART_IDLE		0x04
#define DMA_INIT			0x00

typedef struct _dev_uart dev_uart_t;

struct _dev_uart
{
    uint8_t uart_status;
    fifo_t *rx_fifo;
    fifo_t *tx_fifo;
	
#ifdef USART_DMA
	DMA_Channel_TypeDef *DMAy_Channelx;

    uint8_t *dma_tx_buf;
    uint32_t dma_tx_buf_size;
    uint8_t *dma_rx_buf;
    uint32_t dma_rx_buf_size;
	
    uint16_t last_dma_rx_size;
	
    platform_mutex_t dma_mutex;
    uint8_t dma_status;
#endif
    // uint8_t set_rx_buf;
};


extern struct _dev_uart uart3_dev;

void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t arr[], uint16_t length);
void USART3_Recv( uint8_t *buf, int timeout);
int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size);

int uart_device_init(dev_uart_t *dev);
uint16_t get_dma_remain_cnt(DMA_Channel_TypeDef* DMAy_Channelx);
uint16_t get_dma_recv_cnt(DMA_Channel_TypeDef* DMAy_Channelx);
void uart_dma_rx_done_isr(dev_uart_t *dev);
void uart_dmarx_half_done_isr(dev_uart_t *dev);
void uart_dmarx_idle_isr(dev_uart_t *dev);
int uart_dma_operate(dev_uart_t *dev);

void uart_dma_task(dev_uart_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* __DEV_USART_H */

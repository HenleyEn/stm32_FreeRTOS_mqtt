#ifndef __DEV_USART_H
#define __DEV_USART_H

#include "ESP8266_AT.h"
#include "fifo.h"
#include "platform_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RECV_STATUS 0x01
#define SEND_STATUS 0x02
#define IDLE_STATUS 0x04
#define UART_INIT   0x08

struct _dev_uart
{
    uint8_t uart_status;
    fifo_t rx_fifo;
    fifo_t tx_fifo;

    uint8_t *dma_tx_buf;
    uint32_t dma_tx_buf_size;
    uint8_t *dma_rx_buf;
    uint32_t dma_rx_buf_size;

    uint16_t last_dma_rx_size;
    platform_mutex_t dma_mutex;
};

typedef struct _dev_uart dev_uart_t;

extern struct _dev_uart uart3_dev;

void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t arr[], uint16_t length);
void USART3_Recv( uint8_t *buf, int timeout);
int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size);

int uart_device_init(dev_uart_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* __DEV_USART_H */

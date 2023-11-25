#ifndef __DEV_USART_H
#define __DEV_USART_H

#include "ESP8266_AT.h"
#include "fifo.h"
#ifdef __cplusplus
extern "C" {
#endif

struct _dev_uart
{
    uint8_t uart_status;
    fifo_t rx_fifo;
    fifo_t tx_fifo;

    uint8_t *tx_buf;
    uint32_t tx_buf_size;
    uint8_t *rx_buf;
    uint32_t rx_buf_size;

    uint16_t last_dma_rx_size;
};

typedef struct _dev_uart dev_uart_t;

void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t arr[], uint16_t length);
void USART3_Recv( uint8_t *buf, int timeout);
int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size);


#ifdef __cplusplus
}
#endif

#endif /* __DEV_USART_H */

#ifndef __USART_H
#define __USART_H
#include "ESP8266_AT.h"


void USART1_Config(uint32_t BaudRate);
void USART3_Config(uint32_t BaudRate);

void usart3_rx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size);
void usart3_tx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size);

void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t arr[], uint16_t length);
void USART3_Recv( uint8_t *buf, int timeout);

int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size);


#endif

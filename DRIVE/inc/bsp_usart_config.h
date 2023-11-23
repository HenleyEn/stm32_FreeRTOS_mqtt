#ifndef __BSP_USART_CONFIG_H
#define __BSP_USART_CONFIG_H
#include "ESP8266_AT.h"

#ifdef __cplusplus
extern "C" {
#endif

void USART1_Config(uint32_t BaudRate);
void USART3_Config(uint32_t BaudRate);

void usart3_rx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size);
void usart3_tx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size);


#ifdef __cplusplus
}
#endif

#endif/* __BSP_USART_CONFIG_H */

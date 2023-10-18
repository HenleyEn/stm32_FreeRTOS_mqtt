#ifndef __USART_H
#define __USART_H
#include "include.h"

void USART1_Config(uint32_t BaudRate);
void USART3_Config(uint32_t BaudRate);
void USART3_SendByte(uint8_t Byte);
void USART_SendArray(uint8_t arr[], uint16_t length);

#endif

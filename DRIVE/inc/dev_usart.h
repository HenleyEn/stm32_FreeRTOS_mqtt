#ifndef __DEV_USART_H
#define __DEV_USART_H

#ifdef __cplusplus
extern "C" {
#endif

void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t arr[], uint16_t length);
void USART3_Recv( uint8_t *buf, int timeout);
int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size);


#ifdef __cplusplus
}
#endif

#endif /* __DEV_USART_H */

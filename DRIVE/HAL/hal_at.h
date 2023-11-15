#ifndef HAL_AT_H
#define HAL_AT_H
#include "ESP8266_AT.h"

void HAL_AT_send_cmd(uint8_t* str, int str_len);
void HAL_AT_recv(uint8_t* data, int timeout);
int HAL_AT_read_buf(esp8266_obj_t device, int offset, void* data_buf, int size);

#endif

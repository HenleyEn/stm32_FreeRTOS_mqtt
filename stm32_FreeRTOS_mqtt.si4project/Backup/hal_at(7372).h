#ifndef HAL_AT_H
#define HAL_AT_H

void HAL_AT_send_cmd(uint8_t* str, int str_len);
void HAL_AT_recv(uint8_t* data, int timeout);

#endif

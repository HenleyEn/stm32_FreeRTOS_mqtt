#ifndef ESP8266_AT_H
#define ESP8266_AT_H

#include "include.h"

typedef enum
{
	AT_TIMEOUT = -1,
	AT_OK = 0,
	AT_ERROR =1,
	AT_INIT = 2,
}AT_Status;

typedef struct
{
	uint8_t *wifi_ssid;
	uint8_t *wifi_password;
}wifi_info_t;

typedef struct
{
	uint8_t *device_name;

	wifi_info_t* wifi_info;
	ringbuf_t* cmd_buffer;
	ringbuf_t* data_buffer;
	platform_mutex_t* mutex;
	AT_Status status;
	
}esp8266_obj;

int esp8266_init(esp8266_obj* device, uint8_t* device_name);
int esp8266_destory(esp8266_obj* device);
int AT_send_cmd(esp8266_obj* device, uint8_t *str, int str_len, int timeout);
int AT_recv(void);
int AT_ack(void);

#endif

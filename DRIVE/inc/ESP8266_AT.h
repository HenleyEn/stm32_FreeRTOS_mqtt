#ifndef ESP8266_AT
#define ESP8266_AT

#include "include.h"

typedef struct
{
	
	uint8_t *wifi_ssid;
	uint8_t *wifi_password;
}wifi_info_t;

typedef struct
{
	uint8_t *device_name;
//	char *client_name;

	wifi_info_t* wifi_info;

//	uint8_t *wifi_ssid;
//	uint8_t *wifi_password;

	ringbuf_t* cmd_buffer;
	ringbuf_t* data_buffer;
	platform_mutex* mutex;
	
}esp8266_obj;

int esp8266_init(esp8266_obj* device, uint8_t* device_name);
int esp8266_destory(esp8266_obj* device);
int AT_send_cmd(uint8_t *str, int str_len, uint8_t timeout);
int AT_recv(void);
int AT_ack(void);

#endif

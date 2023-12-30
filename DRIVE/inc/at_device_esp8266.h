#ifndef _AT_DEVICE_ESP8266_H
#define _AT_DEVICE_ESP8266_H
#include "include.h"
#include "dev_usart.h"
#include "at_client.h"

#ifdef __cplusplus
extern "C" {
#endif
extern struct at_device_esp8266 esp8266_dev;

typedef struct at_device_esp8266 at_device_esp8266_t;
struct at_device_esp8266
{
	/* devicre name */
	const char *device_name;

	uint8_t *wifi_ssid;
	uint8_t *wifi_password;
	
	struct at_client *device;
	dev_uart_t *uart_device;

	int recv_line_num;
	void *users_data;
};

int at_device_esp8266_init(struct at_device_esp8266 *esp8266_devide);

void at_test(void* param);

#ifdef __cplusplus
}
#endif

#endif /* _AT_DEVICE_ESP8266_H */

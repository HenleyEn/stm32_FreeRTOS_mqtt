#include "include.h"


#if defined	ESP8266_USING


#define AT_ACK_OK	"OK"
#define AT_ACK_ERR	"ERROR"

#define WIFI_SSID	"1"
#define WIFI_PASSWORD	"1"

/*
static const struct at_cmd cmd_table[] =
{
    {"SEND OK",          "\r\n",           urc_send_func},
    {"SEND FAIL",        "\r\n",           urc_send_func},
    {"Recv",             "bytes\r\n",      urc_send_bfsz_func},
    {"",                 ",CLOSED\r\n",    urc_close_func},
    {"+IPD",             ":",              urc_recv_func},
};
*/

esp8266_obj dev_esp8266;

/*
ringbuf_t cmd_buf;
ringbuf_t data_buf;
platform_mutex frame_mutex;

esp8266_obj dev_esp8266 = {
	.wifi_info_t->wifi_ssid 	= WIFI_SSID;
	.wifi_info_t->wifi_password = WIFI_PASSWORD;
	.cmd_buffer 				= &cmd_buf;
	.data_buffer 				= &data_buf;
	.mutex 						= &frame_mutex;
};


int set_wifi_info(esp8266_obj* device, uint8_t* ssid, uint8_t* password)
{
	if(device != NULL)
	{
		device->wifi_info->wifi_ssid = ssid;
		device->wifi_info->wifi_password = password;
		return TRUE;
	}

	return FALSE;
}
*/
int esp8266_init(esp8266_obj* device, uint8_t* device_name)
{
	if(device != NULL)
	{
		if(device_name != NULL)
		{
			device->device_name = device_name;
			device->cmd_buffer = (ringbuf_t*)pvPortMalloc(sizeof(ringbuf_t));
			device->data_buffer = (ringbuf_t*)pvPortMalloc(sizeof(ringbuf_t));
			device->wifi_info = (wifi_info_t*)pvPortMalloc(sizeof(wifi_info_t));
			device->mutex =(platform_mutex*)pvPortMalloc(sizeof(platform_mutex));

			ringbuf_init(device->cmd_buffer);
			ringbuf_init(device->cmd_buffer);

			platform_mutex_init(device->mutex);
			return TRUE;	
		}
		return FALSE;
	}

	return FALSE;
}
int esp8266_destory(esp8266_obj* device)
{
	if(device != NULL)
	{
		vPortFree(device->cmd_buffer);
		vPortFree(device->data_buffer);
		vPortFree(device->wifi_info);
		vPortFree(device->mutex);

		vPortFree(device);
		return TRUE;
	}

	return FALSE;
}

int AT_send_cmd(uint8_t *str, int str_len, uint8_t timeout)
{
	
	USART3_SendArray(str, str_len);
	USART3_SendArray("\r\n", 2);
	return TRUE;
	
}



#endif


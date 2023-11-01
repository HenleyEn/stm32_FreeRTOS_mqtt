#include "include.h"


#if defined	ESP8266_USING


#define AT_ACK_OK	"OK"
#define AT_ACK_ERR	"ERROR"

#define WIFI_SSID	"1"
#define WIFI_PASSWORD	"1"


esp8266_obj dev_esp8266;

/*
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

static AT_Status set_at_status(esp8266_obj* device, AT_Status status)
{
	if(device != NULL)
	{
		device->status = status;
		return status;
	}
	return AT_ERROR;
}

static AT_Status get_at_status(esp8266_obj* device)
{
	if(device != NULL)
	{
		return device->status;
	}
	return AT_ERROR;
}

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
			device->mutex = (platform_mutex_t*)pvPortMalloc(sizeof(platform_mutex_t));
			
			ringbuf_init(device->cmd_buffer);
			ringbuf_init(device->cmd_buffer);

			platform_mutex_init(device->mutex);
			platform_mutex_lock(device->mutex);//mutex = 0,then it can block and wait
			device->status = AT_INIT;
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


int AT_send_cmd(esp8266_obj* device, uint8_t *str, int str_len, int timeout)
{
	int ret;
	AT_Status status;

	status = get_at_status(device);
	
	HAL_AT_send_cmd(str, str_len);
	HAL_AT_send_cmd("\r\n", 2);

	/*
	 *	status:
	 *			0: OK
	 *			1: ERROR
	 *			-1: TIMEOUT
	 */
	ret = platform_mutex_lock_timeout(device->mutex, timeout);
	if(ret)
	{
		if(status == AT_OK)
		{
			
		}
		else if(status == AT_ERROR)
		{
			
		}
		else if(status == AT_TIMEOUT)
		{
			
		}
		else
		{
			
		}
	}
	return TRUE;
	
}



#endif


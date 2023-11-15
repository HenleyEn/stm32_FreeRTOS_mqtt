#include "include.h"


#if defined	ESP8266_USING


#define AT_OK	0
#define AT_ERR	1

#define WIFI_SSID	"1"
#define WIFI_PASSWORD	"1"


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
	return AT_STATUS_ERROR;
}

static AT_Status get_at_status(esp8266_obj* device)
{
	if(device != NULL)
	{
		return device->status;
	}
	return AT_STATUS_ERROR;
}

int esp8266_init(esp8266_obj* device, uint8_t* device_name)
{
	if(device != NULL)
	{
		if(device_name != NULL)
		{
			device->device_name = device_name;
			device->cmd_buffer = (at_response*)pvPortMalloc(sizeof(at_response));
			device->data_buffer = (ringbuf_t*)pvPortMalloc(sizeof(ringbuf_t));
			device->wifi_info = (wifi_info_t*)pvPortMalloc(sizeof(wifi_info_t));
			device->mutex = (platform_mutex_t*)pvPortMalloc(sizeof(platform_mutex_t));
			
			ringbuf_init(device->data_buffer);

			platform_mutex_init(device->mutex);
			platform_mutex_lock(device->mutex);//mutex = 0,then it can block and wait
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
	if(ret && status != AT_STATUS_INIT)
	{
		if(status == AT_STATUS_OK)
		{
			
		}
		else if(status == AT_STATUS_ERROR)
		{
			
		}
		else if(status == AT_STATUS_TIMEOUT)
		{
			
		}
		else
		{
		}
	}
	return TRUE;
	
}


void at_init(esp8266_obj* device)
{
	esp8266_init(device, "ESP8266");
	
	HAL_AT_send_cmd("AT+RST\r\n", 8);
	
}
at_response_t at_create_resp(int buf_size, int line_num, int timeout)
{
	at_response_t resp;

	resp = (at_response_t)pvPortCalloc(1, sizeof(at_response));

	if(resp == NULL)
	{
		return NULL;
	}

	resp->buf = (char*)pvPortCalloc(1, buf_size);

	if(resp->buf == NULL)
	{
		vPortFree(resp);
		return NULL;
	}

	resp->buf_size = buf_size;
	resp->line_num = line_num;
	resp->line_counts = 0;
	resp->timeout = timeout;

	return resp;
}

void at_delete_resp(at_response_t resp)
{
	if (resp && resp->buf)
	{
		vPortFree(resp->buf);
	}

	if (resp)
	{
		vPortFree(resp);
		resp = NULL;
	}
}

int AT_send_obj_cmd(esp8266_obj* device, at_response_t resp, uint8_t *cmd)
{
	int ret = AT_OK;
	
	if(device == NULL)
	{
		return -AT_ERR;
	}


		platform_mutex_lock_timeout(device->mutex, portMAX_DELAY);

		device->status = AT_RESP_OK;
		device->cmd_buffer = resp;
		
		if (resp != NULL)
		{
			resp->buf_len = 0;
			resp->line_counts = 0;
		}

		HAL_AT_send_cmd(cmd, strlen(cmd));
		HAL_AT_send_cmd("\r\n", 2);

		if(resp != NULL)
		{
			if(device->status != AT_RESP_OK)
			{
				ret = -AT_ERR;
			}
		}

		device->cmd_buffer = NULL;

		platform_mutex_unlock(device->mutex);

		return ret;
}


/**
 * Get one line AT response buffer by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 *
 * @return != RT_NULL: response line buffer
 *          = RT_NULL: input response line error
 */

const char* resp_get_line(at_response_t resp, int resp_line)
{
	char* resp_buf = resp->buf;
	char* resp_line_buf = NULL;
	int line_num = 1;

	if(resp == NULL || resp_line > resp->line_counts || resp_line <= 0)
	{
		return NULL;
	}

    for (line_num = 1; line_num <= resp->line_counts; line_num++)
	{
	    if (resp_line == line_num)
	    {
	        resp_line_buf = resp_buf;

	        return resp_line_buf;
	    }

		/* +1 is '\0' */
	    resp_buf += strlen(resp_buf) + 1;
	}

	return NULL;
}

int resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;


    if ((resp_line_buf = resp_get_line(resp, resp_line)) == NULL)
    {
        return -1;
    }

    va_start(args, resp_expr);

    resp_args_num = sscanf(resp_line_buf, resp_expr, args);

    va_end(args);

    return resp_args_num;
}

#define AT_RECV_SIZE	100

void AT_recv_cmd_task(void* param)
{
	char buf[AT_RECV_SIZE];
	int i = 0;

	int ret;
	while(1)
	{
		HAL_AT_recv(&buf[i], portMAX_DELAY);
		
		if((i != 0) && (buf[i-1] == '\r') && (buf[i] == '\n'))
		{
			buf[i+1] = '\0';
			
			if(strstr(buf, "OK\r\n"))
			{
				printf("OK\r\n");
			}
			else
			{
				i++;
			}
		}
		else
		{
			i++;
		}

		if(i >= AT_RECV_SIZE)
		{
			i = 0;
		}
		
		vTaskDelay(3);
	}
}

#endif


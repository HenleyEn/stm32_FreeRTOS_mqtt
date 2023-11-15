#include "include.h"
#include "ESP8266_AT.h"

#if defined		ESP8266_USING


#define AT_OK	0
#define AT_ERR	1

#define WIFI_SSID	"1"
#define WIFI_PASSWORD	"1"

at_response_t at_create_resp(int buf_size, int line_num, int timeout)
{
	at_response_t resp;

	resp = (at_response_t)pvPortCalloc(1, sizeof(struct at_response));

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

void at_target_OK(char* data)
{
	
}
void at_target_ERR(char* data)
{
	
}
void at_target_IPD(char* data)
{
	
}


int at_obj_set_urc_table(esp8266_obj_t device,  struct at_urc *urc_table, int urc_table_size)
{
	int idx;

	if(device == NULL)
	{
		printf("at_obj_set_urc_table is ERROR\r\n");
		return -AT_ERR;
	}
	
	for(idx = 0; idx < urc_table_size; idx++)
	{
		 configASSERT(urc_table[idx].begin_str);
		 configASSERT(urc_table[idx].end_str);
	}

	if(device->urc_table_size == 0)
	{
		device->urc_table = (at_urc_table_t) pvPortCalloc(1, sizeof(struct at_urc_table));
		
		if (device->urc_table == NULL)
		{
			return -AT_ERR;
		}

		device->urc_table[0].urc = urc_table;
		device->urc_table[0].urc_size = urc_table_size;
		device->urc_table_size++;
	}
	else
	{

		struct at_urc_table *old_urc_table = NULL;
		int old_table_size = device->urc_table_size * sizeof(struct at_urc_table);

		old_urc_table = (struct at_urc_table *) pvPortMalloc(old_table_size);
		if (old_urc_table == NULL)
		{
		return -AT_ERR;
		}
		memcpy(old_urc_table, device->urc_table, old_table_size);

		/* realloc urc table space */
		device->urc_table = (struct at_urc_table *) pvPortRealloc(device->urc_table,
		old_table_size + sizeof(struct at_urc_table));
		
		if (device->urc_table == NULL)
		{
			vPortFree(old_urc_table);
			return -AT_ERR;
		}
		
		memcpy(device->urc_table, old_urc_table, old_table_size);
		
		device->urc_table[device->urc_table_size].urc = urc_table;
		device->urc_table[device->urc_table_size].urc_size = urc_table_size;
		device->urc_table_size++;

		vPortFree(old_urc_table);
	}

	return AT_OK;
	
}

static int get_IPD(char	*buf)
{
	if(strstr(buf, "+IPD"))
		return 1;
	else
		return 0;
}

int get_cmd_recv(char *buf)
{
	if(strstr(buf, "OK\r\n"))
		return 0;
	else if(strstr(buf, "ERROR\r\n"))
		return 1;
	else if(strstr(buf, "+IPD"))
		return 2;
	else
		return -AT_ERR;
}

static  struct at_urc urc_table[3] =
{
	{"OK", 		"\r\n", 	at_target_OK},
	{"ERROR", 	"\r\n", 	at_target_ERR},
	{"+IPD", 	"\r\n", 	at_target_IPD},
};

int esp8266_init(esp8266_obj_t device, uint8_t* device_name)
{
	if(device != NULL)
	{
		if(device_name != NULL)
		{
			device->device_name = device_name;
			device->cmd_buffer = (at_response_t)pvPortMalloc(sizeof(struct at_response));
			device->data_buffer = (ptr_ringbuf_t)pvPortMalloc(sizeof(struct ringbuf));
			device->wifi_info = (wifi_info_t)pvPortMalloc(sizeof(struct wifi_info));
			
			device->mutex = (platform_mutex_t*)pvPortMalloc(sizeof(platform_mutex_t));
			
			ringbuf_init(device->data_buffer);

			platform_mutex_init(device->mutex);
			platform_mutex_lock(device->mutex);//mutex = 0,then it can block and wait

			at_obj_set_urc_table(device, urc_table, sizeof(urc_table)/sizeof(urc_table[0]));
			return TRUE;	
		}
		return FALSE;
	}

	return FALSE;
}

int esp8266_destory(esp8266_obj_t device)
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

void at_init(esp8266_obj_t device)
{
	esp8266_init(device, "ESP8266");	
}

int AT_send_obj_cmd(esp8266_obj_t device, at_response_t resp, uint8_t *cmd)
{
	int ret = AT_OK;
	
	if(device == NULL)
	{
		return -AT_ERR;
	}

	device->status = AT_RESP_OK;
	device->cmd_buffer = resp;
	
	if (resp != NULL)
	{
		resp->buf_len = 0;
		resp->line_counts = 0;
	}

	HAL_AT_send_cmd(cmd, strlen(cmd));
	HAL_AT_send_cmd("\r\n", 2);	
	
	platform_mutex_lock_timeout(device->mutex, portMAX_DELAY);

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

#define AT_RECV_SIZE	100

void AT_recv_cmd_task(void* param)
{
	char buf[AT_RECV_SIZE];
	int i = 0;
	
	while(1)
	{
		HAL_AT_recv(&buf[i], portMAX_DELAY);
		if(i != 0)
		{
			buf[i+1] = '\0';
			
			if(strstr(buf, "OK"))
			{
				printf("OK\r\n");
				i = 0;
			}
			else if(get_IPD(buf))
			{
				printf("get IPD\r\n");
				i = 0;
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


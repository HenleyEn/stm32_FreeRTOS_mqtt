#include "include.h"
#include "ESP8266_AT.h"

#if defined		ESP8266_USING


#define AT_OK	0
#define AT_ERR	1
#define AT_PACK_FULL	3

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

#define WIFI_SSID	"1"
#define WIFI_PASSWORD	"1"

/**
 * Create response object.
 *
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *         = 0: the response data will auto return when received 'OK' or 'ERROR'
 *        != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return != RT_NULL: response object
 *          = RT_NULL: no memory
 */
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

/**
 * 	Delete response object 
 * 	@param resp the response object
*/
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
 * @return != NULL: response line buffer
 *          = NULL: input response line error
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

		/* +1 is '\0' , end recvice data end sign is '\0' */
	    resp_buf += strlen(resp_buf) + 1;
	}

	return NULL;
}

/**
 * Get and parse AT response buffer arguments by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 * @param resp_expr response buffer expression
 *
 * @return -1 : input response line number error or get line buffer error
 *          0 : parsed without match
 *         >0 : the number of arguments successfully parsed
 */
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

/**
 * @brief set URC table
 * 
 * @param device  device object
 * @param urc_table urc table 
 * @param urc_table_size table size
 *
 * @return != AT_OK  OK
 *			= -AT_ERR ERROR
 */
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

static void urc_func(struct esp8266_obj *device, char* data, int data_size)
{
	if (strstr(data, "OK"))
	{
		printf("urc OK\r\n");
	}
	else if (strstr(data, "ERROR"))
	{
		printf("urc ERROR\r\n");
	}
}

static struct at_urc urc_table[3] =
{
	{"OK", 		"\r\n", 	urc_func},
	{"ERROR", 	"\r\n", 	urc_func},
	{"+IPD", 	"\r\n", 	urc_func},
};

int esp8266_init(esp8266_obj_t device, uint8_t* device_name)
{
	if(device != NULL)
	{
		if(device_name != NULL)
		{
			device->device_name = device_name;
			
			device->resp = (at_response_t)pvPortMalloc(sizeof(struct at_response));
			device->data_buffer = (ptr_ringbuf_t)pvPortMalloc(sizeof(struct ringbuf));
			
			device->mutex = (platform_mutex_t*)pvPortMalloc(sizeof(platform_mutex_t));
			
			device->rx_semphr = (platform_semaphore_t)pvPortMalloc(sizeof(struct platform_semaphore));
			device->resp_semphr = (platform_semaphore_t)pvPortMalloc(sizeof(struct platform_semaphore));
			
			ringbuf_init(device->data_buffer);
			
			platform_semaphore_init(device->rx_semphr, 100, 0);
			platform_semaphore_init(device->resp_semphr, 100, 0);

			platform_mutex_init(device->mutex);
			platform_mutex_lock(device->mutex);//mutex = 0,then it can block and wait

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
		vPortFree(device->resp);
		vPortFree(device->data_buffer);
		vPortFree(device->mutex);
		vPortFree(device->resp_semphr);
		
		vPortFree(device);
		memset(device, 0x00, sizeof(struct esp8266_obj));
		
		return TRUE;
	}

	return FALSE;
}

static int at_rx_init(esp8266_obj_t device)
{
    if(device != NULL)
    {
		platform_semphr_unlock(device->rx_semphr);
	    return AT_OK;
    }
    return -AT_ERR;
}

static const struct at_urc *get_urc_obj(esp8266_obj_t device)
{
	int i,j;
	int begin_buf_len, end_buf_len;

	int buffer_len_size;
	char *buffer = NULL;

	const struct at_urc *urc = NULL;
	struct at_urc_table *urc_table = NULL;

	if(device->urc_table == NULL)
	{
		printf("urc is NULL\r\n");
		return NULL;
	}

	buffer = device->recv_line_buf;
	buffer_len_size = device->recv_line_len;

	/* set urc*/
	for(i = 0; i < device->urc_table_size; i++)
	{
		for(j = 0; j < device->urc_table[i].urc_size; j++)
		{
			urc_table = device->urc_table + i;
			urc = urc_table->urc + j;

			begin_buf_len = strlen(urc->begin_str);
			end_buf_len = strlen(urc->end_str);

			if(buffer_len_size < (begin_buf_len + end_buf_len))
			{
				continue;
			}

			/* 
			 * compare buffer and return urc 
			 * if strncmp  < 0 or >0, 
			 * !strncmp = 0  
			 * if strncmp = 0;
			 * !strncmp = 1
			 */
			if(begin_buf_len ? !strncmp(buffer, urc->begin_str, begin_buf_len) : 1
				&& end_buf_len ? !strncmp(buffer + buffer_len_size - end_buf_len, urc->end_str, end_buf_len) : 1)
			{
				return urc;
			}
		}
		
	}

	return NULL;
}

static int device_read_buf(esp8266_obj_t device, int offset, void* data_buf, int size)
{
	int ret;
	if(device == NULL)
	{
		return AT_ERR;
	}

	ret = HAL_AT_read_buf(device, offset, data_buf, size);

	return ret;
}

static int at_client_getchar(esp8266_obj_t device, char *ch, int timeout)
{
    int result = AT_OK;

	while(1)
	{
		printf("FuncName:%s  Line:%d\n", __func__, __LINE__);
	    result = platform_semphr_lock(device->rx_semphr);
		if (result != AT_OK)
		{
		    return result;
		}
		if(device_read_buf(device, 0, ch, 1) == 1)
		{
		    return AT_OK;
		}
	}

}


static int at_recv_readline(esp8266_obj_t device)
{
    int read_len = 0;
    char ch = 0, last_ch = 0;
    int is_full = FALSE;

	/* init buf */
    memset(device->recv_line_buf, 0x00, device->recv_buffer_size);
    device->recv_line_len = 0;

	/* get data or get newline */
    while (1)
    {
		printf("FuncName:%s  Line:%d\n", __func__, __LINE__);
        at_client_getchar(device, &ch, portMAX_DELAY);

		
        if (read_len < device->recv_buffer_size)
        {
            device->recv_line_buf[read_len++] = ch;
            device->recv_line_len = read_len;
        }
        else
        {
            is_full = TRUE;
        }

        /* is newline or URC data */
        if ((ch == '\n' && last_ch == '\r') 
        	|| (device->end_sign != 0 && ch == device->end_sign)
            || get_urc_obj(device))
        {
            if (is_full)
            {
                printf("read line failed. The line data length is out of buffer size(%d)!", device->recv_buffer_size);
                memset(device->recv_line_buf, 0x00, device->recv_buffer_size);
                device->recv_line_len = 0;
                return -AT_PACK_FULL;
            }
            break;
        }
        last_ch = ch;
    }

    return read_len;
}

/*
 *	parse recv data.
 *
 */
void client_parser(esp8266_obj_t device)
{
    const struct at_urc *urc;

    while(1)
    {
		printf("FuncName:%s  Line:%d\n", __func__, __LINE__);
        if (at_recv_readline(device) > 0)
        {
            if ((urc = get_urc_obj(device)) != NULL)
            {
                /* current receive is request, try to execute related operations */
                if (urc->func != NULL)
                {
                    urc->func(device, device->recv_line_buf, device->recv_line_len);
                }
            }
            else if (device->resp != NULL)
            {
                at_response_t resp = device->resp;

                /* current receive is response */
                device->recv_line_buf[device->recv_line_len - 1] = '\0';
                
                if (resp->buf_len + device->recv_line_len < resp->buf_size)
                {
                    /* copy response lines, separated by '\0' */
                    memcpy(resp->buf + resp->buf_len, device->recv_line_buf, device->recv_line_len);

                    /* update the current response information */
                    resp->buf_len += device->recv_line_len;
                    resp->line_counts++;
                }
                else
                {
                    device->resp_status = AT_RESP_BUFF_FULL;
                    printf("Read response buffer failed. The Response buffer size is out of buffer size(%d)!", resp->buf_size);
                }
                /* check response result */
                if (memcmp(device->recv_line_buf, AT_RESP_END_OK, strlen(AT_RESP_END_OK)) == 0
                        && resp->line_num == 0)
                {
                    /* get the end data by response result, return response state END_OK. */
                    device->resp_status = AT_RESP_OK;
                }
                else if (strstr(device->recv_line_buf, AT_RESP_END_ERROR)
                        || (memcmp(device->recv_line_buf, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0))
                {
                    device->resp_status = AT_RESP_ERROR;
                }
                else if (resp->line_counts == resp->line_num && resp->line_num)
                {
                    /* get the end data by response line, return response state END_OK.*/
                    device->resp_status = AT_RESP_OK;
                }
                else
                {
                    continue;
                }

                device->resp = NULL;
                platform_semphr_unlock(device->resp_semphr);
            }
            else
            {
                printf("unrecognized line: %.*s", device->recv_line_len, device->recv_line_buf);
            }
        }
    }
}

void at_init(esp8266_obj_t device)
{
	esp8266_init(device, "ESP8266");
	at_obj_set_urc_table(device, urc_table, sizeof(urc_table)/sizeof(urc_table[0]));
}

int AT_send_obj_cmd(esp8266_obj_t device, at_response_t resp, uint8_t *cmd)
{
	int ret = AT_OK;
	
	if(device == NULL)
	{
		return -AT_ERR;
	}

	device->resp_status = AT_RESP_OK;
	device->resp = resp;
	
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
		if(device->resp_status != AT_RESP_OK)
		{
			printf("execute command (%.*s) failed!", cmd);
			ret = -AT_ERR;
		}
	}

	device->resp = NULL;

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


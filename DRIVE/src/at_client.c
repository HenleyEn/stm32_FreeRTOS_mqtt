#include "include.h"
#include "at_client.h"
//#include "dev_usart.h"

#if defined		ESP8266_USING

struct at_client at_esp8266_device;


#define AT_OK	0
#define AT_ERR	1
#define AT_TIMEOUT		2
#define AT_PACK_FULL	3

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

#define AT_RECV_BUF_LEN		512

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
	at_response_t resp = NULL;

	resp = (at_response_t)pvPortCalloc(1, sizeof(struct at_response));

    if (resp == NULL)
    {
        printf("AT create response object failed! No memory for response object!");
        return NULL;
    }


	resp->buf = (char*)pvPortCalloc(1, buf_size);

	if(resp->buf == NULL)
	{
        printf("AT create response object failed! No memory for response buffer!");
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

const char* at_resp_get_line(at_response_t resp, int resp_line)
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
int at_resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;


    if ((resp_line_buf = at_resp_get_line(resp, resp_line)) == NULL)
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
 * @param table_size table size
 *
 * @return != AT_OK  OK
 *			= -AT_ERR ERROR
 */
int at_obj_set_urc_table(at_client_t device,  struct at_urc *urc_table, int table_size)
{
	int idx;

	if(device == NULL)
	{
		printf("at_obj_set_urc_table is ERROR\r\n");
		return -AT_ERR;
	}
	
	for(idx = 0; idx < table_size; idx++)
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
		device->urc_table[0].urc_size = table_size;
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
		device->urc_table = (struct at_urc_table *) realloc(device->urc_table, 
																	old_table_size + sizeof(struct at_urc_table));
		
		if (device->urc_table == NULL)
		{
			vPortFree(old_urc_table);
			return -AT_ERR;
		}
		
		memcpy(device->urc_table, old_urc_table, old_table_size);
		
		device->urc_table[device->urc_table_size].urc = urc_table;
		device->urc_table[device->urc_table_size].urc_size = table_size;
		device->urc_table_size++;

		vPortFree(old_urc_table);
	}

	return AT_OK;
	
}


/*
static int at_rx_init(at_client_t device)
{
    if(device != NULL)
    {
		platform_semphr_unlock(device->rx_semphr);
	    return AT_OK;
    }
    return -AT_ERR;
}
*/

/**
 * @brief Get the urc obj object
 * 
 * @param device 
 * @return const struct at_urc* 
 */
static const struct at_urc *get_urc_obj(at_client_t device)
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

/**
 * @brief get buffer
 * 
 * @param device 
 * @param offset buf offset
 * @param data_buf data buffer source
 * @param size read buffer size
 * @return int != 1 error
 * 				= 1 ok
 */
static int device_read_buf(at_client_t device, int offset, void* data_buf, int size)
{
	int ret;
	if(device == NULL)
	{
		return AT_ERR;
	}

	ret = HAL_AT_read_buf(device, offset, data_buf, size);

	return ret;
}

/**
 * @brief save the buf to ch
 * 
 * @param device 
 * @param ch save the buffer addr
 * @param timeout wait time
 * @return int  == AT_OK ok
 * 				!= AT_OK error
 */
static int at_client_getchar(at_client_t device, char *ch, int timeout)
{
    int result = AT_OK;

	while(1)
	{
	    result = platform_semphr_lock(&device->rx_semphr);
//	    printf("result = %d\r\n", result);

		if (result != pdPASS)
		{
		    return result;
		}
		if(device_read_buf(device, 0, ch, 1) == 1)
		{
		    return AT_OK;
		}
	}

}

/**
 * @brief get the recevice one line buffer lenght
 * 
 * @param device 
 * @return int  != -AT_PACK_FULL return receive one line buffer lenght
 * 				== -AT_PACK_FULL  the recevice buffer is full
 */
static int at_recv_readline(at_client_t device)
{
    int read_len = 0;
    char ch = 0, last_ch = 0;
    int is_full = FALSE;
	int ret;
	
	/* init buf */
    memset(device->recv_line_buf, 0x00, device->recv_buffer_size);
    device->recv_line_len = 0;
//	printf("recv_buffer_size = %d\r\n", device->recv_buffer_size);
	
	/* get data or get newline */
    while (1)
    {
        ret = at_client_getchar(device, &ch, portMAX_DELAY);
//		printf("ret = %d\r\n", ret);
		/* save data to line buffer */
        if (read_len < device->recv_buffer_size)
        {
            device->recv_line_buf[read_len++] = ch;
            device->recv_line_len = read_len;
        }
        else
        {
            is_full = TRUE;
        }

        /* compare buffer is newline or URC data */
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
	printf("read_len = %d\r\n", read_len);
	printf(device->recv_line_buf);
	printf("\r\n");
    return read_len;
}

/**
 * @brief client parase the buffer task
 * 
 * @param device 
 */
void client_parser(at_client_t device)
{
    const struct at_urc *urc;
    
//    int ret = at_recv_readline(device);
//    printf("at_recv_readline(device) ret = %d\r\n", ret);
    
    while(1)
    {

        if (at_recv_readline(device) > 0)
        {
        	printf("at_recv_readline ret = %d\r\n", at_recv_readline(device));
        	
            if ((urc = get_urc_obj(device)) != NULL)
            {
                /* current recevice is request, try to execute related operations */
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
                	printf("copy response lines, separated by '\0'\r\n");
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
                platform_semphr_unlock(&device->resp_semphr);
            }
            else
            {
                printf("unrecognized line: %.*s", device->recv_line_len, device->recv_line_buf);
            }
        }
    }
}


static int at_client_para_init(struct at_client *device)
{
	int ret = AT_OK;
	device->status = AT_STATUS_UNINITIALIZED;

	device->recv_line_len = 0;
	device->recv_line_buf = (char*)pvPortCalloc(1, device->recv_buffer_size);

	if(device->recv_line_buf == NULL)
	{
		printf("AT client initialize failed! No memory for receive buffer.\r\n");
		ret = ERROR;
	}
	

	platform_mutex_init(&device->mutex);
	
	platform_semaphore_init(&device->rx_semphr, 256, 0);
	if (&device->rx_semphr == NULL)
	{
		printf("AT client initialize failed! at_client_rx_semphr create failed!");
		ret = ERROR;
	}
	
	platform_semaphore_init(&device->resp_semphr, 20, 0);
	if (&device->resp_semphr == NULL)
	{
		printf("AT client initialize failed! at_client_resp_semphr create failed!");
		ret = ERROR;
	}

	device->urc_table = NULL;
	device->urc_table_size = 0;

	return ret;
}

void at_client_init(struct at_client *device, const char* dev_name, int recv_bufsz)
{
	struct at_client* at_dev = NULL;


	at_dev = device;
	at_dev->recv_buffer_size = recv_bufsz;

	at_client_para_init(at_dev);
	at_dev->status = AT_STATUS_INITIALIZED;
}


int at_client_destory(at_client_t device)
{
	if(device != NULL)
	{
		// vPortFree(device->resp);
		vPortFree(device->data_buffer);
		
//		platform_mutex_destroy(device->mutex);
//		platform_semphr_destroy(device->resp_semphr);
//		platform_semphr_destroy(device->rx_semphr);
		memset(device, 0x00, sizeof(struct at_client));

		vPortFree(device);
		
		return TRUE;
	}

	return FALSE;
}
at_client_t get_at_client(void)
{
	return &at_esp8266_device;
}

//todo: resp在哪recv
int at_send_obj_cmd(at_client_t device, at_response_t resp, uint8_t *cmd)
{
	int ret = AT_OK;
	
	if(device == NULL)
	{
		return -AT_ERR;
	}
	platform_mutex_lock_timeout(&device->mutex, portMAX_DELAY);
	
	device->resp_status = AT_RESP_OK;
	device->resp = resp;
	
	if (resp != NULL)
	{
		resp->buf_len = 0;
		resp->line_counts = 0;
	}

	HAL_AT_send_cmd(cmd, strlen(cmd));
	HAL_AT_send_cmd("\r\n", 2);	
	
	if(resp != NULL)
	{
		if(device->resp_status != AT_RESP_OK)
		{
			printf("execute command (%.*s) failed!", cmd);
			ret = -AT_ERR;
		}
	}

	if (resp != NULL)
    {
        if (platform_semphr_lock_timeout(&device->resp_semphr, resp->timeout) != pdPASS)
        {
        	printf("resp_semphr is locked err\r\n");
            device->resp_status = AT_RESP_TIMEOUT;
            ret = -AT_TIMEOUT;
        }
    }

	device->resp = NULL;

	platform_mutex_unlock(&device->mutex);

	return ret;
}

/*
#define AT_RECV_SIZE	100

void at_recv_cmd_task(void* param)
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
*/

#endif


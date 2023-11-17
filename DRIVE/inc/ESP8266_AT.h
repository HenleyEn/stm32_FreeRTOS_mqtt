#ifndef ESP8266_AT_H
#define ESP8266_AT_H

#include "platform_mutex.h"
#include "ringbuf.h"
#include "at.h"
typedef struct esp8266_obj *esp8266_obj_t;

enum at_status
{
    AT_STATUS_UNINITIALIZED = 0,
    AT_STATUS_INITIALIZED,
    AT_STATUS_CLI,

};
typedef enum at_status *at_status_t;

enum at_resp_status
{
     AT_RESP_OK = 0,                   /* AT response end is OK */
     AT_RESP_ERROR = -1,               /* AT response end is ERROR */
     AT_RESP_TIMEOUT = -2,             /* AT response is timeout */
     AT_RESP_BUFF_FULL= -3,            /* AT response buffer is full */
};
typedef enum at_resp_status at_resp_status_t;

struct at_response
{
	char *buf;
	/* the maximum response buffer size */
	int  buf_size;
	/* the length of current response buffer */
	int buf_len;

    /* the number of setting response lines, it set by `at_create_resp()` function
     * == 0: the response data will auto return when received 'OK' or 'ERROR'
     * != 0: the response data will return when received setting lines number data */
	char line_num;

	/* the count of received response lines */
	char line_counts;
	
	at_status_t response_status;
	/* the maximum response time */
	int timeout;
};

typedef struct at_response *at_response_t;

struct at_urc
{
	const char *begin_str;
	const char *end_str;

	void (*func)(struct esp8266_obj *device, char* recv_line_buf, int recv_line_len);
};

typedef  struct at_urc *at_urc_t;

struct at_urc_table
{
	int urc_size;
	 at_urc_t urc;
};
typedef struct at_urc_table *at_urc_table_t;

struct wifi_info
{
	uint8_t *wifi_ssid;
	uint8_t *wifi_password;
};
typedef struct wifi_info *wifi_info_t;

struct esp8266_obj
{
	/* devicre name */
	uint8_t *device_name;
	ptr_ringbuf_t data_buffer;
	
	char end_sign;

	/* resp */
	at_response_t resp;
	at_resp_status_t resp_status;
	struct platform_semaphore *resp_semphr;
	
	char *resp_buf;

	platform_mutex_t* mutex;
	struct platform_semaphore * rx_semphr;

	/* parse recv data */ 
	char *recv_line_buf;
	int recv_line_len;
	int recv_buffer_size;

	/* set urc_table */
	int urc_table_size;
	at_urc_table_t urc_table;
	
};

int esp8266_init(esp8266_obj_t device, uint8_t* device_name);
int esp8266_destory(esp8266_obj_t device);
void AT_recv_cmd_task(void* param);
void client_parser(esp8266_obj_t device);

void at_init(esp8266_obj_t device);
at_response_t at_create_resp(int buf_size, int line_num, int timeout);
void at_delete_resp(at_response_t resp);
int AT_send_obj_cmd(esp8266_obj_t device, at_response_t resp, uint8_t *cmd);
const char* resp_get_line(at_response_t resp, int resp_line);
int resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...);

#endif

#ifndef ESP8266_AT_H
#define ESP8266_AT_H

#include "include.h"

typedef enum
{
	AT_STATUS_TIMEOUT = -2,
	AT_STATUS_ERROR = -1,
	AT_STATUS_OK = 0,
	
	AT_STATUS_INIT = 2,
}AT_Status;

typedef enum
{
    AT_STATUS_UNINITIALIZED = 0,
    AT_STATUS_INITIALIZED,
    AT_STATUS_CLI,

}at_status;

enum at_resp_status
{
     AT_RESP_OK = 0,                   /* AT response end is OK */
     AT_RESP_ERROR = -1,               /* AT response end is ERROR */
     AT_RESP_TIMEOUT = -2,             /* AT response is timeout */
     AT_RESP_BUFF_FULL= -3,            /* AT response buffer is full */
};
 typedef enum at_resp_status at_resp_status_t;


typedef at_status *at_status_t;

typedef struct
{
	uint8_t *wifi_ssid;
	uint8_t *wifi_password;
}wifi_info_t;

typedef struct
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
}at_response;

typedef at_response *at_response_t;

typedef struct
{
	uint8_t *device_name;
	wifi_info_t* wifi_info;
	
	at_response* cmd_buffer;
	at_resp_status_t status;
	
	ringbuf_t* data_buffer;
	platform_mutex_t* mutex;
	
}esp8266_obj;

int esp8266_init(esp8266_obj* device, uint8_t* device_name);
int esp8266_destory(esp8266_obj* device);
int AT_send_cmd(esp8266_obj* device, uint8_t *str, int str_len, int timeout);
int AT_recv(void);
int AT_ack(void);


at_response_t at_create_resp(int buf_size, int line_num, int timeout);
void at_delete_resp(at_response_t resp);
int AT_send_obj_cmd(esp8266_obj* device, at_response_t resp, uint8_t *cmd);
const char* resp_get_line(at_response_t resp, int resp_line);
int resp_parse_line_args(at_response_t resp, int resp_line, const char *resp_expr, ...);


#endif

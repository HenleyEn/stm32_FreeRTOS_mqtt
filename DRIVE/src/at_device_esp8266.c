#include "include.h"
#include "at_client.h"
#include "at_device_esp8266.h"

struct at_device_esp8266 esp8266_dev;

struct at_device_esp8266 esp8266_dev = 
{
	.device_name	=	ESP8266_DEVICE_NAME,
	.wifi_ssid		=	WIFI_SSID,
	.wifi_password	=	WIFI_PASSWORD,
	.device 		=	&at_esp8266_device,
	.uart_device	=	&uart3_dev,
	.recv_line_num	=	ESP8266_SAMPLE_RECV_BUFF_LEN,
};

static void urc_func(struct at_client *device, char* data, int data_size)
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

int at_device_esp8266_init(struct at_device_esp8266 *esp8266_devide)
{
	if(esp8266_devide == NULL)
	{
		return 0;
	}
	
	at_client_init(esp8266_devide->device, "esp8266_dev", esp8266_devide->recv_line_num);
	at_obj_set_urc_table(esp8266_devide->device, urc_table, sizeof(urc_table)/sizeof(urc_table[0]));
	
	return 1;
}

void at_test(void* param)
{
#define AT_CMD_MAX_LEN 128

	at_response_t resp = NULL;
	
//	const char* line_buffer = NULL;
	resp = at_create_resp(256, 0, 1000);

	if(resp != NULL)
	{
		printf("resp data:\r\n");
//		at_send_obj_cmd(&at_esp8266_device, NULL, "AT");	
		at_send_obj_cmd(&at_esp8266_device, resp, "AT+CIFSR");
		
		char resp_arg[AT_CMD_MAX_LEN] = { 0 };
		
		//正则表达式
        const char * resp_expr = "%*[^\"]\"%[^\"]\"";

        printf(" Parse arguments");
        if (at_resp_parse_line_args(resp, 1, resp_expr, resp_arg) == 1)
        {
            printf("Station IP  : %s", resp_arg);
            memset(resp_arg, 0x00, AT_CMD_MAX_LEN);
        }

        if (at_resp_parse_line_args(resp, 2, resp_expr, resp_arg) == 1)
        {
            printf("Station MAC : %s", resp_arg);
        }

		if(resp)
		{
			at_delete_resp(resp);
		}
	}

	while(1)
	{
		vTaskDelay(3);
	}
}





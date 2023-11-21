#include "include.h"

#include "task_manager.h"
#include "ESP8266_AT.h"
#include "USART.h"


struct esp8266_obj esp8266_dev;

TaskHandle_t task_start_handle;
TaskHandle_t task_test_handle;
TaskHandle_t task_parse_handle;

void task_start(void* param)
{
	
	taskENTER_CRITICAL();
	USART1_Config(115200);
	USART3_Config(115200);	
	vTaskDelay(500);
	taskEXIT_CRITICAL(); 


	taskENTER_CRITICAL();
	esp8266_init(&esp8266_dev, "ESP8266_device");
	do_create_parse_task();
	do_create_test_task();
	
	vTaskDelete(task_start_handle); 					
	taskEXIT_CRITICAL(); 
}

void do_create_start_task(void)
{	
	BaseType_t xReturn = pdPASS;					

	xReturn = xTaskCreate(task_start, "task_start", 1024, NULL, 1, &task_start_handle);
	
	if(pdPASS == xReturn)	
		vTaskStartScheduler();
}

void do_create_parse_task(void)
{
	BaseType_t xReturn = pdPASS;					

	xReturn = xTaskCreate((void (*)(void *param))client_parser, 
							"client_parser", 
							1024, 
							&esp8266_dev, 
							1, 
							&task_parse_handle);

	if(pdPASS == xReturn)	
	{
		printf("do_create_parse_task is create\r\n");
	}
	else
	{
		
		printf("do_create_parse_task is not create\r\n");
	}
}

void do_create_test_task(void)
{	
	BaseType_t xReturn = pdPASS;					

	xReturn = xTaskCreate(at_test, "task_test", 500, NULL, 1, &task_test_handle);
	
	if(pdPASS == xReturn)	
	{
		printf("do_create_test_task is create\r\n");
	}
	else
	{
		
		printf("do_create_test_task is not create\r\n");
	}
}

void at_test(void* param)
{
#define AT_CMD_MAX_LEN 128

	at_response_t resp = NULL;
	const char* line_buffer = NULL;
	resp = at_create_resp(200, 0, 1000);

	if(resp != NULL)
	{
		printf("resp data:\r\n");
		AT_send_obj_cmd(&esp8266_dev, resp, "AT+CIFSR");	


		char resp_arg[AT_CMD_MAX_LEN] = { 0 };
        const char * resp_expr = "%*[^\"]\"%[^\"]\"";

        printf(" Parse arguments");
        if (resp_parse_line_args(resp, 1, resp_expr, resp_arg) == 1)
        {
            printf("Station IP  : %s", resp_arg);
            memset(resp_arg, 0x00, AT_CMD_MAX_LEN);
        }

        if (resp_parse_line_args(resp, 2, resp_expr, resp_arg) == 1)
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


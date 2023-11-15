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
	
	at_init(&esp8266_dev);
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

	xReturn = xTaskCreate(AT_recv_cmd_task, "recv_cmd_task", 1024, NULL, 1, &task_parse_handle);

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

	xReturn = xTaskCreate(at_test, "task_test", 1024, NULL, 1, &task_test_handle);
	
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

	printf("at_test\r\n");
	at_response_t resp = NULL;
	const char* line_buffer = NULL;

	resp = at_create_resp(20, 0, 100);
	
	if(resp != NULL)
	{
		AT_send_obj_cmd(&esp8266_dev, resp, "AT");
//		AT_send_obj_cmd(&esp8266_dev, resp, "AT+CWMODE=3");
		printf("resp data\r\n");

		if((line_buffer = resp_get_line(resp, 0)) != NULL)
		{
			printf("data is %s", line_buffer);
		}
		else
		{
			printf("Parse line buffer error!");
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


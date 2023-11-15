#include "include.h"


esp8266_obj esp8266_dev;

TaskHandle_t task_start_handle;
TaskHandle_t task_test_handle;
TaskHandle_t task_prase_handle;

void task_start(void* param)
{
	
	taskENTER_CRITICAL();
	USART1_Config(115200);
	USART3_Config(115200);
	at_init(&esp8266_dev);
	
	vTaskDelay(200);
	
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

	xReturn = xTaskCreate(AT_recv_cmd_task, "recv_cmd_task", 1024, NULL, 1, &task_prase_handle);

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
	at_response_t resp;
	const char* line_buffer = NULL;

	resp = at_create_resp(20, 0, 100);

	while(1)
	{
		AT_send_obj_cmd(&esp8266_dev, resp, "AT");

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

		
		vTaskDelay(1000);
	}

}


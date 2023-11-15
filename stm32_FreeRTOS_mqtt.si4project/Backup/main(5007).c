#include "include.h"

esp8266_obj esp8266_dev;
void task1(void* param);

int main()
{
//	TaskHandle_t xHandleTask1;
	
	USART1_Config(115200);
	USART3_Config(115200);
	printf("Welcome to FreeRTOS\r\n");

//	USART3_SendArray("AT\r\n", 4);
//	esp8266_init(&esp8266_dev, "esp8266_emqt");
	xTaskCreate( task1, "Task1", 2000, "Task1", 1, NULL);

	
		/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}


void task1(void* param)
{
	printf(param);
	at_response_t resp;
	const char* line_buffer = NULL;

	resp = at_create_resp(10, 0, 100);

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
	while(1);
}



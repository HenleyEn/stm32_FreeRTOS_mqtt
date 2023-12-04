#include "include.h"

#include "task_manager.h"
#include "ESP8266_AT.h"
#include "bsp_usart_config.h"
#include "dev_usart.h"

struct esp8266_obj esp8266_dev;

TaskHandle_t task_start_handle;
TaskHandle_t task_parse_handle;
// TaskHandle_t task_uart_dma_handle;
TaskHandle_t task_test_handle;


void task_start(void* param)
{
	
	taskENTER_CRITICAL();
	uart_device_init(&uart3_dev);
	// USART3_Config(115200);
	// USART1_Config(115200);

	vTaskDelay(500);
	taskEXIT_CRITICAL(); 

	taskENTER_CRITICAL();
	esp8266_init(&esp8266_dev, "ESP8266_device");
	do_create_parse_task();
	do_create_test_task();
	taskEXIT_CRITICAL(); 

	vTaskDelete(task_start_handle); 					

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



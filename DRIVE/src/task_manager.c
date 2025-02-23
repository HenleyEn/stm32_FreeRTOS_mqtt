#include "include.h"

#include "task_manager.h"
#include "at_client.h"
#include "bsp_usart_config.h"
#include "dev_usart.h"
#include "at_device_esp8266.h"


TaskHandle_t task_start_handle;
TaskHandle_t task_parse_handle;
TaskHandle_t task_uart_dma_handle;
TaskHandle_t task_test_handle;


void task_start(void* param)
{
	
	taskENTER_CRITICAL();
	uart_device_init(&uart3_dev);

	vTaskDelay(500);
	taskEXIT_CRITICAL(); 

	taskENTER_CRITICAL();
//	esp8266_init(&at_esp8266_device, "esp8266_dev");
	at_device_esp8266_init(&esp8266_dev);
	
	do_create_uart_dma_task();
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
							&at_esp8266_device, 
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

void do_create_uart_dma_task(void)
{	
	BaseType_t xReturn = pdPASS;					

	xReturn = xTaskCreate((void (*)(void *param))uart_dma_task, 
							"create_uart_dma_task", 
							500, 
							&uart3_dev, 
							1, 
							&task_uart_dma_handle);
	
	if(pdPASS == xReturn)	
	{
		printf("do_create_uart_dma_task is create\r\n");
	}
	else
	{
		
		printf("do_create_uart_dma_task is not create\r\n");
	}
}

void do_create_test_task(void)
{	
	BaseType_t xReturn = pdPASS;					

	xReturn = xTaskCreate(at_test, "task_test", 500, &at_esp8266_device, 1, &task_test_handle);
	
	if(pdPASS == xReturn)	
	{
		printf("do_create_test_task is create\r\n");
	}
	else
	{
		
		printf("do_create_test_task is not create\r\n");
	}
}



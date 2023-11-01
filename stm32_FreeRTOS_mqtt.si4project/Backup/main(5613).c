#include "include.h"

esp8266_obj esp8266_dev;

int main()
{
//	TaskHandle_t xHandleTask1;
	
	USART1_Config(115200);
	USART3_Config(115200);
//	printf("Welcome to FreeRTOS\r\n");

	USART3_SendArray("AT\r\n", 4);
	esp8266_init(&esp8266_dev, "esp8266_emqt");
//	xTaskCreate( Task1Funcation, "Task1", 100, "Task1", 1, &xHandleTask1);
	
		/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}


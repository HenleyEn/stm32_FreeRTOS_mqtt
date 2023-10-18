#include "include.h"

void Task1Funcation(void* param);

int main()
{
//	TaskHandle_t xHandleTask1;
	
	USART1_Config(115200);
	USART3_Config(115200);
//	printf("Welcome to FreeRTOS\r\n");

	USART3_SendArray("AT\r\n", 4);
//	xTaskCreate( Task1Funcation, "Task1", 100, "Task1", 1, &xHandleTask1);
	
		/* Start the scheduler. */
	vTaskStartScheduler();

	while (1)
	{
	}
	return 0;
}

void Task1Funcation(void* param)
{
	while(1)
		{
			printf(param);
			printf("\r\n");
		}
}

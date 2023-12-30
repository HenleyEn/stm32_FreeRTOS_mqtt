#include "include.h"
#include "task_manager.h"
#include "dev_usart.h"
#include "bsp_usart_config.h"

//static uint8_t *str = "AT+CIFSR";
//static uint8_t test_buf[256];
int main()
{
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	USART3_Config(115200);
//	usart3_rx_DMA_config((uint32_t)(uart3_dev.dma_rx_buf), uart3_dev.dma_rx_buf_size);

//	HAL_AT_send_cmd(str, strlen(str));
//	HAL_AT_send_cmd("\r\n", 2);
	
	do_create_start_task();
	printf("Welcome to FreeRTOS\r\n");

	//	while(1);
	return 0;
}





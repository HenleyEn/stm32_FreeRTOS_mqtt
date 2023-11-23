#include "include.h"

#include "ESP8266_AT.h"
#include "platform_mutex.h"
#include "ringbuf.h"
#include "USART.h"

struct ringbuf USART3_RingBuf;

/*
 * USART1
 * PA9	TX
 * PA10 RX
 *  
 * USART3
 *  PB10 TX
 *  PB11 RX
 */

void USART1_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Init( USART1, &USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void USART3_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Init( USART3, &USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	ringbuf_init(&USART3_RingBuf);
}

/**
 * @brief DMA USART3 Rx config
 * 
 * @param AddrA recv data addr
 * @param AddrB save data addr
 * @param Size recevice buffer size
 */
void usart3_rx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	DMA_InitTypeDef DMA_InitStructre;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitStructre.DMA_PeripheralBaseAddr = (uint32_t)AddrA;
	DMA_InitStructre.DMA_MemoryBaseAddr = (uint32_t)AddrB;
	DMA_InitStructre.DMA_BufferSize = Size;
	/* 方向：外设-> 内存 */
	DMA_InitStructre.DMA_DIR = DMA_DIR_PeripheralSRC;
	/* DMA_memory_to_memory disable */
	DMA_InitStructre.DMA_M2M = DMA_M2M_Disable;
	/* the Memory data width. Byte width */
	DMA_InitStructre.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	/* whether the memory address register is incremented or not. */
	DMA_InitStructre.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/* the operation mode of the DMAy Channelx. */
	DMA_InitStructre.DMA_Mode = DMA_Mode_Circular;
	/* the Peripheral data width. */
	DMA_InitStructre.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	/* whether the Peripheral address register is incremented or not. */
	DMA_InitStructre.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	/* the software priority for the DMAy Channelx. */
	DMA_InitStructre.DMA_Priority = DMA_Priority_VeryHigh;

	DMA_Init(DMA1_Channel3, &DMA_InitStructre);
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC|DMA_IT_HT, ENABLE);/* 使能DMA半满、溢满 */
	DMA_ClearFlag(DMA1_IT_TC3);	/* 清除相关状态标识 */
	DMA_ClearFlag(DMA1_IT_HT3);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

/**
 * @brief DMA USART3 Tx config
 * 
 * @param AddrA get addrB data addr
 * @param AddrB send buffer source addr
 * @param Size size
 */
void usart3_tx_DMA_config(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel2);
	DMA_Cmd(DMA1_Channel2, DISABLE);

	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)AddrA;/* UART2发送数据地址 */
	DMA_InitStructure.DMA_MemoryBaseAddr 		= (uint32_t)AddrB; 	/* 发送数据buf */
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralDST; 	/* 传输方向:内存->外设 */
	DMA_InitStructure.DMA_BufferSize 			= Size; 			/* 发送数据buf大小 */
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable; 
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte; 
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal; 		/* 单次模式 */
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;	 
	DMA_InitStructure.DMA_M2M 					= DMA_M2M_Disable; 

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC|DMA_IT_TE, ENABLE); /* 使能传输完成中断、错误中断 */
	DMA_ClearFlag(DMA1_IT_TC4);	/* 清除发送完成标识 */
	DMA_Cmd(DMA1_Channel2, ENABLE); /* 启动DMA发送 */
}

extern struct esp8266_obj esp8266_dev;

void USART3_SendByte(uint8_t Byte)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, Byte);
}

void USART3_SendArray(uint8_t arr[], uint16_t length)
{
	uint16_t i;
	for(i=0; i<length; i++)
	{
		USART3_SendByte(arr[i]);
//		ringbuf_write(&USART3_RingBuf, arr[i]);
	}
}

struct ringbuf test_buf;
int len = 0;
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
			// ringbuf_write(esp8266_dev.data_buffer, USART_ReceiveData(USART3));
			esp8266_dev.resp->buf[len++] = USART_ReceiveData(USART3);

			ringbuf_write(&test_buf, USART_ReceiveData(USART3));
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
//			platform_mutex_unlock_from_isr(esp8266_dev.mutex);
			platform_semphr_unlock_from_isr(esp8266_dev.rx_semphr);
			
	}
}

platform_mutex_t USART3_recv_mutex;

void USART3_Recv( uint8_t *buf, int timeout)
{
	while(1)
	{
		if(TRUE == ringbuf_read(buf, esp8266_dev.data_buffer))
		{
			return;
		}
		else
		{
			platform_mutex_lock_timeout(esp8266_dev.mutex, timeout);
		}
	}
}

int USART3_Recv_buf(esp8266_obj_t device, int offset, void* data_buf, int size)
{

	int str_len;
	configASSERT(device);
	
	if(data_buf == NULL)
	{
		return NULL;
	}

	str_len = strlen(device->resp_buf);
	
	if(str_len < size)
	{
		return NULL;
	}
	else
	{
		strncpy(data_buf, device->resp_buf + offset, size);
	}
	
	return 1;
}


// int fputc(int ch, FILE* f)
// {
// 	USART_TypeDef* USARTx = USART1;
// 	while((USARTx->SR & (1 << 7)) == 0); 
// 	USARTx->DR = ch;
// 	return ch;
// }



int fputc(int ch, FILE *f)
{  
    SEGGER_RTT_PutChar(0, ch);
 	return ch;
}


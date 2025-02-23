/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "platform_mutex.h"
#include "dev_usart.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*
void SVC_Handler(void)
{
}
*/
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*
void PendSV_Handler(void)
{
}
*/
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*
void SysTick_Handler(void)
{
}
*/
/**
 * @brief if you want to clear the USART_IT_IDLE, it has two steps;
 *        first: read SR register, so USART_GetITStatus(USART1, USART_IT_IDLE) == SET;
 *        second: read DR register, so USART_ReceiveData(USART1);
 */

void USART1_IRQHandler(void)
{

}

int uart3_cnt = 0;
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
//		// ringbuf_write(at_esp8266_device.data_buffer, USART_ReceiveData(USART3));
////		at_esp8266_device.resp->buf[len++] = USART_ReceiveData(USART3);

//		ringbuf_write(&test_buf, USART_ReceiveData(USART3));
		USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
////			platform_mutex_unlock_from_isr(at_esp8266_device.mutex);
		uart3_cnt++;
		printf("uart3_cnt is %d\r\n", uart3_cnt);
		platform_semphr_unlock_from_isr(&at_esp8266_device.rx_semphr);			
	}
  
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		/* clear the idle flag */
		USART_ReceiveData(USART3);
		
		uart3_dev.dma_status = DMA_UART_IDLE;
		platform_mutex_unlock_from_isr(&uart3_dev.dma_mutex);
		
//		platform_semphr_unlock_from_isr(&at_esp8266_device.rx_semphr);			
		
	}
}

/* Tx */
void DMA1_Channel2_IRQHandler(void)
{

}

/* Rx */
void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)
	{
		uart3_dev.dma_status = DMA_BUF_FULL;

		DMA_ClearFlag(DMA1_IT_TC3);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		platform_mutex_unlock_from_isr(&uart3_dev.dma_mutex);
		
//		platform_semphr_unlock_from_isr(&at_esp8266_device.rx_semphr);			
	}

	if(DMA_GetITStatus(DMA1_IT_HT3) != RESET)
	{
		uart3_dev.dma_status = DMA_BUF_HAIF;

		DMA_ClearFlag(DMA1_IT_HT3);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		platform_mutex_unlock_from_isr(&uart3_dev.dma_mutex);
		
//		platform_semphr_unlock_from_isr(&at_esp8266_device.rx_semphr);			
	}
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

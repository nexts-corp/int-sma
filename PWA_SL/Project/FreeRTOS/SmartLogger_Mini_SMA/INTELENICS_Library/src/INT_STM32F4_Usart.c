/**
  ******************************************************************************
  * @file    INT_STM32F4_Usart.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
#include "main.h"

#include "INT_STM32F4_Usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>
#include "INT_STM32F4_debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t rx_buffer1[RX_BUFFER_SIZE1];						// Buffer for Configuration
uint8_t rx_buffer3[RX_BUFFER_SIZE3];						// Buffer for Configuration

/* This counter is used to determine the string length for USART1 and UART5 */
uint16_t rx_wr_index1 = 0, rx_rd_index1 = 0, rx_counter1 = 0;
extern __IO uint16_t rx_wr_index3;
extern __IO uint16_t rx_rd_index3;
extern __IO uint16_t rx_counter3;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*==========================================================================================*/
/**
  * @brief  Initializes the USART peripheral registers. 
  * @param  None
  * @retval None
  */
void vUSART_Initial(void) {
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	// COM1 set Tx_pin = B6 , Rx_pin = B7 connect to USART1 ( Config & Debug )
  STM_EVAL_COMInit(COM1, &USART_InitStructure);
	// COM2 set Tx_pin = A2 , Rx_pin = A3 connect to USART2 ( Modbus Slave )
//  	STM_EVAL_COMInit(COM2, &USART_InitStructure);
	// COM3 set Tx_pin = D8 , Rx_pin = D9 connect to USART3 ( GSM module )
  STM_EVAL_COMInit(COM3, &USART_InitStructure);
	
	/* Here the USART1 receive interrupt is enabled
	 * and the interrupt controller is configured
	 * to jump to the USART1_IRQHandler() function
	 * if the USART1 receive interrupt occurs
	 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // we want to configure the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	// finally this enables the complete USART1 peripheral
	USART_Cmd(USART1, ENABLE);
	
	/* Here the USART3 receive interrupt is enabled
	 * and the interrupt controller is configured
	 * to jump to the USART3_IRQHandler() function
	 * if the UART5 receive interrupt occurs
	 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		 // we want to configure the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART3 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	// finally this enables the complete UART5 peripheral
	USART_Cmd(USART3, ENABLE);
}
/*==========================================================================================*/
/**
  * @brief  Read serial from USART1(Debug and Configuration).
  * @param  None
  * @retval length : length of buffer
  */
uint16_t usUSART_ReadUart1(uint8_t *readbuf, uint16_t maxread, uint16_t interchar) {
                      
    uint8_t data;  
    uint16_t len = 0;
    
    while(rx_counter1 > 0) {
        data = rx_buffer1[rx_rd_index1++];
        if (rx_rd_index1 == RX_BUFFER_SIZE1) {
            rx_rd_index1 = 0;                                
        }
                
        --rx_counter1;                         
                     
        memcpy(readbuf++, &data, 1);       
        len++;
        if(len == maxread) {
            break;
        }        
        delay_ms(interchar);
    }
		rx_rd_index1 = 0;
		rx_wr_index1 = 0;
    return len;
    
}
/*============================================================================================
 * ISR
============================================================================================*/
/**
  * @brief  Interupt USART1.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void) {
	
	uint8_t data;
	
 	/* check if the USART1 receive interrupt flag was set */
 	if( USART_GetITStatus(USART1, USART_IT_RXNE) ) {

		data = USART1->DR; // the character from the USART1 data register is saved in t
		rx_buffer1[rx_wr_index1++] = data;
		if(rx_wr_index1 == RX_BUFFER_SIZE1) {
			rx_wr_index1 = 0;
		}
		if(++rx_counter1 == RX_BUFFER_SIZE1) {
			printf("WARNING : USART1 BUFFER OVERFLOW %d\r\n", rx_counter1);
			rx_counter1 = 0;      
		}
	}
}
/*==========================================================================================*/
/**
  * @brief  Read serial from USART3(GSM).
  * @param  None
  * @retval length : length of buffer
  */
uint16_t usiUSART_ReadGSM(uint8_t *readbuf, uint16_t maxread, uint16_t interchar) {
	
	uint8_t 	data;  
	uint16_t 	len = 0;
	
	while(rx_counter3 > 0) {
		data = rx_buffer3[rx_rd_index3++];
		memcpy(readbuf++, &data, 1); 
		--rx_counter3;
		
		if(rx_rd_index3 >= RX_BUFFER_SIZE3) {
				rx_rd_index3 = 0;                                
		}
		len++;
		if(len == maxread) {
			break;
		}        
		delay_ms(interchar);
	}
	rx_rd_index3 = 0;
	rx_wr_index3 = 0;
	return len;
}
/*==========================================================================================*/
/**
  * @brief  Interupt UART5.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void) {
	
	uint8_t data;
	
 	/* check if the USART3 receive interrupt flag was set */
 	if( USART_GetITStatus(USART3, USART_IT_RXNE) ) {
		data = USART3->DR; 												// the character from the USART3 data register is saved in t
		rx_buffer3[rx_wr_index3++] = data;
		if(rx_counter3 < RX_BUFFER_SIZE3) {
			rx_counter3++;
		}
		if(rx_wr_index3 >= RX_BUFFER_SIZE3) {
			rx_wr_index3 = 0;
		}
		if(rx_wr_index3 == rx_rd_index3) {
			if(++rx_rd_index3 >= RX_BUFFER_SIZE3) {
				rx_rd_index3 = 0;
			}
			printDebug_Semphr(DESCRIBE, "WARNING : USART3 OVERFLOW %d,%d,%d\r\n", rx_counter3,rx_wr_index3,rx_rd_index3);
		}
	}
}
/*==========================================================================================*/
/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/







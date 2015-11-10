/**
  ******************************************************************************
  * @file    usart.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _INT_STM32F4_USART_H_
#define _INT_STM32F4_USART_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4_discovery.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define RX_BUFFER_SIZE1 256
#define RX_BUFFER_SIZE3 600
/* Exported functions ------------------------------------------------------- */ 
void vUSART_Initial(void);

uint16_t usUSART_ReadUart1(uint8_t *readbuf, uint16_t maxread, uint16_t interchar);
uint16_t usiUSART_ReadGSM(uint8_t *readbuf, uint16_t maxread, uint16_t interchar);



#endif


/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

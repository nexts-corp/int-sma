/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_420RClick.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	26-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_420RCLICK_H
#define _INT_STM32F4_420RCLICK_H

//#include "main.h"
#include <stdint.h>
#include "stm32f4xx.h"

#define TCLICK_SLOPE			(float)204.75        // 4095/20

void v420RCLICK_InitialSPI1(void);

extern __IO uint32_t 	_ADC_TClick;
uint8_t uc420RClick_ReadWriteData(SPI_TypeDef* SPIx, uint16_t writeData); 
uint16_t us420RClick_GetADCValue(uint8_t channel);
void v420TClick_ControlValve(uint32_t percent);

#endif /*_INT_STM32F4_420RCLICK_H*/


/**
  ******************************************************************************
  * @file    INT_STM32F4_adc.h
  * @author  Intelenics Co.,Ltd
  * @version V1.0
  * @date    2-January-2014
  * @brief   This file contains all the functions prototypes of adc_dma.c
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _INT_STM32F4_ADC_H
#define _INT_STM32F4_ADC_H

//#include "main.h"
#include "stm32f4xx.h"

/* Exported variables --------------------------------------------------------*/ 
extern __IO uint32_t ADCValue[];

/* Exported functions --------------------------------------------------------*/ 
void ADC1_SCAN_CH_DMA_Config(void);

#endif /*_INT_STM32F4_ADC_H*/

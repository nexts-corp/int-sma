/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_watchdog.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	26-June-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_WATCHDOG_H
#define _INT_STM32F4_WATCHDOG_H

#include <stdint.h>

void vWDG_IntialIWDG(void);
uint32_t uiWDG_GetLSIFrequency(void);

#endif /*_INT_STM32F4_WATCHDOG_H*/

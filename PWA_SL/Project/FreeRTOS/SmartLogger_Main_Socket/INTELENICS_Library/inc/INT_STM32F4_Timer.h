/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	timer.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-February-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _TIMER_H
#define _TIMER_H

//#include "main.h"
#include <stdint.h>

void vTIMER_initialTimer3(void);
void vTIMER_initialTimer12(void);
uint8_t ucTIMER_SetTimer(uint32_t *keeper, uint32_t period);
int8_t cTIMER_CheckTimerExceed(uint32_t keeper);

#endif /* _TIMER_H */




/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_rtc.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	13-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_RTC_H
#define _INT_STM32F4_RTC_H
	
//#include "main.h"
#include <stdint.h>
#include "stm32f4xx_rtc.h"
	
uint8_t ucRTC_Initial(void);
uint8_t ucRTC_SetDateTime(RTC_DateTypeDef RTC_DateStruct, RTC_TimeTypeDef RTC_TimeStruct, uint8_t time_zone);
uint8_t ucRTC_SetDateTimeFromEpoch(uint32_t epoch_time);
void vRTC_GetDateTime(RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct, int8_t time_zone);
uint32_t uiRTC_GetEpochTime(void);
void vRTC_EpochToDateTime(RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef *RTC_TimeStruct, uint32_t epoch_time);
uint32_t uiRTC_DateTimeToEpoch(RTC_DateTypeDef RTC_DateStruct, RTC_TimeTypeDef RTC_TimeStruct);
	
#endif /*INT_STM32F4_RTC_H*/



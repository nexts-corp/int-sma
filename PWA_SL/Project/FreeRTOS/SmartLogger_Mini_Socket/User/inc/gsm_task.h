/**
  ******************************************************************************
  * @file    gsm_task.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _GSM_TASK_H_
#define _GSM_TASK_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stm32f4xx.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4_discovery.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
extern xQueueHandle Queue_CMDToGSM;
extern uint8_t flag_GSMReady;

/* Exported functions ------------------------------------------------------- */ 
void vGSM_Task(void * pvParameters);

#endif

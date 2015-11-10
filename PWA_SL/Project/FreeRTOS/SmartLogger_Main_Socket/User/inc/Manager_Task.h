/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Manager_Task.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-February-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _MANAGER_TASK_H
#define _MANAGER_TASK_H

//#include "main.h"
#include <stdint.h>
#include "INT_STM32F4_ProtocolComm.h"

extern uint8_t DefEvSrc(uint8_t ch);

/*Define Alarm Source --------------------------------------------------------*/
#define SRC_SENS_EVENT              			0x70

#define SRC_420SENSn_EVENT(n) 						(DefEvSrc(n))
#define SRC_420SENSn_LOWER(n)             (DefEvSrc(n)+1)
#define SRC_420SENSn_LOWEREXTREME(n)      (DefEvSrc(n)+2)
#define SRC_420SENSn_UPPER(n)             (DefEvSrc(n)+3)
#define SRC_420SENSn_UPPEREXTREME(n)      (DefEvSrc(n)+4)
#define SRC_420SENSn_ERROR(n)             (DefEvSrc(n)+5)

#define SRC_DI_EVENT_ANY            			0x71

#define SRC_DI_EVENT(n)             			(0x40 | n)
#define SRC_DI_RISING(n)            			(0x50 | n)
#define SRC_DI_FALLING(n)           			(0x60 | n)

#define	MODE_FULL												0x01
#define	MODE_SAVING											0x02

void vManager_Task(void *pvParameters);
uint8_t ucMNG_ReadDS1820(float *temp);
/* Function Prototype --------------------------------------------------------*/
//extern PTC_RES ucSL_SendPacketToGSM(uint8_t *recvAddr, uint8_t *data, uint32_t dataLen, uint32_t timeout);
extern uint16_t read_uart(uint8_t *readbuf, uint16_t maxread, uint16_t interchar);


#endif /* _MANAGER_TASK_H */






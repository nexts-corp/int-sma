/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011 
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
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
  * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
 /**
  ******************************************************************************
  * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    main.h
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.    
  *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB and
  *          STM32F4DIS-LCD modules. 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ff.h"
#include "SL_ConfigStruct.h"

/*Communication Status -------------------------------------------------------*/
#define COMMU_NULL												(uint8_t)0x00
#define COMMU_CONNECTED										(uint8_t)0x01
#define COMMU_REG_PENDING									(uint8_t)0x02
#define COMMU_REGISTERED									(uint8_t)0x04
#define COMMU_JOIN_PENDING								(uint8_t)0x10
#define COMMU_JOINED											(uint8_t)0x20

#define GSM_CMD_CONNECT										(uint8_t)0x10
#define GSM_CMD_CLOSE											(uint8_t)0x20
#define GSM_CMD_RECONNECT									(uint8_t)0x30
#define GSM_CMD_HWRESET										(uint8_t)0x40
#define GSM_CMD_HWPWROFF									(uint8_t)0x50
#define GSM_CMD_HWPWRON										(uint8_t)0x60

#define UNCHECK_REGISTER									(uint8_t)0x00
#define CHECK_REGISTERED									(uint8_t)0x01

#define CONF_ACK_NONE											0
#define CONF_ACK_OK												1
#define CONF_ACK_ERR											2

/* Structure use in queue communication between task -------------------------*/
typedef struct {
	uint8_t 	packet[255];
	uint8_t		length;
}PACKET_DATA;

//extern xQueueHandle Queue_ConfigToSD;
extern xQueueHandle Queue_DataToSD;
extern xQueueHandle Queue_DataFromSD;
extern xQueueHandle Queue_DataLogToSD;
extern xQueueHandle Queue_DataLogFromSD;
extern xQueueHandle Queue_EventLogToSD;
extern xQueueHandle Queue_EventLogFromSD;
extern xQueueHandle Queue_ConfigLogToSD;
extern xQueueHandle Queue_ConfigLogFromSD;
extern xQueueHandle Queue_DebugToSD;
//extern xQueueHandle Queue_SaveRegis;
extern xQueueHandle Queue_CopyFromSD;
extern xQueueHandle Queue_SDTaskWDG;
extern xQueueHandle Queue_CtrlValve;
extern xQueueHandle Queue_CtrlValveMB;

extern __IO uint8_t 				flag_ReqSensorData;
extern __IO uint8_t					_Communication_Status;


/*-------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */  
void Time_Update(void);
void Delay(uint32_t nCount);
void delay_us(uint32_t delay);
void delay_ms(uint32_t delay);
void fatfs_err (FRESULT rc);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/


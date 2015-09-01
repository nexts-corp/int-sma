/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_DataQueue.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	28-April-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_DATAQUEUE_H
#define _INT_STM32F4_DATAQUEUE_H

#include <stdint.h>
#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_log.h"

#define MAX_DATAnSTAT_QUEUE		15
#define MAX_EVENT_QUEUE				5
#define MAX_CONFIG_QUEUE			5

typedef struct {
	uint32_t 		timestamp;
	uint8_t 		numStatusSet;
	STATUS_SET 	statusSet[MAX_STATUS];
	uint8_t			numDataSet;
	DATA_SET 		dataSet[MAX_DATA];
}DATAnSTAT_QUEUE;

typedef struct {
	uint32_t 		timestamp;
	uint8_t 		eid;
	uint32_t		eData;
}EVENT_QUEUE;

typedef struct {
	uint8_t 		accd;
	uint32_t 		timestamp;
	uint8_t 		cnfg;
	uint8_t 		spec[SIZE_MAX_SPEC]; 
	uint16_t 		spec_len;
}CONFIG_QUEUE;

uint8_t ucDATAQUEUE_AddDataStat(uint32_t timestamp, uint8_t numStat, STATUS_SET *statSet, uint8_t numDat, DATA_SET *datSet);
uint8_t ucDATAQUEUE_FetchDataStat(uint32_t *timestamp, uint8_t *numStat, STATUS_SET *statSet, uint8_t *numDat, DATA_SET *datSet);
uint8_t ucDATAQUEUE_GetDataAmount(void);
uint8_t ucDATAQUEUE_AddEvent(uint32_t timestamp, uint8_t eid, uint32_t eData);
uint8_t ucDATAQUEUE_FetchEvent(uint32_t *timestamp, uint8_t *eid, uint32_t *eData);
uint8_t ucDATAQUEUE_GetEventAmount(void);
uint8_t ucDATAQUEUE_AddConfig(uint8_t accd, uint32_t timestamp, uint8_t cnfg, void *spec, uint16_t specLen);
uint8_t ucDATAQUEUE_FetchConfig(uint8_t *accd, uint32_t *timestamp, uint8_t *cnfg, uint8_t *spec, uint16_t *specLen);
uint8_t ucDATAQUEUE_GetConfigAmount(void);

#endif /*_INT_STM32F4_DATAQUEUE_H*/





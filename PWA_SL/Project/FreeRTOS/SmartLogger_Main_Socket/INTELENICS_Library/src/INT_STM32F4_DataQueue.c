/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_DataQueue.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	28-April-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <string.h>
#include "INT_STM32F4_DataQueue.h"

/*-- Queue for data packet --*/
uint8_t 				dataStatQ_count = 0;
uint8_t					write_dataStatQ	= 0;
uint8_t					read_dataStatQ	= 0;
DATAnSTAT_QUEUE dataStat_Queue[MAX_DATAnSTAT_QUEUE];

/*-- Queue for event packet --*/
uint8_t 				eventQ_count 		= 0;
uint8_t					write_eventQ		= 0;
uint8_t					read_eventQ			= 0;
EVENT_QUEUE 		event_Queue[MAX_EVENT_QUEUE];

/*-- Queue for config packet --*/
uint8_t 				configQ_count 	= 0;
uint8_t					write_configQ		= 0;
uint8_t					read_configQ		= 0;
CONFIG_QUEUE 		config_Queue[MAX_CONFIG_QUEUE];


/*==========================================================================================*/
/**
  * @brief  Add data&status in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_AddDataStat(uint32_t timestamp, uint8_t numStat, STATUS_SET *statSet, uint8_t numDat, DATA_SET *datSet) {
	
	if(dataStatQ_count >= MAX_DATAnSTAT_QUEUE) {
		return 0;
	}
	dataStat_Queue[write_dataStatQ].timestamp 		= timestamp;
	dataStat_Queue[write_dataStatQ].numStatusSet 	= numStat;
	memcpy(&dataStat_Queue[write_dataStatQ].statusSet[0], &statSet[0], (numStat*sizeof(STATUS_SET)));
	dataStat_Queue[write_dataStatQ].numDataSet 		= numDat;
	memcpy(&dataStat_Queue[write_dataStatQ].dataSet[0], &datSet[0], (numDat*sizeof(DATA_SET)));
	
	write_dataStatQ = (write_dataStatQ + 1) % MAX_DATAnSTAT_QUEUE;
	return ++dataStatQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Fetch data&status from queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_FetchDataStat(uint32_t *timestamp, uint8_t *numStat, STATUS_SET *statSet, uint8_t *numDat, DATA_SET *datSet) {
	
	if(dataStatQ_count <= 0) {
		return 0;
	}
	*timestamp 	= dataStat_Queue[read_dataStatQ].timestamp;
	*numStat 		= dataStat_Queue[read_dataStatQ].numStatusSet;
	memcpy(&statSet[0], &dataStat_Queue[read_dataStatQ].statusSet[0], dataStat_Queue[read_dataStatQ].numStatusSet*sizeof(STATUS_SET));
	*numDat 		= dataStat_Queue[read_dataStatQ].numDataSet;
	memcpy(&datSet[0], &dataStat_Queue[read_dataStatQ].dataSet[0], dataStat_Queue[read_dataStatQ].numDataSet*sizeof(DATA_SET));
	
	read_dataStatQ = (read_dataStatQ + 1) % MAX_DATAnSTAT_QUEUE;
	return --dataStatQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Get number of data in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_GetDataAmount(void) {
	return dataStatQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Add event in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_AddEvent(uint32_t timestamp, uint8_t eid, uint32_t eData) {
	
	if(eventQ_count >= MAX_EVENT_QUEUE) {
		return 0;
	}
	event_Queue[write_eventQ].timestamp 	= timestamp;
	event_Queue[write_eventQ].eid 				= eid;
	event_Queue[write_eventQ].eData 			= eData;
	
	write_eventQ = (write_eventQ + 1) % MAX_EVENT_QUEUE;
	return ++eventQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Fetch event from queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_FetchEvent(uint32_t *timestamp, uint8_t *eid, uint32_t *eData) {
	
	if(eventQ_count <= 0) {
		return 0;
	}
	*timestamp 	= event_Queue[read_eventQ].timestamp;
	*eid 				= event_Queue[read_eventQ].eid;
	*eData 			= event_Queue[read_eventQ].eData;
	
	read_eventQ = (read_eventQ + 1) % MAX_EVENT_QUEUE;
	return --eventQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Get number of data in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_GetEventAmount(void) {
	return eventQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Add event in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_AddConfig(uint8_t accd, uint32_t timestamp, uint8_t cnfg, void *spec, uint16_t specLen) {
	
	if(configQ_count >= MAX_CONFIG_QUEUE) {
		return 0;
	}
	config_Queue[write_configQ].accd				= accd;
	config_Queue[write_configQ].timestamp 	= timestamp;
	config_Queue[write_configQ].cnfg				= cnfg;
	config_Queue[write_configQ].spec_len		= specLen;
	memcpy(config_Queue[write_configQ].spec, spec, specLen);
	
	write_configQ = (write_configQ + 1) % MAX_CONFIG_QUEUE;
	
	return ++configQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Fetch event from queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_FetchConfig(uint8_t *accd, uint32_t *timestamp, uint8_t *cnfg, uint8_t *spec, uint16_t *specLen) {
	
	if(configQ_count <= 0) {
		return 0;
	}
	*accd				= config_Queue[read_configQ].accd;
	*timestamp 	= config_Queue[read_configQ].timestamp;
	*cnfg				= config_Queue[read_configQ].cnfg;
	*specLen		= config_Queue[read_configQ].spec_len;
	memcpy(spec, config_Queue[read_configQ].spec, config_Queue[read_configQ].spec_len);
	
	read_configQ = (read_configQ + 1) % MAX_CONFIG_QUEUE;
	
	return --configQ_count;
}
/*==========================================================================================*/
/**
  * @brief  Get number of data in queue
  * @param  
  * @retval Number of data in Queue
  */
uint8_t ucDATAQUEUE_GetConfigAmount(void) {
	return configQ_count;
}
/*==========================================================================================*/




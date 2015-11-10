/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_PacketQueue.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	27-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"
#include "SD_Task.h"
#include "stm32f4_SDCard.h"
#include "INT_STM32F4_PacketQueue.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_ProtocolParser.h"


/* Function Prototype --------------------------------------------------------*/
static int8_t cPKTQUEUE_FindFreeSlot(PACKET_QUEUE *packetQueue);
static uint8_t ucPKTQUEUE_isIdExist(PACKET_QUEUE *packetQueue, uint8_t id);

/* Global Structure ----------------------------------------------------------*/
PACKET_QUEUE 					packet_Queue[MAX_PKT_QUEUE];
/* Global Variable -----------------------------------------------------------*/
uint8_t 								_Queue_counter = 0;
extern xQueueHandle 		Queue_PacketToGSM;
extern __IO bool				flag_isSDInstall;

/*==========================================================================================*/
/**
  * @brief  This function is initial PacketQueue
  * @param  PacketQueue
  * @retval None
  */
void vPKTQUEUE_Initial(PACKET_QUEUE *packetQueue) {
 
	uint8_t i;      
																
	for(i = 0; i < MAX_PKT_QUEUE; i++) {
//		free(packetQueue[i].packetData);
//		packetQueue[i].packetData  = NULL;
		packetQueue[i].packetLen   = 0;
		packetQueue[i].packetID    = 0;    
		packetQueue[i].retryCount  = 0;    
		packetQueue[i].timeout     = 0;    
		packetQueue[i].retryTime   = 0;    
	}
	return;                 
}
/*==========================================================================================*/
/**
  * @brief  This function is add packet PacketQueue
  * @param  
  * @retval Status
	*					 - 0x00 Success
	*					 - 0x01 PacketQueue is Full
	*					 - 0x02 Malloc failed
  */
uint8_t ucPKTQUEUE_Add(PACKET_QUEUE *packetQueue, uint8_t *queueCounter, uint8_t id, uint8_t *packet, uint8_t length, uint16_t timeout) {

	int8_t slot;
	
	slot = cPKTQUEUE_FindFreeSlot(packetQueue);
	if(slot < 0) {                         
		/* PacketQueue is Full */
		printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Add> Packet_queue full. \r\n", "");
		return 0x01;
	}                                                              
//	printDebug_Semphr(INFO, "%80sok1 ", "");
//	packetQueue[slot].packetData = (uint8_t *)pvPortMalloc(length);
//	if(packetQueue[slot].packetData == NULL) {
//		/* Malloc failed */
//		printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Add> malloc(%d) failed. \r\n", "", length);
//		return 0x02;
//	}             
//	printDebug_Semphr(INFO, "ok2 ");
	memcpy(packetQueue[slot].packetData, packet, length); 
	packetQueue[slot].packetLen   = length;
	packetQueue[slot].packetID    = id;    
	packetQueue[slot].retryCount  = 0;    
	packetQueue[slot].timeout     = timeout; 
																				 
	ucTIMER_SetTimer(&(packetQueue[slot].retryTime), packetQueue[slot].timeout);
	
	*queueCounter += 1;                              
	printDebug_Semphr(INFO, "%80s<PKTQUEUE_Add> FID[%02X] (num:%d)\r\n", "", id, *queueCounter);         
	return 0x00; 
}
/*==========================================================================================*/
/**
  * @brief  This function is remove packet PacketQueue
  * @param  
  * @retval Status
	*					 - 0x00 Success
	*					 - 0x01 Queue not found
  */
uint8_t ucPKTQUEUE_Remove(PACKET_QUEUE *packetQueue, uint8_t *queueCounter, uint8_t id) {
 
	uint8_t i;               
	
	for(i = 0; i < MAX_PKT_QUEUE; i++) {
		if(id == packetQueue[i].packetID) {        /* Scan id in PacketQueue */   
//			vPortFree(packetQueue[i].packetData);                    
//			packetQueue[i].packetData  = NULL;
			packetQueue[i].packetLen   = 0;
			packetQueue[i].packetID    = 0;    
			packetQueue[i].retryCount  = 0;    
			packetQueue[i].timeout     = 0;    
			packetQueue[i].retryTime   = 0;    
			*queueCounter -= 1; 																																 
			printDebug_Semphr(INFO, "%80s<PKTQUEUE_Remove> FID[%02X] \r\n", "", id); 
			return 0;         
		}
	}
	printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Remove> FID[%02X] not found!!\r\n", "", id);
	/* Queue not found */
	return 0x01;
}
/*==========================================================================================*/
/**
  * @brief  This function re-transmittion packet data
  * @param  PacketQueue
  * @retval None
  */
void vPKTQUEUE_Retransmit(PACKET_QUEUE *packetQueue, uint8_t *queueCounter) {

	uint8_t 			i;
	uint32_t 			timestamp		= 0;
	uint8_t 			frameType 	= 0;
	
//	FIL 					file;
//	FRESULT 			fatRes;
//	LOGRESULT 		logRes;
	PACKET_DATA 	packet_data;
	
	/* Data & Status Report */
	uint8_t				num_stat		= 0;
	uint8_t 			num_data		= 0;
	STATUS_SET 		status[MAX_STATUS];
	DATA_SET 			data[MAX_DATA];
	/* Event Report */
	uint8_t 			eid					=	0;
	uint32_t			edat				= 0;
	/* Configuration Management */
	uint8_t				accd;
	uint8_t 			cnfg;
	uint8_t				spec[SIZE_MAX_SPEC];
	uint16_t			specLen;
	
	ST_LOG_DATA 	log_data;
	ST_LOG_EVENT	log_event;
	ST_LOG_CONFIG log_config;
	
	for(i = 0; i < MAX_PKT_QUEUE; i++) {
		if(cTIMER_CheckTimerExceed(packetQueue[i].retryTime) && (packetQueue[i].packetID != 0)) {  
			if(packetQueue[i].retryCount >= struct_DevConfig.transmitRetry) {
				
				/* Packet timeout, Save packet to log file */
				frameType = ucPTCPARSER_getFrameType(packetQueue[i].packetData);
				timestamp = uiPTCPARSER_getTimestamp(packetQueue[i].packetData);
				if(frameType == DNSTREPC) {
					if(packetQueue[i].packetID == _DataLog_WaitID) {
						_DataLog_WaitID = 0x00;
					}else {
						if(flag_isSDInstall) {
							if(!ucPTCPARSER_IsolateDataPacket(packetQueue[i].packetData, packetQueue[i].packetLen, &num_stat, status, &num_data, data)) {					
								log_data.cmd 				= LOG_CMD_WRITE;
								log_data.timestamp 	= timestamp;
								log_data.num_status = num_stat;
								memcpy(log_data.status, status, (num_stat*sizeof(STATUS_SET)));
								log_data.num_data 	= num_data;
								memcpy(log_data.data, data, (num_data*sizeof(DATA_SET)));						
								if(!xQueueSend(Queue_DataLogToSD, &log_data, 0)) {
									printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit>: Send Queue_DataLogToSD failed. \r\n","");
								}
							}else {
								printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit> PacketData in queue error. \r\n", "");
							}
						}else {
							printDebug_Semphr(WARNING, "%80s<PKTQUEUE_Retransmit>: Couldn't save log_data, SD_Card not install. \r\n", "");
						}
					}
				}else if(frameType == EVENREPC) {
					if(packetQueue[i].packetID == _EventLog_WaitID) {
						_EventLog_WaitID = 0x00;
					}else {
						if(flag_isSDInstall) {
							if(!ucPTCPARSER_IsolateEventPacket(packetQueue[i].packetData,packetQueue[i].packetLen, &eid, &edat)) {
								log_event.cmd 				= LOG_CMD_WRITE;
								log_event.timestamp 	= timestamp;
								log_event.eid 				= eid;
								log_event.edata 			= edat;
								if(!xQueueSend(Queue_EventLogToSD, &log_event, 0)) {
									printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit>: Send Queue_EventLogToSD failed. \r\n","");
								}
							}else {
								printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit> PacketEvent in queue error. \r\n", "");
							}
						}else {
							printDebug_Semphr(WARNING, "%80s<PKTQUEUE_Retransmit>: Couldn't save log_event, SD_Card not install. \r\n", "");
						}
					}
				}else if(frameType == CONFMANC) {
					if(packetQueue[i].packetID == _ConfigLog_WaitID) {
						_ConfigLog_WaitID = 0x00;
					}else {
						if(flag_isSDInstall) {
							if(!ucPTCPARSER_IsolateConfigPacket(packetQueue[i].packetData,packetQueue[i].packetLen, &accd, &timestamp, &cnfg, spec, &specLen)) {
								log_config.cmd 				= LOG_CMD_WRITE;
								log_config.accd				= accd;
								log_config.timestamp 	= timestamp;
								log_config.cnfg				= cnfg;
								log_config.spec_len		= specLen;
								memcpy(log_config.spec, spec, specLen);
								if(!xQueueSend(Queue_ConfigLogToSD, &log_config, 0)) {
									printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit>: Send Queue_ConfigLogToSD failed. \r\n","");
								}
							}else {
								printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit> PacketConfig in queue error. \r\n", "");
							}
						}else {
							printDebug_Semphr(WARNING, "%80s<PKTQUEUE_Retransmit>: Couldn't save log_config, SD_Card not install. \r\n", "");
						}
					}
				}
				/* Reject packet from queue */
				ucPKTQUEUE_Remove(packetQueue, queueCounter, packetQueue[i].packetID);
			}else {                                                       
				packetQueue[i].retryCount++;      // increase retry counter 
				ucTIMER_SetTimer(&(packetQueue[i].retryTime), packetQueue[i].timeout);     
				printDebug_Semphr(INFO, "%80s<PKTQUEUE_Retransmit> FID[%02X] %d times\r\n","",packetQueue[i].packetID, packetQueue[i].retryCount);
				memcpy(packet_data.packet, packetQueue[i].packetData, packetQueue[i].packetLen);
				packet_data.length = packetQueue[i].packetLen;							
				if(!xQueueSend(Queue_PacketToGSM, &packet_data, 0)) {
					printDebug_Semphr(MINOR_ERR, "%80s<PKTQUEUE_Retransmit> Queue send failed. \r\n", "");
				}
			}
		}
	}   
	return;      
} 
/*==========================================================================================*/
/**
  * @brief  This function is generate Packet ID
  * @param  PacketQueue
  * @retval Packet ID
  */
uint8_t ucPKTQUEUE_GenerateID(PACKET_QUEUE *packetQueue) {
                 
	uint8_t id;

	do {
		id = (rand() % 256) & 0xFF;
	}while(ucPKTQUEUE_isIdExist(packetQueue, id) || (id == 0x00) || (id == 0xFF));
	
	return id;  
}
/*==========================================================================================*/


/*============================================================================================
Support Function
============================================================================================*/
/**
  * @brief  This function is find free slot of PacketQueue
  * @param  PacketQueue
  * @retval Slot
  */
static int8_t cPKTQUEUE_FindFreeSlot(PACKET_QUEUE *packetQueue) {

	uint8_t i;
	
	for(i = 0; i < MAX_PKT_QUEUE; i++) {
		if(packetQueue[i].packetLen == 0) {
			return i;    
		}
	}          
	return -1; 
}
/*==========================================================================================*/
/**
  * @brief  This function is check PacketID if exist in PacketQueue.
  * @param  PacketQueue, ID
  * @retval Status
	*					 - 0x00: If PacketID is not exist in PacketQueue.
	* 				 - 0x01: If PacketID is exist in PacketQueue.
  */ 
static uint8_t ucPKTQUEUE_isIdExist(PACKET_QUEUE *packetQueue, uint8_t id) {

	uint8_t i;       
	
	for(i = 0; i < MAX_PKT_QUEUE; i++) {
			if(id == packetQueue[i].packetID) {
					return 0x01;                           /* found id in PacketQueue */
			}
	}          
	return 0x00;
}
/*==========================================================================================*/








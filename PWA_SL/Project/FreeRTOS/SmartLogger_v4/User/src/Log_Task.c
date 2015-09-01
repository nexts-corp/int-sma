/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Log_Task.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	28-08-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"
#include "SD_TASK.h"
#include "stm32f4_SDCard.h"
#include "INT_STM32F4_Debug.h"
#include "INT_STM32F4_Log.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_DataQueue.h"
#include "INT_STM32F4_PacketQueue.h"

//static void vLOGTASK_SendDataLog(void);
//static void vLOGTASK_SendEventLog(void);
//static void vLOGTASK_SendConfigLog(void);

extern __IO bool 			flag_HaveDataLog;
extern __IO bool 			flag_HaveConfigLog;
extern __IO bool 			flag_HaveEventLog;

void vLog_Task(void *pvParameters) {
	
	uint32_t 	sendLogTimer = 0;
//	uint8_t 	dataQ_amount = 0;
	uint8_t		printLevel;
	uint8_t		j;
	
	ST_LOG_DATA 		log_data;
	ST_LOG_EVENT  	log_event;
	ST_LOG_CONFIG  	log_config;
	PTC_RES					ptc_res;

	printDebug_Semphr(DESCRIBE, "[LOG_TASK]: Running... \r\n");	
	while(1) {
		
		/* Device Enable -----------------------------------------------------*/
		if(struct_ConfigOperating.deviceEnable) {
			/* Send log up to Server ---------------------------------------------*/
			if((_Communication_Status & COMMU_CONNECTED) && (_Communication_Status & COMMU_JOINED)) {
				if(cTIMER_CheckTimerExceed(sendLogTimer)) {
					if(_Queue_counter == 0) {
						/*-- Request data log from SD_Task --*/
						if((_DataLog_WaitID == 0x00) && (flag_HaveDataLog)) {	  				/* Check, No log in during sent */
//							printDebug_Semphr(INFO, "[LOG_TASK]: Get data log. \r\n");
							log_data.cmd = LOG_CMD_READ;
							if(!xQueueSend(Queue_DataLogToSD, &log_data, 0)) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send Queue_DataLogToSD failed. \r\n");
							}
							delay_ms(50);		// delay for stable
						}
						/*-- Request event log from SD_Task --*/
						if((_EventLog_WaitID == 0x00) && (flag_HaveEventLog)) {					/* Check, No log in during sent */
//							printDebug_Semphr(INFO, "[LOG_TASK]: Get event log. \r\n");
							log_event.cmd = LOG_CMD_READ;
							if(!xQueueSend(Queue_EventLogToSD, &log_event, 0)) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send Queue_EventLogToSD failed. \r\n");
							}
							delay_ms(50);		// delay for stable
						}
						/*-- Request config log from SD_Task --*/
						if((_ConfigLog_WaitID == 0x00) && (flag_HaveConfigLog)) {					/* Check, No log in during sent */
//							printDebug_Semphr(INFO, "[LOG_TASK]: Get config log. \r\n");
							log_config.cmd = LOG_CMD_READ;
							if(!xQueueSend(Queue_ConfigLogToSD, &log_config, 0)) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send Queue_ConfigLogToSD failed. \r\n");
							}
							delay_ms(50);		// delay for stable
						}
					}
					ucTIMER_SetTimer(&sendLogTimer, 2);
				}
				
				/*-- Receive data log from SD_Task --*/
				if(_DataLog_WaitID == 0x00) {
					if(xQueueReceive(Queue_DataLogFromSD, &log_data, 0)) {
						if(log_data.cmd == LOG_ACK_OK) {
							if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
								printLevel = INFO;
								printDebug(printLevel, "[LOG_TASK]: Receive log from SD_Task \r\n");
								for(j = 0; j < log_data.num_data; j++) {
									printDebug(printLevel, "[LOG_TASK]: channel: %02X  ",log_data.data[j].channel);
									printDebug(printLevel, "dataUnit: %02X  ",log_data.data[j].dataUnit);
									printDebug(printLevel, "sensorType: %02X  ",log_data.data[j].sensorType);
									printDebug(printLevel, "value: %0.4f  \r\n",log_data.data[j].value);
								}
								xSemaphoreGive(xSemaphore);
							}
							/*-- Send Data&Status Report to Server --*/
							printDebug_Semphr(INFO, "[LOG_TASK]: Send data log \r\n");
							ptc_res = xPROCOMM_SendDataNStatusReportCMD(log_data.timestamp, RECORD_LOG, log_data.num_status, log_data.status, log_data.num_data, log_data.data, struct_DevConfig.retransmitInterval, GSM_PORT);
							if(ptc_res != PTC_OK) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send packet failed(PTC_%d) \r\n", ptc_res);
							}
						}else if(log_data.cmd == LOG_ACK_EMPTY) {
							
						}else if(log_data.cmd == LOG_ACK_FAIL) {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Ack error from SD_Task. \r\n");
						}else {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Unknown data ack from SD_Task (0x%02X) \r\n", log_data.cmd);
						}
					}
				}
				
				/*-- Receive event log from SD_Task --*/
				if(_EventLog_WaitID == 0x00) {	
					if(xQueueReceive(Queue_EventLogFromSD, &log_event, 0)) {
						if(log_event.cmd == LOG_ACK_OK) {
							if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
								printLevel = DETAIL;
								printDebug(printLevel, "[LOG_TASK]: Receive event log from SD_Task \r\n");
								printDebug(printLevel, "[LOG_TASK]: Timestamp: %d  ", log_event.timestamp);
								printDebug(printLevel, "EID: %02X  ", log_event.eid);
								printDebug(printLevel, "EDATA: %08X \r\n", log_event.edata);
								xSemaphoreGive(xSemaphore);
							}
							
							/*-- Send Event Report to Server --*/
							printDebug_Semphr(INFO, "[LOG_TASK]: Send event log \r\n");
							ptc_res = xPROCOMM_SendEventReportCMD(log_event.timestamp, RECORD_LOG, log_event.eid, log_event.edata, struct_DevConfig.retransmitInterval);
							if(ptc_res != PTC_OK) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send packet failed(PTC_%d) \r\n", ptc_res);
							}
						}else if(log_event.cmd == LOG_ACK_EMPTY) {
							
						}else if(log_event.cmd == LOG_ACK_FAIL) {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Ack error from SD_Task. \r\n");
						}else {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Unknown event ack from SD_Task (0x%02X) \r\n", log_event.cmd);
						}
					}
				}
				
				/*-- Receive config log from SD_Task --*/
				if(_ConfigLog_WaitID == 0x00) {
					if(xQueueReceive(Queue_ConfigLogFromSD, &log_config, 0)) {
						if(log_config.cmd == LOG_ACK_OK) {
							if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
								printLevel = INFO;
								printDebug(printLevel, "[LOG_TASK]: Receive config log from SD_Task \r\n");
								printDebug(printLevel, "[LOG_TASK]: Timestamp: %d  ", log_config.timestamp);
								printDebug(printLevel, "ACCD: %02X  ", log_config.accd);
								printDebug(printLevel, "CNFG: %02X ", log_config.cnfg);
								printDebug(printLevel, "SpecLen: %d \r\n", log_config.spec_len);
								xSemaphoreGive(xSemaphore);
							}
							
							/*-- Send Event Report to Server --*/
							printDebug_Semphr(INFO, "[LOG_TASK]: Send config log \r\n");
							ptc_res = xPROCOMM_SendConfigManageCMD(log_config.accd, RECORD_LOG, log_config.timestamp, log_config.cnfg, log_config.spec, log_config.spec_len, struct_DevConfig.retransmitInterval);
							if(ptc_res != PTC_OK) {
								printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Send packet failed(PTC_%d) \r\n", ptc_res);
							}
						}else if(log_config.cmd == LOG_ACK_EMPTY) {
							
						}else if(log_config.cmd == LOG_ACK_FAIL) {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Ack error from SD_Task. \r\n");
						}else {
							printDebug_Semphr(MINOR_ERR, "[LOG_TASK]: Unknown config ack from SD_Task (0x%02X) \r\n", log_config.cmd);
						}
					}
				}
					
			}
		}/* Device Enable */
		
	}/* Main Loop */
}/* vLog_Task */
/*==========================================================================================*/




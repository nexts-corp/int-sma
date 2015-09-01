/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_Log.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	5-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "stm32f4_SDCard.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_csvfile.h"

extern void printDebug_Semphr(uint8_t Debug_ID, char *fmtstr, ...);

static LOGRESULT xLOG_SetIndex(FIL *file, uint32_t indexAddr, uint32_t index);
static uint32_t uiLOG_Index2Addr(uint32_t index, uint32_t recordSize);

/*Flag Send*/
const uint8_t FLAG_NOT_SEND 	=	0xAA;
const uint8_t FLAG_SENT				= 0xFF;

volatile bool flag_dataLogOverflow = false;
volatile bool flag_eventLogOverflow = false;
volatile bool flag_configLogOverflow = false;

/*============================================================================================
User Function
============================================================================================*/
/**
  * @brief  Save Debug_Log into Log_File (Log limited 100 bytes)
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_WriteDebugLog(FIL *file, uint32_t timestamp, char *debug_log) {
	
	char						str_date[20];
	char						str_log[280];
	uint32_t				i;
	uint32_t  			writeAddr		= 0;
//	uint32_t				startAddr			=	0;
	uint32_t				byte_counter	= 0;
	uint32_t				log_size			= 0;
	
	FRESULT					fatfs_res;
	LOGRESULT				log_res;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	/*-- Make string date/time --*/
	timestamp += (3600*7); 	// GMT+7
	vRTC_EpochToDateTime(&RTC_DateStruct, &RTC_TimeStruct, timestamp);
	/*-- Date --*/
	if(RTC_DateStruct.RTC_Date < 10) { 		// Date
		sprintf(&str_date[0], "%d", 0);
		sprintf(&str_date[1], "%d/", RTC_DateStruct.RTC_Date);
	}else {
		sprintf(&str_date[0], "%d/", RTC_DateStruct.RTC_Date);
	}
	if(RTC_DateStruct.RTC_Month < 10) {		// Month
		sprintf(&str_date[3], "%d", 0);
		sprintf(&str_date[4], "%d/", RTC_DateStruct.RTC_Month);
	}else {
		sprintf(&str_date[3], "%d/", RTC_DateStruct.RTC_Month);
	}
	sprintf(&str_date[6], "%d ", (RTC_DateStruct.RTC_Year+2000));		// Year
	/*-- Time --*/
	if(RTC_TimeStruct.RTC_Hours < 10) { 		// Hours
		sprintf(&str_date[11], "%d", 0);
		sprintf(&str_date[12], "%d:", RTC_TimeStruct.RTC_Hours);
	}else {
		sprintf(&str_date[11], "%d:", RTC_TimeStruct.RTC_Hours);
	}
	if(RTC_TimeStruct.RTC_Minutes < 10) {		// Minutes
		sprintf(&str_date[14], "%d", 0);
		sprintf(&str_date[15], "%d:", RTC_TimeStruct.RTC_Minutes);
	}else {
		sprintf(&str_date[14], "%d:", RTC_TimeStruct.RTC_Minutes);
	}
	if(RTC_TimeStruct.RTC_Seconds < 10) {		// Second
		sprintf(&str_date[17], "%d", 0);
		sprintf(&str_date[18], "%d ", RTC_TimeStruct.RTC_Seconds);
	}else {
		sprintf(&str_date[17], "%d ", RTC_TimeStruct.RTC_Seconds);
	}
	
	memcpy(&str_log[0], str_date, sizeof(str_date));
	byte_counter += sizeof(str_date);
	for(i = 0; i < 100; i++) {
		log_size++;
		if(debug_log[i] == '\n') {
			break;
		}
	}
	memcpy(&str_log[20], debug_log, log_size);
	byte_counter += log_size;
	
//	printDebug_Semphr(INFO, "<LOG_WriteDebugLog>:size %d, %s \r\n", byte_counter, str_log);
	
		/*-- Get Write Address --*/
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeAddr);
	if(log_res != LOG_FR_OK) {
		return log_res;
	}
	
	fatfs_res = xSDCARD_WriteData(file, writeAddr, str_log, byte_counter);
	if(fatfs_res != FR_OK) {
		return (LOGRESULT)fatfs_res;
	}
	
	/*-- Set next write address --*/
	writeAddr += byte_counter;
	log_res = xLOG_SetIndex(file, WRITE_INDEX_ADDR, writeAddr);
	if(log_res != FR_OK) {
		return log_res;
	}
	
	return LOG_FR_OK;
}
/*==========================================================================================*/
/**
  * @brief  Save Data&Status in Backup Data&Status_File
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_SaveData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	lastIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	uint8_t 	strWr[SIZE_MAXDAT];   // reserve in case max data
	uint8_t 	statSize 		= numStat*sizeof(STATUS_SET);
	uint8_t 	dataSize		= numData*sizeof(DATA_SET);
//	char			strCSV[CSV_RECORD_SIZE];
	
//	ucCSVFILE_AssemCSV(strCSV, timestamp, numStat, status, numData, data);
	
	/*-- Get Last Record Index --*/
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &lastIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/

	/*-- Check, Is file full --*/
	if(lastIndex >= MAX_DATAnSTAT_INDEX) {
		printDebug_Semphr(WARNING, "%80s<LOG_SaveData>: Backup data file is full (num:%d) \r\n", "", lastIndex);
		return LOG_FULL;
	}

	startAddr = uiLOG_Index2Addr(lastIndex, SIZE_LOG_DATA);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NSTAT, &numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, status, statSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_STAT, strWr, statSize);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NDAT, &numData, SIZE_NDAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, data, dataSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_DAT, strWr, dataSize);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	
	/*-- Update index --*/
	lastIndex++;
	log_res = xLOG_SetIndex(file, WRITE_INDEX_ADDR, lastIndex);
	if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	
	printDebug_Semphr(INFO, "%80s<LOG_SaveData>: Amount:%d \r\n", "", lastIndex);
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Get Data&Status in Backup Data_File 
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint8_t		num_data 		= 0;
	uint8_t		num_status 	= 0;
	uint32_t	lastIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	
	/*-- Get Last Record Index --*/
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &lastIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/

	if(lastIndex == 0) {
		return LOG_EMPTY;
	}
	if(getIndex >= lastIndex) {
		return LOG_INVALID_INDEX;
	}
	
	startAddr = uiLOG_Index2Addr(getIndex, SIZE_LOG_DATA);
	
	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NSTAT, numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	num_status = *numStat;
	res = xSDCARD_ReadData(file, startAddr+OFFSET_STAT, status, num_status*sizeof(STATUS_SET));
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/

	res = xSDCARD_ReadData(file, startAddr+OFFSET_NDAT, numData, SIZE_NDAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/

	num_data = *numData;
	res = xSDCARD_ReadData(file, startAddr+OFFSET_DAT, data, num_data*sizeof(DATA_SET));
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	printDebug_Semphr(INFO, "%80s<LOG_GetData>: READ:%d \r\n", "", getIndex);
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Write Data&Status Log in Data&Status Log_File
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_WriteData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	writeIndex 	= 0;
	uint32_t  readIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	uint8_t 	strWr[SIZE_MAXDAT];   // reserve in case max data
	uint8_t 	statSize 		= numStat*sizeof(STATUS_SET);
	uint8_t 	dataSize		= numData*sizeof(DATA_SET);
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/

	startAddr = uiLOG_Index2Addr(writeIndex, SIZE_LOG_DATA);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NSTAT, &numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, status, statSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_STAT, strWr, statSize);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NDAT, &numData, SIZE_NDAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, data, dataSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_DAT, strWr, dataSize);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	
	writeIndex++;
	if(writeIndex >= MAX_DATA_INDEX) {
		writeIndex = 0;
	}
	if(writeIndex == readIndex) {
		/* WARNING: Log is overflow */
		flag_dataLogOverflow = true;
		if(++readIndex >= MAX_DATA_INDEX) {readIndex = 0;}
		log_res = xLOG_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	}
	
	printDebug_Semphr(INFO, "%80s<LOG_WriteData>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_dataLogOverflow);

	log_res = xLOG_SetIndex(file, WRITE_INDEX_ADDR, writeIndex);
	if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Data&Status Log in Data Log_File 
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_ReadData(FIL *file, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint8_t		num_data = 0;
	uint8_t		num_status = 0;
	uint32_t	readIndex = 0;
	uint32_t	writeIndex = 0;
	uint32_t 	startAddr = 0;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {
		return log_res;			/*xLOG_GetIndex error*/
	}
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {
		return log_res;			/*xLOG_GetIndex error*/
	}
	
	if(readIndex == writeIndex) {
		/*Data&Status Log is empty*/
		return LOG_EMPTY;
	}
	
	startAddr = uiLOG_Index2Addr(readIndex, SIZE_LOG_DATA);
	
	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {
		return (LOGRESULT)res;		/*xSDCARD_ReadData error*/
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NSTAT, numStat, SIZE_NSTAT);
	if(res != FR_OK) {
		return (LOGRESULT)res;		/*xSDCARD_ReadData error*/
	}else {
		num_status = *numStat;
		if(num_status > MAX_STATUS) {
			printDebug_Semphr(INFO, "%80s<LOG_ReadData>: Log Error, Index_%d (NumStat: %d) \r\n", "", readIndex, num_status);
			return LOG_ERR;
		}
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_STAT, status, num_status*sizeof(STATUS_SET));
	if(res != FR_OK) {
		return (LOGRESULT)res;		/*xSDCARD_ReadData error*/
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NDAT, numData, SIZE_NDAT);
	if(res != FR_OK) {
		return (LOGRESULT)res;		/*xSDCARD_ReadData error*/
	}else {
		num_data = *numData;
		if(num_data > MAX_DATA) {
			printDebug_Semphr(INFO, "%80s<LOG_ReadData>: Log Error, Index_%d (NumData: %d) \r\n", "", readIndex, num_data);
			return LOG_ERR;
		}
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_DAT, data, num_data*sizeof(DATA_SET));
	if(res != FR_OK) {
		return (LOGRESULT)res;		/*xSDCARD_ReadData error*/
	}
	if((readIndex-1) == writeIndex) {
		flag_dataLogOverflow = false;
	}
	printDebug_Semphr(INFO, "%80s<LOG_ReadData>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_dataLogOverflow);
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Write event log in Event Log_File
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_WriteEvent(FIL *file, uint32_t timestamp, uint8_t eid, uint32_t edat) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	writeIndex = 0, readIndex = 0;
	uint32_t 	startAddr = 0;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	
	startAddr = uiLOG_Index2Addr(writeIndex, SIZE_LOG_EVENT);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	
	res = xSDCARD_WriteData(file, startAddr+OFFSET_EID, &eid, SIZE_EID);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	
	res = xSDCARD_WriteData(file, startAddr+OFFSET_EDAT, &edat, SIZE_EDAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	writeIndex++;
	if(writeIndex >= MAX_EVENT_INDEX) {
		writeIndex = 0;
	}
	if(writeIndex == readIndex) {
		/* WARNING: Log is overflow */
		flag_eventLogOverflow = true;
		if(++readIndex >= MAX_EVENT_INDEX) {readIndex = 0;}
		log_res = xLOG_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	}
	
	printDebug_Semphr(INFO, "%80s<LOG_WriteEvent>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_eventLogOverflow);
	
	log_res = xLOG_SetIndex(file, WRITE_INDEX_ADDR, writeIndex);
	if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Event_Log in Event Log_File 
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_ReadEvent(FIL *file, uint32_t *timestamp, uint8_t *eid, uint32_t *edat) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	readIndex = 0, writeIndex = 0;
	uint32_t 	startAddr = 0;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	
	if(readIndex == writeIndex) {
		/*Data&Status Log is empty*/
		return LOG_EMPTY;
	}

	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetLastIndexLog error*/
	
	startAddr = uiLOG_Index2Addr(readIndex, SIZE_LOG_EVENT);

	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	res = xSDCARD_ReadData(file, startAddr+OFFSET_EID, eid, SIZE_EID);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	res = xSDCARD_ReadData(file, startAddr+OFFSET_EDAT, edat, SIZE_EDAT);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	
	if((readIndex-1) == writeIndex) {
		flag_eventLogOverflow = false;
	}
	
	printDebug_Semphr(INFO, "%80s<LOG_ReadEvent>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_eventLogOverflow);
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Write config log in Config Log_File
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_WriteConfig(FIL *file, uint32_t timestamp, uint8_t accd, uint8_t cnfg, void *spec, uint8_t specLen) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	writeIndex = 0, readIndex = 0;
	uint32_t 	startAddr = 0;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	
	startAddr = uiLOG_Index2Addr(writeIndex, SIZE_LOG_CONFIG);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_WriteData(file, startAddr+OFFSET_ACCD, &accd, SIZE_ACCD);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_WriteData(file, (startAddr+OFFSET_CNFG), &cnfg, SIZE_CNFG);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_WriteData(file, (startAddr+OFFSET_SPECLEN), &specLen, SIZE_SPECLEN);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_WriteData(file, (startAddr+OFFSET_SPEC), spec, specLen);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/

	writeIndex++;
	if(writeIndex >= MAX_CONFIG_INDEX) {
		writeIndex = 0;
	}
	if(writeIndex == readIndex) {
		/* WARNING: Log is overflow */
		flag_configLogOverflow = true;
		if(++readIndex >= MAX_CONFIG_INDEX) {readIndex = 0;}
		log_res = xLOG_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	}
	
	printDebug_Semphr(INFO, "%80s<LOG_WriteConfig>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_configLogOverflow);
	
	log_res = xLOG_SetIndex(file, WRITE_INDEX_ADDR, writeIndex);
	if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Config_Log in Config Log_File 
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_ReadConfig(FIL *file, uint32_t *timestamp, uint8_t *accd, uint8_t *cnfg, void *spec, uint8_t *spec_len) {
	
	FRESULT		res;
	LOGRESULT	log_res;
	uint32_t	readIndex = 0, writeIndex = 0;
	uint32_t 	startAddr = 0;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	
	if(readIndex == writeIndex) {
		/*Data&Status Log is empty*/
		return LOG_EMPTY;
	}

	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetLastIndexLog error*/
	
	startAddr = uiLOG_Index2Addr(readIndex, SIZE_LOG_CONFIG);

	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_ReadData error*/
	res = xSDCARD_ReadData(file, startAddr+OFFSET_ACCD, accd, SIZE_ACCD);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_ReadData(file, (startAddr+OFFSET_CNFG), cnfg, SIZE_CNFG);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_ReadData(file, (startAddr+OFFSET_SPECLEN), spec_len, SIZE_SPECLEN);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	res = xSDCARD_ReadData(file, (startAddr+OFFSET_SPEC), spec, *spec_len);
	if(res != FR_OK) {return (LOGRESULT)res;}			/*xSDCARD_WriteData error*/
	
	if((readIndex-1) == writeIndex) {
		flag_configLogOverflow = false;
	}
	printDebug_Semphr(INFO, "%80s<LOG_ReadConfig>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_configLogOverflow);
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Data&Status Log in Data Log_File 
  * @param 	
  * @retval Log_File function return code
  */
LOGRESULT xLOG_SetNextReadIndex(FIL *file, uint8_t file_id) {
	
	uint32_t	max_index = 0;
	uint32_t	readIndex = 0;
	uint32_t	writeIndex = 0;
	FRESULT		res;
	LOGRESULT	log_res;
	
	log_res = xLOG_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/
	log_res = xLOG_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(log_res != LOG_FR_OK) {return log_res;}			/*xLOG_GetIndex error*/

	if(file_id == ID_DATALOG) {
		max_index = MAX_DATA_INDEX;
	}else if(file_id == ID_EVENTLOG) {
		max_index = MAX_EVENT_INDEX;
	}else if(file_id == ID_CONFIGLOG) {
		max_index = MAX_CONFIG_INDEX;
	}else {
		return LOG_INVALID_LOGID;
	}

	/* Move to next index */
	readIndex += 1;    
	if(readIndex >= max_index) {
		readIndex = 0;
	}
	if(readIndex == writeIndex) {
		/* Clear Log */ 
		readIndex = 0;
		writeIndex = 0;
		/* Create new file replace old file */
		if(file_id == ID_DATALOG) {
			res = xSDCARD_CreateFile(file, (const char*)LOG_DATAnSTAT, CREATE_ALWAYS);
			if(res != FR_OK) {
				return (LOGRESULT)res;
			}
		}else if(file_id == ID_EVENTLOG) {
			res = xSDCARD_CreateFile(file, (const char*)LOG_EVENT, CREATE_ALWAYS);
			if(res != FR_OK) {
				return (LOGRESULT)res;
			}
		}else if(file_id == ID_CONFIGLOG) {
			res = xSDCARD_CreateFile(file, (const char*)LOG_CONFIG, CREATE_ALWAYS);
			if(res != FR_OK) {
				return (LOGRESULT)res;
			}
		}else {
			return LOG_INVALID_LOGID;
		}
		/* Set WRITE_INDEX as default (0x00000000) */
		res = xSDCARD_WriteData(file, WRITE_INDEX_ADDR, &writeIndex, sizeof(uint32_t));
		if(res != FR_OK) {return (LOGRESULT)res;}
		/* Set READ_INDEX as default (0x00000000) */
		res = xSDCARD_WriteData(file, READ_INDEX_ADDR, &readIndex, sizeof(uint32_t));
		if(res != FR_OK) {return (LOGRESULT)res;}
	}else {
		log_res = xLOG_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(log_res != FR_OK) {return log_res;}			/*xLOG_SetIndex error*/
	}

	return log_res;
}
/*==========================================================================================*/




/*============================================================================================
Support Function
============================================================================================*/
/**
  * @brief  Get index of last log
  * @param 	
  * @retval File function return code
  */
LOGRESULT xLOG_GetIndex(FIL *file, uint32_t indexAddr, uint32_t *index) {
	
	FRESULT		res;
	
	res = xSDCARD_ReadData(file, indexAddr, index, sizeof(uint32_t));
	return (LOGRESULT)res;
}
/*==========================================================================================*/
/**
  * @brief  Set index of last log
  * @param 	
  * @retval File function return code
  */
static LOGRESULT xLOG_SetIndex(FIL *file, uint32_t indexAddr, uint32_t index) {
	
	FRESULT		res;
	
	res = xSDCARD_WriteData(file, indexAddr, &index, sizeof(uint32_t));
	return (LOGRESULT)res;
}
/*==========================================================================================*/
/**
  * @brief  Convert index to real address in SD Card
  * @param 	
  * @retval File function return code
  */
static uint32_t uiLOG_Index2Addr(uint32_t index, uint32_t recordSize) {
	
	uint32_t 	address = 0;
	
	address = (index*recordSize)+INDEX_SIZE;
	
	return address;
}
/*==========================================================================================*/


/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_RECORD.c
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
#include "INT_STM32F4_Record.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_csvfile.h"

extern void printDebug_Semphr(uint8_t Debug_ID, char *fmtstr, ...);

static RECORDRESULT xRECORD_SetIndex(FIL *file, uint32_t indexAddr, uint32_t index);
static uint32_t uiRECORD_Index2Addr(uint32_t index, uint32_t recordSize);

volatile bool flag_dataRecordOverflow = false;

/*============================================================================================
User Function
============================================================================================*/
/**
  * @brief  Save Data&Status in Record file
  * @param 	
  * @retval Record_File function return code
  */
RECORDRESULT xRECORD_SaveData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data) {
	
	FRESULT		res;
	RECORDRESULT	record_res;
	uint32_t	lastIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	uint8_t 	strWr[SIZE_MAXDAT];   // reserve in case max data
	uint8_t 	statSize 		= numStat*sizeof(STATUS_SET);
	uint8_t 	dataSize		= numData*sizeof(DATA_SET);
//	char			strCSV[CSV_RECORD_SIZE];
	
//	ucCSVFILE_AssemCSV(strCSV, timestamp, numStat, status, numData, data);
	
	/*-- Get Last Record Index --*/
	record_res = xRECORD_GetIndex(file, WRITE_INDEX_ADDR, &lastIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/

	/*-- Check, Is file full --*/
	if(lastIndex >= MAX_DATAnSTAT_INDEX) {
		printDebug_Semphr(WARNING, "%80s<RECORD_SaveData>: Backup data file is full (num:%d) \r\n", "", lastIndex);
		return RECORD_FULL;
	}

	startAddr = uiRECORD_Index2Addr(lastIndex, SIZE_RECORD_DATA);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NSTAT, &numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, status, statSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_STAT, strWr, statSize);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NDAT, &numData, SIZE_NDAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, data, dataSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_DAT, strWr, dataSize);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/
	
	/*-- Update index --*/
	lastIndex++;
	record_res = xRECORD_SetIndex(file, WRITE_INDEX_ADDR, lastIndex);
	if(record_res != FR_OK) {return record_res;}			/*xRECORD_SetIndex error*/
	
	printDebug_Semphr(INFO, "%80s<RECORD_SaveData>: Amount:%d \r\n", "", lastIndex);
	
	return record_res;
}
/*==========================================================================================*/
/**
  * @brief  Get Data&Status in Backup Data_File 
  * @param 	
  * @retval RECORD_File function return code
  */
RECORDRESULT xRECORD_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data) {
	
	FRESULT		res;
	RECORDRESULT	record_res;
	uint8_t		num_data 		= 0;
	uint8_t		num_status 	= 0;
	uint32_t	lastIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	
	/*-- Get Last Record Index --*/
	record_res = xRECORD_GetIndex(file, WRITE_INDEX_ADDR, &lastIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/

	if(lastIndex == 0) {
		return RECORD_EMPTY;
	}
	if(getIndex >= lastIndex) {
		return RECORD_INVALID_INDEX;
	}
	
	startAddr = uiRECORD_Index2Addr(getIndex, SIZE_RECORD_DATA);
	
	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_ReadData error*/
	
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NSTAT, numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_ReadData error*/
	
	num_status = *numStat;
	res = xSDCARD_ReadData(file, startAddr+OFFSET_STAT, status, num_status*sizeof(STATUS_SET));
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_ReadData error*/

	res = xSDCARD_ReadData(file, startAddr+OFFSET_NDAT, numData, SIZE_NDAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_ReadData error*/

	num_data = *numData;
	res = xSDCARD_ReadData(file, startAddr+OFFSET_DAT, data, num_data*sizeof(DATA_SET));
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_ReadData error*/
	
	printDebug_Semphr(INFO, "%80s<RECORD_GetData>: READ:%d \r\n", "", getIndex);
	
	return record_res;
}
/*==========================================================================================*/
/**
  * @brief  Write Data&Status RECORD in Data&Status RECORD_File
  * @param 	
  * @retval RECORD_File function return code
  */
RECORDRESULT xRECORD_WriteData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data) {
	
	FRESULT		res;
	RECORDRESULT	record_res;
	uint32_t	writeIndex 	= 0;
	uint32_t  readIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	uint8_t 	strWr[SIZE_MAXDAT];   // reserve in case max data
	uint8_t 	statSize 		= numStat*sizeof(STATUS_SET);
	uint8_t 	dataSize		= numData*sizeof(DATA_SET);
	
	record_res = xRECORD_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/
	record_res = xRECORD_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/

	startAddr = uiRECORD_Index2Addr(writeIndex, SIZE_RECORD_DATA);

	res = xSDCARD_WriteData(file, startAddr, &timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NSTAT, &numStat, SIZE_NSTAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, status, statSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_STAT, strWr, statSize);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	res = xSDCARD_WriteData(file, startAddr+OFFSET_NDAT, &numData, SIZE_NDAT);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/

	memcpy(strWr, data, dataSize);
	res = xSDCARD_WriteData(file, startAddr+OFFSET_DAT, strWr, dataSize);
	if(res != FR_OK) {return (RECORDRESULT)res;}			/*xSDCARD_WriteData error*/
	
	writeIndex++;
	if(writeIndex >= MAX_DATA_INDEX) {
		writeIndex = 0;
	}
	if(writeIndex == readIndex) {
		/* WARNING: RECORD is overflow */
		flag_dataRecordOverflow = true;
		if(++readIndex >= MAX_DATA_INDEX) {readIndex = 0;}
		record_res = xRECORD_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(record_res != FR_OK) {return record_res;}			/*xRECORD_SetIndex error*/
	}
	
	printDebug_Semphr(INFO, "%80s<RECORD_WriteData>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_dataRecordOverflow);

	record_res = xRECORD_SetIndex(file, WRITE_INDEX_ADDR, writeIndex);
	if(record_res != FR_OK) {return record_res;}			/*xRECORD_SetIndex error*/
	
	return record_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Data&Status RECORD in Data RECORD_File 
  * @param 	
  * @retval RECORD_File function return code
  */
RECORDRESULT xRECORD_ReadData(FIL *file, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data) {
	
	FRESULT		res;
	RECORDRESULT	record_res;
	uint8_t		num_data = 0;
	uint8_t		num_status = 0;
	uint32_t	readIndex = 0;
	uint32_t	writeIndex = 0;
	uint32_t 	startAddr = 0;
	
	record_res = xRECORD_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(record_res != RECORD_FR_OK) {
		return record_res;			/*xRECORD_GetIndex error*/
	}
	record_res = xRECORD_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(record_res != RECORD_FR_OK) {
		return record_res;			/*xRECORD_GetIndex error*/
	}
	
	if(readIndex == writeIndex) {
		/*Data&Status RECORD is empty*/
		return RECORD_EMPTY;
	}
	
	startAddr = uiRECORD_Index2Addr(readIndex, SIZE_RECORD_DATA);
	
	res = xSDCARD_ReadData(file, startAddr, timestamp, SIZE_TIMESTAMP);
	if(res != FR_OK) {
		return (RECORDRESULT)res;		/*xSDCARD_ReadData error*/
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NSTAT, numStat, SIZE_NSTAT);
	if(res != FR_OK) {
		return (RECORDRESULT)res;		/*xSDCARD_ReadData error*/
	}else {
		num_status = *numStat;
		if(num_status > MAX_STATUS) {
			printDebug_Semphr(INFO, "%80s<RECORD_ReadData>: RECORD Error, Index_%d (NumStat: %d) \r\n", "", readIndex, num_status);
			return RECORD_ERR;
		}
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_STAT, status, num_status*sizeof(STATUS_SET));
	if(res != FR_OK) {
		return (RECORDRESULT)res;		/*xSDCARD_ReadData error*/
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_NDAT, numData, SIZE_NDAT);
	if(res != FR_OK) {
		return (RECORDRESULT)res;		/*xSDCARD_ReadData error*/
	}else {
		num_data = *numData;
		if(num_data > MAX_DATA) {
			printDebug_Semphr(INFO, "%80s<RECORD_ReadData>: RECORD Error, Index_%d (NumData: %d) \r\n", "", readIndex, num_data);
			return RECORD_ERR;
		}
	}
	res = xSDCARD_ReadData(file, startAddr+OFFSET_DAT, data, num_data*sizeof(DATA_SET));
	if(res != FR_OK) {
		return (RECORDRESULT)res;		/*xSDCARD_ReadData error*/
	}
	if((readIndex-1) == writeIndex) {
		flag_dataRecordOverflow = false;
	}
	printDebug_Semphr(INFO, "%80s<RECORD_ReadData>: WRITE:%d  READ:%d  [%d]\r\n", "", writeIndex, readIndex, flag_dataRecordOverflow);
	
	return record_res;
}
/*==========================================================================================*/
/**
  * @brief  Read Data&Status RECORD in Data RECORD_File 
  * @param 	
  * @retval RECORD_File function return code
  */
RECORDRESULT xRECORD_SetNextReadIndex(FIL *file, uint8_t file_id) {
	
	uint32_t	max_index = 0;
	uint32_t	readIndex = 0;
	uint32_t	writeIndex = 0;
	FRESULT		res;
	RECORDRESULT	record_res;
	
	record_res = xRECORD_GetIndex(file, WRITE_INDEX_ADDR, &writeIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/
	record_res = xRECORD_GetIndex(file, READ_INDEX_ADDR, &readIndex);
	if(record_res != RECORD_FR_OK) {return record_res;}			/*xRECORD_GetIndex error*/

	if(file_id == ID_DATARECORD) {
		max_index = MAX_DATA_INDEX;
	}else {
		return RECORD_INVALID_RECORDID;
	}

	/* Move to next index */
	readIndex += 1;    
	if(readIndex >= max_index) {
		readIndex = 0;
	}
	if(readIndex == writeIndex) {
		/* Clear RECORD */ 
		readIndex = 0;
		writeIndex = 0;
		/* Create new file replace old file */
		if(file_id == ID_DATARECORD) {
			res = xSDCARD_CreateFile(file, (const char*)RECORD_DATAnSTAT, CREATE_ALWAYS);
			if(res != FR_OK) {
				return (RECORDRESULT)res;
			}
		}else {
			return RECORD_INVALID_RECORDID;
		}
		/* Set WRITE_INDEX as default (0x00000000) */
		res = xSDCARD_WriteData(file, WRITE_INDEX_ADDR, &writeIndex, sizeof(uint32_t));
		if(res != FR_OK) {return (RECORDRESULT)res;}
		/* Set READ_INDEX as default (0x00000000) */
		res = xSDCARD_WriteData(file, READ_INDEX_ADDR, &readIndex, sizeof(uint32_t));
		if(res != FR_OK) {return (RECORDRESULT)res;}
	}else {
		record_res = xRECORD_SetIndex(file, READ_INDEX_ADDR, readIndex);
		if(record_res != FR_OK) {return record_res;}			/*xRECORD_SetIndex error*/
	}

	return record_res;
}
/*==========================================================================================*/
/*============================================================================================
Support Function
============================================================================================*/
/**
  * @brief  Get index of last RECORD
  * @param 	
  * @retval File function return code
  */
RECORDRESULT xRECORD_GetIndex(FIL *file, uint32_t indexAddr, uint32_t *index) {
	
	FRESULT		res;
	
	res = xSDCARD_ReadData(file, indexAddr, index, sizeof(uint32_t));
	return (RECORDRESULT)res;
}
/*==========================================================================================*/
/**
  * @brief  Set index of last RECORD
  * @param 	
  * @retval File function return code
  */
static RECORDRESULT xRECORD_SetIndex(FIL *file, uint32_t indexAddr, uint32_t index) {
	
	FRESULT		res;
	
	res = xSDCARD_WriteData(file, indexAddr, &index, sizeof(uint32_t));
	return (RECORDRESULT)res;
}
/*==========================================================================================*/
/**
  * @brief  Convert index to real address in SD Card
  * @param 	
  * @retval File function return code
  */
static uint32_t uiRECORD_Index2Addr(uint32_t index, uint32_t recordSize) {
	
	uint32_t 	address = 0;
	
	address = (index*recordSize)+INDEX_SIZE;
	
	return address;
}
/*==========================================================================================*/


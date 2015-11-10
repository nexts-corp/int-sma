/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_csvfile.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	29-April-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_CSVFILE_H
#define _INT_STM32F4_CSVFILE_H

#include <stdlib.h>
#include "ff.h"
#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_log.h"

#define CSV_FILE					"SLData.csv"
#define CSV_INDEX_ADDR		0x00000000
#define CSV_NEWLINE_ADDR	0x00000008
#define CSV_INDEX_SIZE		10
#define CSV_RECORD_SIZE		235

FRESULT xCSVFILE_InsertData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *stat_set, uint8_t numDat, DATA_SET *dat_set);
LOGRESULT xCSVFILE_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *stat_set, uint8_t *numDat, DATA_SET *dat_set);
FRESULT xCSVFILE_GetLastIndex(FIL *file, uint32_t *index);
FRESULT xCSVFILE_SetLastIndex(FIL *file, uint32_t index);

uint8_t ucCSVFILE_AssemCSV(char *strData, uint32_t timestamp, uint8_t numStat, STATUS_SET *stat_set, uint8_t numDat, DATA_SET *dat_set);


#endif /*_INT_STM32F4_CSVFILE_H*/





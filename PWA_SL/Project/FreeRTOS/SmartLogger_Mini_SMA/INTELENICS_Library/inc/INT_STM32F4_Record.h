/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_Record.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	5-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_RECORD_H
#define _INT_STM32F4_RECORD_H

#include <stdint.h>
#include "ff.h"
#include "INT_STM32F4_ProtocolComm.h"
	
/*-- File ID --*/
#define ID_DATARECORD					0x11

/*-- Index --*/
#define WRITE_INDEX_ADDR		0x00000000
#define READ_INDEX_ADDR			0x00000004
#define INDEX_SIZE					10

#define SIZE_TIMESTAMP			sizeof(uint32_t)

/*-- Data&Status File --*/
#define DATA_FILE						"SLDATA.HEX"
#define MAX_DATAnSTAT_INDEX	4320			/* 1 days (If sampling rate = 20 sec) */
#define SIZE_NSTAT					sizeof(uint8_t)
#define SIZE_MAXSTAT				(MAX_STATUS*sizeof(STATUS_SET))  																/* 8x6  = 48 	bytes. */
#define SIZE_NDAT						sizeof(uint8_t)
#define SIZE_MAXDAT					(MAX_DATA*sizeof(DATA_SET))      																/* 11x7 = 77 	bytes. */												
#define OFFSET_NSTAT				(SIZE_TIMESTAMP)
#define OFFSET_STAT					(SIZE_TIMESTAMP+SIZE_NSTAT)
#define OFFSET_NDAT					(SIZE_TIMESTAMP+SIZE_NSTAT+SIZE_MAXSTAT)
#define OFFSET_DAT					(SIZE_TIMESTAMP+SIZE_NSTAT+SIZE_MAXSTAT+SIZE_NDAT)
#define SIZE_RECORD_DATA				(SIZE_TIMESTAMP+SIZE_NSTAT+SIZE_MAXSTAT+SIZE_NDAT+SIZE_MAXDAT) 	/* 131 				bytes. */	

/*-- Data&Status RECORD --*/
#define RECORD_DATAnSTAT				"DATA.RECORD"
#define MAX_DATA_INDEX			44640       /* 31 days (If sampling rate = 60 sec) */

/*-- RECORD function return code (RECORDRESULT) --*/
typedef enum {
	RECORD_FR_OK = 0,								/* (0) Succeeded */
	RECORD_FR_DISK_ERR,							/* (1) A hard error occured in the low level disk I/O layer */
	RECORD_FR_INT_ERR,								/* (2) Assertion failed */
	RECORD_FR_NOT_READY,							/* (3) The physical drive cannot work */
	RECORD_FR_NO_FILE,								/* (4) Could not find the file */
	RECORD_FR_NO_PATH,								/* (5) Could not find the path */
	RECORD_FR_INVALID_NAME,					/* (6) The path name format is invalid */
	RECORD_FR_DENIED,								/* (7) Acces denied due to prohibited access or directory full */
	RECORD_FR_EXIST,									/* (8) Acces denied due to prohibited access */
	RECORD_FR_INVALID_OBJECT,				/* (9) The file/directory object is invalid */
	RECORD_FR_WRITE_PROTECTED,				/* (10) The physical drive is write protected */
	RECORD_FR_INVALID_DRIVE,					/* (11) The RECORDical drive number is invalid */
	RECORD_FR_NOT_ENABLED,						/* (12) The volume has no work area */
	RECORD_FR_NO_FILESYSTEM,					/* (13) There is no valid FAT volume on the physical drive */
	RECORD_FR_MKFS_ABORTED,					/* (14) The f_mkfs() aborted due to any parameter error */
	RECORD_FR_TIMEOUT,								/* (15) Could not get a grant to access the volume within defined period */
	RECORD_FR_LOCKED,								/* (16) The operation is rejected according to the file shareing policy */
	RECORD_FR_NOT_ENOUGH_CORE,				/* (17) LFN working buffer could not be allocated */
	RECORD_FR_TOO_MANY_OPEN_FILES,		/* (18) Number of open files > _FS_SHARE */
	
	RECORD_ERR,											/* (19) RECORD error */
	RECORD_FULL,											/* (20) RECORD is full */
	RECORD_EMPTY,										/* (21) RECORD is empty */
	RECORD_INVALID_RECORDID,						/* (22) Invalid file id */
	RECORD_INVALID_INDEX,						/* (23) Invalid file id */
	
	RECORD_INVALID_ACTUAL_LEN = 255,	/* (255) */
}RECORDRESULT;

RECORDRESULT xRECORD_GetIndex(FIL *file, uint32_t indexAddr, uint32_t *index);

RECORDRESULT xRECORD_WriteDebugRECORD(FIL *file, uint32_t timestamp, char *debug_RECORD);
RECORDRESULT xRECORD_SaveData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data);
RECORDRESULT xRECORD_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data);
RECORDRESULT xRECORD_WriteData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numDat, DATA_SET *data);
RECORDRESULT xRECORD_ReadData(FIL *file, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data);
RECORDRESULT xRECORD_WriteEvent(FIL *file, uint32_t timestamp, uint8_t eid, uint32_t edat);
RECORDRESULT xRECORD_ReadEvent(FIL *file, uint32_t *timestamp, uint8_t *eid, uint32_t *edat);
RECORDRESULT xRECORD_WriteConfig(FIL *file, uint32_t timestamp, uint8_t accd, uint8_t cnfg, void *spec, uint8_t specLen);
RECORDRESULT xRECORD_ReadConfig(FIL *file, uint32_t *timestamp, uint8_t *accd, uint8_t *cnfg, void *spec, uint8_t *specLen);
RECORDRESULT xRECORD_SetNextReadIndex(FIL *file, uint8_t file_id);
	
#endif /*_INT_STM32F4_RECORD_H*/
	




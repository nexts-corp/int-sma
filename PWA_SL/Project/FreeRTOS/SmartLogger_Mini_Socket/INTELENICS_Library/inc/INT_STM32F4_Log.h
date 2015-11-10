/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_Log.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	5-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_LOG_H
#define _INT_STM32F4_LOG_H

#include <stdint.h>
#include "ff.h"
#include "INT_STM32F4_ProtocolComm.h"
	
/*-- File ID --*/
#define ID_DATALOG					0x11
#define ID_EVENTLOG					0x12
#define ID_CONFIGLOG				0x13

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
#define SIZE_LOG_DATA				(SIZE_TIMESTAMP+SIZE_NSTAT+SIZE_MAXSTAT+SIZE_NDAT+SIZE_MAXDAT) 	/* 131 				bytes. */	

/*-- Data&Status Log --*/
#define LOG_DATAnSTAT				"DATA.LOG"
#define MAX_DATA_INDEX			44640       /* 31 days (If sampling rate = 60 sec) */

/*-- Event Log --*/
#define LOG_EVENT						"EVENT.LOG"
#define MAX_EVENT_INDEX			10240									// (10240 index) x (10 bytes) = 102.4 kBytes (EventRep file size)
#define SIZE_EID						sizeof(uint8_t)
#define SIZE_EDAT						sizeof(uint32_t)
#define OFFSET_EID					(SIZE_TIMESTAMP)
#define OFFSET_EDAT					(SIZE_TIMESTAMP+SIZE_EID)
#define SIZE_LOG_EVENT			(SIZE_TIMESTAMP+SIZE_EID+SIZE_EDAT)

/*-- Config Log --*/
#define LOG_CONFIG					"CONFIG.LOG"
#define MAX_CONFIG_INDEX		1000
#define SIZE_ACCD						sizeof(uint8_t)
#define SIZE_CNFG						sizeof(uint8_t)
#define SIZE_SPECLEN				sizeof(uint8_t)
#define SIZE_MAX_SPEC				161								// Max spec size is Properties Config
#define OFFSET_ACCD					SIZE_TIMESTAMP
#define OFFSET_CNFG					(SIZE_TIMESTAMP+SIZE_ACCD)
#define OFFSET_SPECLEN			(SIZE_TIMESTAMP+SIZE_ACCD+SIZE_CNFG)
#define OFFSET_SPEC					(SIZE_TIMESTAMP+SIZE_ACCD+SIZE_CNFG+SIZE_SPECLEN)
#define SIZE_LOG_CONFIG			(SIZE_TIMESTAMP+SIZE_ACCD+SIZE_CNFG+SIZE_SPECLEN+SIZE_MAX_SPEC)

/*-- Log function return code (LOGRESULT) --*/
typedef enum {
	LOG_FR_OK = 0,								/* (0) Succeeded */
	LOG_FR_DISK_ERR,							/* (1) A hard error occured in the low level disk I/O layer */
	LOG_FR_INT_ERR,								/* (2) Assertion failed */
	LOG_FR_NOT_READY,							/* (3) The physical drive cannot work */
	LOG_FR_NO_FILE,								/* (4) Could not find the file */
	LOG_FR_NO_PATH,								/* (5) Could not find the path */
	LOG_FR_INVALID_NAME,					/* (6) The path name format is invalid */
	LOG_FR_DENIED,								/* (7) Acces denied due to prohibited access or directory full */
	LOG_FR_EXIST,									/* (8) Acces denied due to prohibited access */
	LOG_FR_INVALID_OBJECT,				/* (9) The file/directory object is invalid */
	LOG_FR_WRITE_PROTECTED,				/* (10) The physical drive is write protected */
	LOG_FR_INVALID_DRIVE,					/* (11) The logical drive number is invalid */
	LOG_FR_NOT_ENABLED,						/* (12) The volume has no work area */
	LOG_FR_NO_FILESYSTEM,					/* (13) There is no valid FAT volume on the physical drive */
	LOG_FR_MKFS_ABORTED,					/* (14) The f_mkfs() aborted due to any parameter error */
	LOG_FR_TIMEOUT,								/* (15) Could not get a grant to access the volume within defined period */
	LOG_FR_LOCKED,								/* (16) The operation is rejected according to the file shareing policy */
	LOG_FR_NOT_ENOUGH_CORE,				/* (17) LFN working buffer could not be allocated */
	LOG_FR_TOO_MANY_OPEN_FILES,		/* (18) Number of open files > _FS_SHARE */
	
	LOG_ERR,											/* (19) Log error */
	LOG_FULL,											/* (20) Log is full */
	LOG_EMPTY,										/* (21) Log is empty */
	LOG_INVALID_LOGID,						/* (22) Invalid file id */
	LOG_INVALID_INDEX,						/* (23) Invalid file id */
	
	LOG_INVALID_ACTUAL_LEN = 255,	/* (255) */
}LOGRESULT;

LOGRESULT xLOG_GetIndex(FIL *file, uint32_t indexAddr, uint32_t *index);

LOGRESULT xLOG_WriteDebugLog(FIL *file, uint32_t timestamp, char *debug_log);
LOGRESULT xLOG_SaveData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numData, DATA_SET *data);
LOGRESULT xLOG_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data);
LOGRESULT xLOG_WriteData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *status, uint8_t numDat, DATA_SET *data);
LOGRESULT xLOG_ReadData(FIL *file, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data);
LOGRESULT xLOG_WriteEvent(FIL *file, uint32_t timestamp, uint8_t eid, uint32_t edat);
LOGRESULT xLOG_ReadEvent(FIL *file, uint32_t *timestamp, uint8_t *eid, uint32_t *edat);
LOGRESULT xLOG_WriteConfig(FIL *file, uint32_t timestamp, uint8_t accd, uint8_t cnfg, void *spec, uint8_t specLen);
LOGRESULT xLOG_ReadConfig(FIL *file, uint32_t *timestamp, uint8_t *accd, uint8_t *cnfg, void *spec, uint8_t *specLen);
LOGRESULT xLOG_SetNextReadIndex(FIL *file, uint8_t file_id);
	
#endif /*_INT_STM32F4_LOG_H*/
	




#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include "main.h"
#include "memcard.h"

#define SD_MANG_PRINT_DEBUG            0

#define SD_READ_CONF_FILE              0x00
#define SD_WRITE_CONF_FILE             0x01

extern iChar_t viFolderDataName[];
extern iChar_t viFolderDataLogName[];
extern iChar_t viFolderEventName[];
extern iChar_t viFolderStatusName[];
extern iChar_t viFolderErrorName[];     
extern iChar_t viFolderConfigName[]; 

extern iChar_t viDirRootPath[];
extern iChar_t viDirDataPath[];
extern iChar_t viDirDataLogPath[];
extern iChar_t viDirEventPath[];
extern iChar_t viDirStatusPath[];
extern iChar_t viDirErrorPath[]; 
extern iChar_t viDirConfigPath[];

extern iChar_t viDataFName[];
extern iChar_t viDataLogFName[];
extern iChar_t viEventFName[];
extern iChar_t viStatusFName[];
extern iChar_t viErrorFName[];
extern iChar_t viConfigFName[];

FRESULT directory_scan(char *path);


iInt_t iInitSDCard();
void iFDisplayReturn(FRESULT viFResulyt_arg);
void iFCreate(FIL *pviOutFilePtr_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg);
void iFRwite(const iChar_t * const pviDataBuff,iUInt_t ivLength_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long const * const pviWritePtr_arg);
void iFDelete(iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg);
void iFCreateFileDaily(iChar_t *pviFilename_arg);
iChar_t iFSize(iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long *pviOutFileSize_arg);
iChar_t iFRead(iChar_t * pviDataBuff,iUInt_t ivLength_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long const * const pviReadPtr_arg);


#endif 
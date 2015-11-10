#ifndef DATABASE_H
#define DATABASE_H

#include "main.h"

#define DB_STATUS_NOT_SENT_def      'N'
#define DB_STATUS_SENT_SUCC_def     'Y'
#define DB_STATUS_SATTLEMENT_def    'S'
#define DB_STATUS_LOGGING_def       'L'
#define DB_BLOCK_WRITE_def          255
#define DB_BLOCK_READ_def           255
#define DB_ID_MAX_RECORD_def        3000
#define DB_DELIMITER_def            '|'

#define DB_MODE_INSERT              1
#define DB_MODE_UPDATE              2

#define DB_PRINT_DEBUG            0

typedef void* piDataBaseHandle_t;

typedef struct{
   iUInt_t viLineID;
   iChar_t status;
   iUInt_t length;
   iChar_t *value;
}iTable_t;

typedef struct{
//   iChar_t *dirPath;
//   iChar_t *fileName;
   iTable_t viTable;
}iDB_t;

extern eeprom unsigned int viLineID;

//iChar_t iDataLogInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg);
iUInt_t iDataInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg);
iChar_t iDataSelect(iUInt_t viRecordID_arg,iUChar_t viStatus_arg);

#endif 
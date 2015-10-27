#ifndef DATABASE_H
#define DATABASE_H

#include "main.h"


typedef void* piDataBaseHandle_t;

typedef struct{
   iUInt_t viLineID;
   iChar_t status;
   iUInt_t length;
   iChar_t *value;
}iTable_t;

typedef struct{
   iChar_t *dirPath;
   iChar_t *fileName;
   iTable_t viTable;
}iDB_t;

iChar_t iDataLogInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg);
iChar_t iDataInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg);

#endif 
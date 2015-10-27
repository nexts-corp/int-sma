#include "database.h"
#include "sdManager.h"


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

piDataBaseHandle_t pviDataBaseHandle = NULL;

iChar_t iDataLogInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg){
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    iChar_t *pviDataBuffInsert;
    const iChar_t viStatusField = 'N'; 
    const iChar_t viTokenSplite = '|'; 
    const iChar_t viNewLine[] = {0x0d,0x0a};           //"\r\n"
    iChar_t viDataBuff[255];

    

    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){
        pviDBLogger->dirPath = viDirDataLogPath;
        pviDBLogger->fileName = viDataLogFName;
        pviDBLogger->viTable.status = 'N';  
        
        memcpy(&viDataBuff[0],&viStatusField,1); 
        memcpy(&viDataBuff[1],&viTokenSplite,1);
        memcpy(&viDataBuff[1+1],&pviDataBuffer_arg[0],viLength_arg);  
        memcpy(&viDataBuff[1+1+viLength_arg],viNewLine,2); 
        
        pviDBLogger->viTable.length = viLength_arg+4;
        pviDBLogger->viTable.value = viDataBuff; 
        iFRwite(pviDBLogger->viTable.value,pviDBLogger->viTable.length,pviDBLogger->dirPath,pviDBLogger->fileName); 
        viReturn = 1;
        
        free(pviDBLogger);
        printDebug("[iDataLogInsert]Insert is success.\r\n");
    }else{
        printDebug("[iDataLogInsert]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturn;
}



iChar_t iDataLogUpdate(iChar_t *pviDataBuffer_arg,iUInt_t viLineID_arg){
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    
    return viReturn;
}

iChar_t iDataLogSelect(iChar_t *pviDataBuffer_arg,iUInt_t viLineID_arg){
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    
    return viReturn;
}


iChar_t iDataInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg){
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    iChar_t *pviDataBuffInsert;
    const iChar_t viStatusField = 'N'; 
    const iChar_t viTokenSplite = '|'; 
    const iChar_t viNewLine[] = {0x0d,0x0a};           //"\r\n"
    iChar_t viDataBuff[255];

    

    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){
        pviDBLogger->dirPath = viDirDataPath;
        pviDBLogger->fileName = viDataFName;
        pviDBLogger->viTable.status = 'N';  
        
        memcpy(&viDataBuff[0],&viStatusField,1); 
        memcpy(&viDataBuff[1],&viTokenSplite,1);
        memcpy(&viDataBuff[1+1],&pviDataBuffer_arg[0],viLength_arg);  
        memcpy(&viDataBuff[1+1+viLength_arg],viNewLine,2); 
        
        pviDBLogger->viTable.length = viLength_arg+4;
        pviDBLogger->viTable.value = viDataBuff; 
        iFRwite(pviDBLogger->viTable.value,pviDBLogger->viTable.length,pviDBLogger->dirPath,pviDBLogger->fileName); 
        viReturn = 1;
        
        free(pviDBLogger);
        printDebug("[iDataInsert]Insert is success.\r\n");
    }else{
        printDebug("[iDataInsert]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturn;
}
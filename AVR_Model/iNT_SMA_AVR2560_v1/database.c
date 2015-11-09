#include "database.h"
#include "sdManager.h"


extern iChar_t viFolderDataName[];
extern iChar_t viFolderDataLogName[];
//extern iChar_t viFolderEventName[];
//extern iChar_t viFolderStatusName[];
//extern iChar_t viFolderErrorName[];     
//extern iChar_t viFolderConfigName[]; 

extern iChar_t viDirRootPath[];
extern iChar_t viDirDataPath[];
//extern iChar_t viDirDataLogPath[];
//extern iChar_t viDirEventPath[];
//extern iChar_t viDirStatusPath[];
//extern iChar_t viDirErrorPath[]; 
//extern iChar_t viDirConfigPath[];

extern iChar_t viDataFName[];
//extern iChar_t viDataLogFName[];
//extern iChar_t viEventFName[];
//extern iChar_t viStatusFName[];
//extern iChar_t viErrorFName[];
//extern iChar_t viConfigFName[];

piDataBaseHandle_t pviDataBaseHandle = NULL;
eeprom unsigned int viLineID=0;



iChar_t iDataUpdate(iUInt_t viRecordID_arg,iUChar_t viStatus_arg){             //'N' log and not send data , 'Y' sent data ,'S' settlemented
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    iChar_t *pviDataBuffInsert;
    iChar_t viDataBuff[255];  
    unsigned int viRecordID = 0;   
    unsigned long indexRead = 0;
    unsigned int viLenOfData = 0;
    unsigned long viFileSize = 0;
    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));  
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)==1){ 
           if(viFileSize == 0){
              printDebug("[iDataUpdate]File is empty.\r\n");
           }else{
              printDebug("[iDataUpdate]File size(%ld).\r\n",viFileSize); 
              iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexRead);
              print_payload(viDataBuff,255); 
           } 
        } 
        free(pviDBLogger);
        printDebug("[iDataUpdate]iDataSelect is success.\r\n"); 
        viReturn = 1;
    }else{
        printDebug("[iDataUpdate]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturn;
}

iUInt_t iDataInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg){
    iDB_t *pviDBLogger;
    iUInt_t viReturnID = 0;
    iChar_t *pviDataBuffInsert;
    const iChar_t viStatusField = 'N'; 
    iChar_t viDataBuff[255];  
    unsigned int viRecordID = 0;   
    unsigned int indexWrite = 0;
    unsigned long viFileSize = 0;
    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));  
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)){ 
           if(viFileSize == 0){
              viLineID = 0;
           } 
        } 
        viRecordID = ++viLineID;
        indexWrite = 0;
        memcpy(&viDataBuff[indexWrite],&viRecordID,2);
        indexWrite += 2;
        memcpy(&viDataBuff[indexWrite],&viStatusField,1); 
        indexWrite += 1;
        memcpy(&viDataBuff[indexWrite],&viLength_arg,2);
        indexWrite += 2; 
        memcpy(&viDataBuff[indexWrite],&pviDataBuffer_arg[0],viLength_arg);
        indexWrite += viLength_arg;  
        
        pviDBLogger->viTable.length = indexWrite;  
        pviDBLogger->viTable.value = viDataBuff; 
        iFRwite(pviDBLogger->viTable.value,sizeof(viDataBuff),(const char*)viDirDataPath,(const char*)viDataFName,DB_BLOCK_WRITE_def);      
        
        free(pviDBLogger);
        
        printDebug("[iDataInsert]Insert is success.\r\n");     
        viReturnID = viRecordID;
    }else{
        printDebug("[iDataInsert]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturnID;
}

iChar_t iDataSelect(iUChar_t viStatus_arg){             //'N' log and not send data , 'Y' sent data ,'S' settlemented
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    iChar_t *pviDataBuffInsert;
    iChar_t viDataBuff[255];  
    unsigned int viRecordID = 0;   
    unsigned long indexRead = 0;
    unsigned int viLenOfData = 0;
    unsigned long viFileSize = 0;
    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));  
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)==1){ 
           if(viFileSize == 0){
              printDebug("[iDataSelect]File is empty.\r\n");
           }else{
              printDebug("[iDataSelect]File size(%ld).\r\n",viFileSize); 
              iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexRead);
              print_payload(viDataBuff,255); 
           } 
        } 
        free(pviDBLogger);
        printDebug("[iDataSelect]iDataSelect is success.\r\n"); 
        viReturn = 1;
    }else{
        printDebug("[iDataSelect]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturn;
}
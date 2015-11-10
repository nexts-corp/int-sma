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



iChar_t iDataUpdate(iUInt_t viRecordID_arg,iUChar_t viStatus_arg,iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg){    //update status         //'N' log and not send data , 'Y' sent data ,'S' settlemented
    iDB_t *pviDBLogger;
    iUInt_t viReturnID = 0;
    iChar_t *pviDataBuffInsert;
    //const iChar_t viStatusField = 'Y';   
    iChar_t viStatusField = (const char)viStatus_arg; 
    iChar_t viDataBuff[DB_BLOCK_BUFFER_def];  
    unsigned int viRecordID = 0; 
    unsigned long indexWrite = 0;  
    unsigned long indexWriteFile = 0; 
    unsigned long indexReadFile = 0;
    unsigned long viFileSize = 0;
    unsigned long viRecordIDBuff = 0;
    
    if(viRecordID_arg <= 0){    //reserv a all select 
        return -1;
    }else if(viRecordID_arg == 1){
        indexReadFile = 0;
    }else if(viRecordID_arg > 1){
        indexReadFile = (DB_BLOCK_BUFFER_def*(viRecordID_arg-1));
    }
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));
        printDebug("[iDataUpdate]Check file.\r\n");   
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)){ 
           if(viFileSize == 0){
              printDebug("[iDataUpdate]File is empty.\r\n");
           }else{  //read last record id 
              printDebug("[iDataUpdate]File size(%ld),Read RecordID(%ld),Index read(%ld).\r\n",viFileSize,viRecordID_arg,indexReadFile); 
              iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexReadFile);
              printDebug("[iDataUpdate]Read data:[\r\n"); 
              print_payload(viDataBuff,DB_BLOCK_BUFFER_def);
              printDebug("]\r\n"); 
              memcpy(&viRecordIDBuff,viDataBuff,2); 
              if(viRecordIDBuff!=viRecordID_arg){
                 printDebug("[iDataUpdate]mem diff allocate.\r\n");
              }else{
                 printDebug("[iDataUpdate]Record id(%ld),status send(%c).\r\n",viRecordID_arg,viDataBuff[2]);
              }
           } 
        } 
        
        if(viRecordID_arg <= 0){    //reserv a all select 
            return -1;
        }else if(viRecordID_arg == 1){
            indexWriteFile = 0;
        }else if(viRecordID_arg > 1){
            indexWriteFile = (DB_BLOCK_BUFFER_def*(viRecordID_arg-1));
        }
        
        indexWrite = 0;
        memcpy(&viDataBuff[indexWrite],&viStatusField,1); 
        indexWrite += 1;
        
        if(viStatus_arg == 'L'){
            memcpy(&viDataBuff[indexWrite],&viLength_arg,2);
            indexWrite += 2; 
            memcpy(&viDataBuff[indexWrite],&pviDataBuffer_arg[0],viLength_arg);
            indexWrite += viLength_arg;
        }
        
        
        pviDBLogger->viTable.length = indexWrite;  
        pviDBLogger->viTable.value = viDataBuff;  
         
        indexWriteFile += 2;             //go to status addr 
        printDebug("[iDataUpdate]File size(%ld),Read RecordID(%ld),Index write(%ld).\r\n",viFileSize,viRecordID_arg,(indexWriteFile));
        iFRwite(pviDBLogger->viTable.value,indexWrite,(const char*)viDirDataPath,(const char*)viDataFName,&indexWriteFile);
              
          
        iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexReadFile);
        printDebug("[iDataUpdate]Read data:[\r\n"); 
        print_payload(viDataBuff,DB_BLOCK_BUFFER_def);
        printDebug("]\r\n"); 
        memcpy(&viRecordIDBuff,viDataBuff,2); 
        if(viRecordIDBuff!=viRecordID_arg){
           printDebug("[iDataUpdate]mem diff allocate.\r\n");
        }else{
          printDebug("[iDataUpdate]Record id(%ld),status send(%c).\r\n",viRecordID_arg,viDataBuff[2]);
        }   
        
        free(pviDBLogger);
        
        printDebug("[iDataUpdate]Update is success.\r\n");     
        //viReturnID = viRecordID;
    }else{
        printDebug("[iDataUpdate]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturnID;
}

iUInt_t iDataInsert(iChar_t const * const pviDataBuffer_arg,iUInt_t viLength_arg){
    iDB_t *pviDBLogger;
    iUInt_t viReturnID = 0;
    iChar_t *pviDataBuffInsert;
    const iChar_t viStatusField = 'N'; 
    iChar_t viDataBuff[DB_BLOCK_BUFFER_def];  
    unsigned int viRecordID = 0;   
    unsigned long indexWrite = 0;
    unsigned long indexWriteFile = 0;
    unsigned long indexReadFile = 0;
    unsigned long viFileSize = 0;
    
    
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));
        printDebug("[iDataInsert]Check file.\r\n");   
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)){ 
           if(viFileSize == 0){
              viLineID = 0; 
              indexWriteFile = 0;
              printDebug("[iDataInsert]File is empty.\r\n");
           }else{  //read last record id
              if(viLineID <= 0){    //reserv a all select 
                  if((viFileSize/DB_BLOCK_BUFFER_def)>1){
                     indexReadFile = (DB_BLOCK_BUFFER_def*((viFileSize/DB_BLOCK_BUFFER_def)-1));
                  }
              }else if(viLineID == 1){
                  indexReadFile = 0;
              }else if(viLineID > 1){
                  indexReadFile = (DB_BLOCK_BUFFER_def*(viLineID-1));
              } 
              
//              if((viFileSize/DB_BLOCK_BUFFER_def)==1){
//                 indexRead = 0;
//              }else if((viFileSize/DB_BLOCK_BUFFER_def)>1){
//                 indexRead = (DB_BLOCK_BUFFER_def*((viFileSize/DB_BLOCK_BUFFER_def)-1));
//              }
              iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexReadFile);
              memcpy(&viRecordID,&viDataBuff[0],2);
              printDebug("[iDataInsert]Last record id(%ld).\r\n",viRecordID); 
              viLineID = viRecordID;
           } 
        }
         
        viRecordID = ++viLineID; 
        if(viRecordID <= 0){    //reserv a all select 
            return -1;
        }else if(viRecordID == 1){
            indexWriteFile = 0;
        }else if(viRecordID > 1){
            indexWriteFile = (DB_BLOCK_BUFFER_def*(viRecordID-1));
        }
                
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
        iFRwite(pviDBLogger->viTable.value,sizeof(viDataBuff),(const char*)viDirDataPath,(const char*)viDataFName,&indexWriteFile);      
        
        free(pviDBLogger);
        
        printDebug("[iDataInsert]Insert is success.\r\n");     
        viReturnID = viRecordID;
    }else{
        printDebug("[iDataInsert]Databuff_arg can't allocate mem.\r\n");
    } 
    return viReturnID;
}

iChar_t iDataSelect(iUInt_t viRecordID_arg,iUChar_t viStatus_arg){             //'N' log and not send data , 'Y' sent data ,'S' settlemented
    iDB_t *pviDBLogger;
    iChar_t viReturn = 0;
    iChar_t *pviDataBuffInsert;
    iChar_t viDataBuff[DB_BLOCK_BUFFER_def];  
    unsigned int viRecordID = 0;   
    unsigned long indexReadFile = 0;
    unsigned int viLenOfData = 0;
    unsigned long viFileSize = 0;
    unsigned long viRecordIDBuff = 0;
    
    if(viRecordID_arg <= 0){    //reserv a all select 
        return -1;
    }else if(viRecordID_arg == 1){
        indexReadFile = 0;
    }else if(viRecordID_arg > 1){
        indexReadFile = (DB_BLOCK_BUFFER_def*(viRecordID_arg-1));
    }
    pviDBLogger = (iDB_t * const)malloc(sizeof(iDB_t));       
    if(pviDBLogger!=NULL){ 
        memset(viDataBuff,0,sizeof(viDataBuff));  
        if(iFSize((char const * const)viDirDataPath,(char const * const)viDataFName,&viFileSize)==1){ 
           if(viFileSize == 0){
              printDebug("[iDataSelect]File is empty.\r\n");
           }else{
              printDebug("[iDataSelect]File size(%ld),Read RecordID(%ld),Index read(%ld).\r\n",viFileSize,viRecordID_arg,indexReadFile); 
              iFRead(viDataBuff,(iUInt_t)DB_BLOCK_READ_def,(char const * const)viDirDataPath,(char const * const)viDataFName,&indexReadFile);
              print_payload(viDataBuff,DB_BLOCK_BUFFER_def); 
              memcpy(&viRecordIDBuff,viDataBuff,2); 
              if(viRecordIDBuff!=viRecordID_arg){
                 printDebug("[iDataSelect]mem diff allocate.\r\n");
              }else{
                 printDebug("[iDataSelect]Record id(%ld),status send(%c).\r\n",viRecordID_arg,viDataBuff[2]);
              }
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
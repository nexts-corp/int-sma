#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"        
#include "debug.h"
                                        
unsigned char LOGFILE_DATA[]            = "data.log";           // -- data log file name -- //
unsigned char LOGFILE_EVENT[]           = "event.log";          // -- event log file name -- //
unsigned char LOGFILE_ERROR[]           = "error.log";          // -- error log file name -- //
unsigned char LOG_INIT_STR[SIZE_RECORD] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
/*============================================================*/     
                                                                
/**************************************************************/
/************** Function Pointer to memcard.h *****************/
/**************************************************************/
/*============================================================*/
int (*LOG_openFile)(FIL *, unsigned char *) = &SD_openFile;
int (*LOG_closeFile)(FIL *)                 = &SD_closeFile;
/*============================================================*/     
                                                                
/**************************************************************/
/******************* Data Log Management **********************/
/**************************************************************/
char isDataLogEmpty     = 0;
/*============================================================*/
int LOG_insertData(FIL *file, unsigned long int timestamp, unsigned int channel, float value)
{
    int                 res;
    unsigned long int   index; 
    unsigned long int   startAddr;  
    unsigned char       flag;
    
    res             = LOG_getLastInsert(file, &index);
    if(res < 0){    printDebug("<LOG_insertData> LOG_getLastInsert() failed\r\n");      return -1;  }                     
                                     
    flag            = 'P';               
    
    startAddr       = index2Addr(index,SIZE_LOG_DATA); 
                 
    res             = SD_writeData(file, startAddr+OFFSET_FLAG,         (unsigned char *) &flag,        SIZE_FLAG);           
    if(res < 0){    printDebug("<LOG_insertData> SD_writeData(flag) failed\r\n");       return -1;  }  
    
    res             = SD_writeData(file, startAddr+OFFSET_TIMESTAMP,    (unsigned char *) &timestamp,   SIZE_TIMESTAMP);           
    if(res < 0){    printDebug("<LOG_insertData> SD_writeData(timestamp) failed\r\n");  return -1;  }   
    
    res             = SD_writeData(file, startAddr+OFFSET_DATA_CHANNEL, (unsigned char *) &channel,     SIZE_CHANNEL);           
    if(res < 0){    printDebug("<LOG_insertData> SD_writeData(channel) failed\r\n");    return -1;  }  
    
    res             = SD_writeData(file, startAddr+OFFSET_DATA_VALUE,   (unsigned char *) &value,       SIZE_VALUE);           
    if(res < 0){    printDebug("<LOG_insertData> SD_writeData(value) failed\r\n");      return -1;  }    
               
    res = LOG_setLastInsert(file,++index);   
    if(res < 0){    printDebug("<LOG_insertData> LOG_setLastInsert(%ld) failed\r\n",index);      return -1;  }
                  
    isDataLogEmpty  = 0;
    return index;
}
/*============================================================*/
int LOG_readData(FIL *file, unsigned long int *timestamp, unsigned int *channel, float *value, unsigned long int *index)
{   
    int                 res;
    unsigned long int   lastLogInsert;
    unsigned long int   lastLogSend; 
    unsigned long int   startAddr;   
    unsigned char       flag;
    
                                                                       
    res             = LOG_getLastInsert(file, &lastLogInsert);
    if(res < 0){    printDebug("<LOG_readData> LOG_getLastInsert() failed\r\n");        return -1;  }      
    res             = LOG_getLastSend(file, &lastLogSend);
    if(res < 0){    printDebug("<LOG_readData> LOG_getLastSend() failed\r\n");          return -1;  }                     
      
    printDebug("<LOG_readEvent> lastSend/lastInsert  --> %ld/%ld\r\n",lastLogSend,lastLogInsert);
                                    
    if(lastLogSend >= lastLogInsert)
    {             
        printDebug("<LOG_readData> Log empty..!!\r\n");      

        res = LOG_createFile(LOGFILE_DATA,SD_FORCE,LOG_INIT_STR,SIZE_RECORD);       
        if(res < 0)
        {
            printDebug("<DEBUG_ClearLogFile> LOG_createFile(%s) failed\r\n",LOGFILE_DATA);   
        }
        
        isDataLogEmpty  = 1;
        return 0; 
    }               
    
    startAddr       = index2Addr(lastLogSend,SIZE_LOG_DATA);                                             
                 
    res             = SD_readData(file, startAddr+OFFSET_FLAG,          (unsigned char *) &flag,        SIZE_FLAG);           
    if(res < 0){    printDebug("<LOG_readData> SD_readData(flag) failed\r\n");          return -1;  }      
    
    res             = SD_readData(file, startAddr+OFFSET_TIMESTAMP,     (unsigned char *) timestamp,    SIZE_TIMESTAMP);           
    if(res < 0){    printDebug("<LOG_readData> SD_readData(timestamp) failed\r\n");     return -1;  }   
    
    res             = SD_readData(file, startAddr+OFFSET_DATA_CHANNEL, (unsigned char *) channel,       SIZE_CHANNEL);           
    if(res < 0){    printDebug("<LOG_readData> SD_readData(channel) failed\r\n");       return -1;  }  
    
    res             = SD_readData(file, startAddr+OFFSET_DATA_VALUE,    (unsigned char *) value,        SIZE_VALUE);           
    if(res < 0){    printDebug("<LOG_readData> SD_readData(value) failed\r\n");         return -1;  } 
                                                    
    *index          = lastLogSend; 
    
    return 1;
}
/*============================================================*/     
                                                                
/**************************************************************/
/******************* Event Log Management *********************/
/**************************************************************/   
char isEventLogEmpty    = 0;
/*============================================================*/
int LOG_insertEvent(FIL *file, unsigned long int timestamp, unsigned char id, unsigned char status)
{
    int                 res;
    unsigned long int   index; 
    unsigned long int   startAddr;   
    unsigned char       flag;
    
    res = LOG_getLastInsert(file, &index);
    if(res < 0){    printDebug("<LOG_insertEvent> LOG_getLastInsert() failed\r\n");     return -1;  }                     
                                     
    flag            = 'P';               
    
    startAddr       = index2Addr(index,SIZE_LOG_EVENT); 
                 
    res             = SD_writeData(file, startAddr+OFFSET_FLAG,         (unsigned char *) &flag,        SIZE_FLAG);           
    if(res < 0){    printDebug("<LOG_insertEvent> SD_writeData(flag) failed\r\n");      return -1;  }  
    
    res             = SD_writeData(file, startAddr+OFFSET_TIMESTAMP,    (unsigned char *) &timestamp,   SIZE_TIMESTAMP);           
    if(res < 0){    printDebug("<LOG_insertEvent> SD_writeData(timestamp) failed\r\n"); return -1;  }   
    
    res             = SD_writeData(file, startAddr+OFFSET_EVENT_ID,     (unsigned char *) &id,          SIZE_ID);           
    if(res < 0){    printDebug("<LOG_insertEvent> SD_writeData(id) failed\r\n");        return -1;  }  
    
    res             = SD_writeData(file, startAddr+OFFSET_EVENT_STATUS, (unsigned char *) &status,      SIZE_STATUS);           
    if(res < 0){    printDebug("<LOG_insertEvent> SD_writeData(status) failed\r\n");    return -1;  }    
               
    res = LOG_setLastInsert(file,++index);   
    if(res < 0){    printDebug("<LOG_insertEvent> LOG_setLastInsert(%ld) failed\r\n",index);     return -1;  }
             
    isEventLogEmpty = 0;
    
    return index;
}
/*============================================================*/
int LOG_readEvent(FIL *file, unsigned long int *timestamp, unsigned char *id, unsigned char *status, unsigned long int *index)
{                                                                      
    int                 res;
    unsigned long int   lastLogInsert;
    unsigned long int   lastLogSend; 
    unsigned long int   startAddr;   
    unsigned char       flag;
    
                                                                       
    res             = LOG_getLastInsert(file, &lastLogInsert);
    if(res < 0){    printDebug("<LOG_readEvent> LOG_getLastInsert() failed\r\n");       return -1;  }      
    res             = LOG_getLastSend(file, &lastLogSend);
    if(res < 0){    printDebug("<LOG_readEvent> LOG_getLastSend() failed\r\n");         return -1;  }                     
                                                    
    printDebug("<LOG_readEvent> lastSend/lastInsert  --> %ld/%ld\r\n",lastLogSend,lastLogInsert);
    
    if(lastLogSend >= lastLogInsert)
    {             
        printDebug("<LOG_readEvent> Log empty..!!\r\n");     
                                                    
        res = LOG_createFile(LOGFILE_EVENT,SD_FORCE,LOG_INIT_STR,SIZE_RECORD);       
        if(res < 0)
        {
            printDebug("<DEBUG_ClearLogFile> LOG_createFile(%s) failed\r\n",LOGFILE_EVENT);  
        }
                           
        isEventLogEmpty     = 1;
        return 0; 
    }               
    
    startAddr       = index2Addr(lastLogSend,SIZE_LOG_EVENT);                                             
                 
    res             = SD_readData(file, startAddr+OFFSET_FLAG,          (unsigned char *) &flag,        SIZE_FLAG);           
    if(res < 0){    printDebug("<LOG_readEvent> SD_readData(flag) failed\r\n");         return -1;  }      
    
    res             = SD_readData(file, startAddr+OFFSET_TIMESTAMP,     (unsigned char *) timestamp,    SIZE_TIMESTAMP);           
    if(res < 0){    printDebug("<LOG_readEvent> SD_readData(timestamp) failed\r\n");    return -1;  }   
    
    res             = SD_readData(file, startAddr+OFFSET_EVENT_ID,      (unsigned char *) id,           SIZE_ID);           
    if(res < 0){    printDebug("<LOG_readEvent> SD_readData(id) failed\r\n");           return -1;  }  
    
    res             = SD_readData(file, startAddr+OFFSET_EVENT_STATUS,  (unsigned char *) status,       SIZE_STATUS);           
    if(res < 0){    printDebug("<LOG_readEvent> SD_readData(status) failed\r\n");       return -1;  }    
    
    *index  = lastLogSend; 
    return 1;
}
/*============================================================*/     
                                                                
/**************************************************************/
/******************* Log Indexing Function ********************/
/**************************************************************/
/*============================================================*/
int LOG_getLastInsert(FIL *file, unsigned long int *index)
{
    int             res;
    
    res = SD_readData(file, 0, (unsigned char *) index, sizeof(unsigned long int));  
    if(res < 0)
    {
        printDebug("<LOG_getLastInsert> SD_readData() failed\r\n");
        return -1;
    }                                        
    
    printDebug("<LOG_getLastInsert> got %ld\r\n", *index);

    return 0;
}
/*============================================================*/
int LOG_setLastInsert(FIL *file, unsigned long int index)
{
    int             res;
    
    printDebug("<LOG_setLastInsert> writing %ld\r\n", index);  
    print_payload((unsigned char *) &index, sizeof(unsigned long int));
    
    res = SD_writeData(file, 0, (unsigned char *) &index, sizeof(unsigned long int));  
    if(res < 0)
    {
        printDebug("<LOG_setLastInsert> SD_readData() failed\r\n");
        return -1;
    }                                        

    return 0;
}    
/*============================================================*/
int LOG_getLastSend(FIL *file, unsigned long int *index)
{
    int             res;
    
    res = SD_readData(file, 4, (unsigned char *) index, sizeof(unsigned long int));  
    if(res < 0)
    {
        printDebug("<LOG_getLastSend> SD_readData() failed\r\n");
        return -1;
    }                                        
    
    printDebug("<LOG_getLastSend> got %ld\r\n", *index);

    return 0;
}
/*============================================================*/
int LOG_setLastSend(FIL *file, unsigned long int index)
{
    int             res;
    
    printDebug("<LOG_setLastSend> writing %ld\r\n", index);  
    print_payload((unsigned char *) &index, sizeof(unsigned long int));
    
    res = SD_writeData(file, 4, (unsigned char *) &index, sizeof(unsigned long int));  
    if(res < 0)
    {
        printDebug("<LOG_setLastSend> SD_readData() failed\r\n");
        return -1;
    }                                        

    return 0;
}
/*============================================================*/         
                                                                
/**************************************************************/
/********************* Support Function ***********************/
/**************************************************************/
/*============================================================*/
unsigned long int index2Addr(unsigned long int index,int recordSize)
{
    unsigned long int address=0;                                                              
    
    address = ( index * recordSize ) + SIZE_LAST_LOG_INDEX + SIZE_LAST_LOG_SENT + SIZE_PADDING; 
    
    return address;
}                     
/*============================================================*/
int LOG_createFile(unsigned char *filename, unsigned char mode, unsigned char *initString, unsigned int len)
{
    FIL         file;
    int         res;              
        
    res = SD_createFile(filename,mode);
    if(res < 0){
        printDebug("<LOG_createFile> SD_createFile(%s) failed\r\n",filename);      
        return -1;   
    }         
    else if(res == 0)
    {      
        printDebug("<LOG_createFile> %s already exist\r\n", filename);            
        return 0;
    }  
    else
    {                                                           
        printDebug("<LOG_createFile> %s created\r\n", filename);  
        if(len == 0) // -- no initial data -- //
        {                    
            printDebug("<LOG_createFile> %s contained no initial data\r\n");
            return 1;        
        }                                             
        
        // -- write the file initial data -- // 
        res = SD_openFile(&file,filename);        
        if(res < 0)
        {
            printDebug("<LOG_createFile> SD_openFile(%s) failed\r\n",filename); 
            return -1;
        }                                                               
        else
        {                                                    
            printDebug("<LOG_createFile> writing initial data\r\n");
            print_payload(initString,len);
            res = SD_writeData(&file,0,initString,len);
            if(res < 0)
            {                                        
                printDebug("<LOG_createFile> SD_writeData() failed Writing.. \r\n"); 
                print_payload(initString,len);
                return -1;
            } 
            
            res = SD_closeFile(&file);
            if(res < 0)
            {
                printDebug("<LOG_createFile> SD_closeFile() failed\r\n");    
                return -1;
            }  
        }
    }          
            
            
    return 1;
}
/*============================================================*/
int LOG_init(void)
{
    int         res,ret = 0;  
    
    printDebug("<LOG_init> Initial log file\r\n");                 
     
           
    res = LOG_createFile(LOGFILE_DATA,0,LOG_INIT_STR,sizeof(LOG_INIT_STR));       
    if(res < 0)
    {
        printDebug("<LOG_init> LOG_createFile(%s) failed\r\n",LOGFILE_DATA);   
        ret = ret-1;
    }
    
    res = LOG_createFile(LOGFILE_EVENT,0,LOG_INIT_STR,sizeof(LOG_INIT_STR));       
    if(res < 0)
    {
        printDebug("<LOG_init> LOG_createFile(%s) failed\r\n",LOGFILE_EVENT);  
        ret = ret-1;       
    }
    
    res = LOG_createFile(LOGFILE_ERROR,0,NULL,0);       
    if(res < 0)
    {
        printDebug("<LOG_init> LOG_createFile(%s) failed\r\n",LOGFILE_ERROR);    
        ret = ret-1;     
    }         
        
    return ret;
}
/*============================================================*/


#ifndef LOG_H
#define LOG_H

#include "memcard.h"
                                        
#define SIZE_RECORD             16     
#define SIZE_LAST_LOG_INDEX     sizeof(unsigned long int)
#define SIZE_LAST_LOG_SENT      sizeof(unsigned long int)
#define SIZE_PADDING            (SIZE_RECORD - (SIZE_LAST_LOG_INDEX + SIZE_LAST_LOG_SENT)) 
         
#define SIZE_FLAG               sizeof(unsigned char)
#define SIZE_TIMESTAMP          sizeof(unsigned long int)
#define SIZE_CHANNEL            sizeof(unsigned int)
#define SIZE_VALUE              sizeof(float)                                            
#define SIZE_ID                 sizeof(unsigned char)                                            
#define SIZE_STATUS             sizeof(unsigned char)                                                             
#define SIZE_LOG_DATA           (SIZE_FLAG + SIZE_TIMESTAMP + SIZE_CHANNEL + SIZE_VALUE)               
#define SIZE_LOG_EVENT          (SIZE_FLAG + SIZE_TIMESTAMP + SIZE_ID + SIZE_STATUS)

#define OFFSET_FLAG             0
#define OFFSET_TIMESTAMP        SIZE_FLAG
#define OFFSET_DATA_CHANNEL     SIZE_FLAG + SIZE_TIMESTAMP
#define OFFSET_DATA_VALUE       SIZE_FLAG + SIZE_TIMESTAMP + SIZE_CHANNEL
#define OFFSET_EVENT_ID         SIZE_FLAG + SIZE_TIMESTAMP
#define OFFSET_EVENT_STATUS     SIZE_FLAG + SIZE_TIMESTAMP + OFFSET_EVENT_ID   

                                  
extern unsigned char LOGFILE_DATA[];
extern unsigned char LOGFILE_EVENT[];
extern unsigned char LOGFILE_ERROR[];  
extern unsigned char LOG_INIT_STR[];

extern int (*LOG_openFile)(FIL *, unsigned char *); 
extern int (*LOG_closeFile)(FIL *);        
             
extern char isDataLogEmpty;
extern char isEventLogEmpty;      
                            
int LOG_insertData(FIL *file, unsigned long int timestamp, unsigned int channel, float value);
int LOG_readData(FIL *file, unsigned long int *timestamp, unsigned int *channel, float *value, unsigned long int *index);
int LOG_insertEvent(FIL *file, unsigned long int timestamp, unsigned char id, unsigned char status);
int LOG_readEvent(FIL *file, unsigned long int *timestamp, unsigned char *id, unsigned char *status, unsigned long int *index);
int LOG_getLastInsert(FIL *file, unsigned long int *index);
int LOG_setLastInsert(FIL *file, unsigned long int index);
int LOG_getLastSend(FIL *file, unsigned long int *index);
int LOG_setLastSend(FIL *file, unsigned long int index);
unsigned long int index2Addr(unsigned long int index,int recordSize);
int LOG_createFile(unsigned char *filename, unsigned char mode, unsigned char *initString, unsigned int len);  
int LOG_init(void);


#endif

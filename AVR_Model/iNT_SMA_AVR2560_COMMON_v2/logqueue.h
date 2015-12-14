#ifndef LOGQUEUE_H
#define LOGQUEUE_H

#include "dataqueue.h"
#include "eventqueue.h"

struct data_log_st
{
    unsigned char           id;   
    struct data_record_st   data; 
};

struct event_log_st
{
    unsigned char           id;
    struct event_record_st  event;   
};
                                    
int  addDataLogQueue(unsigned char id, unsigned long int timestamp, unsigned int channel, float value);
int  readDataLogQueue(unsigned char id, unsigned long int *timestamp, unsigned int *channel, float *value);
int  removeDataLogQueue(unsigned char id);                        
char findFreeSlotDataLogQueue(void);
void initDataLogQueue(void);                              
int  addEventLogQueue(unsigned char id, unsigned long int timestamp, unsigned char eventID, unsigned char eventStatus);
int  readEventLogQueue(unsigned char id, unsigned long int *timestamp, unsigned char *eventID, unsigned char *eventStatus);
int  removeEventLogQueue(unsigned char id);      
char findFreeSlotEventLogQueue(void);
void initEventLogQueue(void);

#endif

#include <stdlib.h>
#include <stdio.h>
#include "eventqueue.h"                                       
#include "debug.h"

struct event_record_st eventQ[MAX_EVENT_QUEUE];
int last_wr_eventQ = 0;
int last_rd_eventQ = 0;
int eventQ_len = 0;

/*============================================================*/
int addEventQueue(unsigned long int timestamp, unsigned char eventID, unsigned char eventStatus)
{                
    if(eventQ_len == MAX_EVENT_QUEUE){   
        return -1;
    }                           
    
    eventQ[last_wr_eventQ].timestamp        = timestamp;
    eventQ[last_wr_eventQ].eventID          = eventID;
    eventQ[last_wr_eventQ].eventStatus      = eventStatus;       
                                                 
    last_wr_eventQ = (last_wr_eventQ + 1) % MAX_EVENT_QUEUE;
    
    eventQ_len++;                    
    
    return 0;       
}
/*============================================================*/
int fetchEventQueue(unsigned long int *timestamp, unsigned char *eventID, unsigned char *eventStatus)
{    
    if(eventQ_len == 0){                          
        return -1;
    }             
    
    *timestamp      =   eventQ[last_rd_eventQ].timestamp;
    *eventID        =   eventQ[last_rd_eventQ].eventID;
    *eventStatus    =   eventQ[last_rd_eventQ].eventStatus;             
                  
    last_rd_eventQ  = (last_rd_eventQ + 1) % MAX_EVENT_QUEUE;
    
    eventQ_len--;
    return 0;
}
/*============================================================*/

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#define MAX_EVENT_QUEUE  20

struct event_record_st
{
    unsigned long int   timestamp;   
    unsigned char       eventID;
    unsigned char       eventStatus;
};

//extern struct event_record_st eventQ[]; 
                                                                                
int addEventQueue(unsigned long int timestamp, unsigned char eventID, unsigned char eventStatus);
int fetchEventQueue(unsigned long int *timestamp, unsigned char *eventID, unsigned char *eventStatus);
                           
#endif

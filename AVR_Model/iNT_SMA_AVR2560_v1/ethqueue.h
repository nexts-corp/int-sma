#ifndef ETHQUEUE_H
#define ETHQUEUE_H   

#include "timer.h"

#define ETH_MAX_WAIT_QUEUE  15
#define ETH_MAX_PKT_LEN     255
#define ETH_MAX_RETRY       3
//#define ETH_MAX_RETRY       0
                     
//struct ethqueue{                                
//    char            retryCount;    
//    TIMER           retryTime;
//    int             timeout;          
//    unsigned char   packetLen;     
//    char            packetID; 
//    char            *packetData;
//};
#endif
//
//extern struct ethqueue eth_waitQueue[];       
//
//int  ETH_addWaitQueue(char id, char *pktptr, int len, int timeout);
//int  ETH_removeWaitQueue(char id);
//void ETH_initWaitQueue(void);
//char ETH_isIdExist(char id);
//char ETH_findFreeSlotWaitQueue(void);       
//char ETH_generateId(void);

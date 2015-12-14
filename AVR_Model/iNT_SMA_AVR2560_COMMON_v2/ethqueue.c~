#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ethqueue.h"
#include "debug.h"

#define MAX_SESS_LIST   100

struct ethqueue eth_waitQueue[ETH_MAX_WAIT_QUEUE];
char   ETH_waitQCount = 0;         
 
char   sess_list[MAX_SESS_LIST];
int    last_sess_id = 0;

/*============================================================*/
int ETH_addWaitQueue(char id, char *pktptr, int len, int timeout)
{
    int slot;
    
    slot = ETH_findFreeSlotWaitQueue();
    if(slot < 0){                         
        printDebug("\r\n\r\n<ETH_addWaitQueue> WARNING : Wait queue FULL\r\n\r\n");
        return -1;
    }                                                              
    
    
    eth_waitQueue[slot].packetData = (char *) malloc(len);
    if(eth_waitQueue[slot].packetData == NULL)
    {
        printDebug("<ETH_addWaitQueue> malloc(%d) failed\r\n",len);
        return -1;
    }             
    
    memcpy( eth_waitQueue[slot].packetData, pktptr, len ); 
    
    eth_waitQueue[slot].packetLen   = len;
    eth_waitQueue[slot].packetID    = id;    
    eth_waitQueue[slot].retryCount  = 0;    
    eth_waitQueue[slot].timeout     = timeout; 
       
    //eth_waitQueue[slot].retryTime   = 0;                                        
    TIMER_setTimer(&(eth_waitQueue[slot].retryTime), eth_waitQueue[slot].timeout);
    
    ETH_waitQCount++;                              
    printDebug("\r\n                                     Added (%02X) to eth_waitQueue[%d]\r\n", id, slot);         
    //print_payload(eth_waitQueue[slot].packetData,eth_waitQueue[slot].packetLen);
    return ETH_waitQCount;
}
/*============================================================*/
int ETH_removeWaitQueue(char id)
{ 
    char i;                             
    for(i=0;i<ETH_MAX_WAIT_QUEUE;i++){
        if(id == eth_waitQueue[i].packetID){        // scan id in eth_waitQueue        
            //printDebug("                                 Removing (%02X) from eth_waitQueue\r\n", id);     
            //print_payload(eth_waitQueue[i].packetData,eth_waitQueue[i].packetLen);      
            //memset( eth_waitQueue[i].packetData, '\0', ETH_MAX_PKT_LEN );      
            free(eth_waitQueue[i].packetData);                    
            eth_waitQueue[i].packetData  = NULL;
            eth_waitQueue[i].packetLen   = 0;
            eth_waitQueue[i].packetID    = 0;    
            eth_waitQueue[i].retryCount  = 0;    
            eth_waitQueue[i].timeout     = 0;    
            eth_waitQueue[i].retryTime   = 0;    
            ETH_waitQCount--;                                       
            //printDebug("Removed\r\n");                       
            printDebug("\r\n                                     Removed (%02X) to eth_waitQueue[%d]\r\n", id, i);  
            return ETH_waitQCount;                  
        }
    }                 
    printDebug("\r\n\r\nWARNING : ETH_removeWaitQueue(%02X) not found!!\r\n\r\n",id);
    return -1;
}
/*============================================================*/
void ETH_initWaitQueue(void)
{ 
    char i;      
                                  
    for(i=0;i<ETH_MAX_WAIT_QUEUE;i++){           
        //memset( eth_waitQueue[i].packetData, '\0', ETH_MAX_PKT_LEN );      
        free(eth_waitQueue[i].packetData);  
        eth_waitQueue[i].packetData  = NULL;
        eth_waitQueue[i].packetLen   = 0;
        eth_waitQueue[i].packetID    = 0;    
        eth_waitQueue[i].retryCount  = 0;    
        eth_waitQueue[i].timeout     = 0;    
        eth_waitQueue[i].retryTime   = 0;    
    }                                  
    return;                      
}             
/*============================================================*/
char ETH_isIdExist(char id)
{
    char i;      
                                  
    for(i=0;i<ETH_MAX_WAIT_QUEUE;i++){
        if(id == eth_waitQueue[i].packetID){
            return 1;                           // found id in eth_waitQueue
        }
    }          
    return 0;
}
/*============================================================*/
char ETH_isIdUsed(char id)
{
    char i;      
                                  
    for(i=0;i<MAX_SESS_LIST;i++){
        if(id == sess_list[i]){
            return 1;                           // found id in eth_waitQueue
        }
    }          
    return 0;
}
/*============================================================*/
char ETH_findFreeSlotWaitQueue(void)
{
    char i;
    for(i=0;i<ETH_MAX_WAIT_QUEUE;i++){
        if(eth_waitQueue[i].packetLen == 0){
            return i;    
        }
    }          
    return -1;
}
/*============================================================*/
char ETH_generateId(void)
{                  
    char id;
          
    do{
        id = (rand() % 256) & 0xFF;
    }while(ETH_isIdExist(id) || (id == 0) || (id == 0xFF) || ETH_isIdUsed(id));
    sess_list[last_sess_id] = id;
    last_sess_id = (last_sess_id + 1) % MAX_SESS_LIST;     
    
    return id;
} 
/*============================================================*/

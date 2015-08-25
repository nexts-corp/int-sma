#include <stdlib.h>
#include <stdio.h>
#include "dataqueue.h"                                             
#include "debug.h"

struct data_record_st dataQ[MAX_DATA_QUEUE];
int last_wr_dataQ = 0;
int last_rd_dataQ = 0;
int dataQ_len = 0;

/*============================================================*/
int addDataQueue(unsigned long int timestamp, unsigned int channel, float value)
{                
    if(dataQ_len == MAX_DATA_QUEUE){   
        return -1;
    }                           
    
    dataQ[last_wr_dataQ].timestamp      = timestamp;
    dataQ[last_wr_dataQ].channel        = channel;
    dataQ[last_wr_dataQ].value          = value;       
                                                 
    last_wr_dataQ = (last_wr_dataQ + 1) % MAX_DATA_QUEUE;
    
    dataQ_len++;                    
    
    return 0;       
}
/*============================================================*/
int fetchDataQueue(unsigned long int *timestamp, unsigned int *channel, float *value)
{    
    if(dataQ_len == 0){                          
        return -1;
    }             
    
    *timestamp  =   dataQ[last_rd_dataQ].timestamp;
    *channel    =   dataQ[last_rd_dataQ].channel;
    *value      =   dataQ[last_rd_dataQ].value;             
                  
    last_rd_dataQ = (last_rd_dataQ + 1) % MAX_DATA_QUEUE;
    
    dataQ_len--;
    return 0;
}
/*============================================================*/

#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#define MAX_DATA_QUEUE  10

struct data_record_st
{
    unsigned long int   timestamp;
    unsigned int        channel;
    float               value;    
};

//extern struct data_record_st dataQ[]; 

int addDataQueue(unsigned long int timestamp, unsigned int channel, float value);          
int fetchDataQueue(unsigned long int *timestamp, unsigned int *channel, float *value);
                           
#endif

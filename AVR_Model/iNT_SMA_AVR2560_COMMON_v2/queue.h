#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

typedef struct{
	char value[255];
	int length;
}iSTData_t; //struct data test

typedef struct{
    int itemQuantity;    //member of item
    int itemCount;        //count of cirrenct imet
    int itemLength;     //length of item
    int itemLock;        //lock , unlock of queue  
    
    int *head;
    int *tail;
    int *writeTo;
    
    int *readDataFrom;    

}iQueue_t;

//typedef struct{
//    int *head;
//    int *tail;
//    int *writeTo;
//    
//    int *readDataFrom;    
//    int itemQuantity;    //member of item
//    int itemCount;        //count of cirrenct imet
//    int itemLength;     //length of item
//    int itemLock;        //lock , unlock of queue
//}iQueue_t;

typedef void * piQueueHandle_t;

piQueueHandle_t iQueueCreate(const unsigned int iItemQuantity_p,const unsigned int iItemLength_p);
int iQueueSend(piQueueHandle_t const iQueue_p,const void* const piItem_p);
int iQueueReceive(piQueueHandle_t iQueue_p,void* const piItem_p);



#endif 


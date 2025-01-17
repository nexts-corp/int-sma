#ifndef TRANSMIT_INITIAL_H
#define TRANSMIT_INITIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRANS_ERROR     0
#define TRANS_FAIL      1
#define TRANS_SUCCESS   2

typedef void * iTransmitHandle_t;
typedef unsigned char iUBaseType_t;
typedef char iBaseType_t;
typedef unsigned int iUInt_t;
typedef int iInt_t;
typedef unsigned long iULong_t;
typedef long iLong_t;

typedef struct{
       iBaseType_t status;
       
       union{
         iBaseType_t *value;
         iUBaseType_t length;
       }u;
        
}iTransmit_t;

iTransmitHandle_t transmitInit();
iTransmitHandle_t transmitGet();
iTransmitHandle_t transmitSet();
iTransmitHandle_t transmitCreate();
iTransmitHandle_t transmitDelete();

#endif 
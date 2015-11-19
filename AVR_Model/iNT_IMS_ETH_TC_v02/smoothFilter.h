#ifndef SMOOTH_FILTER_H
#define SMOOTH_FILTER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union{
   int status;
   float value; 
}iSmoothData_t;


typedef void * pviSmoothHandle_t;
extern pviSmoothHandle_t pviSmoothSensorHandle;
extern iSmoothData_t iSmoothData;


iSmoothData_t smoothFilter(pviSmoothHandle_t pviSmoothHandle_p,int adcValue_p);

pviSmoothHandle_t smoothAllocateMem(int itemSize);


#endif 
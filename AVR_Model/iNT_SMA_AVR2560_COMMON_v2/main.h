#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include "schedulerTask.h"
#include "queue.h"

typedef void * pTaskHandle;
typedef unsigned char iUChar_t;
typedef char iChar_t;
typedef unsigned int iUInt_t;
typedef int iInt_t;
typedef long iLong_t;
typedef unsigned long iULong_t;
typedef float iFloat_t;

extern piQueueHandle_t pviQueueTaskAToTaskB;
extern piQueueHandle_t pviQueueTaskBToTaskA;

extern piQueueHandle_t pviQueueSensorToMang;
extern piQueueHandle_t pviQueueMangToSensor;

extern piQueueHandle_t pviQueueMangToConfig;
extern piQueueHandle_t pviQueueDeviceToMang;

typedef struct{
    int length;
    char value[300];
}iData_t;

typedef struct{
    int length;
    char value[100];
}iDataMini_t;

typedef struct{
    iUInt_t viId;
    iUChar_t viCmd;
    iUInt_t viLength;
    iChar_t * pviValue;
}iConfigFormat_t;

extern iData_t viRXData;
extern iData_t viTXData;

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;
extern iDataMini_t viEventReport;

#endif 
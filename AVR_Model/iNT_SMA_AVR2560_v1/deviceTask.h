#ifndef DEVICE_TASK_H
#define DEVICE_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

#define DEVICE_ENABLE       0
#define ALARM_ENABLE        1
#define ALARM_ENABLE        1

typedef struct{
    iChar_t viCMD;
    iChar_t viData; 
}iDeviceCMD;

extern eeprom iUChar_t viFlagTerInit;           //Terminal initial frist
void deviceTask(void *pviParameter);


#endif 
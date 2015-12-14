#ifndef EVENT_TASK_H
#define EVENT_TASK_H

#include "main.h"
typedef struct{
    iChar_t viEventID; 
    float viData;
}iEventReport_t;

extern iEventReport_t viEventReportBuff;
void eventTask(void *pviParameter);

#endif 
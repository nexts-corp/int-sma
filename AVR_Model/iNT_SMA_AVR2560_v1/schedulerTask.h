#ifndef SCHEDULER_TASK_H
#define SCHEDULER_TASK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

#define MAX_TASK_NAME       16
#define MAX_TASK            6
typedef void * itaskHandel_t;
typedef void (*piTaskFunction)(void *);
typedef void (*piTaskHookFunction)(void *);

typedef unsigned int uint_t;
typedef int int_t;
typedef unsigned char uchar_t;
typedef int char_t;
typedef unsigned char byte_t;

typedef struct{
    uint_t id;
    piTaskHookFunction hookFunction;
    uchar_t name[16];
    uint_t priority;
}iTask_t;


itaskHandel_t iTaskCreate(piTaskFunction piTaskCode_p,const char * const name_p,uint_t priority_p);
void startSchedulerTask();

#endif 
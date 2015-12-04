#ifndef INTERRUPT_TASK_H
#define INTERRUPT_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

typedef int  piInterrupt_t;

extern piInterrupt_t pviUart0TimeOut;
extern int wiz100srSetTimer;

void interruptTask(void *pviParameter);


#endif 
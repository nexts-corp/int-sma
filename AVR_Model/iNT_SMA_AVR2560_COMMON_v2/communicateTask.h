#ifndef COMMUNICATE_TASK_H
#define COMMUNICATE_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

#define INTERFACE_LAN_TYPE      0
#define INTERFACE_WIFI_TYPE     1
#define INTERFACE_GSM_TYPE      2

void communicateTask(void * pviParameter);

#endif 
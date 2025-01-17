#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include "main.h"
#include "dataAndStatusTask.h"

#define STATUS_MAX  0x02

#define STATUS_BATTERY_TYPE  0x01

#define STATUS_BIT_LAVEL    0x01
#define STATUS_BYTE_LAVEL   0x02
#define STATUS_INTEGER      0x03
#define STATUS_FLOAT        0x04


iChar_t iStatusRead(iDataMini_t *pviOutData_arg);
void iStatusReadByType(iStatusReport_t *pviInOutData_arg);
//float iStatusReadBattery();

#endif 
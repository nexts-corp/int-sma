#ifndef LAN_H
#define LAN_H
#include "main.h"
#include "wiz100rs.h"

#define LEN_DATA_AVAILABLE       0x01
#define LEN_DATA_EMPTY           0x00

#define LEN_DATA_WRITE_SUCCESS   0x01
#define LEN_DATA_WRITE_FAIL      0x00

#define LAN_CONNECTED       0x01
#define LAN_CLOSE           0x02
#define LAN_UNKNOW          0x00
#define LAN_CONN_FAIL       0x00

//extern TIMER            commLostTimer;

iUChar_t iLanReadData(iData_t * pviData_agr);
//iUChar_t iLanWriteData();
iUChar_t iLanWriteData(iData_t * pviData_agr);
iChar_t iLanInit();
void iLanRestart();
iUChar_t iLanStatus();

#endif 
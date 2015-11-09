#ifndef MANAGER_TASK_H
#define MANAGER_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

#define REALTIME_MODE   0x00
#define LOGGER_MODE     0x01

#define DEVICE_DISABLE   0x00
#define DEVICE_ENABLE    0x01

#define LAN_INTERFACE   0x00
#define WIFI_INTERFACE  0x01
#define GSM_INTERFACE   0x02

#define MANG_QUEUE_MAX  2
#define MANG_RETRANSMIT 3

typedef struct{
    iUInt_t recordID;
    iChar_t fid[2];
    iChar_t reTransmit; 
    iUInt_t length; 
    void *value;
}iMangQueue_t;


void managerTask(void *pviParameter);
void iTagParser(iData_t *pviData_arg);
iUInt_t iHostReqProcess(iData_t * pviTXDataBuff_arg,iData_t * pviRXDataBuff_arg);
iInt_t iMangQueueAddItem(iUInt_t viRecordID_arg,iData_t *pviDataItem_arg,iChar_t *pviFid_arg);
iInt_t iMangQueueDelItem(iChar_t *pviFid_arg);
void iMangQueueDisplay();
iChar_t iMangCheckRetransmit(iChar_t *pviFid_arg);
void memcpyd(char *dptr, char eeprom *eptr,  int len);
void memcpye(char eeprom *eptr, char *dptr, int len);
void printConfig(char eeprom *eptr,unsigned int len);
void iTerAckByTagB(iChar_t viTagName, iChar_t viPropNumber);
void iSyncUniTime(unsigned long int *viUniTime_arg);


#endif 
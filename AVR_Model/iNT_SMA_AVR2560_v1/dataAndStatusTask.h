#ifndef DATA_AND_STATUS_TASK_H
#define DATA_AND_STATUS_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

#define SENSER_MAX_TYPE          2           //   ADC,DIGITAL

#define SENSER_ADC_MAX_SIZE      5
#define SENSER_DIGI_MAX_SIZE     1

#define SENSOR_ADC               0
#define SENSOR_DIGI              1

typedef struct{
    iChar_t viChannelID;
    iChar_t viSensorID; 
    iChar_t viUnitID;
    float viData;
}iDataReport_t;


typedef struct{
    iChar_t viStatusType; 
    iChar_t viDataType;
    float viData;
}iStatusReport_t;

typedef struct{
     unsigned int adc[SENSER_ADC_MAX_SIZE]; 
     float dht11Temp; 
     float dht11Humi;
}iSensorData_t;

extern iDataReport_t    viDataReportBuff;
extern iStatusReport_t  viStatusReportBuff;




void task_a(void *pviParameter);
void dataAndStatusTask(void *pviParameter);
void swContextSensor();
#endif 
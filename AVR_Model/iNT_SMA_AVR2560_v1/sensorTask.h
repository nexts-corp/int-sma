#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H
#include "main.h"
#include "queue.h"
#include "debug.h"

#define SENSER_MAX_TYPE          2           //   ADC,DIGITAL

#define SENSER_ADC_MAX_SIZE      5
#define SENSER_DIGI_MAX_SIZE     1

#define SENSOR_ADC               0
#define SENSOR_DIGI              1

typedef struct{
     unsigned int adc[SENSER_ADC_MAX_SIZE]; 
     float dht11Temp; 
     float dht11Humi;
}iSensorData_t;






void task_a(void *pviParameter);
void sensorTask(void *pviParameter);
void swContextSensor();
#endif 
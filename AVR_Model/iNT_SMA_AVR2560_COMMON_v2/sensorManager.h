#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "main.h"
#include "dataAndStatusTask.h"



//#define  SEN_ANALOG_GROUP   0x10
#define  SEN_ADC_TYPE       0x11
#define  SEN_K_TYPE         0x12
#define  SEN_TMEC_TYPE      0x13       //Thermocouple
#define  SEN_TMCP_TYPE      0x14       //Thermocouple
#define  SEN_TP100_TYPE     0x16
#define  SEN_VOLT_TYPE      0x17
#define  SEN_AMP_TYPE       0x18
#define  SEN_420_TYPE       0x19

//#define  SEN_DIGITAL_GROUP  0x20
#define  SEN_DHT11_TYPE         0x21
#define  SEN_DHT22_TYPE         0x22
#define  SEN_DS1820_TYPE        0x23
#define  SEN_DS18B20_TYPE       0x24

#define  SEN_ADC10BIT_UNIT    0x31
#define  SEN_TEMP_C_UNIT      0x32
#define  SEN_VOLTT_UNIT       0x33
#define  SEN_AMP_UNIT         0x34
#define  SEN_420_UNIT         0x35

#define SENSOR_PRINT_DEBUG  1

iChar_t iSensorRead(iDataMini_t *pviOutData_arg);
void iSensorReadByType(iDataReport_t *pviInOutData_arg);

#endif 
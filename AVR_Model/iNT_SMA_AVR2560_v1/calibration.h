#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <mega2560.h>
#include <stdio.h>
#include "adc.h"
#include "io.h"

#define CAL_DEFAULT             0x00
           
#define CAL_EXTOFFSET           0x01
#define CAL_EXTLINEAR           0x02                 
#define CAL_EXTPOLYNOMIAL       0x03 
#define CAL_SELFCOMPARE         0x04//0x01

#define CAL_POINT1  0x00
#define CAL_POINT2  0x01
#define CAL_POINT3  0x02 

#define CAL_GET 0x00 
#define CAL_SET 0x01

typedef struct{
    unsigned int prop;     //get, set
    unsigned int channelID;
    unsigned int pointID;
    float pointValue;        //Point value 
    unsigned int calType; //format type exm, (offset,linear,polynomial)
}ST_CAL_CONFIG;


typedef struct{
    float stdRef[3];
    float sensorRef[3]; 
    unsigned int calType; //format type exm, (offset,linear,polynomial)
}ST_CAL_CHANNEL;

typedef struct{
   ST_CAL_CHANNEL sChennel[5]; 
}ST_CAL;

extern ST_CAL_CONFIG sensorCalConfig;
extern eeprom  ST_CAL sensorCalulate;          //improtance
extern void (*pfCalibrate) (ST_CAL_CONFIG calConfig);   //pointer function of calibation set


void calGet(ST_CAL_CONFIG calConfig);
void calSet(ST_CAL_CONFIG calConfig);
void calSetConfirm(ST_CAL_CONFIG calConfig);
void calSetClearing(ST_CAL_CONFIG calConfig);
float calProcessValue(float sensorValue,unsigned int channelID);


#endif 
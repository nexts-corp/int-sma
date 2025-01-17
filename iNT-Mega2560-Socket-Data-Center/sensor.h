#ifndef SENSOR_H
#define SENSOR_H


#include <mega2560.h>
#include <stdio.h>
#include "adc.h"
#include "io.h"

/********************************************
SENSOR STATUS
bit 7   6   5   4       3   2   1   0
    0   0   0   0       0   0   0   0                    
    ^   ^   ^   ^       ^   ^   ^   ^   Normal Condition
    |           |       |   |   |   ^-- Lower Limit Exceeded
    |           |       |   |   ^------ Lower Extreme Limit Exceeded
    |           |       |   ^---------- Upper Limit Exceeded
    |           |       ^-------------- Upper Extreme Limit Exceeded
    |           ^---------------------- Time Delay Expired               
    |
    |                       ^   ^   ^   Failure Condition   
    |                       |   |   ^-- Lower Bound Scale Error (when bit 6 = '1') 
    |                       |   ^------ Upper Bound Scale Error    
    |                       ^---------- Sensor Disconnected         
    |
    ^---------------------------------- 0 = Normal Condition, 1 = Failure Condition       
-- note --
 - (set all bit to '1' for unused sensor)
 
********************************************/
// --- Status Code --- //
#define SENS_NORMAL             0x00
#define SENS_ERROR              0x80
#define SENS_ERR_LOWER_BOUND    0x81
#define SENS_ERR_UPPER_BOUND    0x82
#define SENS_ERR_DISCONNECT     0x84
#define SENS_ERR_SHORT          0x90
#define SENS_ERR_UNKNOWN        0xFF

#define CAL_DEFAULT             0x00
#define CAL_SELFCOMPARE         0x01           
#define CAL_EXTOFFSET           0x02
#define CAL_EXTLINEAR           0x03                 
#define CAL_EXTPOLYNOMIAL       0x04       

#define CAL_STD_REF_P1              0x01           //standrad value reference
#define CAL_STD_REF_P2              0x02
#define CAL_STD_REF_P3              0x03                  

#define CALPOINT(x)             (100+x)

// ---- Limit Bound (refer to Thai Red Cross model) ---- //
#define TMEC_SHORT_LIMIT        2.0            
#define TMEC_OPEN_LIMIT         1021.0
#define PT100_SHORT_LIMIT       8.0
#define PT100_OPEN_LIMIT        970.0

//extern eeprom float calS1[5];//standrad point 1
//extern eeprom float calS2[5];//standrad point 2
//extern eeprom float calS3[5];//standrad point 3
////
//extern eeprom float calX1[5];//input sensor point 1
//extern eeprom float calX2[5];//input sensor point 2
//extern eeprom float calX3[5];//input sensor point 3







struct sens_val_st
{
    float           value;               //-- sensor value -- //
    unsigned char   status;              //-- sensor status -- //
};
                                  
extern struct sens_val_st       sensor[];


int sensorRead(int ch, struct sens_val_st *sensor);      
void updateSensorADC(int ch); 
float voltPeak(float data[]);
float average(float data[],int count); 
float sensorGetValue(unsigned int channelID);
//int sensorCalibrate1(int p1, int p2);                       
//int sensorCalibrate2(int prbId); 
//int sensorGetCurrentSetPoint(int ch,int pnt,unsigned char *buf);          
//int sensorSetCurrentSetPoint(int ch,int pnt,unsigned char *buf);
//void getInterval(float adcRaw, float *x1, float *x2, float *y1, float *y2);

#endif


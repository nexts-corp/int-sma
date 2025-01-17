#ifndef SENSOR_H
#define SENSOR_H

#include "adc.h"
#include "io.h"

/*********************************************
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
 
*********************************************/
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

#define CALPOINT(x)             (100+x)

// ---- Limit Bound (refer to Thai Red Cross model) ---- //
#define TMEC_SHORT_LIMIT        2.0            
#define TMEC_OPEN_LIMIT         1021.0
#define PT100_SHORT_LIMIT       8.0
#define PT100_OPEN_LIMIT        970.0

//#define TMCP_OPEN_LIMIT         1.620 //Over range +160  
//#define TMCP_SHORT_LIMIT       -1.464 //Over range -200

//#define TMCP_OPEN_LIMIT         1.470 //Over range +160  
//#define TMCP_SHORT_LIMIT       -1.504 //Over range -200

#define TMCP_OPEN_LIMIT         1.470 //Over range +160  
#define TMCP_SHORT_LIMIT       -1.504 //Over range -200

struct sens_val_st
{
    float           value;              // -- sensor value -- //
    unsigned char   status;             // -- sensor status -- //
};

//typedef struct{
//
//}iSensor_t;
                                  
extern struct sens_val_st       sensor[];

int sensorRead(int ch, struct sens_val_st *sensor);        
int sensorCalibrate1(int p1, int p2);                       
int sensorCalibrate2(int prbId); 
int sensorGetCurrentSetPoint(int ch,int pnt,unsigned char *buf);          
int sensorSetCurrentSetPoint(int ch,int pnt,unsigned char *buf);
void getInterval(float adcRaw, float *x1, float *x2, float *y1, float *y2);
void iSensorGetADCValue();
float iUpdateSmoothlyADC(int ch);

#endif

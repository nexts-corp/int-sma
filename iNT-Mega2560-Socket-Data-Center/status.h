#ifndef STATUS_H
#define STATUS_H

#include "ethcomm.h"
#include "sensor.h"
#include "adc.h"
#include "io.h"
#include "memcard.h"

#define MTNC_NORMAL             0x00
#define MTNC_MIP                0x01

#define GSM_NORMAL              0x00
#define GSM_POOR_SIGNAL         0x01
#define GSM_DATE_EXPIRED        0x02
#define GSM_NO_OPERATOR         0x04

#define GBAL_NORMAL             0x00
#define GBAL_LOW_BALANCE        0x01

#define INDIC_MUTE_ALARM        0x01

#define PWR_NORMAL              0x00
#define PWR_DOWN                0x01                

#define BATT_NORMAL             0x00
#define BATT_LOW                0x01                // -- show on oled -- //

#define MEM_NORMAL              0x00
#define MEM_LOW                 0x01                // -- show on oled -- //
#define MEM_ERROR               0x02
                
#define COMM_NORMAL             0x00
#define COMM_POOR               0x01
#define COMM_LOST               0x02                

#define COMM_LAN_CONNECT        0x00
#define COMM_LAN_DISCONNECT     0x01  

#define DIN_FALLING             0x00
#define DIN_RISING              0x01
                                    
#define DOUT_LOW                0x00
#define DOUT_HIGH               0x01

#define AIN_LOWER               0x00
#define AIN_HIGHER              0x01

#define DATA_NORMAL             0x00
#define DATA_LOWER_LIMIT        0x01
#define DATA_LOWER_DELAY        0x11
#define DATA_LOWER_EXTREME      0x02
#define DATA_UPPER_LIMIT        0x04
#define DATA_UPPER_DELAY        0x14
#define DATA_UPPER_EXTREME      0x08

/*
//-- These Status moved to sensor.h --//
#define SENS_NORMAL             0x00
#define SENS_ERROR              0x80
#define SENS_ERR_LOWER_BOUND    0x81
#define SENS_ERR_UPPER_BOUND    0x82
#define SENS_ERR_DISCONNECT     0x84
#define SENS_ERR_SHORT          0x90
#define SENS_ERR_UNKNOWN        0xFF
*/


#define UNKNOWN                 0xFF

struct status_st
{
    char                id;
    char                status;    
};

extern /*eeprom*/ char sensorStatus[];
extern /*eeprom*/ char dataStatus[];  
extern char mtncStatus;              
extern char powerSrcStatus;    
extern char batteryStatus;                    
extern char dinStatus[];
extern char doutStatus[];
extern char serverCommStatus;
extern char lanStatus;
extern char memoryStatus;
                            
extern float batteryVolt; 

int updateSensorStatus(int ch);
int updateDataStatus(int ch);      
int updateBatteryStatus(void);            
int updateDinStatus(void);
int updateDoutStatus(void);                    
int updateCommStatus(void);                  
int updateMemoryStatus(void);
int getDeviceState(unsigned short *state);    


#endif

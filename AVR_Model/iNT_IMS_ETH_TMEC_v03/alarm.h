#ifndef ALARM_H
#define ALARM_H

#include "sensor.h"
#include "status.h"
#include "timer.h"

#define SRC_SENS_EVENT              0x70

#define SRC_SENS1_EVENT             0xC0
#define SRC_SENS1_LOWER             0xC1
#define SRC_SENS1_LOWERDELAY        0xC2
#define SRC_SENS1_LOWEREXTREME      0xC3
#define SRC_SENS1_UPPER             0xC4
#define SRC_SENS1_UPPERDELAY        0xC5
#define SRC_SENS1_UPPEREXTREME      0xC6
#define SRC_SENS1_ERROR             0xC7

#define SRC_SENS2_EVENT             0xC8
#define SRC_SENS2_LOWER             0xC9
#define SRC_SENS2_LOWERDELAY        0xCA
#define SRC_SENS2_LOWEREXTREME      0xCB
#define SRC_SENS2_UPPER             0xCC
#define SRC_SENS2_UPPERDELAY        0xCD
#define SRC_SENS2_UPPEREXTREME      0xCE
#define SRC_SENS2_ERROR             0xCF
                 
#define SRC_SENS3_EVENT             0xD0
#define SRC_SENS3_LOWER             0xD1
#define SRC_SENS3_LOWERDELAY        0xD2
#define SRC_SENS3_LOWEREXTREME      0xD3
#define SRC_SENS3_UPPER             0xD4
#define SRC_SENS3_UPPERDELAY        0xD5
#define SRC_SENS3_UPPEREXTREME      0xD6
#define SRC_SENS3_ERROR             0xD7

#define SRC_SENS4_EVENT             0xD8
#define SRC_SENS4_LOWER             0xD9
#define SRC_SENS4_LOWERDELAY        0xDA
#define SRC_SENS4_LOWEREXTREME      0xDB
#define SRC_SENS4_UPPER             0xDC
#define SRC_SENS4_UPPERDELAY        0xDD
#define SRC_SENS4_UPPEREXTREME      0xDE
#define SRC_SENS4_ERROR             0xDF

#define SRC_SENS5_EVENT             0xE0
#define SRC_SENS5_LOWER             0xE1
#define SRC_SENS5_LOWERDELAY        0xE2
#define SRC_SENS5_LOWEREXTREME      0xE3
#define SRC_SENS5_UPPER             0xE4
#define SRC_SENS5_UPPERDELAY        0xE5
#define SRC_SENS5_UPPEREXTREME      0xE6
#define SRC_SENS5_ERROR             0xE7

#define SRC_DI_EVENT_ANY            0x71
#define SRC_DI_RISING_ANY           0x72
#define SRC_DI_FALLING_ANY          0x73

#define SRC_DI_EVENT(x)             (0x40 | x)
#define SRC_DI_RISING(x)            (0x50 | x)
#define SRC_DI_FALLING(x)           (0x60 | x)

#define SRC_AI_EVENT_ANY            0x74
#define SRC_AI_HIGHER_ANY           0x75
#define SRC_AI_LOWER_ANY            0x76

#define SRC_AI_EVENT(x)             (0x10 | x)
#define SRC_AI_HIGHER(x)            (0x20 | x)
#define SRC_AI_LOWER(x)             (0x30 | x)

#define SRC_MUTE_ALARM              0x80
#define SRC_MIP                     0x81
#define SRC_POWER_DOWN              0x82
#define SRC_LOW_BATTERY             0x83
#define SRC_LOW_COMM_SIGNAL         0x84
#define SRC_LOW_MEMORY              0x85
#define SRC_MEMORY_ERROR            0x86
#define SRC_LOST_OF_COMM            0x87
#define SRC_GSM_LOW_SIGNAL          0x90
#define SRC_GSM_DATE_EXPIRED        0x91
#define SRC_GSM_NO_OPERATOR         0x92
#define SRC_GSM_TOP_UP              0x93
#define SRC_GSM_LOW_BALANCE         0x94
#define SRC_GSM_UNKNOWN_FAILURE     0x9F

#define MUTE_PERMAMENT              0x01
#define MUTE_TEMPORARY              0x02

extern TIMER muteAlarmExpired;                           
extern char muteMode;              

extern /*eeprom*/ char sensorAlarm[];
extern /*eeprom*/ char dataAlarm[]; 
extern /*eeprom*/ char dinAlarm[];
extern /*eeprom*/ char batteryAlarm;
extern /*eeprom*/ char memoryAlarm;    
     
int checkAlarm(char *srcList,int srcLen);
int checkAlarmFromSrc(char alarmSrc);                    
int checkSendMIPAlarm(unsigned char *code);
int checkSendPowerAlarm(unsigned char *code);
int checkSendBatteryAlarm(unsigned char *code);
int checkSendMemoryAlarm(unsigned char *code);
int checkSendDinAlarm(int ch, unsigned char *code);
int checkSendSensorAlarm(int ch, unsigned char *code);
int checkSendDataAlarm(int ch, unsigned char *code);     
int ALARM_mute(unsigned long int period);         
int ALARM_checkMuteExpired(void);                           
void resetAlarm(void);

#endif

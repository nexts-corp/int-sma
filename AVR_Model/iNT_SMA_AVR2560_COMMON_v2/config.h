#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"
#include "configDef.h"

#define SENSOR_DISABLE_def           0x00
#define SENSOR_ENABLE_def            0x01





#define MAX_GROUP_TAG           15

#define MAX_PROP_SUB_TAG        0x0E

#define NETWORK_CONF_def 0x00

typedef struct{
    iChar_t viTag;
    iChar_t viLength[30];
    void *viValue;
}iTLV_t; 


typedef struct{
     iChar_t viNetworkPort;
     iChar_t viMethod;
     iChar_t viLocolIP[4];
     iChar_t viLocalPort[4];
     iChar_t viSubNetMask[4];
     iChar_t viGateWay[4];
     iChar_t viServerIP[4];
     iChar_t viServerPort[4];
     iChar_t viDNS_IP[4];
     iChar_t viSDN[32];
     iChar_t viSSID[32];
     iChar_t viPassword[20];
     iChar_t viSecurity;
     iChar_t viAPN[20];
}iNetworkConfig_t;

typedef struct{
     iChar_t viDeviceType[20];
     iChar_t viFirmWareVersion[20];
     iChar_t viDeviceName[40];
     iChar_t viDeviceID[20];
     iChar_t viHostName[40];
     iChar_t viHostID[20];
     iChar_t viTimeZone;
}iPropertieseConfig_t;

typedef struct{
     iChar_t viDeviceEnable;
}iOperationConfig_t;

typedef struct{
     iChar_t viAlarmEnable;
     iChar_t viSensorEnable; 
     float viSamplingRate;
     iChar_t viSensorOutput;
     iChar_t viSensorTypeID;
     iChar_t viDataUnit;
     float viMinimum;
     float viMaximum;
     float viLower;
     float viLowerDelay;
     float viLowerExtreme;
     float viUpper;
     float viUpperDelay;
     float viUpperExtreme;
}iSensorChannelConfig_t;

typedef struct{
     iChar_t viAlarmEnable;
     iChar_t viDI_Enable; 
     iChar_t viMode;
     iChar_t viDetectEdge;
}iDI_ChannelConfig_t;

typedef struct{
     iChar_t viD_O_Enable;
     iChar_t viState; 
     float viDelay;
}iDO_ChannelConfig_t;

typedef struct{
     iChar_t viPWM_Enable;
     float viLevel;
}iPWM_ChannelConfig_t;

typedef struct{
   iChar_t viDeviceType[20]; 
   iChar_t viFirmVersion[20];
   iChar_t viDeviceName[40];
   iChar_t viDeviceID[20];
   iChar_t viHostName[40];
   iChar_t viHostID[20];
   iChar_t viTimeZone;
}iPropperties_t;

typedef struct{
   iChar_t viDeviceEnable;
   iChar_t viAlarmEnable;
   iChar_t viSensorEnable[7];
}iOperate_t;


typedef struct{
   iChar_t viEnable;
   iChar_t viAlarmDelay[2];
}iAuxDigiInput_t;

typedef struct{
   iChar_t viEnable;
   iChar_t viOnEventState;
   iChar_t viNumOfEventSrc;
   iChar_t viBindToEventScr[25];
}iAuxDigiOutput_t;



typedef struct{
   iChar_t viChannelID;
   iChar_t viSensorID;
   iChar_t viUnitID;
   
   iChar_t viAlarmEnable; 
   float viAlarmLower;
   float viAlarmLowerDelay;
   float viAlarmLowerExtreme;
   float viAlarmUpper;
   float viAlarmUpperDelay;
   float viAlarmUpperExtreme;
}iSensorConfig_t;


typedef struct{
   iNetworkConfig_t  viNetworkConfig; 
   iPropertieseConfig_t viPropertieseConfig;
   iOperationConfig_t viOperationConfig;
   iSensorChannelConfig_t viSensorChannelConfig[MAX_SENSOR_CHANNEL_def];
   iDI_ChannelConfig_t viDI_ChannelConfig[MAX_DI_CHANNEL_def]; 
   iDO_ChannelConfig_t viDO_ChannelConfig[MAX_DO_CHANNEL_def];
   iPWM_ChannelConfig_t viPWM_ChannelConfig;
   
   iPropperties_t    viProperties;
   iOperate_t        viOperate; 
   iSensorConfig_t   viSensorConfig[7];
   iAuxDigiInput_t   viAuxDigiInput;
   iAuxDigiOutput_t  viAuxDigiOutput;
}iDVConfig_t;




extern eeprom iDVConfig_t viDVConfigUpdate;
extern iDVConfig_t viDVConfigBuffer;

void iConfigDefaultInt();
void iMemcpye(char eeprom *eptr, char *dptr, int len);
void iMemcpyd(char *dptr, char eeprom *eptr,  int len);
void iPrintConfig(char eeprom *eptr,unsigned int len);



#endif




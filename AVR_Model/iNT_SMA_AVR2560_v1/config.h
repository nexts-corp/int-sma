#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"

#define SENSOR_DISABLE_def           0x00
#define SENSOR_ENABLE_def            0x01


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
   iPropperties_t    viProperties;
   iOperate_t        viOperate; 
   iSensorConfig_t   viSensorConfig[7];
   iAuxDigiInput_t   viAuxDigiInput;
   iAuxDigiOutput_t  viAuxDigiOutput;
}iDVConfig_t;


extern eeprom iDVConfig_t viDVConfigUpdate;
//
//typedef struct{
//   iChar_t viEnable;
//   iChar_t 
//}iAuxAnalogInput_t;
//
//typedef struct{
//
//}iAuxAnalogOutput_t;
//
//typedef struct{
//
//}iBuzzer_t;
//
//typedef struct{
//
//}iEnvirronment_t;
//
//typedef struct{
//
//}iStatusReport_t;
//
//typedef struct{
//
//}iDirection_t;
//
//typedef struct{
//
//}iSensorConfig_t;
//
//typedef struct{
//    
//}iDeviceConfig_t;
//
//typedef struct{
//
//}iDevice_t;


//#include "configstructure.h"
//
//// -- configuration group -- //
//#define CONF_PROPERTIES         0x10
//#define CONF_OPERATE            0x20
//#define CONF_SENSOR             0x30
//#define CONF_AUXDI              0x40
//#define CONF_AUXDO              0x50
//#define CONF_AUXAI              0x60
//#define CONF_AUXAO              0x70
//#define CONF_BUZZER             0x80
//#define CONF_ENVALARM           0x90
//#define CONF_STATREPORT         0xA0
//                  
//#define REPORT_SINGLE           0x10
//#define REPORT_MULTIPLE         0x20
//#define REPORT_ALL              0x30
//
//#define MODE_REALTIME           0x10
//#define MODE_DATALOGGER         0x20
//                                      
//extern char is_properties_config_update;
//extern char is_operating_config_update;
//extern char is_sensor_config_update[];          
//extern char is_din_config_update[];          
//extern char is_dout_config_update[];
//extern char is_buzzer_config_update;
//extern char is_status_config_update;
//extern char is_env_config_update;
//                                              
//extern struct prop_cnfg_st properties_config;
//extern struct oper_cnfg_st operating_config;
//extern struct sens_cnfg_st sensor_config[];            
//extern struct din_cnfg_st  din_config[];          
//extern struct dout_cnfg_st dout_config[];
//extern struct buzz_cnfg_st buzz_config; 
//extern struct stat_cnfg_st status_config;  
//extern struct alm_cnfg_st  env_config;
//
//extern flash char FIRMWARE_VERSION[];
//                      
//int getPropertiesConfig(struct prop_cnfg_st *readConfig);
//int getOperatingConfig(struct oper_cnfg_st *readConfig);
//int getSensorConfig(int ch, struct sens_cnfg_st *readConfig);
//int getAuxDinConfig(int ch, struct din_cnfg_st *readConfig);
//int getAuxDoutConfig(int ch, struct dout_cnfg_st *readConfig);
//int getAuxAinConfig(int ch, struct ain_cnfg_st *readConfig);
//int getAuxAoutConfig(int ch, struct aout_cnfg_st *readConfig);  
//int getBuzzerConfig(struct buzz_cnfg_st *readConfig);
//int getEnvAlarmConfig(struct alm_cnfg_st *readConfig);
//int getStatusReportConfig(struct stat_cnfg_st *readConfig);
//
//int setPropertiesConfig(struct prop_cnfg_st *newConfig);
//int setOperatingConfig(struct oper_cnfg_st *newConfig);
//int setSensorConfig(int ch, struct sens_cnfg_st *newConfig);
//int setAuxDinConfig(int ch, struct din_cnfg_st *newConfig);
//int setAuxDoutConfig(int ch, struct dout_cnfg_st *newConfig);
//int setAuxAinConfig(int ch, struct ain_cnfg_st *newConfig);
//int setAuxAoutConfig(int ch, struct aout_cnfg_st *newConfig);
//int setBuzzerConfig(struct buzz_cnfg_st *newConfig);
//int setEnvAlarmConfig(struct alm_cnfg_st *newConfig);
//int setStatusReportConfig(struct stat_cnfg_st *newConfig); 
//
//int setDefaultConfig(void);
//int isDeviceConfig(void);
//int getAllConfig(void);

#endif




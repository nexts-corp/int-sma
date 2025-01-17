#ifndef CONFIGSTRUCTURE_H
#define CONFIGSTRUCTURE_H
                                    
//// -- for IMS Achitecture future support -- //
//#define DEVTYPE_LENGTH      20
//#define NAME_LENGTH         40
//#define ID_LENGTH           20
//#define FW_LENGTH           20
//#define MAX_DI_CHANNEL      16
//#define MAX_DO_CHANNEL      5 //16
//#define MAX_AI_CHANNEL      16
//#define MAX_AO_CHANNEL      16
//#define MAX_SENS_CHANNEL    5
//#define MAX_EVENT_SOURCE    35                          
//
//// -- for currently developing model -- //
//#define USE_DIN             2               // -- currently 2 channel in model ETH-TypeK -- //   
//#define USE_DOUT            2               // -- currently 2 channel in model ETH-TypeK -- //
//
//struct prop_cnfg_st
//{
//    unsigned char   deviceType[DEVTYPE_LENGTH];
//    unsigned char   firmwareVersion[FW_LENGTH];
//    unsigned char   deviceName[NAME_LENGTH];
//    unsigned char   deviceID[ID_LENGTH];
//    unsigned char   hostName[NAME_LENGTH];
//    unsigned char   hostID[ID_LENGTH];
//    signed char     timeZone; 
//};
//
//struct oper_cnfg_st
//{
//    unsigned char   mode;
//    unsigned char   deviceEnable;
//    unsigned char   alarmEnable;
//    unsigned char   sensorEnable[MAX_SENS_CHANNEL];
//};
//
//struct sens_cnfg_st
//{
//    unsigned int    samplingRate;
//    unsigned int    reOccurrence;
//    unsigned char   timeStartHour;
//    unsigned char   timeStartMin;
//    unsigned char   timeStopHour;
//    unsigned char   timeStopMin;
//                                             
//    unsigned char   alarmEnable;
//    float           lowerLimit;
//    unsigned int    lowerDelay;
//    float           lowerExtreme;
//    float           upperLimit;
//    unsigned int    upperDelay;
//    float           upperExtreme;
//    //int           percentVariant;
//};
//
//struct din_cnfg_st
//{
//    unsigned char   channelEnable;
//    unsigned int    alarmDelay;
//};
//
//struct dout_cnfg_st
//{
//    unsigned char   channelEnable;
//    unsigned char   stateOnEvent;         
//    unsigned char   eventSourceNumber;
//    unsigned char   eventSource[MAX_EVENT_SOURCE];
//};
//
//struct ain_cnfg_st
//{
//    unsigned char   channelEnable;
//    float           alarmLevel;
//};
//
//struct aout_cnfg_st
//{
//    unsigned char   channelEnable;             
//    float           levelOnEvent;       
//    unsigned char   eventSourceNumber;
//    unsigned char   eventSource[MAX_EVENT_SOURCE];
//};
//
//struct buzz_cnfg_st
//{
//    unsigned char   enable;   
//    unsigned char   eventSourceNumber;
//    unsigned char   eventSource[MAX_EVENT_SOURCE];
//};
//
//struct alm_cnfg_st
//{
//    float           gsmRSSILevel;
//    int             gsmCreditBalance;
//    float           batteryLevel;
//    float           commSignalLevel;
//    int             commLostDelay;
//    unsigned int    memoryMin;              // -- in Mega-Byte unit
//};
//
//struct stat_cnfg_st
//{
//    unsigned char   accm;
//    unsigned int    intv;
//};
//
//// -- size of configuration (bytes) -- //
//#define SIZE_PROPERTIES         sizeof( struct prop_cnfg_st )                           // -- 161     
//#define SIZE_OPERATE            sizeof( struct oper_cnfg_st )                           // -- 9
//#define SIZE_SENSOR             sizeof( struct sens_cnfg_st )                           // -- 29
//#define SIZE_AUXDI              sizeof( struct din_cnfg_st  )                           // -- 34
//#define SIZE_AUXDO              sizeof( struct dout_cnfg_st )                           // -- 34
//#define SIZE_AUXAI              sizeof( struct ain_cnfg_st  )                           // -- 66
//#define SIZE_AUXAO              sizeof( struct aout_cnfg_st )                           // -- 82
//#define SIZE_ENVALARM           sizeof( struct alm_cnfg_st  )                           // -- 18
//#define SIZE_STAT               sizeof( struct stat_cnfg_st )                           // -- 3
//
//extern eeprom struct prop_cnfg_st   config_prop;
//extern eeprom struct oper_cnfg_st   config_oper;
//extern eeprom struct sens_cnfg_st   config_sens[];
//extern eeprom struct din_cnfg_st    config_din[];
//extern eeprom struct dout_cnfg_st   config_dout[];
//extern eeprom struct ain_cnfg_st    config_ain[];
//extern eeprom struct aout_cnfg_st   config_aout[];
//extern eeprom struct buzz_cnfg_st   config_buzz;
//extern eeprom struct alm_cnfg_st    config_alm;
//extern eeprom struct stat_cnfg_st   config_stat;
//
//extern eeprom char                  config_prop_flag;
//extern eeprom char                  config_oper_flag;
//extern eeprom char                  config_sens_flag[];
//extern eeprom char                  config_din_flag[]; 
//extern eeprom char                  config_dout_flag[];
//extern eeprom char                  config_ain_flag[];
//extern eeprom char                  config_aout_flag[];
//extern eeprom char                  config_buzz_flag;
//extern eeprom char                  config_alm_flag;
//extern eeprom char                  config_stat_flag;

#endif

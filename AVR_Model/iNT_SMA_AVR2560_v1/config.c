#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"

eeprom iDVConfig_t viDVConfigDefualt;
eeprom iDVConfig_t viDVConfigUpdate;



//#include "config.h"
//#include "debug.h"
//
//char is_properties_config_update                = 1;
//char is_operating_config_update                 = 1;
//char is_sensor_config_update[MAX_SENS_CHANNEL]  = {1,1,1,1,1};
//char is_din_config_update[MAX_DI_CHANNEL]       = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
//char is_dout_config_update[MAX_DO_CHANNEL]      = {1,1,1,1,1};//,1,1,1,1,1,1,1,1,1,1,1};
//char is_buzzer_config_update                    = 1;
//char is_status_config_update                    = 1;
//char is_env_config_update                       = 1;
//
//struct prop_cnfg_st properties_config;
//struct oper_cnfg_st operating_config; 
//struct oper_cnfg_st operating_config;                             
//struct sens_cnfg_st sensor_config[MAX_SENS_CHANNEL];       
//struct din_cnfg_st  din_config[MAX_DI_CHANNEL];
//struct dout_cnfg_st dout_config[MAX_DO_CHANNEL];
//struct buzz_cnfg_st buzz_config; 
//struct stat_cnfg_st status_config;
//struct alm_cnfg_st  env_config;
//
////flash char FIRMWARE_VERSION[] = "iNT-ETH-0.24b";  
////iNT-ETH-DC-0.01a 
//flash char FIRMWARE_VERSION[] = "iNT-ETH-DC-0.01a";        
///*============================================================*/
//
///**************************************************************/
///*************** Read Configuration from EEPROM ***************/
///**************************************************************/             
///*============================================================*/
//int getPropertiesConfig(struct prop_cnfg_st *readConfig)
//{          
//    int i;
//                                    
//    //printDebug("<getPropertiesConfig>..\r\n");
//    
//    if( config_prop_flag != 0x01){
//        printDebug("<getPropertiesConfig> no configuration, use default\r\n");   
//        memset(readConfig,0x00,sizeof(struct prop_cnfg_st));
//        readConfig->timeZone   = 7;   
//    }                                                  
//    else{                                     
//           
//        for(i=0;i<DEVTYPE_LENGTH;i++)
//        {          
//            readConfig->deviceType[i]   = config_prop.deviceType[i];    
//        }                            
//        
//        /*
//        for(i=0;i<FW_LENGTH;i++)
//        {          
//            //printDebug("%c",config_prop.firmwareVersion[i]);
//            readConfig->firmwareVersion[i]   = config_prop.firmwareVersion[i]; 
//        }       
//        */                                               
//        memcpyf(readConfig->firmwareVersion,FIRMWARE_VERSION,strlenf(FIRMWARE_VERSION));
//        
//        for(i=0;i<NAME_LENGTH;i++)
//        {          
//            readConfig->deviceName[i]   = config_prop.deviceName[i];    
//        }
//        
//        for(i=0;i<ID_LENGTH;i++)
//        {          
//            readConfig->deviceID[i]   = config_prop.deviceID[i];    
//        }                      
//              
//        for(i=0;i<NAME_LENGTH;i++)
//        {          
//            readConfig->hostName[i]   = config_prop.hostName[i];    
//        }                      
//          
//        for(i=0;i<ID_LENGTH;i++)
//        {          
//            readConfig->hostID[i]   = config_prop.hostID[i];    
//        }                                         
//                                 
//        readConfig->timeZone   = config_prop.timeZone;   
//    }
//    return 0;
//}
///*============================================================*/
//int getOperatingConfig(struct oper_cnfg_st *readConfig)
//{
//    int i;
//    
//    if( config_oper_flag != 0x01){
//        printDebug("<getOperatingConfig> no configuration, use default\r\n");       
//        
//        readConfig->mode                    =   MODE_DATALOGGER;     
//        readConfig->deviceEnable            =   0x00;    
//        readConfig->alarmEnable             =   0x00;
//          
//        for(i=0;i<MAX_SENS_CHANNEL;i++)
//        {                                                    
//            readConfig->sensorEnable[i]     =   0x00;   
//        }               
//                                                          
//        /* // -- for test -- //
//        readConfig->mode                    =   MODE_REALTIME;     
//        readConfig->deviceEnable            =   0x01;    
//        readConfig->alarmEnable             =   0x01;
//          
//        for(i=0;i<MAX_SENS_CHANNEL;i++)
//        {                                                    
//            if(i==0){
//                readConfig->sensorEnable[i]     =   0x01;        
//            }   
//            else{
//                readConfig->sensorEnable[i]     =   0x00;    
//            }      
//        }                                              
//        */
//    }        
//    else{      
//        readConfig->mode                    =   config_oper.mode;
//        readConfig->deviceEnable            =   config_oper.deviceEnable;
//        readConfig->alarmEnable             =   config_oper.alarmEnable;       
//        
//        for(i=0;i<MAX_SENS_CHANNEL;i++)
//        {
//            readConfig->sensorEnable[i]     =   config_oper.sensorEnable[i];
//        }                           
//    }        
//               
//                                                                       
//    printDebug("<getOperatingConfig> mode            >> 0x%02X\r\n",readConfig->mode);                  
//    printDebug("<getOperatingConfig> deviceEnable    >> 0x%02X\r\n",readConfig->deviceEnable);                
//    printDebug("<getOperatingConfig> alarmEnable     >> 0x%02X\r\n",readConfig->alarmEnable);               
//    printDebug("<getOperatingConfig> sensorEnable    >> ");
//    
//    for(i=0;i<MAX_SENS_CHANNEL;i++)
//    {  
//        printDebug("[0x%02X]",readConfig->sensorEnable[i]);
//    }                                                           
//    printDebug("\r\n");
//    
//    return 0;
//}
///*============================================================*/
//int getSensorConfig(int ch, struct sens_cnfg_st *readConfig)
//{
//    if((ch < 0) || (ch >= MAX_SENS_CHANNEL)){
//        printDebug("<getSensorConfig> sensor channel out of range (%d)\r\n",ch);
//        return -1;
//    }
// 
//    if( config_sens_flag[ch] != 0x01){
//        printDebug("<getSensorConfig> no configuration, use default\r\n");  
//        readConfig->samplingRate        =   300;
//        readConfig->reOccurrence        =   0x8000;
//        readConfig->timeStartHour       =   0;
//        readConfig->timeStartMin        =   0;
//        readConfig->timeStopHour        =   0;
//        readConfig->timeStopMin         =   0;
//                                                 
//        readConfig->alarmEnable         =   0x00;
//        readConfig->lowerLimit          =   10.0;
//        readConfig->lowerDelay          =   20;
//        readConfig->lowerExtreme        =   0.0;
//        readConfig->upperLimit          =   180.0;
//        readConfig->upperDelay          =   20;
//        readConfig->upperExtreme        =   200.0;
//        /*
//        readConfig->alarmEnable         =   0x00;
//        readConfig->lowerLimit          =   0.0;
//        readConfig->lowerDelay          =   10;
//        readConfig->lowerExtreme        =   0.0;
//        readConfig->upperLimit          =   0.0;
//        readConfig->upperDelay          =   10;
//        readConfig->upperExtreme        =   0.0; 
//        */
//    }
//    else{
//        readConfig->samplingRate        =   config_sens[ch].samplingRate;
//        readConfig->reOccurrence        =   config_sens[ch].reOccurrence;
//        readConfig->timeStartHour       =   config_sens[ch].timeStartHour;
//        readConfig->timeStartMin        =   config_sens[ch].timeStartMin;
//        readConfig->timeStopHour        =   config_sens[ch].timeStopHour;
//        readConfig->timeStopMin         =   config_sens[ch].timeStopMin;
//                                                 
//        readConfig->alarmEnable         =   config_sens[ch].alarmEnable;
//        readConfig->lowerLimit          =   config_sens[ch].lowerLimit;
//        readConfig->lowerDelay          =   config_sens[ch].lowerDelay;
//        readConfig->lowerExtreme        =   config_sens[ch].lowerExtreme;
//        readConfig->upperLimit          =   config_sens[ch].upperLimit;
//        readConfig->upperDelay          =   config_sens[ch].upperDelay;
//        readConfig->upperExtreme        =   config_sens[ch].upperExtreme;
//    }
//    return 0;
//}
///*============================================================*/
//int getAuxDinConfig(int ch, struct din_cnfg_st *readConfig)
//{                  
//    if((ch < 0) || (ch >= MAX_DI_CHANNEL)){
//        printDebug("<getAuxDinConfig> digital input channel out of range (%d)\r\n",ch);
//        return -1;
//    }
//    
//    if( config_din_flag[ch] != 0x01){
//        printDebug("<getAuxDinConfig> no configuration, use default\r\n");
//        
//        readConfig->channelEnable       =   0x00;
//        readConfig->alarmDelay          =   0;      
//        
//        /* // -- for test -- // 
//        if(ch < 2)  readConfig->channelEnable       =   0x01;
//        else        readConfig->channelEnable       =   0x00;
//            
//        readConfig->alarmDelay          =   0;       
//        */
//    }        
//    else{
//        readConfig->channelEnable       =   config_din[ch].channelEnable;
//        readConfig->alarmDelay          =   config_din[ch].alarmDelay;            
//    }
//    return 0;
//}
///*============================================================*/
//int getAuxDoutConfig(int ch, struct dout_cnfg_st *readConfig)
//{                     
//    int i;
//    
//    if((ch < 0) || (ch >= MAX_DO_CHANNEL)){
//        printDebug("<getAuxDoutConfig> digital output channel out of range (%d)\r\n",ch);
//        return -1;
//    }
//    
//    if( config_dout_flag[ch] != 0x01){
//        printDebug("<getAuxDoutConfig> no configuration, use default\r\n");
//        readConfig->channelEnable       =   0x00;
//        readConfig->stateOnEvent        =   0x01;     
//        readConfig->eventSourceNumber   =   0;                   
//        
//        for(i=0;i<MAX_EVENT_SOURCE;i++){ 
//            readConfig->eventSource[i]  =   0x00;              
//        }    
//    }         
//    else{
//        readConfig->channelEnable       =   config_dout[ch].channelEnable;
//        readConfig->stateOnEvent        =   config_dout[ch].stateOnEvent;     
//        readConfig->eventSourceNumber   =   config_dout[ch].eventSourceNumber;                   
//                                                                                   
//        printDebug("<getAuxDoutConfig> -- CH %d -- \r\n",ch);              
//        printDebug("<getAuxDoutConfig> enable         [0x%02X]\r\n",config_dout[ch].channelEnable);    
//        printDebug("<getAuxDoutConfig> stateOnEvent   [0x%02X]\r\n",config_dout[ch].stateOnEvent);     
//        printDebug("<getAuxDoutConfig> numberEvSrc    [%d]->",config_dout[ch].eventSourceNumber);    
//        for(i=0;i<config_dout[ch].eventSourceNumber;i++){             
//            readConfig->eventSource[i]  =   config_dout[ch].eventSource[i];                                                       
//            printDebug("[0x%02X]",readConfig->eventSource[i]);             
//        }                                               
//        for(i=config_dout[ch].eventSourceNumber;i<MAX_EVENT_SOURCE;i++){
//            readConfig->eventSource[i]  =   0x00;                                                                          
//            //printDebug("[0x%02X]",readConfig->eventSource[i]);    
//        }                           
//        printDebug("\r\n");
//                                                                
//    }
//    return 0;
//}
///*============================================================*/
//int getAuxAinConfig(int ch, struct ain_cnfg_st *readConfig)
//{                            
//    if((ch < 0) || (ch >= MAX_AI_CHANNEL)){
//        printDebug("<getAuxAinConfig> analog input channel out of range (%d)\r\n",ch);
//        return -1;
//    }
//    
//    if( config_ain_flag[ch] != 0x01){
//        printDebug("<getAuxAinConfig> no configuration, use default\r\n");
//        readConfig->channelEnable       =   0x00;
//        readConfig->alarmLevel          =   0;   
//    }                      
//    else{
//        readConfig->channelEnable       =   config_ain[ch].channelEnable;
//        readConfig->alarmLevel          =   config_ain[ch].alarmLevel;          
//    }
//    return 0;
//}
///*============================================================*/
//int getAuxAoutConfig(int ch, struct aout_cnfg_st *readConfig)
//{   
//    int i;   
//                       
//    if((ch < 0) || (ch >= MAX_AO_CHANNEL)){
//        printDebug("<getAuxAoutConfig> analog output channel out of range (%d)\r\n",ch);
//        return -1;
//    }                     
//    
//    if( config_aout_flag[ch] != 0x01){
//        printDebug("<getAuxAoutConfig> no configuration, use default\r\n");
//        readConfig->channelEnable       =   0x00;
//        readConfig->levelOnEvent        =   0.0;     
//        readConfig->eventSourceNumber   =   0;                   
//        
//        for(i=0;i<MAX_EVENT_SOURCE;i++){ 
//            readConfig->eventSource[i]  =   0x00;              
//        }  
//    }
//    else{
//        readConfig->channelEnable       =   config_aout[ch].channelEnable;
//        readConfig->levelOnEvent        =   config_aout[ch].levelOnEvent;     
//        readConfig->eventSourceNumber   =   config_aout[ch].eventSourceNumber;                   
//        
//        for(i=0;i<config_aout[ch].eventSourceNumber;i++){ 
//            readConfig->eventSource[i]  =   config_aout[ch].eventSource[i];              
//        }                           
//        for(i=config_aout[ch].eventSourceNumber;i<MAX_EVENT_SOURCE;i++){
//            readConfig->eventSource[i]  =   0x00;
//        }   
//    }
//    return 0;
//}                 
///*============================================================*/
//int getBuzzerConfig(struct buzz_cnfg_st *readConfig)
//{   
//    char errSrc[4] = {0xC7,0xCF,0xD7,0xDF};
//    int i;                   
//    
//    if( config_buzz_flag != 0x01){                                          
//        printDebug("<getBuzzerConfig> no configuration, use default\r\n");
//        readConfig->enable              =   0x01;
//        readConfig->eventSourceNumber   =   sizeof(errSrc);                     
//                                                           
//        //printDebug("<getBuzzerConfig> eventSourceNumber = %d ->",readConfig->eventSourceNumber);
//        for(i=0;i<sizeof(errSrc);i++){            
//            if(i<sizeof(errSrc)){                             
//                readConfig->eventSource[i]  =   errSrc[i];   
//                //printDebug("[0x%02X]",readConfig->eventSource[i]);
//            }                                    
//            else{
//                readConfig->eventSource[i]  =   0x00;    
//            }          
//        }                                          
//        //printDebug("\r\n");        
//    } 
//    else{
//        readConfig->enable              =   config_buzz.enable;
//        readConfig->eventSourceNumber   =   config_buzz.eventSourceNumber;                     
//        
//        for(i=0;i<config_buzz.eventSourceNumber;i++){ 
//            readConfig->eventSource[i]  =   config_buzz.eventSource[i];              
//        }                                
//        for(i=config_buzz.eventSourceNumber;i<MAX_EVENT_SOURCE;i++){
//            readConfig->eventSource[i]  =   0x00;
//        }   
//    }                 
//    
//    printDebug("<getBuzzerConfig> enable            >>[0x%02X]\r\n",    readConfig->enable);
//    printDebug("<getBuzzerConfig> eventSourceNumber >>[%d]\r\n",         readConfig->eventSourceNumber);       
//    printDebug("<getBuzzerConfig> eventSource       >>");    
//    for(i=0;i<MAX_EVENT_SOURCE;i++){            
//        printDebug("[0x%02X]",readConfig->eventSource[i]);       
//    }             
//    printDebug("\r\n");
//    
//    return 0;
//}
///*============================================================*/
//int getEnvAlarmConfig(struct alm_cnfg_st *readConfig)
//{
//    if( config_alm_flag != 0x01 ){            
//        printDebug("<getEnvAlarmConfig> no configuration, use default\r\n");
//        readConfig->gsmRSSILevel         =   0;
//        readConfig->gsmCreditBalance     =   0;
//        readConfig->batteryLevel         =   0;
//        readConfig->commSignalLevel      =   0;
//        readConfig->commLostDelay        =   0;
//        readConfig->memoryMin            =   0;
//    }
//    else{
//        readConfig->gsmRSSILevel         =   config_alm.gsmRSSILevel;
//        readConfig->gsmCreditBalance     =   config_alm.gsmCreditBalance;
//        readConfig->batteryLevel         =   config_alm.batteryLevel;
//        readConfig->commSignalLevel      =   config_alm.commSignalLevel;
//        readConfig->commLostDelay        =   config_alm.commLostDelay;
//        readConfig->memoryMin            =   config_alm.memoryMin;
//    }                                            
//    
//    printDebug("<getEnvAlarmConfig> gsmRSSILevel         [%f]\r\n",readConfig->gsmRSSILevel);
//    printDebug("<getEnvAlarmConfig> gsmCreditBalance     [%d]\r\n",readConfig->gsmCreditBalance);
//    printDebug("<getEnvAlarmConfig> batteryLevel         [%f]\r\n",readConfig->batteryLevel);
//    printDebug("<getEnvAlarmConfig> commSignalLevel      [%f]\r\n",readConfig->commSignalLevel);
//    printDebug("<getEnvAlarmConfig> commLostDelay        [%d]\r\n",readConfig->commLostDelay);
//    printDebug("<getEnvAlarmConfig> memoryMin            [%u]\r\n",readConfig->memoryMin,);
//    
//    return 0;
//}
///*============================================================*/
//int getStatusReportConfig(struct stat_cnfg_st *readConfig)
//{
//    if( config_stat_flag != 0x01 ){            
//        printDebug("<getStatusReportConfig> no configuration, use default\r\n");
//        readConfig->accm                =   0x30;
//        readConfig->intv                =   60;
//    }
//    else{
//        readConfig->accm                =   config_stat.accm;
//        readConfig->intv                =   config_stat.intv;
//    }                                                                                            
//    
//    printDebug("<getStatusReportConfig> accm        [0x%02X]\r\n",readConfig->accm);
//    printDebug("<getStatusReportConfig> intv        [%d]\r\n",readConfig->intv);
//    
//    return 0;
//}
///*============================================================*/ 
//
///**************************************************************/
///**************** Write Configuration to EEPROM ***************/
///**************************************************************/             
///*============================================================*/
//int setPropertiesConfig(struct prop_cnfg_st *newConfig)
//{                  
//    int i;    
//    
//    for(i=0;i<DEVTYPE_LENGTH;i++)
//    {          
//        config_prop.deviceType[i]       = newConfig->deviceType[i];    
//    }                            
//    
//    /*
//    for(i=0;i<FW_LENGTH;i++)
//    {          
//        config_prop.firmwareVersion[i]  = newConfig->firmwareVersion[i];    //printDebug("%c",config_prop.firmwareVersion[i]);
//    }
//    */                      
//       
//    for(i=0;i<NAME_LENGTH;i++)
//    {   
//        config_prop.deviceName[i]       = newConfig->deviceName[i];          
//    }                      
//    
//    for(i=0;i<ID_LENGTH;i++)
//    {  
//        config_prop.deviceID[i]         = newConfig->deviceID[i];           
//    }                      
//    
//    for(i=0;i<NAME_LENGTH;i++)
//    {  
//        config_prop.hostName[i]         = newConfig->hostName[i];            
//    }                     
//     
//    for(i=0;i<ID_LENGTH;i++)
//    {   
//        config_prop.hostID[i]           = newConfig->hostID[i];                
//    }               
//                 
//    config_prop.timeZone                = newConfig->timeZone;              
//    
//    config_prop_flag                    = 0x01;      
//    
//    is_properties_config_update          = 1;
//    
//    return 0;
//}
///*============================================================*/
//int setOperatingConfig(struct oper_cnfg_st *newConfig)
//{                
//    int i;
//                                                     
//    if( (newConfig->mode != MODE_REALTIME) && (newConfig->mode != MODE_DATALOGGER) ){
//        printDebug("<setOperatingConfig> Error: operating mode invalid [0x%02X]\r\n", newConfig->mode);
//        return -1;
//    }
//    
//    config_oper.mode                    =   newConfig->mode;
//    config_oper.deviceEnable            =   newConfig->deviceEnable;
//    config_oper.alarmEnable             =   newConfig->alarmEnable;           
//                                                                       
//    printDebug("\r\n mode            >> 0x%02X",config_oper.mode);                  
//    printDebug("\r\n deviceEnable    >> 0x%02X",config_oper.deviceEnable);                
//    printDebug("\r\n alarmEnable     >> 0x%02X",config_oper.alarmEnable);               
//    printDebug("\r\n sensorEnable    >> ");
//    
//    for(i=0;i<MAX_SENS_CHANNEL;i++)
//    {
//        config_oper.sensorEnable[i]     =   newConfig->sensorEnable[i];     
//        printDebug("[0x%02X]",config_oper.sensorEnable[i]);
//    }                                                           
//    printDebug("\r\n");
//                                                                          
//    config_oper_flag                    =   0x01;   
//    
//    is_operating_config_update          =   1;
//    
//    return 0;
//}
///*============================================================*/
//int setSensorConfig(int ch, struct sens_cnfg_st *newConfig)
//{                 
//    if((ch < 0) || (ch >= MAX_SENS_CHANNEL)){                                 
//        printDebug("<setSensorConfig> ERR: channel out of range\r\n");   
//        return -1;
//    }      
//    if(newConfig->samplingRate == 0){
//        printDebug("<setSensorConfig> ERR: samplingRate couldn't be set to 0\r\n");       
//        return -1;
//    }                                                                
//    
//    if(newConfig->samplingRate < 60){
//        newConfig->samplingRate         =   60;
//    }
//    
//    config_sens[ch].samplingRate        =   newConfig->samplingRate;
//    config_sens[ch].reOccurrence        =   newConfig->reOccurrence;
//    config_sens[ch].timeStartHour       =   newConfig->timeStartHour;
//    config_sens[ch].timeStartMin        =   newConfig->timeStartMin;
//    config_sens[ch].timeStopHour        =   newConfig->timeStopHour;
//    config_sens[ch].timeStopMin         =   newConfig->timeStopMin;
//                                             
//    config_sens[ch].alarmEnable         =   newConfig->alarmEnable;
//    config_sens[ch].lowerLimit          =   newConfig->lowerLimit;
//    config_sens[ch].lowerDelay          =   newConfig->lowerDelay;
//    config_sens[ch].lowerExtreme        =   newConfig->lowerExtreme;
//    config_sens[ch].upperLimit          =   newConfig->upperLimit;
//    config_sens[ch].upperDelay          =   newConfig->upperDelay;
//    config_sens[ch].upperExtreme        =   newConfig->upperExtreme;               
//               
//    printDebug("\r\n samplingRate    >> 0x%04X",config_sens[ch].samplingRate);      
//    printDebug("\r\n reOccurrence    >> 0x%04X",config_sens[ch].reOccurrence);      
//    printDebug("\r\n timeStartHour   >> 0x%02X",config_sens[ch].timeStartHour);      
//    printDebug("\r\n timeStartMin    >> 0x%02X",config_sens[ch].timeStartMin);      
//    printDebug("\r\n timeStopHour    >> 0x%02X",config_sens[ch].timeStopHour);      
//    printDebug("\r\n timeStopMin     >> 0x%02X",config_sens[ch].timeStopMin);      
//    
//    printDebug("\r\n alarmEnable     >> 0x%02X",        config_sens[ch].alarmEnable);      
//    printDebug("\r\n lowerLimit      >> %f [%04lX]",     config_sens[ch].lowerLimit,config_sens[ch].lowerLimit);      
//    printDebug("\r\n lowerDelay      >> %d [%02X]",     config_sens[ch].lowerDelay,config_sens[ch].lowerDelay);      
//    printDebug("\r\n lowerExtreme    >> %f [%04lX]",     config_sens[ch].lowerExtreme,config_sens[ch].lowerExtreme);      
//    printDebug("\r\n upperLimit      >> %f [%04lX]",     config_sens[ch].upperLimit,config_sens[ch].upperLimit);      
//    printDebug("\r\n upperDelay      >> %d [%02X]",     config_sens[ch].upperDelay,config_sens[ch].upperDelay);      
//    printDebug("\r\n upperExtreme    >> %f [%04lX]",     config_sens[ch].upperExtreme,config_sens[ch].upperExtreme);   
//    printDebug("\r\n");   
//    
//    config_sens_flag[ch]                = 0x01;         
//    is_sensor_config_update[ch]          = 1;
//    
//    return 0;
//}
///*============================================================*/
//int setAuxDinConfig(int ch, struct din_cnfg_st *newConfig)
//{              
//    if((ch < 0) || (ch > MAX_DI_CHANNEL)){
//        return -1;
//    }            
//    
//    config_din[ch].channelEnable        =   newConfig->channelEnable;
//    config_din[ch].alarmDelay           =   newConfig->alarmDelay;
//                                                                                   
//    printDebug("\r\n channel         >> [%d]",      ch); 
//    printDebug("\r\n channelEnable   >> 0x%02X",    config_din[ch].channelEnable);      
//    printDebug("\r\n alarmDelay      >> %d",        config_din[ch].alarmDelay);      
//    
//    config_din_flag[ch]                 =   0x01;            
//    is_din_config_update[ch]            =   1;
//    return 0;
//}
///*============================================================*/
//int setAuxDoutConfig(int ch, struct dout_cnfg_st *newConfig)
//{         
//    int i;
//             
//    if((ch < 0) || (ch > MAX_DO_CHANNEL)){      
//        printDebug("\r\n<setAuxDoutConfig> Channel Error!!\r\n");
//        return -1;
//    }                      
//    
//    if( (newConfig->eventSourceNumber) > MAX_EVENT_SOURCE ){       
//        printDebug("\r\n<setAuxDoutConfig> eventSourceNumber is too long (%d)!!\r\n",(newConfig->eventSourceNumber));
//        return -1;
//    }                          
//    
//    config_dout[ch].channelEnable       =   0x01; //newConfig->channelEnable;      // Edit bug from software
//    config_dout[ch].stateOnEvent        =   0x00; //newConfig->stateOnEvent;   
//    config_dout[ch].eventSourceNumber   =   newConfig->eventSourceNumber;     
//    
//    printDebug("\r\n channel           >> [%d]",    ch); 
//    printDebug("\r\n channelEnable     >> 0x%02X",  config_dout[ch].channelEnable);      
//    printDebug("\r\n stateOnEvent      >> 0x%02X",  config_dout[ch].stateOnEvent);        
//    printDebug("\r\n eventSourceNumber >> %d",      config_dout[ch].eventSourceNumber);    
//    printDebug("\r\n eventSource       >> ");      
//     
//    for(i=0;i<config_dout[ch].eventSourceNumber;i++){
//       
//        config_dout[ch].eventSource[i]  =   newConfig->eventSource[i];   
//        
//        if( config_dout[ch].eventSource[i] != 0x00 ){
//            printDebug("[0x%02X]",config_dout[ch].eventSource[i]);    
//        }
//    }                 
//    printDebug("\r\n");
//    
//    config_dout_flag[ch]                =   0x01;         
//    is_dout_config_update[ch]           =   1;                
//    
//    return 0;
//}
///*============================================================*/
//int setAuxAinConfig(int ch, struct ain_cnfg_st *newConfig)
//{                
//    if((ch < 0) || (ch > MAX_AI_CHANNEL)){
//        return -1;
//    }   
//                                                  
//    config_ain[ch].channelEnable        =   newConfig->channelEnable;
//    config_ain[ch].alarmLevel           =   newConfig->alarmLevel;        
//                                                                                   
//    printDebug("\r\n channel         >> [%d]",      ch); 
//    printDebug("\r\n channelEnable   >> 0x%02X",    config_ain[ch].channelEnable);      
//    printDebug("\r\n alarmLevel      >> %d",        config_ain[ch].alarmLevel); 
//    
//    config_ain_flag[ch]                 = 0x01;         
//    
//    return 0;
//}
///*============================================================*/
//int setAuxAoutConfig(int ch, struct aout_cnfg_st *newConfig)
//{         
//    int i;
//    
//    if((ch < 0) || (ch > MAX_AO_CHANNEL)){                  
//        printDebug("\r\n<setAuxAoutConfig> Channel Error!!\r\n");
//        return -1;
//    }                           
//    
//    if( (newConfig->eventSourceNumber) > MAX_EVENT_SOURCE ){       
//        printDebug("\r\n<setAuxAoutConfig> eventSourceNumber is too long (%d)!!\r\n",(newConfig->eventSourceNumber));
//        return -1;
//    }         
//    
//    config_aout[ch].channelEnable       =   newConfig->channelEnable;   
//    config_aout[ch].levelOnEvent        =   newConfig->levelOnEvent;  
//    config_aout[ch].eventSourceNumber   =   newConfig->eventSourceNumber;                         
//    
//    printDebug("\r\n channel           >> [%d]",    ch); 
//    printDebug("\r\n channelEnable     >> 0x%02X",  config_aout[ch].channelEnable);      
//    printDebug("\r\n levelOnEvent      >> %f",      config_aout[ch].levelOnEvent);        
//    printDebug("\r\n eventSourceNumber >> %d",      config_aout[ch].eventSourceNumber);    
//    printDebug("\r\n eventSource       >> ");      
//    
//    for(i=0;i<config_aout[ch].eventSourceNumber;i++){                                          
//
//        config_aout[ch].eventSource[i]  =   newConfig->eventSource[i];        
//        
//        if( config_aout[ch].eventSource[i] != 0x00 ){
//            printDebug("[0x%02X]",config_aout[ch].eventSource[i]);    
//        }
//    }                                                             
//    printDebug("\r\n");
//    
//    config_aout_flag[ch]                =   0x01;       
//    
//    return 0;
//}                                  
///*============================================================*/
//int setBuzzerConfig(struct buzz_cnfg_st *newConfig)
//{         
//    int i;                  
//    
//    if( (newConfig->eventSourceNumber) > MAX_EVENT_SOURCE ){       
//        printDebug("\r\n<setBuzzerConfig> eventSourceNumber is too long (%d)!!\r\n",(newConfig->eventSourceNumber));
//        return -1;
//    }                       
//    
//    config_buzz.enable                  =   newConfig->enable;   
//    config_buzz.eventSourceNumber       =   newConfig->eventSourceNumber;      
//    
//    printDebug("\r\n enable            >> 0x%02X",  config_buzz.enable);      
//    printDebug("\r\n eventSourceNumber >> %d",      config_buzz.eventSourceNumber);     
//    printDebug("\r\n eventSource       >>");    
//    
//    for(i=0;i<config_buzz.eventSourceNumber;i++){                                     
//    
//        config_buzz.eventSource[i]      =   newConfig->eventSource[i];             
//        
//        if( config_buzz.eventSource[i] != 0x00 ){
//            printDebug("[0x%02X]",config_buzz.eventSource[i]);    
//        }
//        
//    }
//    printDebug("\r\n");                                  
//    
//    config_buzz_flag                    =   0x01;    
//    
//    is_buzzer_config_update             =   1;   
//    
//    return 0;
//}
///*============================================================*/
//int setEnvAlarmConfig(struct alm_cnfg_st *newConfig)
//{   
//    config_alm.gsmRSSILevel             =   newConfig->gsmRSSILevel;
//    config_alm.gsmCreditBalance         =   newConfig->gsmCreditBalance;
//    config_alm.batteryLevel             =   newConfig->batteryLevel;
//    config_alm.commSignalLevel          =   newConfig->commSignalLevel;
//    config_alm.commLostDelay            =   newConfig->commLostDelay;
//    config_alm.memoryMin                =   newConfig->memoryMin;  
//       
//    printDebug("\r\n gsmRSSILevel     >> %f dB",    config_alm.gsmRSSILevel);       
//    printDebug("\r\n gsmCreditBalance >> %d Baht",  config_alm.gsmCreditBalance);       
//    printDebug("\r\n batteryLevel     >> %f Volts", config_alm.batteryLevel);       
//    printDebug("\r\n commSignalLevel  >> %f dB",    config_alm.commSignalLevel);       
//    printDebug("\r\n commLostDelay    >> %d sec",   config_alm.commLostDelay);       
//    printDebug("\r\n memoryMin        >> %d MB.",   config_alm.memoryMin);        
//    
//    config_alm_flag                 = 0x01;        
//    is_env_config_update            = 1;
//    return 0;
//}
///*============================================================*/
//int setStatusReportConfig(struct stat_cnfg_st *newConfig)
//{   
//    config_stat.accm                    =   newConfig->accm;
//    config_stat.intv                    =   newConfig->intv;
//       
//    printDebug("\r\n accm     >> %f dB",    config_stat.accm);       
//    printDebug("\r\n intv     >> %d Baht",  config_stat.intv);       
//    
//    config_stat_flag                    = 0x01;        
//    is_status_config_update             = 1;
//    return 0;
//}
///*============================================================*/    
//
///**************************************************************/
///********* Service Configuration Management Function **********/
///**************************************************************/             
///*============================================================*/
//int setDefaultConfig(void)
//{
//    int i;
//    
//    config_prop_flag            = 0x00;
//    config_oper_flag            = 0x00;
//    
//    for(i=0;i<MAX_SENS_CHANNEL;i++){
//        config_sens_flag[i]     = 0x00;                
//    }                        
//                    
//    for(i=0;i<MAX_DI_CHANNEL;i++){
//        config_din_flag[i]      = 0x00;                     
//    }  
//                          
//    for(i=0;i<MAX_DO_CHANNEL;i++){
//        config_dout_flag[i]     = 0x00;                     
//    }      
//                      
//    for(i=0;i<MAX_AI_CHANNEL;i++){
//        config_ain_flag[i]      = 0x00;                     
//    }      
//                      
//    for(i=0;i<MAX_AO_CHANNEL;i++){
//        config_aout_flag[i]     = 0x00;                    
//    }                  
//                  
//    config_buzz_flag            = 0x00;   
//    config_alm_flag             = 0x00;         
//    config_stat_flag            = 0x00;      
//    
//    return 0;
//}
///*============================================================*/ 
//int isDeviceConfig(void)
//{
//    int i   = 0;
//    
//    if( config_prop_flag || config_oper_flag || config_buzz_flag || config_alm_flag || config_stat_flag ){
//        return 1;
//    }            
//    
//    for(i=0;i<MAX_SENS_CHANNEL;i++){
//        if(config_sens_flag[i]){
//            return 1;
//        }
//    }            
//    
//    //for(i=0;i<MAX_DI_CHANNEL;i++){
//    for(i=0;i<2;i++){
//        if(config_din_flag[i]){
//            return 1;
//        }
//    }            
//    
//    //for(i=0;i<MAX_DO_CHANNEL;i++){
//    for(i=0;i<2;i++){
//        if(config_dout_flag[i]){
//            return 1;
//        }
//    }            
//    return 0;
//}
///*============================================================*/ 
//int getAllConfig(void)
//{
//    int i,err;
//    
//    // -- load configuration -- //     
//    getPropertiesConfig(&properties_config); 
//    is_properties_config_update = 0;
//        
//    getOperatingConfig(&operating_config); 
//    is_operating_config_update = 0;
//                                    
//    for(i=0;i<MAX_SENS_CHANNEL;i++){
//        err = getSensorConfig(i,&sensor_config[i]);   
//        if(err < 0){
//            printDebug("<main> getSensorConfig(%d) error\r\n",i);
//            continue;
//        }
//        is_sensor_config_update[i] = 0;  
//    }                                      
//    
//    getBuzzerConfig(&buzz_config); 
//    is_buzzer_config_update = 0;
//                            
//    for(i=0;i<USE_DIN;i++){                                        
//        getAuxDinConfig(i,&din_config[i]); 
//        is_din_config_update[i] = 0;                                                 
//    }  
//        
//    for(i=0;i<USE_DOUT;i++){                                        
//        getAuxDoutConfig(i,&dout_config[i]); 
//        is_dout_config_update[i] = 0;        
//    }                         
//    
//    getStatusReportConfig(&status_config);
//    is_status_config_update = 0;             
//                                                  
//    getEnvAlarmConfig(&env_config);  
//    is_env_config_update = 0;            
//    
//    return 0;
//}
///*============================================================*/ 

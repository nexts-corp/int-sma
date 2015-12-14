#include "eventManager.h"
#include "config.h"
#include "eventTask.h"
#include "dataAndStatusTask.h"

extern eeprom iDVConfig_t viDVConfigUpdate;

extern iDataReport_t viDataReportBuff[];

extern iDataMini_t viStatusReport;

extern iEventReport_t viEventReportBuff;

iChar_t iEventRead(iDataMini_t *pviOutData_arg){
    iChar_t i;
    iChar_t viReturn = 0;  
    const iChar_t viCMD[2] = {0x3a,0x05};
    iUChar_t viIndexWrite = 0; 
    
    iTestSetValueSensorConfig();
    //check sensor event   
    viIndexWrite = 0;
    for(i=0;i<5;i++){
        #asm("wdr")
        if(viDVConfigUpdate.viSensorConfig[i].viAlarmEnable == SENSOR_ENABLE_def){ 
            //check lower case
            if((viDataReportBuff[i].viData < viDVConfigUpdate.viSensorConfig[i].viAlarmLowerExtreme) && (viDataReportBuff[i].viData > viDVConfigUpdate.viSensorConfig[i].viAlarmLower)){
                switch(i){
                    case 0:{
                        iPrintEventByCase(SENSOR1_LOWER_EXTREME_EVENT_def);
                        break;
                    }
                    case 1:{
                        iPrintEventByCase(SENSOR2_LOWER_EXTREME_EVENT_def);
                        break;
                    }
                    case 2:{
                        iPrintEventByCase(SENSOR3_LOWER_EXTREME_EVENT_def);
                        break;
                    }
                    case 3:{
                        iPrintEventByCase(SENSOR4_LOWER_EXTREME_EVENT_def);
                        break;
                    }
                    case 4:{
                        iPrintEventByCase(SENSOR5_LOWER_EXTREME_EVENT_def);
                        break;
                    }
                }
            }else if(viDataReportBuff[i].viData < viDVConfigUpdate.viSensorConfig[i].viAlarmLower){
                switch(i){
                    case 0:{
                        iPrintEventByCase(SENSOR1_LOWER_EVENT_def);
                        break;
                    }
                    case 1:{
                        iPrintEventByCase(SENSOR2_LOWER_EVENT_def);
                        break;
                    }
                    case 2:{
                        iPrintEventByCase(SENSOR3_LOWER_EVENT_def);
                        break;
                    }
                    case 3:{
                        iPrintEventByCase(SENSOR4_LOWER_EVENT_def);
                        break;
                    }
                    case 4:{
                        iPrintEventByCase(SENSOR5_LOWER_EVENT_def);
                        break;
                    }
                }
            } 
            
            //check upper case
            if((viDataReportBuff[i].viData > viDVConfigUpdate.viSensorConfig[i].viAlarmUpperExtreme) && (viDataReportBuff[i].viData < viDVConfigUpdate.viSensorConfig[i].viAlarmUpper)){
                switch(i){
                    case 0:{
                        iPrintEventByCase(SENSOR1_UPPERY_EXTREME_EVENT_def);
                        break;
                    }
                    case 1:{
                        iPrintEventByCase(SENSOR2_UPPERY_EXTREME_EVENT_def);
                        break;
                    }
                    case 2:{
                        iPrintEventByCase(SENSOR3_UPPERY_EXTREME_EVENT_def);
                        break;
                    }
                    case 3:{
                        iPrintEventByCase(SENSOR4_UPPERY_EXTREME_EVENT_def);
                        break;
                    }
                    case 4:{
                        iPrintEventByCase(SENSOR5_UPPERY_EXTREME_EVENT_def);
                        break;
                    }
                }
            }else if(viDataReportBuff[i].viData > viDVConfigUpdate.viSensorConfig[i].viAlarmUpper){
                switch(i){
                    case 0:{
                        iPrintEventByCase(SENSOR1_UPPER_EVENT_def);
                        break;
                    }
                    case 1:{
                        iPrintEventByCase(SENSOR2_UPPER_EVENT_def);
                        break;
                    }
                    case 2:{
                        iPrintEventByCase(SENSOR3_UPPER_EVENT_def);
                        break;
                    }
                    case 3:{
                        iPrintEventByCase(SENSOR4_UPPER_EVENT_def);
                        break;
                    }
                    case 4:{
                        iPrintEventByCase(SENSOR5_UPPER_EVENT_def);
                        break;
                    }
                }
            }
        }
    }
}



void iPrintEventByCase(iChar_t viEventID_arg){
    printDebug("[iPrintEventByCase]id(%02X)\r\n",viEventID_arg);
}

void iTestSetValueSensorConfig(){
    /*test*/
    viDVConfigUpdate.viSensorConfig[0].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[0].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[0].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[0].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[0].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[0].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[0].viAlarmUpperExtreme = 0.0;
    
    viDVConfigUpdate.viSensorConfig[1].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[1].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[1].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[1].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[1].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[1].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[1].viAlarmUpperExtreme = 0.0; 
     
    viDVConfigUpdate.viSensorConfig[2].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[2].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[2].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[2].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[2].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[2].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[2].viAlarmUpperExtreme = 0.0;
    
    viDVConfigUpdate.viSensorConfig[3].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[3].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[3].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[3].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[3].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[3].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[3].viAlarmUpperExtreme = 0.0;
    
    viDVConfigUpdate.viSensorConfig[4].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[4].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[4].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[4].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[4].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[4].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[4].viAlarmUpperExtreme = 0.0;
    
    viDVConfigUpdate.viSensorConfig[5].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[5].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[5].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[5].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[5].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[5].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[5].viAlarmUpperExtreme = 0.0;
    
    viDVConfigUpdate.viSensorConfig[6].viAlarmEnable = 0x01; 
    viDVConfigUpdate.viSensorConfig[6].viAlarmLower = 0.0;
    viDVConfigUpdate.viSensorConfig[6].viAlarmLowerDelay = 0.0;
    viDVConfigUpdate.viSensorConfig[6].viAlarmLowerExtreme = 0.0;
    viDVConfigUpdate.viSensorConfig[6].viAlarmUpper = 0.0;
    viDVConfigUpdate.viSensorConfig[6].viAlarmUpperDelay = 0.0;  
    viDVConfigUpdate.viSensorConfig[6].viAlarmUpperExtreme = 0.0;                   
}
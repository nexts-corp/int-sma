#include "sensorManager.h"
#include "config.h"
#include "sensor.h"
#include "managerTask.h"
extern eeprom iDVConfig_t viDVConfigUpdate;
extern iDataReport_t viDataReportBuff[];

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;

iChar_t iSensorRead(iDataMini_t *pviOutData_arg){
    iChar_t i;
    iChar_t viReturn = 0;  
    const iChar_t viCMD[2] = {0x5a,0x07};
    iUChar_t viIndexWrite = 0;
    
    /*test*/
    viDVConfigUpdate.viOperate.viSensorEnable[0] = 1; 
    viDVConfigUpdate.viOperate.viSensorEnable[1] = 1;
    viDVConfigUpdate.viOperate.viSensorEnable[2] = 1;
    viDVConfigUpdate.viOperate.viSensorEnable[3] = 1;
    viDVConfigUpdate.viOperate.viSensorEnable[4] = 1; 
    viDVConfigUpdate.viSensorConfig[0].viChannelID = 0; 
    viDVConfigUpdate.viSensorConfig[1].viChannelID = 1;
    viDVConfigUpdate.viSensorConfig[2].viChannelID = 2;
    viDVConfigUpdate.viSensorConfig[3].viChannelID = 3;
    viDVConfigUpdate.viSensorConfig[4].viChannelID = 4;
    viDVConfigUpdate.viSensorConfig[0].viSensorID = SEN_ADC_TYPE;
    viDVConfigUpdate.viSensorConfig[1].viSensorID = SEN_ADC_TYPE;
    viDVConfigUpdate.viSensorConfig[2].viSensorID = SEN_ADC_TYPE;
    viDVConfigUpdate.viSensorConfig[3].viSensorID = SEN_ADC_TYPE;
    viDVConfigUpdate.viSensorConfig[4].viSensorID = SEN_ADC_TYPE;
    viDVConfigUpdate.viSensorConfig[0].viUnitID = SEN_ADC10BIT_UNIT;
    viDVConfigUpdate.viSensorConfig[1].viUnitID = SEN_ADC10BIT_UNIT;
    viDVConfigUpdate.viSensorConfig[2].viUnitID = SEN_ADC10BIT_UNIT;
    viDVConfigUpdate.viSensorConfig[3].viUnitID = SEN_ADC10BIT_UNIT;
    viDVConfigUpdate.viSensorConfig[4].viUnitID = SEN_ADC10BIT_UNIT;

#if (0)    
    printDebug("viOperate\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viOperate,sizeof(viDVConfigUpdate.viOperate)); 
    printDebug("SensorConfig[0]\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viSensorConfig[0],sizeof(viDVConfigUpdate.viSensorConfig)); 
    printDebug("SensorConfig[1]\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viSensorConfig[1],sizeof(viDVConfigUpdate.viSensorConfig));
    printDebug("SensorConfig[2]\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viSensorConfig[2],sizeof(viDVConfigUpdate.viSensorConfig));
    printDebug("SensorConfig[3]\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viSensorConfig[3],sizeof(viDVConfigUpdate.viSensorConfig));
    printDebug("SensorConfig[4]\r\n");
    printConfig((char eeprom *)&viDVConfigUpdate.viSensorConfig[4],sizeof(viDVConfigUpdate.viSensorConfig));
    ///////// 
#endif 
    
    viIndexWrite = 0;
    for(i=0;i<5;i++){
        #asm("wdr")
       if(viDVConfigUpdate.viOperate.viSensorEnable[i]==1){
           viDataReportBuff[i].viChannelID = viDVConfigUpdate.viSensorConfig[i].viChannelID;
           viDataReportBuff[i].viSensorID = viDVConfigUpdate.viSensorConfig[i].viSensorID;
           viDataReportBuff[i].viUnitID =   viDVConfigUpdate.viSensorConfig[i].viUnitID; 
           iSensorReadByType(&viDataReportBuff[i]);
           memcpy(&pviOutData_arg->value[viIndexWrite],viCMD,2);
           viIndexWrite += 2;
           memcpy(&pviOutData_arg->value[viIndexWrite],&viDataReportBuff[i],sizeof(viDataReportBuff[i]));
           viIndexWrite += sizeof(viDataReportBuff[i]);
           pviOutData_arg->length = viIndexWrite;
       }
    }
    printDebug("[iSensorRead]Data:[\r\n");
    print_payload(&pviOutData_arg->value[0],pviOutData_arg->length);
    printDebug("]\r\n");
    viReturn = 1;
    return viReturn;
}

void iSensorReadByType(iDataReport_t *pviInOutData_arg){
    switch(pviInOutData_arg->viSensorID){
        case SEN_ADC_TYPE:{
            pviInOutData_arg->viData = read_adc(8+pviInOutData_arg->viChannelID); 
            printDebug("[iSensorReadByType]Channel[%d] data : %f\r\n",pviInOutData_arg->viChannelID,pviInOutData_arg->viData);
            break;
        }
    }
}

float iADCTypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iKTypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iTP100TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iVoltTypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iAmpTypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float i420TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iDHT11TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iDHT22TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iDS1820TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}

float iDS18B20TypeCalculator(){
    float viReturn = 0;
    return viReturn;
}
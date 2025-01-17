#include "statusManager.h"
#include "adc.h"

//extern eeprom iDVConfig_t viDVConfigUpdate;
extern iStatusReport_t viStatusReportBuff;

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;

iChar_t iStatusRead(iDataMini_t *pviOutData_arg){
    iChar_t i;
    iChar_t viReturn = 0;  
    const iChar_t viCMD[2] = {0x2a,0x06};
    iUChar_t viIndexWrite = 0;
    
    /*test*/ 
    #asm("wdr")
    viStatusReportBuff.viStatusType = STATUS_BATTERY_TYPE;
    viStatusReportBuff.viDataType =  STATUS_FLOAT; 
    iStatusReadByType(&viStatusReportBuff); 
    
    viIndexWrite = 0;
    memcpy(&pviOutData_arg->value[viIndexWrite],viCMD,2);
    viIndexWrite += 2;
    memcpy(&pviOutData_arg->value[viIndexWrite],&viStatusReportBuff,sizeof(viStatusReportBuff));
    viIndexWrite += sizeof(viStatusReportBuff); 
    pviOutData_arg->length = viIndexWrite; 
    
    printDebug("[iStatusRead]Data:[\r\n");
    print_payload(&pviOutData_arg->value[0],pviOutData_arg->length);
    printDebug("]\r\n"); 
    
    
    viReturn = 1;
     
    return viReturn;
}

void iStatusReadByType(iStatusReport_t *pviInOutData_arg){
     //float viReturn = 0.0;
     switch(pviInOutData_arg->viStatusType){
          case STATUS_BATTERY_TYPE:{ 
            //pviInOutData_arg->viData = iStatusReadBattery(); 
            pviInOutData_arg->viData = 2.56; 
            break;
          }
     }
//     viReturn = 1;
//     return viReturn;
}

//float iStatusReadBattery(){
//    float  viBatt = 0.0;
//    float  viADCDividerMax = 635.5;//2.05      // from voltage divider 120k & 150k
//    float  viVoltMaxBatt = 3.7;                        
//                                                  
//    //viBatt = (float)((read_adc(14)/viADCDividerMax) * viVoltMaxBatt);                // Battery Read   
//    viBatt = 2.50;
//    return viBatt;
//}
//
//float iStatusReadMainPower(iChar_t viStatusType_arg){
//     float viMainPwr = 0.0;
//     if(MAIN_POWER_CONNECT){
//         viMainPwr = 1.0;
//     }else{
//         viMainPwr = 0.0;
//     }
//     return viMainPwr;
//}
//
//float iStatusReadSDCard(iChar_t viStatusType_arg){
//     float viReturn = 0.0;
//     
//     return viReturn;
//}
//
//float iStatusReadLan(iChar_t viStatusType_arg){
//     float viReturn = 0.0;
//     
//     return viReturn;
//}
//
//float iStatusReadWifi(iChar_t viStatusType_arg){
//     float viReturn = 0.0;
//     
//     return viReturn;
//}
//
//float iStatusReadDI0(iChar_t viStatusType_arg){
//     float viReturn = 0.0;
//     
//     return viReturn;
//}
//
//float iStatusReadDI1(iChar_t viStatusType_arg){
//     float viReturn = 0.0;
//     
//     return viReturn;
//}
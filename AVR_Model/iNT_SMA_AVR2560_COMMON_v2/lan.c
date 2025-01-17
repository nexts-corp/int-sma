#include "lan.h"
#include "timer.h"
#include "io.h"
#include "debug.h" 

extern iData_t viRXData;
extern iData_t viTXData;


char AckHostData[20] = {
    0x7E,
    0x00,0x10,
    0x08,0x00,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x64,0x75,0xC9,0x55,
    0x00,0x00,
    0xD8
};

iUChar_t iLanReadData(iData_t * pviData_agr){
    iUChar_t viReturn;
    TIMER   timeout; 
    iUChar_t viModeConnect = 0;     //0= no check connect  ,1 == check connect
    
   viReturn = LEN_DATA_EMPTY;
   TIMER_setTimer(&timeout, 7);
    
   iWizRXMode = WIZ_RX_DATA_MODE; 
  // delay_ms(1000);
   while(!TIMER_checkTimerExceed(timeout)){
      delay_ms(50); 
      #asm("wdr")  
      if(iWizRead()){     
        printDebug("[iLanReadData]iWizRead:OK.\r\n");  
        viReturn = LEN_DATA_AVAILABLE;
        if(viRXData.length>0){ 
            pviData_agr->length = viRXData.length;
            memcpy(&pviData_agr->value[0],&viRXData.value[0],viRXData.length);
            printDebug("[iLanReadData]Data:OK[\r\n");
            print_payload(&viRXData.value[0], viRXData.length);
            printDebug("]\r\n"); 
        }else{
        
        }
        break;
      }else{
        printDebug("[iLanReadData]iWizRead:empty.\r\n");
        viReturn = LEN_DATA_EMPTY; 
      }
   }
    return viReturn;
}

//iUChar_t iLanWriteData(){
//    iUChar_t viReturn; 
//    TIMER   timeout;
//    
//    iWizRXMode = WIZ_RX_STAT_MODE;  
//    viReturn = LEN_DATA_WRITE_FAIL;
//    TIMER_setTimer(&timeout, 20);
//    
//    while(!TIMER_checkTimerExceed(timeout)){   
//       delay_ms(50);
//       #asm("wdr")
//       if(iWizConnected()){
//            printDebug("[iLanWriteData]Status:Connected.\r\n");
//            iWizRXMode = WIZ_RX_DATA_MODE;  
//            iWizSend(viGenDataElem,sizeof(viGenDataElem));
//            viReturn = LEN_DATA_WRITE_SUCCESS;
//            break;
//       } else{
//            viReturn = LEN_DATA_WRITE_FAIL;
//            printDebug("[iLanWriteData]Status:Close.\r\n");
//       }       
//    }
//    return viReturn;
//}

iUChar_t iLanWriteData(iData_t * pviData_agr){
    iUChar_t viReturn; 
    TIMER   timeout; 
    iUChar_t viModeConnect = 0;     //0= no check connect  ,1 == check connect
    
    iWizRXMode = WIZ_RX_STAT_MODE;  
    viReturn = LEN_DATA_WRITE_FAIL;
    //TIMER_setTimer(&timeout, 20); 
    
//       if(iWizConnected()){
//            printDebug("[iLanWriteData]Status:Connected.\r\n");
//            iWizRXMode = WIZ_RX_DATA_MODE;  
//            iWizSend(pviData_agr->value,pviData_agr->length);
//            iWizRXMode = WIZ_RX_DATA_MODE;
//            viReturn = LEN_DATA_WRITE_SUCCESS;
//       } else{
//            viReturn = LEN_DATA_WRITE_FAIL;
//            printDebug("[iLanWriteData]Status:Close.\r\n");
//       }
       
       if(viModeConnect==0){
           iWizRXMode = WIZ_RX_DATA_MODE;  
           iWizSend(pviData_agr->value,pviData_agr->length); 
           viReturn = LEN_DATA_WRITE_SUCCESS;
       }
             
       
    return viReturn;
}

iChar_t iLanInit(){
       iChar_t viReturn;
       //iWizReActiveDataMode();
      // iWizRXMode = WIZ_RX_STAT_MODE; 
       iWizRXMode = WIZ_RX_DATA_MODE;
       if(iWizConnected()){
            printDebug("[iLanInit]Status:Connected.\r\n");
            iWizRXMode = WIZ_RX_DATA_MODE;
            viReturn = LEN_DATA_WRITE_SUCCESS;
       } else{
            viReturn = LEN_DATA_WRITE_FAIL;
            printDebug("[iLanInit]Status:close or unknow.\r\n");
       }  
       return viReturn;
}

void iLanRestart(){
     iWizReActiveDataMode();
}

iUChar_t iLanStatus(){
       iUChar_t viReturn;
       viReturn = iWizCheckStatus();
       return viReturn;
}


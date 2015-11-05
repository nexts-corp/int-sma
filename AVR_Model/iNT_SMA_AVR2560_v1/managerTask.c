#include "io.h" 
#include "timer.h"
#include "ds1672.h"
#include "managerTask.h"
#include "dataAndStatusTask.h"
#include "smaProtocol.h"
#include "lan.h"
#include "database.h"

extern piQueueHandle_t pviQueueTaskAToTaskB;
extern piQueueHandle_t pviQueueSensorToMang;
extern piQueueHandle_t pviQueueDeviceToMang;

extern eeprom iUChar_t viFlagTerInit;           //Terminal initial frist

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;

iMangQueue_t viMangQueue[MANG_QUEUE_MAX];

iSensorData_t pviRXReceiveSensor;
//iUChar_t de

//iData_t viTXDataBuff;
//iData_t viRXDataBuff;

void managerTask(void *pviParameter){
     int i; 
     iData_t    viTXDataBuff;
     iData_t    viRXDataBuff; 
     iUChar_t   viHostRetransmit = 3; 
     iChar_t *  pviDatAndStatBuff;   
     iUInt_t    viDatAndStatLength = 0;    
     unsigned long int viUniTime = 0;
     
//     char viGenDataElem[80] = {
//         0x7e,
//         0x00,0x4c,
//         0x02,0x00,
//         0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
//         0x50,0x72,0xC9,0x55,
//         0x00,0x01,
//         0x2A,0x06,0x11,0x04,0xCD,0xCC,0xC7,0x42,
//         0x2A,0x06,0x12,0x04,0x00,0x00,0x00,0x00,
//         0x2A,0x06,0x13,0x04,0x00,0x00,0xE0,0x41,
//         0x5A,0x07,0x31,0x0c,0x0d,0x00,0x00,0x00,0x00, 
//         0x5A,0x07,0x32,0x0d,0x0d,0x01,0x0f,0xf0,0x3b, 
//         0x5A,0x07,0x41,0x0a,0x09,0x00,0x0f,0xe0,0x41,
//         0x5A,0x07,0x42,0x0a,0x0b,0x00,0x0f,0x08,0x42,
//         0x70
//    } ;
    
    char viGenDataElem[60] = {
         0x2A,0x06,0x11,0x04,0xCD,0xCC,0xC7,0x42,
         0x2A,0x06,0x12,0x04,0x00,0x00,0x00,0x00,
         0x2A,0x06,0x13,0x04,0x00,0x00,0xE0,0x41,
         0x5A,0x07,0x31,0x0c,0x0d,0x00,0x00,0x00,0x00, 
         0x5A,0x07,0x32,0x0d,0x0d,0x01,0x0f,0xf0,0x3b, 
         0x5A,0x07,0x41,0x0a,0x09,0x00,0x0f,0xe0,0x41,
         0x5A,0x07,0x42,0x0a,0x0b,0x00,0x0f,0x08,0x42
    } ; 
     char mti[2] = {0x02,0x00}; 
     char tid[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
     TIMER   viDelay_s; 
     TIMER   viTEver1min;
     iUChar_t viCommuInterface = 0;
     iUChar_t modeOperate = REALTIME_MODE;
     iUChar_t deviceStatus = DEVICE_ENABLE;        
     
    TIMER_setTimer(&viTEver1min, 60);  
    //TIMER_setTimer(&viTEver1min, 30);
    while(!TIMER_checkTimerExceed(viTEver1min)){
       #asm("wdr")
    }
    
    #asm("wdr")
    
     printDebug("[managerTask]Task Running...\r\n");    
     
     
//    if(iQueueReceive(pviQueueSensorToMang,&pviRXReceiveSensor)==1){
//        printDebug("<managerTask>ACD");
//        for(i=0;i<SENSER_ADC_MAX_SIZE;i++){
//            printDebug("[%d]:%d | ",(i+1),pviRXReceiveSensor.adc[i]);
//        } 
//        printDebug("\r\n");
//        printDebug("<managerTask>DHT11 Temp-c(%f), Humidity(%f)\r\n",pviRXReceiveSensor.dht11Temp,pviRXReceiveSensor.dht11Humi);
//	}else{
//		printDebug("<managerTask>data can not sent a data to pviQueueSensorToMang.\r\n");
//	}  
    
//
//    if(iQueueReceive(pviQueueDeviceToMang,&pviRXReceiveSensor)==1){
//        printDebug("<managerTask>ACD");
//        for(i=0;i<SENSER_ADC_MAX_SIZE;i++){
//            printDebug("[%d]:%d | ",(i+1),pviRXReceiveSensor.adc[i]);
//        } 
//        printDebug("\r\n");
//        printDebug("<managerTask>DHT11 Temp-c(%f), Humidity(%f)\r\n",pviRXReceiveSensor.dht11Temp,pviRXReceiveSensor.dht11Humi);
//	}else{
//		printDebug("<managerTask>data can not sent a data to pviQueueSensorToMang.\r\n");
//	} 
    
    //Terminal frist initial//
    if(viFlagTerInit==0){
        mti[0] = 0x08;
        mti[1] = 0x00;
        iPTCPack(&viTXDataBuff,(char *)"",0,mti,tid);
        if(iLanWriteData(&viTXDataBuff)){
            if(iLanReadData(&viRXDataBuff)){
                memcpy(&viUniTime,&viRXDataBuff.value[13],4); 
                iSyncUniTime(&viUniTime);
                if(iPTCParser(&viRXDataBuff)){
                    viFlagTerInit = 1;
                    iTagParser(&viRXDataBuff);      //tag process  othor do..
                    if(iPTCCheckHostReq(&viRXDataBuff)){
                    
                    }
                }
                //iHostReqProcess(&viTXDataBuff,&viRXDataBuff);
            }
        } 
    }else{
    
        //deviceStatus = DEVICE_DISABLE;
        deviceStatus = DEVICE_ENABLE;  
        
        if(deviceStatus == DEVICE_ENABLE){
            TIMER_setTimer(&viDelay_s, 5);
            while(!TIMER_checkTimerExceed(viDelay_s)){
               #asm("wdr")
            }
            
            viDatAndStatLength = (viDataReport.length + viStatusReport.length);
            if(viDatAndStatLength<=0){
               modeOperate = LOGGER_MODE;
               if(1){   //if SD record file empty then to return
                  return;
               }          
            }else{
               pviDatAndStatBuff = (iChar_t*)malloc(viDatAndStatLength);
               if(pviDatAndStatBuff!=NULL){
                  memcpy(&pviDatAndStatBuff[0],viDataReport.value,viDataReport.length);
                  memcpy(&pviDatAndStatBuff[viDataReport.length],viStatusReport.value,viStatusReport.length);
               }else{
                  free(pviDatAndStatBuff);
                  printDebug("[managerTask]data and ststus can't allocate mem.\r\n"); 
                  return;
               } 
               modeOperate = REALTIME_MODE;
            }
            
            
            if(modeOperate == REALTIME_MODE){
                 mti[0] = 0x02;
                 mti[1] = 0x00; 
            }else if(modeOperate == LOGGER_MODE){
                 mti[0] = 0x03;
                 mti[1] = 0x00; 
            }
            
            //iPTCPack(&viTXDataBuff,viGenDataElem,sizeof(viGenDataElem),mti,tid);   
            iPTCPack(&viTXDataBuff,(const char*) pviDatAndStatBuff,viDatAndStatLength,mti,tid);
            free(pviDatAndStatBuff);
             
            iMangQueueAddItem(&viTXDataBuff,&viTXDataBuff.value[17]); //add protocol in mang queue
            iMangQueueDisplay();//display mang queue
            delay_ms(1000);
            iDataInsert(viTXDataBuff.value,viTXDataBuff.length);
            #asm("wdr")
            
            //print_payload(viTXDataBuff.value, (sizeof(viGenDataElem)+20));               //20 = header + check sum   
            while(iMangCheckRetransmit(&viTXDataBuff.value[17])!=0){ 
                if(iLanWriteData(&viTXDataBuff)){ 
                      if(iLanReadData(&viRXDataBuff)){
                            memcpy(&viUniTime,&viRXDataBuff.value[13],4); 
                            iSyncUniTime(&viUniTime);
                            iMangQueueDelItem(&viRXDataBuff.value[17]);       //del queue with fid 
                            if(iPTCParser(&viRXDataBuff)){    //report success
                                  printDebug("[managerTask]Print.[\r\n");
                                  print_payload(viRXDataBuff.value,viRXDataBuff.length);
                                  printDebug("]\r\n"); 
                                  
                                  iTagParser(&viRXDataBuff);      //tag process  othor do..
                                  
                                   
                                  if(iPTCCheckHostReq(&viRXDataBuff)){
                                     mti[0] = 0x08;
                                     mti[1] = 0x00;
                                     iPTCPack(&viTXDataBuff,(char *)"",0,mti,tid);
                                     while(viHostRetransmit!=0){
                                        if(iLanWriteData(&viTXDataBuff)){
                                            if(iLanReadData(&viRXDataBuff)){
                                                iHostReqProcess(&viTXDataBuff,&viRXDataBuff);
                                            }
                                            break;
                                        } 
                                        viHostRetransmit--;
                                     }
                                  }
                            }
                            break;
                       }else{
                            //iMangQueueDelItem(&viRXDataBuff.value[17]);       //del queue with fid
                       }
                }
                if(iMangCheckRetransmit(&viTXDataBuff.value[17])==0){
                     iDataLogInsert(viTXDataBuff.value,viTXDataBuff.length);
                     iMangQueueDelItem(&viTXDataBuff.value[17]);       //del queue with fid  due over retransmit
                }
            }
            
            
        }
    
    }
    
    
    
}

iUInt_t iHostReqProcess(iData_t * pviTXDataBuff_arg,iData_t * pviRXDataBuff_arg){
    char mti[2] = {0x08,0x00}; 
    char tid[8] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}; 
    iUInt_t viReturn = 0; 
    iUChar_t viRetransmit = 1; 
    iChar_t viHostNeedFlag = 0;
    
//    iPTCPack(pviTXDataBuff_arg,(char *)"",0,mti,tid);
//    iMangQueueAddItem(pviTXDataBuff_arg,&pviTXDataBuff_arg->value[17]); //add protocol in mang queue 
//    iMangQueueDisplay();//display mang queue
//    iDataInsert(pviTXDataBuff_arg->value,pviTXDataBuff_arg->length);
    printDebug("[iHostReqProcess]TX Print 1.[\r\n");
    print_payload(pviTXDataBuff_arg->value,pviTXDataBuff_arg->length);
    printDebug("]\r\n");  
    
    printDebug("[iHostReqProcess]RX Print 2.[\r\n");
    print_payload(pviRXDataBuff_arg->value,pviRXDataBuff_arg->length);
    printDebug("]\r\n");  
    
    if(iPTCParser(pviRXDataBuff_arg)){    //report success
        printDebug("[iHostReqProcess]Print.[\r\n");
        print_payload(pviRXDataBuff_arg->value,pviRXDataBuff_arg->length);
        printDebug("]\r\n"); 
                                  
        iTagParser(pviRXDataBuff_arg);      //tag process  othor do..
    }
//    
//    printDebug("[iHostReqProcess]FID(%02x%02x).\r\n",*(pviTXDataBuff_arg->value+17),*(pviTXDataBuff_arg->value+18));
    
//        if(iLanReadData(pviRXDataBuff_arg)){ 
//            iMangQueueDelItem((pviTXDataBuff_arg->value+17));       //del queue with fid 
//            if(iPTCParser(pviRXDataBuff_arg)){    report success
//                printDebug("[iHostReqProcess]Print 1-1.[\r\n");
//                print_payload(pviTXDataBuff_arg->value,pviTXDataBuff_arg->length);
//                printDebug("]\r\n"); 
//            }
//        }
//    
//    while(iMangCheckRetransmit((pviTXDataBuff_arg->value+17))!=0){ 
//        if(iLanWriteData(pviTXDataBuff_arg)){
//            if(iLanReadData(pviRXDataBuff_arg)){ 
//                iMangQueueDelItem((pviTXDataBuff_arg->value+17));       //del queue with fid 
//                if(iPTCParser(pviRXDataBuff_arg)){    report success
//                    printDebug("[iHostReqProcess]Print 1-1.[\r\n");
//                    print_payload(pviTXDataBuff_arg->value,pviTXDataBuff_arg->length);
//                    printDebug("]\r\n"); 
//                }
//            }                   
//        }
//        if(iMangCheckRetransmit((pviTXDataBuff_arg->value+17))==0){
//             iDataLogInsert(pviTXDataBuff_arg->value,pviTXDataBuff_arg->length);
//             iMangQueueDelItem((pviTXDataBuff_arg->value+17));       //del queue with fid  due over retransmit
//        }
//    }
    
    
    return viReturn;
}

void iDataReportTX(){

}

void iStatusReportTX(iData_t * pviTXDataBuff_arg,iData_t * pviRXDataBuff_arg){

}

void iEventReportTX(){

}

void iTerAckByTagB(iChar_t viTagName, iChar_t viPropNumber){              //host requst = A , terminal ack = B
     iChar_t viMTI[2] = {0x08,0x00};
     iChar_t viTID[8];
     iChar_t viAckData[4] = {0x4b,0x02,0x10,0x00};  
     iData_t * pviDataTX; 
     iData_t * pviDataRX;
     
     pviDataTX = (iData_t*)malloc(sizeof(iData_t)); 
     pviDataRX = (iData_t*)malloc(sizeof(iData_t));
     if(pviDataTX!=NULL && pviDataRX!=NULL){
         //memcpy(viTID,&viDVConfigUpdate.viProperties.viDeviceID[0],8);
         printConfig(&viDVConfigUpdate.viProperties.viDeviceID[0],sizeof(viDVConfigUpdate.viProperties.viDeviceID));
         memcpyd(viTID,(char eeprom  *)&viDVConfigUpdate.viProperties.viDeviceID[0],8);
         print_payload(viTID,8);
         if(viTagName==TAG_4A){ 
            if(viPropNumber==CONF_PROPERTIES){
                iPTCPack(pviDataTX,viAckData,sizeof(viAckData),viMTI,viTID);
                if(iLanWriteData(pviDataTX)){
                    if(iLanReadData(pviDataRX)){
                        if(iPTCParser(pviDataRX)){
                            viFlagTerInit = 1;
                            iTagParser(pviDataRX);      //tag process  othor do..
                            if(iPTCCheckHostReq(pviDataRX)){
                            
                            }
                        }
                    }
                }
            }
            
         }
         free(pviDataTX);
         free(pviDataRX);
     }else{
        printDebug("[iTerAckByTagB]Not allocate memory.\r\n");
     }
}

void iTagParser(iData_t *pviData_arg){
    int i,j;
    iUInt_t viDataLen = 0; 
    
    printDebug("[iTagParser]Host response\r\n");  
    viDataLen = pviData_arg->value[1];
    printDebug("[iTagParser]Len(%d)\r\n",viDataLen);
        
    if(pviData_arg->value[0] == 0x00){
        for(i=2;i<(viDataLen);i+=2){
             //printDebug("[iTagParser]Loop:%02x(%d)",iPTC_RX.dat.value[i],iPTC_RX.dat.value[(i+1)]);
             switch(pviData_arg->value[i]){
                case TAG_2A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]);
                   break;
                }
                case TAG_3A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                }
                case TAG_4A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                } 
                case TAG_5A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                }
                case TAG_6A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                }
                case TAG_7A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                }
                case TAG_8A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",pviData_arg->value[i],pviData_arg->value[(i+1)]); 
                   break;
                }
                case TAG_9A:{
                   printDebug("[iTagParser]Tag:%02x(%d)\r\n",&pviData_arg->value[i],&pviData_arg->value[(i+1)]); 
                   break;
                }
             }
        }
    }else if(pviData_arg->value[0] == TAG_4A){
        switch(pviData_arg->value[2]){ 
             case CONF_PROPERTIES:{      //config properties
                printDebug("[iTagParser]Host need to config(Properties)\r\n");
                memcpye((char eeprom *)&viDVConfigUpdate.viProperties,&pviData_arg->value[3],viDataLen); 
                printConfig((char eeprom *)&viDVConfigUpdate.viProperties,sizeof(viDVConfigUpdate.viProperties)); 
                iTerAckByTagB(TAG_4A,CONF_PROPERTIES);
                break;
             }
             case CONF_OPERATEING:{      //config properties
                printDebug("[iTagParser]Host need to config(Operating)\r\n");
                memcpye((char eeprom *)&viDVConfigUpdate.viOperate,&pviData_arg->value[3],viDataLen); 
                printConfig((char eeprom *)&viDVConfigUpdate.viOperate,sizeof(viDVConfigUpdate.viOperate));
                break;
             }
             case CONF_BUZZER:{      //config properties
                printDebug("[iTagParser]Host need to config(Buzzer)\r\n");
                break;
             } 
             case CONF_ENVIRON_COND:{      //config properties
                printDebug("[iTagParser]Host need to config(Environment Alarm Condition)\r\n");
                break;
             }
        } 
        
        if((pviData_arg->value[2]&0xf0) == CONF_DIGI_IN){
             printDebug("[iTagParser]Host need to config(Digital Input)\r\n");
        }else if((pviData_arg->value[2]&0xf0) == CONF_DIGI_OUT){
             printDebug("[iTagParser]Host need to config(Digital Output)\r\n");
        }else if((pviData_arg->value[2]&0xf0) == CONF_ANA_IN){
             printDebug("[iTagParser]Host need to config(Analog Input)\r\n");
        }else if((pviData_arg->value[2]&0xf0) == CONF_ANA_OUT){
             printDebug("[iTagParser]Host need to config(Analog Output)\r\n");
        }
        
    }else{
       printDebug("[iTagParser]Host response unknow(%02x).\r\n",pviData_arg->value[0]);
    }
    
    
}


iInt_t iMangQueueAddItem(iData_t *pviDataItem_arg,iChar_t *pviFid_arg){
    iInt_t viReturn = -1;
    iInt_t i;  
    iChar_t viFid[2];
     
    memcpy(viFid,&pviFid_arg[0],2);
    for(i=0;i<MANG_QUEUE_MAX;i++){
        if(viMangQueue[i].value == NULL){
            viMangQueue[i].value = (iChar_t * const)malloc(pviDataItem_arg->length);
            if(viMangQueue[i].value != NULL){
                viMangQueue[i].length = pviDataItem_arg->length; 
                memcpy(viMangQueue[i].value,&pviDataItem_arg->value[0],pviDataItem_arg->length);
                memcpy(&viMangQueue[i].fid[0],&viFid[0],2);
                viMangQueue[i].reTransmit = MANG_RETRANSMIT; 
                printDebug("[iMangQueueAddItem]Add by FID(%02x%02x).\r\n",viFid[0],viFid[1]);
                viReturn = 1;
                break;
            }else{
               printDebug("[iMangQueueAddItem]can't allocate mem.\r\n"); 
               viReturn = -1;
            }
        }else{
           viReturn = 0;
        }
    }
    if(viReturn == 0){
        printDebug("[iMangQueueAddItem]MangQueue Full.\r\n");
    }
    return viReturn;
}
iInt_t iMangQueueDelItem(iChar_t *pviFid_arg){
    iInt_t viReturn = -1;
    iInt_t i; 
    iChar_t viFid[2];  
    
    memcpy(viFid,&pviFid_arg[0],2);
    printDebug("[iMangQueueDelItem]Del by FID(%02x%02x).\r\n",viFid[0],viFid[1]);
    for(i=0;i<MANG_QUEUE_MAX;i++){
        if(viMangQueue[i].value != NULL){
            if(memcmp(viMangQueue[i].fid,&viFid[0],2)==0){
                free(viMangQueue[i].value);
                viMangQueue[i].value = NULL;
                memset(viMangQueue[i].fid,0,2);
                viMangQueue[i].length = 0;  
                viReturn = 1;
                break;
            }else{
                viReturn = -1;
            }
        }else{
            viReturn = 0;
        }
    }
    if(viReturn == 0){
        printDebug("[iMangQueueDelItem]Queue is empty.\r\n");
    }else if(viReturn == -1){
        printDebug("[iMangQueueDelItem]FID invalid.\r\n");
    }
    return viReturn;
}

void iMangQueueDisplay(){
    iInt_t viReturn = -1;
    iInt_t i;
    
    for(i=0;i<MANG_QUEUE_MAX;i++){
        printDebug("[iMangQueueDisplay]Queue %d.[\r\n",i+1);
        if(viMangQueue[i].value != NULL){
           print_payload(viMangQueue[i].value,viMangQueue[i].length);
        }else{
           printDebug("Empty\r\n");
        }
        printDebug("]\r\n");
    }
}

iChar_t iMangCheckRetransmit(iChar_t *pviFid_arg){
    iInt_t viReturn = -1;
    iInt_t i;
    iChar_t viFid[2]; 
    
    memcpy(viFid,&pviFid_arg[0],2);
    for(i=0;i<MANG_QUEUE_MAX;i++){
        if(viMangQueue[i].value != NULL){
           if(memcmp(viMangQueue[i].fid,&viFid[0],2)==0){  
                printDebug("[iMangCheckRetransmit]Retransmit remain %d.\r\n",viMangQueue[i].reTransmit);
                viMangQueue[i].reTransmit--;
                viReturn = viMangQueue[i].reTransmit; 
                if(viReturn<0){
                   viReturn = 0;
                } 
                break;
            }else{
                viReturn = -1;
            }
        }else{
           printDebug("There is no FID(%02x%02x) in queue.\r\n",viFid[0],viFid[1]);
        }
        printDebug("]\r\n");
    }
    return viReturn;
}

iChar_t mangInitDevice(){

}

void memcpye(char eeprom *eptr, char *dptr, int len)
{
   int i=0; 
   for(i=0;i<len;i++) {
      *eptr++ = *dptr;
      dptr++;
   }
   return;
}

void memcpyd(char *dptr, char eeprom *eptr,  int len)
{
   int i=0; 
   for(i=0;i<len;i++) {
      *dptr++ = *eptr;
      eptr++;
   }
   return;
}

void printConfig(char eeprom *eptr,unsigned int len)
{
   int i=0,j=0;
   printDebug("[printConfig]Data:[\r\n"); 
   printDebug("%04d\t\t",0);
   for(i=0,j=1;i<len;i++,j++) {
      printDebug("%02x",*(eptr++));
      if(j==8){
         printDebug("\t\t");
      }else if(j==16){
         printDebug("\r\n");
         printDebug("%04d\t\t",i+1);
         j=0;
      }
   } 
   printDebug("\r\n]\r\n");
   return;
}

void iSyncUniTime(unsigned long int *viUniTime_arg){
     unsigned long int viUniTimeTerminal; 
     unsigned long int viUniTimeServer;
     struct rtc_structure rtcTest; 
     
     //tim 
     viUniTimeTerminal = DS1672_read(); 
     viUniTimeServer = *viUniTime_arg; 
      
     if((viUniTimeServer-viUniTimeTerminal)>40){
//         printDebug("[iSyncUniTime]Terminal %ld\r\n",viUniTimeTerminal);
//         printDebug("[iSyncUniTime]Server %ld\r\n",viUniTimeServer);  
//         printDebug("[iSyncUniTime]Time Diff %ld\r\n",(viUniTimeServer-viUniTimeTerminal)); 
//         
//         printDebug("[iSyncUniTime]Update Time.\r\n");  
         RTC_setBinary(viUniTimeServer);
//         RTC_getDateTime(&rtcTest,0);
//         printRTC(rtcTest);
     }else if((viUniTimeServer-viUniTimeTerminal)>(-40)){ 
//         printDebug("[iSyncUniTime]Terminal %ld\r\n",viUniTimeTerminal);
//         printDebug("[iSyncUniTime]Server %ld\r\n",viUniTimeServer);  
//         printDebug("[iSyncUniTime]Time Diff %ld\r\n",(viUniTimeServer-viUniTimeTerminal)); 
//         
//         printDebug("[iSyncUniTime]Update Time.\r\n"); 
         RTC_setBinary(viUniTimeServer);
//         RTC_getDateTime(&rtcTest,0);
//         printRTC(rtcTest);
     }
     
     
}

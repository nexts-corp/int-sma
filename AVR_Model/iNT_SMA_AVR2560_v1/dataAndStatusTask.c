#include <delay.h>
#include "dataAndStatusTask.h"
#include "sensorManager.h"
#include "statusManager.h"
#include "sensor.h"
#include "dht11.h"

unsigned int viSwitchContextSensor;

extern piQueueHandle_t pviQueueTaskAToTaskB;

extern piQueueHandle_t pviQueueSensorToMang;
extern piQueueHandle_t pviQueueMangToSensor;

iDataReport_t viDataReportBuff;
iStatusReport_t viStatusReportBuff;

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;

iSensorData_t pviSensorData;

void dataAndStatusTask(void *pviParameter){
    printDebug("<sensorTask>Task Running...\r\n"); 
     
//    swContextSensor();
//    swContextSensor(); 

    //printDebug("[sensorTask],Queue->Quantity(%d).\r\n",((iQueue_t *)pviQueueSensorToMang)->itemCount);
//    if(iQueueSend(pviQueueSensorToMang,&pviSensorData)==1){
//        printDebug("<sensorTask>pviQueueSensorToMang can sent a data.\r\n");
//    }else{
//        printDebug("<sensorTask>data can not sent a data to pviQueueSensorToMang.\r\n");
//    }
    #asm("wdr")
    iSensorRead(&viDataReport);                        //the 5A packed 
    #asm("wdr")
    iStatusRead(&viStatusReport);                      //the 5A packed 
    #asm("wdr")
    delay_ms(100);
}
//
//void swContextSensor(){
//     unsigned int i; 
//     ST_DHT11 *dhtDevice; 
//     int viReturn;   
//     //pviSensorData_t  pviSensorData;
//     #asm("wdr")
//     if(viSwitchContextSensor == SENSER_MAX_TYPE){
//          viSwitchContextSensor = 0;
//     }
//     switch(viSwitchContextSensor){
//         case SENSOR_ADC:{  
//            for(i=0;i<SENSER_ADC_MAX_SIZE;i++){
//                //printDebug("ACD[%d] : %d\r\n",(i+1),read_adc(8+i));
//                pviSensorData.adc[i] =  read_adc(8+i);
//            }
//            break;
//         }
//         case SENSOR_DIGI:{  
//            delay_ms(300);
//            viReturn =DHT11Read(dhtDevice);
//            if(viReturn==DHT11_SUCCESS){
//               //printDebug("DHT11 Temp-c(%f), Humidity(%f)\r\n",dhtDevice->temp,dhtDevice->humi);
//               pviSensorData.dht11Temp = dhtDevice->temp;
//               pviSensorData.dht11Humi = dhtDevice->humi;
//            }else if(viReturn==DHT11_ERROR_CHECKSUM){
//               printDebug("DHT11 errer(check sum).\r\n");
//            }else if(viReturn==DHT11_ERROR_TIMEOUT){
//               printDebug("DHT11 errer(time out).\r\n");
//            }
//            break;
//         }
//         default:{
//            break;
//         }
//     } 
//     ++ viSwitchContextSensor;
//}
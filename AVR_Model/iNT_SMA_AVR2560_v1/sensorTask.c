#include <delay.h>
#include "sensorTask.h"
#include "sensorManager.h"
#include "statusManager.h"
#include "sensor.h"
#include "dht11.h"

unsigned int viSwitchContextSensor;

extern piQueueHandle_t pviQueueTaskAToTaskB;

extern piQueueHandle_t pviQueueSensorToMang;
extern piQueueHandle_t pviQueueMangToSensor;

extern iDataMini_t viDataReport;
extern iDataMini_t viStatusReport;

iSensorData_t pviSensorData;

//void task_a(void *pviParameter){
//    pTaskHandle pviReturn;
//    char buffferQueue[10] = "bongkot";
//    char buffferQueue2[10] = "chaiwaree";
//    iData_t data;
//    
//    memcpy(data.value,buffferQueue,sizeof(buffferQueue));
//    data.length = sizeof(buffferQueue);
//    
//    if(iQueueSend(pviQueueTaskAToTaskB,&data)==1){
//        printDebug("<main>pviQueueTaskAToTaskB can sent a data.\r\n");
//    }else{
//        printDebug("<main>data can not sent a data(a) to pviQueueData1.\r\n");
//    }
//    
//    memcpy(data.value,buffferQueue2,sizeof(buffferQueue));
//    data.length = sizeof(buffferQueue2);
//    
//    if(iQueueSend(pviQueueTaskAToTaskB,&data)==1){
//        iQueueReadMyself(pviQueueTaskAToTaskB);
//        printDebug("<main>pviQueueTaskAToTaskB can sent a data.\r\n");
//    }else{
//        printDebug("<main>data can not sent a data(a) to pviQueueData1.\r\n");
//    }
//}
void sensorTask(void *pviParameter){
    printDebug("<sensorTask>Task Running...\r\n"); 
     
//    swContextSensor();
//    swContextSensor(); 

    //printDebug("[sensorTask],Queue->Quantity(%d).\r\n",((iQueue_t *)pviQueueSensorToMang)->itemCount);
//    if(iQueueSend(pviQueueSensorToMang,&pviSensorData)==1){
//        printDebug("<sensorTask>pviQueueSensorToMang can sent a data.\r\n");
//    }else{
//        printDebug("<sensorTask>data can not sent a data to pviQueueSensorToMang.\r\n");
//    }

    iSensorRead(&viDataReport); 
    iStatusRead(&viStatusReport); 
    delay_ms(1000);
}

void swContextSensor(){
     unsigned int i; 
     ST_DHT11 *dhtDevice; 
     int viReturn;   
     //pviSensorData_t  pviSensorData;
     #asm("wdr")
     if(viSwitchContextSensor == SENSER_MAX_TYPE){
          viSwitchContextSensor = 0;
     }
     switch(viSwitchContextSensor){
         case SENSOR_ADC:{  
            for(i=0;i<SENSER_ADC_MAX_SIZE;i++){
                //printDebug("ACD[%d] : %d\r\n",(i+1),read_adc(8+i));
                pviSensorData.adc[i] =  read_adc(8+i);
            }
            break;
         }
         case SENSOR_DIGI:{  
            delay_ms(300);
            viReturn =DHT11Read(dhtDevice);
            if(viReturn==DHT11_SUCCESS){
               //printDebug("DHT11 Temp-c(%f), Humidity(%f)\r\n",dhtDevice->temp,dhtDevice->humi);
               pviSensorData.dht11Temp = dhtDevice->temp;
               pviSensorData.dht11Humi = dhtDevice->humi;
            }else if(viReturn==DHT11_ERROR_CHECKSUM){
               printDebug("DHT11 errer(check sum).\r\n");
            }else if(viReturn==DHT11_ERROR_TIMEOUT){
               printDebug("DHT11 errer(time out).\r\n");
            }
            break;
         }
         default:{
            break;
         }
     } 
     ++ viSwitchContextSensor;
}
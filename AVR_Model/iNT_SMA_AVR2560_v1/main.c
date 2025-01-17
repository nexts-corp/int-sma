/*****************************************************
Project : SMA_AVR2560_V1 : 1
Date    : 01/6/2015
Author  : Next-Corp Team
Company : Next-Corp Co.,Ltd.
Comments: 
Version Format : 


Chip type               : ATmega2560
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
*****************************************************/
#include <mega2560.h>
//#include <stdlib.h>
//#include <string.h>
#include <delay.h>
#include "main.h"
#include "ims_eth_header.h"
#include "debug.h"
#include "uart.h"
//#include "queue.h"
#include "dataAndStatusTask.h"
#include "managerTask.h"
#include "debugTask.h"
#include "communicateTask.h"
#include "interruptTask.h"
#include "deviceTask.h"
#include "logTask.h"
#include "eventTask.h"
#include "lan.h"
#include "smaProtocol.h"
#include "sdManager.h"
#include "database.h"
           
#define MAX(a,b)                (((a) > (b)) ? (a) : (b))
#define MIN(a,b)                (((a) < (b)) ? (a) : (b))
//#define RANDOM(a,b)             ((rand() % (MAX(a,b) - MIN(a,b))) + MIN(a,b))

#define PERIOD_CHECK_MEMORY     300
      
// I2C Bus functions (for INT-AVXB-00-04)
#asm
   .equ __i2c_port=0x0B ;PORTD
   .equ __sda_bit=1
   .equ __scl_bit=0
#endasm

//unsigned char FIRMWARE_VERSION[] = "COMM-ETH-0.01a";           

piQueueHandle_t pviQueueData1 = NULL;
piQueueHandle_t pviQueueSensorToMang = NULL;
piQueueHandle_t pviQueueMangToSensor = NULL;
//piQueueHandle_t pviQueueMangToSDCard = NULL;

piQueueHandle_t pviQueueMangToConfig = NULL;
piQueueHandle_t pviQueueDeviceToMang = NULL;

itaskHandel_t piDeviceTask = NULL;
iData_t viBuffData;
iData_t viRXData;
iData_t viTXData;

iDataMini_t viDataReport;
iDataMini_t viStatusReport;
iDataMini_t viEventReport;

/*============================================================*/
void main(void){

    
    
    int err,i=0,k=0,res,last_status;   
    do{ err = initial_system();  }while(err);  
    
    printDebug("[main]Initial...\r\n");
    
//    pviQueueData1 = iQueueCreate(10,sizeof(int));
//	printDebug("<main>pviQueueData1 adr : %p\r\n",pviQueueData1);
//
//    if(iQueueSend(pviQueueData1,&a)==1){
//    
//	}else{
//		printDebug("<main>data can not sent a data(b) to pviQueueData1.\r\n");
//	} 
//    
//    if(iQueueReceive(pviQueueData1,&b)==1){
//        printDebug("<main>data : %d\r\n",b);
//	}else{
//		printDebug("<main>data can not sent a data(b) to pviQueueData1.\r\n");
//	}
    delay_ms(5000);      
    iWizReadConfig();
//    while(1){
//     #asm("wdr")   
//     delay_ms(1000);
//    }        
       
//    ETH_switchMode((unsigned char) MODE_NORM);
    if(iLanInit()){
        printDebug("[main]Lan Connected.\r\n");
    }else{
        printDebug("[main]Lan Fail.\r\n");
        iWizReadConfig();
        iLanInit();
    }
    
    iInitSDCard();
    
    ///////////////create queue////////////////
    pviQueueSensorToMang = iQueueCreate(2,sizeof(iSensorData_t));
    if(pviQueueSensorToMang == (int *)(-1)){ 
       printDebug("[main],pviQueueSensorToMang can not create.\r\n");
    }else{
       printDebug("[main],pviQueueSensorToMang can create.\r\n"); 
//       printDebug("[main],Queue->Quantity(%d).\r\n",((iQueue_t *)pviQueueSensorToMang)->itemCount);
    }
     
    pviQueueMangToConfig = iQueueCreate(2,sizeof(iConfigFormat_t));
    if(pviQueueMangToConfig == (int *)(-1)){ 
       printDebug("[main],pviQueueMangToConfig can not create.\r\n");
    }else{
       printDebug("[main],pviQueueMangToConfig can create.\r\n"); 
    }  
    
    pviQueueDeviceToMang = iQueueCreate(1,sizeof(unsigned char));
    if(pviQueueDeviceToMang == (int *)(-1)){ 
       printDebug("[main],pviQueueDeviceToMang can not create.\r\n");
    }else{
       printDebug("[main],pviQueueDeviceToMang can create.\r\n"); 
    } 
     
//    pviQueueMangToSensor = iQueueCreate(10,sizeof(int));
//    if(pviQueueMangToSensor == (int *)(-1)){ 
//       printDebug("<main>pviQueueSensorToMang can not create.\r\n");
//    }
    
    ///////////////create task//////////////// 
    piDeviceTask = iTaskCreate(logTask,"logger",5);
    if(piDeviceTask){
        printDebug("<main>logTask addr : %p\r\n",piDeviceTask);
    }  
    
    piDeviceTask = iTaskCreate(dataAndStatusTask,"dataAndStatus",5);      
    if(piDeviceTask){
        printDebug("<main>sensorTask addr : %p\r\n",piDeviceTask);
    }
//    printDebug("[main],Queue->Quantity(%d).\r\n",((iQueue_t *)pviQueueSensorToMang)->itemCount);		
    piDeviceTask = iTaskCreate(managerTask,"mang",5);
    if(piDeviceTask){
        printDebug("<main>managerTask addr : %p\r\n",piDeviceTask);
    }  
    

//    
//    piDeviceTask = iTaskCreate(debugTask,"debug",5);
//    if(piDeviceTask){
//        printDebug("<main>debugTask addr : %p\r\n",&piDeviceTask);
//    } 
//    
//    piDeviceTask = iTaskCreate(communicateTask,"commu",5);
//    if(piDeviceTask){
//        printDebug("<main>communicateTask addr : %p\r\n",&piDeviceTask);
//    }  
//    
//    piDeviceTask = iTaskCreate(interruptTask,"intp",10);
//    if(piDeviceTask){
//        printDebug("<main>interruptTask addr : %p\r\n",&piDeviceTask);
//    } 
//    
//    piDeviceTask = iTaskCreate(deviceTask,"device",5);
//    if(piDeviceTask){
//        printDebug("<main>deviceTask addr : %p\r\n",&piDeviceTask);
//    } 

    piDeviceTask = iTaskCreate(eventTask,"event",5);
    if(piDeviceTask){
        printDebug("<main>eventTask addr : %p\r\n",piDeviceTask);
    } 
  
   startSchedulerTask();
}

#include "deviceTask.h"
#include "config.h"

eeprom iUChar_t viFlagTerInit = 0;

extern piQueueHandle_t pviQueueMangToConfig;

iConfigFormat_t viDataConfig;

void deviceTask(void *pviParameter){
    printDebug("[deviceTask]Task Running...\r\n");   
    
    if(viFlagTerInit!=0){
    
    }else{
    
    }
     
    if(iQueueReceive(pviQueueMangToConfig,&viDataConfig)==1){
        printDebug("[deviceTask]There is configuration.\r\n");
	}else{
		printDebug("[deviceTask]There is not config.\r\n");
	} 
}
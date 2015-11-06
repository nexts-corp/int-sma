#include "logManager.h"
#include "ds1672.h"

eeprom unsigned long viDate_e = 0;
extern iChar_t viDataFName[];

void iGenFileDaily(){
    iChar_t viFileName[20];
    struct rtc_structure viRTCGet;
    RTC_getDateTime(&viRTCGet,7); 
    
    if((viDate_e == 0) || (viDate_e != viRTCGet.date) || (strncmp(viDataFName,"0:./00000000",12)==0)){
        printDebug("[iGenFileDaily]\r\n"); 
        viDate_e =  viRTCGet.date;
        printRTC(viRTCGet);
        if((viRTCGet.month<10) && (viRTCGet.day<10)){
            sprintf(viFileName,"0:./%d0%d0%d.nc",viRTCGet.year,viRTCGet.month,viRTCGet.day);
        }else if(viRTCGet.month<10){
            sprintf(viFileName,"0:./%d0%d%d.nc",viRTCGet.year,viRTCGet.month,viRTCGet.day);
        }else if(viRTCGet.day<10){
            sprintf(viFileName,"0:./%d%d0%d.nc",viRTCGet.year,viRTCGet.month,viRTCGet.day);
        }else{
            sprintf(viFileName,"0:./%d%d%d.nc",viRTCGet.year,viRTCGet.month,viRTCGet.day);
        }
        
        memcpy(viDataFName,viFileName,strlen(viFileName)); 
        printDebug("[iGenFileDaily]File name: %s\r\n",viDataFName);
    }
    
}
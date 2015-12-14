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
        sprintf(viFileName,"0:./%04d%02d%02d.nx",viRTCGet.year,viRTCGet.month,viRTCGet.date);
        memcpy(viDataFName,viFileName,strlen(viFileName)); 
        printDebug("[iGenFileDaily]File name: %s\r\n",viDataFName);
    } 
    
    
    
    //Clear all file that status 'N','Y' and 'L'
    //Queue files not settlement
    //read file max 90 files
}
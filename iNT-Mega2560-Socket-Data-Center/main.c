/*****************************************************
Project : COMM-ETH : 0.01a
Date    : 17/7/2012
Author  : Tawan Srisang
Company : Intelenics Co.,Ltd.
Comments: 
Version Format : 1Major.2Minor.1Edition


Chip type               : ATmega2560
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
*****************************************************/
#include <mega2560.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include "ims_eth_header.h"
#include "config.h"
#include "timer.h"
#include "ds1672.h"
#include "sensor.h"        
#include "status.h"
#include "alarm.h"
#include "dataqueue.h"
#include "eventqueue.h"
#include "log.h"
#include "logqueue.h"
#include "oled.h"
#include "menu.h"
#include "io.h"
#include "debug.h"
#include "gaussian.h"
#include "dht11.h"

#include "uart.h"
#include <stdio.h>
           
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

int updateStatus(void);                              
int eventCapture(void);
int statusReport(void);
int sensorDataReport(void);                                        
int eventReport(void);       
int sensorDataSave(void);
int eventSave(void);
int logReport(void);
extern void updateSensorADC(int ch);
extern int DHT11Read(ST_DHT11 *dhtDevice);

/*============================================================*/
void main(void)
{                                 
    int                     err,i=0,k=0,res,last_status;                               
                                                      
    TIMER                   sensor_report_timer[5];    
    TIMER                   status_report_timer;
    TIMER                   check_reset_timer;         
    TIMER                   check_memory_timer;  
    TIMER                   clear_duplicate_timer = 0;       
    TIMER                   readSensorTimer = 0;
    unsigned long int       timestamp_rd;                        
                                       
    char                    key_press = 0;        
    
    float tmpA = 0.0;
    float tmpB = 0.0; 
    float tmpC = 0.0;        
    
    /*bongkot*/
    int countSenserRead = 0;
    
    struct rtc_structure st_rtc;   
    //ST_DHT11 dhtDevice;
    //int dhtRet = 0;
    
    // -------- System Initialize ----------- //
    do{ err = initial_system();  }while(err);       
    
//    /*bongkot*/
//    while(1){ 
//        #asm("wdr")
//        dhtRet = DHT11Read(&dhtDevice);
//        if(dhtRet == DHT11_SUCCESS){
//           printDebug("DHT11: Temp(%f), Humi(%f) \r\n",dhtDevice.temp,dhtDevice.humi);
//        }else if(dhtRet == DHT11_ERROR_TIMEOUT){      //DHT11_ERROR_TIMEOUT
//           printDebug("DHT11:Time out. \r\n");
//        }else if(dhtRet == DHT11_ERROR_CHECKSUM){      //DHT11_ERROR_TIMEOUT
//           printDebug("DHT11:Check sum error. \r\n");
//        }
//        
//        delay_ms(2000);
//    }
//    
    /*bongkot*/  
//    while(1){ 
//        #asm("wdr")
//        for(countSenserRead=0;countSenserRead<5;countSenserRead++){
////            if(TIMER_checkTimerExceed(readSensorTimer)){       
////                TIMER_setTimer(&readSensorTimer, 5); 
////                updateSensorADC(countSenserRead);                    // -- update adcData[ch] -- //
////                //printDebug("[%d]ADC: %f, Volt: %f \r\n", i, adcData[i], (adcData[i]/1023)*2.56);   
////                //printDebug("[%d]ADC: %f, Volt: %f \r\n", i, adcData[countSenserRead], (adcData[countSenserRead]/1023)*3.30);
////            } 
//            
//             updateSensorADC(countSenserRead);     
////             updateSensorADC(1); 
////             delay_ms(100);
////             updateSensorADC(2);
//             delay_ms(100);
//        }   
//        //delay_ms(2000); 
//        
//    }
                   
    //ETH_CONF;
                                                       
    ETH_switchMode((unsigned char) MODE_NORM); 
    
    /*bongkot*/ 
//    ETH_NORM;                        
//    ETH_reset();
//    ETH_OFF;     
//    delay_ms(2000);
//    ETH_ON;         
    
    // -- alarm indicator initialize -- //                                       
    buzzer_on = 0;          // -- buzzer off -- //    
    for(k=0;k<2;k++)        // -- MAX_DO_CHANNEL
    {                                                                                         
        IO_dout(k,!dout_config[k].stateOnEvent);     // -- Digital Output X OFF -- //              
    }                                         
    
    initDataLogQueue();
    initEventLogQueue();  
    ETH_initWaitQueue();
                    
    printDebug("\r\n------ Initial Complete ------\r\n");
    printDebug("Contact : contact.tawan@gmail.com\r\n"); 
                                
    
    //while(1){ printDebug("<main> TEMP_SEL = 0x%02X\r",TEMP_SEL);    delay_ms(100);   #asm("wdr")}
             
    for(i=0;i<MAX_SENS_CHANNEL;i++){                    
        if(operating_config.sensorEnable[i]){
            TIMER_setTimer(&sensor_report_timer[i], 5);              
        }
    }       
    
//    gaussian(271.8683, 392.0240, 489.2156, 
//                 -90, -20, 50, 
//                 &tmpA, &tmpB, &tmpC);                  
//                 
//    printDebug("<main> --A--> %f\r\n", tmpA);                     
//    printDebug("<main> --B--> %f\r\n", tmpB);                    
//    printDebug("<main> --C--> %f\r\n", tmpC); 
                                              
    TIMER_setTimer(&status_report_timer, 7);
    TIMER_setTimer(&check_reset_timer, 60);
    TIMER_setTimer(&check_memory_timer, PERIOD_CHECK_MEMORY);

    while(1){                             
           
        #asm("wdr")            
        
        // Reset at 05.00 in every day
        if(TIMER_checkTimerExceed(check_reset_timer))
        {       
            TIMER_setTimer(&check_reset_timer, 60);
            RTC_getDateTime(&st_rtc, 7);
//            printRTC(st_rtc);
            if(st_rtc.hour == 5) {
                if(st_rtc.min == 0) {
                    while(1);
                }
            }  
        }            
        
        // ----------- Received Debug Request ---------- //
        if(flag_PackageTerminate3){    debug_req();     }
        
        // ----------- On Device Key Press ------------- //
        key_press = IO_keyCheck();
        if(key_press){
            MENU_processKey(key_press);
        }
              
        // ----------- Ethernet Communication ---------- //
        ETH_receive();                   
        ETH_retransmit();
          
        // ----- check if configuration(s) has been updated ----- //
        if(is_properties_config_update){
            getPropertiesConfig(&properties_config); 
            is_properties_config_update = 0;
        }                     
        
        if(is_operating_config_update){
            getOperatingConfig(&operating_config); 
            is_operating_config_update = 0;
        }                                          
                                    
        for(i=0;i<MAX_SENS_CHANNEL;i++){
            if(is_sensor_config_update[i]){
                err = getSensorConfig(i,&sensor_config[i]);   
                if(err < 0){
                    printDebug("<main> getSensorConfig(%d) error\r\n",i);
                    continue;
                }
                is_sensor_config_update[i] = 0;  
                TIMER_setTimer(&sensor_report_timer[i], 5);
            }                               
        }                                      
        
        if(is_buzzer_config_update){            // -- update buzzer configuration if needed -- //
            getBuzzerConfig(&buzz_config); 
            is_buzzer_config_update = 0;
        }                           
                            
        for(i=0;i<USE_DIN;i++){                                        
            if(is_din_config_update[i]){        // -- update digital input configuration if needed -- //
                getAuxDinConfig(i,&din_config[i]); 
                is_din_config_update[i] = 0;                                                      
            }                         
        }                         
                            
        for(i=0;i<USE_DOUT;i++){                                        
            if(is_dout_config_update[i]){       // -- update digital output configuration if needed -- //
                getAuxDoutConfig(i,&dout_config[i]); 
                is_dout_config_update[i] = 0;                                                      
            }                         
        }              
        
        if(is_status_config_update){
            getStatusReportConfig(&status_config);
            is_status_config_update = 0;             
            
            TIMER_setTimer(&status_report_timer, 7);
        }                                 
        
        if(is_env_config_update){
            getEnvAlarmConfig(&env_config);
            is_env_config_update = 0;             
        }
        
        if(operating_config.deviceEnable)                       // ----- device enable ------ //
        {    
            // -------------- Sensor Association -------------- //
            for(i=0;i<MAX_SENS_CHANNEL;i++)                     
            {                    
                if(operating_config.sensorEnable[i])            // .. sensor enable .. //
                {                               
                    //** test **//               
//                    if(TIMER_checkTimerExceed(readSensorTimer))
//                    {       
//                        TIMER_setTimer(&readSensorTimer, 5);    
//                        updateSensorADC(i);                    // -- update adcData[ch] -- //
//                        printDebug("[%d]ADC: %f, Volt: %f \r\n", i, adcData[i], (adcData[i]/1023)*2.56);
//                    }
                    //-- test --//      
                    
//                    printDebug("<main> sensorRead\r\n");  
                    err = sensorRead(i,&sensor[i]);
                    if(err < 0)                                 // . sensor value error . //
                    {                                      
                        continue;
                    }
                    else                                        // . sensor value valid . //
                    {   
                        // --==== More detail in config (Re-Occurrance) ====-- //  
                        if(TIMER_checkTimerExceed(sensor_report_timer[i]))
                        {       
                            TIMER_setTimer(&sensor_report_timer[i], sensor_config[i].samplingRate);   
                                                               
                            timestamp_rd = DS1672_read();          
                            err = addDataQueue(timestamp_rd, i, sensor[i].value);    
                            if(err<0){
                                printDebug("<main> addSensorDataQueue(%d,%.1f) failed\r\n",i,sensor[i].value);
                                continue;   // -- continue other sensor 
                            }                                          
                        }       
                        else{
                            //printDebug("<main> [%d] %ld:%ld\r\n",i,baseCounter,sensor_report_timer[i]);
                        }      
                    } 
                    
                }
                else                                            // .. sensor disable .. //
                {
                }                               
            } // -- iterate each probe -- //   
                     
            #asm("wdr")
            // --------------- Status Management --------------- //                    
           // printDebug("<main> updateStatus()\r\n");
            last_status = updateStatus();       
            
            // --------------- Alarm Management --------------- //
            if(operating_config.alarmEnable)                    // .. alarm enable .. //          
            {                            
                // -- check alarm all -- //
                if(last_status < 0)                             // .. status update falied .. //
                {
                    printDebug("<main> updateStatus() failed\r\n");
                }  
                else if((last_status > 0) && !muteMode)         // .. found some status error and alarm not muted .. //
                {                            
                    // -- Buzzer Alarm -- //                                                                 
                    if(buzz_config.enable == 0x01 )             // -- buzzer enabled -- //
                    {
                        // -- check alarm source binded to Buzzer -- //                       
                        res = checkAlarm(buzz_config.eventSource, buzz_config.eventSourceNumber);       // -- alarm.h -- //
                        if(res){
                            buzzer_on = 1;          // -- buzzer on -- //   
                        }                        
                        else{    
                            buzzer_on = 0;          // -- buzzer off -- //
                        }
                    }                             
                    else                                        // -- buzzer disabled -- //
                    {                     
                        buzzer_on = 0;                  // -- buzzer off -- //                      
                    }

                    // -- Digital Output Alarm (iterate each channel) -- //                           
                    for(k=0;k<USE_DOUT;k++)
                    {                                    
                        if(dout_config[k].channelEnable == 0x01)            // -- digital output enabled -- //
                        {
                            // -- check alarm Digital Output -- //                                                                          
//                            printDebug("<main> dout[%d] alarm check -- \r\n",k);
                            res = checkAlarm(dout_config[k].eventSource, dout_config[k].eventSourceNumber);       // -- alarm.h -- //
                            if(res){              
                                IO_dout(k,dout_config[k].stateOnEvent);     // -- Digital Output X ON -- // 
                            }
                            else{                                                                          
                                IO_dout(k,!dout_config[k].stateOnEvent);    // -- Digital Output X OFF -- //                        
                            }                        
                        }                    
                        else                                                // -- digital output disabled -- //
                        {                                                                                          
                            IO_dout(k,!dout_config[k].stateOnEvent);        // -- Digital Output X OFF -- // 
                        }
                    }
                }                                   
                else                                            // .. no event .. //
                {
                    // -- no alarm -- // ^_^                                         
                    buzzer_on = 0;                  // -- buzzer off -- //    
                    for(k=0;k<USE_DOUT;k++)         // -- MAX_DO_CHANNEL
                    {                                          
                        IO_dout(k,!dout_config[k].stateOnEvent);     // -- Digital Output X OFF -- //              
                    }
                }
                
                res = eventCapture();                           // -- send event packet if needed -- // 
            }               
            else                                                // .. alarm disable .. //
            {                                    
            //    printDebug("<main> alarmDisable()\r\n");                                                
                // -- no alarm -- // ;-P                                           
                buzzer_on = 0;                      // -- buzzer off -- //    
                for(k=0;k<USE_DOUT;k++)             // -- MAX_DO_CHANNEL
                {                                                                                  
                    IO_dout(k,!dout_config[k].stateOnEvent);    // -- Digital Output X OFF -- //              
                } 
            }        
                    
            // -- check mute expired -- //    
            if(muteMode){                       
               // printDebug("<main> ALARM_checkMuteExpired()\r\n");      
                if(ALARM_checkMuteExpired()){    
                    muteAlarmExpired = 0xFFFFFFFF;
                    muteMode = 0;
                }
            }     
                       
            // ---------------- Operating Mode ---------------- // 
//            operating_config.mode = MODE_REALTIME;      //test
            if(operating_config.mode == MODE_REALTIME)          // .. realtime mode .. //
            {                 
                if(!duplicateListClear)
                {                        
                    res = TIMER_checkTimerExceed(clear_duplicate_timer);               
                    if(res != 0){          
                        TIMER_setTimer(&clear_duplicate_timer, 10);   
                        ETH_clearDuplicateRequestCmd();  
                    }                     
                    
                    sensorDataSave();
                    eventSave();
                }
                else
                {  
                    if(TIMER_checkTimerExceed(status_report_timer))
                    {       
                        TIMER_setTimer(&status_report_timer, status_config.intv);   
                                                      
                        statusReport();
                    }                                           
                     
                    // -- report log only when no realtime data not ready -- //
                    if( (sensorDataReport() == 0) && (eventReport() == 0) && (sdFreeSpace > 0) )   
                    {                
                        logReport();
                    }         
                }
            }
            else if(operating_config.mode == MODE_DATALOGGER)   // .. datalogger mode .. //
            {             
               // printDebug("<main> sensorDataSave()\r\n");  
                sensorDataSave();                        
              //  printDebug("<main> eventSave()\r\n");  
                eventSave();
            }                   
            else                                                // .. unknown mode .. //      
            {
                printDebug("<main> operating mode error!! [0x%02X]\r\n",operating_config.mode);
            }
                                           
            #asm("wdr")
            if(timer_1sec_flag)
            {                                                 
              //  printDebug("<main> timer_1sec_flag\r\n");  
                if(TIMER_checkTimerExceed(check_memory_timer) && (memoryStatus != ERROR))
                {                                                                           
              //      printDebug("<main> Check Memory \r\n");
                    TIMER_setTimer(&check_memory_timer, PERIOD_CHECK_MEMORY);
                                    
                    res = SD_readMemoryRemain(&sdFreeSpace);   
                    if(res < 0){
                        printDebug("<main> SD_readMemoryRemain() failed\r\n"); 
                    }    
                }                     
                
                if(currentPage == P_MONITOR)
                {                                 
              //      printDebug("<main> OLED_showSensorValue\r\n");  
                    OLED_showSensorValue();    
                    
                    for(i=0;i<2;i++){  
                        OLED_showInputStatus(i,dinStatus[i]);   
                    }                                        

                    for(i=0;i<2;i++){  
                        OLED_showOutputStatus(i,doutStatus[i]);
                    }                                          
                                                              
               //     printDebug("<main> OLED_showDateTime\r\n");  
                    OLED_showDateTime(properties_config.timeZone); 
                                                        
              //      printDebug("<main> OLED_showAlarmStatus\r\n");          
                    OLED_showAlarmStatus();
                }                        
                                                
              //  printDebug("<main> powerSrcStatus\r\n");  
                switch(powerSrcStatus){
                    case PWR_NORMAL     :   OLED_showPowerStatus(PWR_NORMAL,0);                     break;
                    case PWR_DOWN       :   OLED_showPowerStatus(PWR_DOWN,batteryVolt);             break; 
                    default             :   break; 
                }                        
                                          
            //    printDebug("<main> serverCommStatus\r\n");  
                switch(serverCommStatus){
                    case COMM_NORMAL    :   OLED_showServerCommStatus(OLED_SERVER_CONNECT);         break;
                    case COMM_LOST      :   OLED_showServerCommStatus(OLED_SERVER_DISCONNECT);      break; 
                    default             :   OLED_showServerCommStatus(OLED_SERVER_UNKNOWN);         break; 
                }
                                   
            //    printDebug("<main> lanStatus\r\n");  
                switch(lanStatus){
                    case COMM_LAN_CONNECT       :   OLED_showLanStatus(OLED_LAN_CONNECT);           break;
                    case COMM_LAN_DISCONNECT    :   OLED_showLanStatus(OLED_LAN_DISCONNECT);        break; 
                    default                     :   break; 
                }
                                       
            //    printDebug("<main> OLED_showBuzzerStatus\r\n");  
                if(muteMode){
                    OLED_showBuzzerStatus(OLED_BUZZER_MUTE);
                }
                else if(buzz_config.enable == 0x01){
                    OLED_showBuzzerStatus(OLED_BUZZER_ENABLE);
                }
                else{
                    OLED_showBuzzerStatus(OLED_BUZZER_DISABLE);
                }
                                                     
           //     printDebug("<main> memoryStatus\r\n"); 
                switch(memoryStatus){   
                    case MEM_NORMAL         :   OLED_showMemoryStatus(OLED_MEMORY_NORMAL);          break;
                    case MEM_LOW            :   OLED_showMemoryStatus(OLED_MEMORY_LOW);             break; 
                    case MEM_ERROR          :   OLED_showMemoryStatus(OLED_MEMORY_ERROR);           break; 
                    default                 :   break; 
                }                   
                                       
                timer_1sec_flag = 0;
            }  
        } // -- device enable -- //
        else                                                    // ----- device disable ----- //
        {                                                                       
            // -- no alarm -- // ;-P                                           
            buzzer_on = 0;                      // -- buzzer off -- //    
            for(k=0;k<USE_DOUT;k++)             // -- MAX_DO_CHANNEL
            {                                                                                  
                IO_dout(k,!dout_config[k].stateOnEvent);    // -- Digital Output X OFF -- //              
            } 
                     
                                      
            if(TEMP_SEL == TEMP_TYPE_K){   
                if(hwCalId == 9){
                    printDebug("<main> [Device disable] [TYPE-K] $CMDSNS N : X.XX                    \r");
                }
                else if(SENSOR_CONNECT){
                    printDebug("<main> [Device disable] [TYPE-K] $CMDSNS %d : %f                     \r",hwCalId,(adcData[hwCalId]*ADC_CONV_FAC*100));
                }        
                else{                       
                    printDebug("<main> [Device disable] [TYPE-K] $CMDSNS %d : disconnected           \r",hwCalId);
                }                                                                                        
                delay_ms(200);
            }                
            else if(TEMP_SEL == TEMP_TMEC){
                    printDebug("<main> [Device disable] [TMEC]                                       \r");
            }
            else if(TEMP_SEL == TEMP_PT100){
                    printDebug("<main> [Device disable] [PT100]                                      \r");
            }
            else if(TEMP_SEL == TEMP_RESERVED){
                    printDebug("<main> [Device disable] [RESERVED]                                   \r");
            }
        }                                                                                                              
        
        #asm("wdr")
    }                  
    
}
/*============================================================*/

/**************************************************************/
/****************** Status & Event Management *****************/
/**************************************************************/
/*============================================================*/
int updateStatus(void)   // -- manipulate function in status.c -- //
{
    int                 res=0, i=0, err=0;
    static TIMER        ETH_rst_timer=0xFFFFFFFF;
        
    // --------------- Update Sensor's Event --------------- //
    for(i=0;i<MAX_SENS_CHANNEL;i++)
    {   
        if(operating_config.sensorEnable[i])        // .. sensor enable .. //
        {                       
            // ----------- Update Probe Status ------------------------- //                      
            err =   updateSensorStatus(i);          
            if(err >= 0){
                res |=  err;                              
                if(sensor_config[i].alarmEnable && !err)        // . sensor alarm enable . //
                {         
                    // ----------- Update Temperature Status ------------------- //                      
                    err =   updateDataStatus(i);
                    res |=  err;
                }            
            }
        }           
    }
        
    // --- Update Battery Status --- //        
    err =   updateBatteryStatus();
    res |=  err;
    // --- Update Digital Input Status --- //     
    err =   updateDinStatus();             
    res |=  err;             
    // --- Update Digital Output Status --- //   
    err =   updateDoutStatus();           
    res |=  err;
    // --- Update Analog Input Status --- //
    // --- Update Communication Status --- //       
    err =   updateCommStatus();                      
    if(err){                      
                  
        if((ETH_rst_timer == 0xFFFFFFFF) || (ETH_rst_timer == 0)){               
            TIMER_setTimer(&ETH_rst_timer,15);                                   
            printDebug("<updateStatus> ETH_rst_timer time left = %ld(%lX)\r\n",(ETH_rst_timer-baseCounter),(ETH_rst_timer-baseCounter));
        }           
                
        if(TIMER_checkTimerExceed(ETH_rst_timer) == 1){
            ETH_NORM;
            ETH_reset();                          
            ETH_rst_timer = 0xFFFFFFFF;
        }           
    }
    res |=  err;
    // --- Update Memory Card Status --- //
    err =   updateMemoryStatus();  
    res |=  err;
    
    return res;  
}                                        
/*============================================================*/
int eventCapture(void)
{
    int                 res=0, i=0, err=0;    
    unsigned long int   timestamp_rd;         
    unsigned char       statusCode;
    
    // --------------- Check for Maintenance Event --------------- //
    err =   checkSendMIPAlarm(&statusCode);
    res |=  err;                          
    if(err){                       
        timestamp_rd    = DS1672_read();                             
        if(statusCode)
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Monitor Mode !!!!!\r\n");    
            //printOLED("AL12%1d",i+1);
        }
        else
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Maintenance In Progress !!!!!\r\n");      
            //printOLED("AL02%1d",i+1);
        }                                     
        err = addEventQueue(timestamp_rd,EVENT_MTNC,statusCode);       
        if(err < 0){
            printDebug("<eventCapture> addEventQueue() failed\r\n");
        }
    }
         
    /*                 
    // -- Analog Input Event -- //
    // -- reserved -- //      
    */
    // --------------- Check for Power Down Event --------------- //
    err =   checkSendPowerAlarm(&statusCode);
    res |=  err;                          
    if(err){                       
        timestamp_rd    = DS1672_read();                             
        if(statusCode)
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Main Power Down !!!!!\r\n");    
            //printOLED("AL12%1d",i+1);
        }
        else
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Main Power Up !!!!!\r\n");      
            //printOLED("AL02%1d",i+1);
        }                                     
        err = addEventQueue(timestamp_rd,EVENT_PWR,statusCode);       
        if(err < 0){
            printDebug("<eventCapture> addEventQueue() failed\r\n");
        }
    }                         
                                                            
    // --------------- Check for Battery Event --------------- //
    err =   checkSendBatteryAlarm(&statusCode);
    res |=  err;                          
    if(err){                       
        timestamp_rd    = DS1672_read();                             
        if(statusCode)
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Battery Low !!!!!\r\n");    
            //printOLED("AL12%1d",i+1);
        }
        else
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Battery OK !!!!!\r\n");      
            //printOLED("AL02%1d",i+1);
        }                                     
        err = addEventQueue(timestamp_rd,EVENT_BAT,statusCode);       
        if(err < 0){
            printDebug("<eventCapture> addEventQueue() failed\r\n");
        }
    }       
    
    // --------------- Check for Memory Event --------------- //
    err =   checkSendMemoryAlarm(&statusCode);
    res |=  err;                          
    if(err){                       
        timestamp_rd    = DS1672_read();                             
        if(statusCode == ES_LOWMEMORY)
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Memory Low !!!!!\r\n");    
            //printOLED("AL12%1d",i+1);
        } 
        else if(statusCode == ES_MEMORYERROR)
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Memory Error !!!!!\r\n");    
            //printOLED("AL12%1d",i+1);
        }
        else
        {
            printDebug("\r\n<eventCapture> !!!!!!!!!!! Memory OK !!!!!\r\n");      
            //printOLED("AL02%1d",i+1);
        }                                     
        err = addEventQueue(timestamp_rd,EVENT_MEM,statusCode);       
        if(err < 0){
            printDebug("<eventCapture> addEventQueue() failed\r\n");
        }
    }       
                                                    
    // --------------- Check for Digital Input Event --------------- //
    for(i=0;i<USE_DIN;i++)
    {
        if(din_config[i].channelEnable)
        {
            err =   checkSendDinAlarm(i,&statusCode);
            res |=  err;                          
            if(err){                       
                timestamp_rd    = DS1672_read();                             
                if(statusCode)
                {
                    printDebug("\r\n<eventCapture> !!!!!!!!!!! Din %d Rising !!!!!\r\n", i+1);    
                    //printOLED("AL12%1d",i+1);
                }
                else
                {
                    printDebug("\r\n<eventCapture> !!!!!!!!!!! Din %d Falling !!!!!\r\n", i+1);      
                    //printOLED("AL02%1d",i+1);
                }                                     
                err = addEventQueue(timestamp_rd,EVENT_DIN(i),statusCode);       
                if(err < 0){
                    printDebug("<eventCapture> addEventQueue() failed\r\n");
                }
            }       
        }// -- din channel Enable -- //  
    }// -- for loop -- //        
   
    // --------------- Check for Sensor's Event --------------- //
    for(i=0;i<MAX_SENS_CHANNEL;i++)
    {   
        if(operating_config.sensorEnable[i])        // .. sensor enable .. //
        {                       
            // ----------- Check Probe Alarm ------------------------- //                      
            err =   checkSendSensorAlarm(i,&statusCode);          
            res |=  err;                         
            if(err){                       
                timestamp_rd    = DS1672_read();                                   
                if(statusCode)
                {
                    printDebug("\r\n<eventCapture> !!!!!!!!!!! PROBE %d DISCONNECT !!!!!\r\n", i+1);    
                    printOLED("AL12%1d",i+1);
                }
                else
                {
                    printDebug("\r\n<eventCapture> !!!!!!!!!!! PROBE %d CONNECT !!!!!\r\n", i+1);      
                    printOLED("AL02%1d",i+1);
                }                                                                        
                
                err = addEventQueue(timestamp_rd,EVENT_SEN(i),statusCode);       
                if(err < 0){
                    printDebug("<eventCapture> addEventQueue() failed\r\n");
                }
            }// -- check sensor alarm -- //
            else
            {
                if(sensor_config[i].alarmEnable)        // . sensor alarm enable . //
                {         
                    // ----------- Check Temperature Alarm ------------------- //                      
                    err =   checkSendDataAlarm(i,&statusCode);
                    res |=  err;                          
                    if(err){                       
                        timestamp_rd    = DS1672_read();                             
                        if(statusCode)
                        {
                            printDebug("\r\n<eventCapture> !!!!!!!!!!! PROBE %d Temperature ERROR !!!!!\r\n", i+1);    
                            printOLED("AL12%1d",i+1);
                        }
                        else
                        {
                            printDebug("\r\n<eventCapture> !!!!!!!!!!! PROBE %d Temperature OK !!!!!\r\n", i+1);      
                            printOLED("AL02%1d",i+1);
                        }                                     
                        err = addEventQueue(timestamp_rd,EVENT_DAT(i),statusCode);       
                        if(err < 0){
                            printDebug("<eventCapture> addEventQueue() failed\r\n");
                        }
                    }
                }// -- sensor.alarmEnable -- //
            }// -- check data alarm -- //   
        }// -- sensor Enable -- //
    }// -- for loop -- //    
                
    
    return res; 
}
/*============================================================*/    

/**************************************************************/
/***************** Realtime Report Management *****************/
/**************************************************************/
/*============================================================*/
int statusReport(void)
{   
    int                     res = 0;
    unsigned long int       timestamp_rd; 
    unsigned char           *param;
    int                     paramLen = 0;                         
    
    unsigned short          deviceState;
    
    switch (status_config.accm)
    {
        case REPORT_SINGLE      :   printDebug("<statusReport> REPORT_SINGLE mode not supported yet\r\n");         
                                    return -1;
        case REPORT_MULTIPLE    :   printDebug("<statusReport> REPORT_MULTIPLE mode not supported yet\r\n");       
                                    return -1;
        case REPORT_ALL         :   param = (unsigned char *) malloc(sizeof(char) * 11);
                                    if(param == NULL){
                                        printDebug("<statusReport> malloc failed (%d bytes)\r\n",paramLen);      
                                        return -1;
                                    }             
                                    
                                    res = getDeviceState(&deviceState);
                                    if(res<0){
                                        printDebug("<statusReport> getDeviceState() failed\r\n");
                                        free ( param ); 
                                        return -1;
                                    }
                                                                                
                                    param[paramLen++] = (deviceState >> 8) & 0x00FF;   
                                    param[paramLen++] = (deviceState & 0x00FF);       
                                    param[paramLen++] = powerSrcStatus;                          
                                    
                                                                    
                                    memcpy(&param[paramLen],&batteryVolt,sizeof(batteryVolt));    
                                    paramLen         += sizeof(batteryVolt);  
                                                                         
                                    param[paramLen++] = dinStatus[0];
                                    param[paramLen++] = dinStatus[1];
                                    param[paramLen++] = doutStatus[0];
                                    param[paramLen++] = doutStatus[1];       
                                                                 
                                    break;
        default                 :   printDebug("<statusReport> status report accm unknown [0x%02X]\r\n",status_config.accm);        
                                    return -1;
    }
    
    timestamp_rd = DS1672_read();
               
    res = ETH_sendStatusReportCmd(timestamp_rd,status_config.accm,param,paramLen);
    if(res < 0)
    {            
        printDebug("<statusReport> ETH_sendStatusReportCmd() failed\r\n");
    }          
                                                                                 
    free ( param ); 
}
/*============================================================*/
int sensorDataReport(void)
{
    int                     count = 0;
    int                     res;                  
    unsigned long int       timestamp;
    unsigned int            channel;
    float                   value;
    
    // -- for debug -- //
    //struct rtc_structure    dt;   
    // --------------- //               
    //printDebug("<sensorDataReport> ------------------------- \r\n");
    
    while (count < MAX_DATA_QUEUE)
    {
        res = fetchDataQueue(&timestamp, &channel, &value);
        if(res < 0)
        {
            break;
        }
        else
        {              
            printDebug("<sensorDataReport> [%ld][%d][%.2f]\r\n",timestamp,channel,value);             
            /*                             
            binary_to_date(timestamp, &dt);  
            printRTC(dt);                   
            printDebug("timestamp [%ld]\r\n",   timestamp);        
            printDebug("channel   [%d]\r\n",    channel);        
            printDebug("value     [%.2f]\r\n",  value);
            */
            
            if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) || (channel > (MAX_SENS_CHANNEL-1)) )
            {
                printDebug("\r\n<sensorDataReport> === ERROR INFOMATION IN TEMP QUEUE ===\r\n");
                continue;
            }                                                                                            
            
            
            res = ETH_sendDataReportCmd(TYP_RLT,timestamp,channel,value);
            if(res < 0)
            {            
                printDebug("<sensorDataReport> ETH_sendDataReportCmd() failed\r\n");    
                // --- Keep in Log --- //
                continue;
            }             
            count++;
        }     
        
    }      
    
    return count;
}
/*============================================================*/
int eventReport(void)
{
    int                     count = 0;
    int                     res;                  
    unsigned long int       timestamp;
    unsigned char           eventID;
    unsigned char           eventStatus;
    
    // -- for debug -- //
    //struct rtc_structure    dt;   
    // --------------- //
    //printDebug("<eventReport> ------------------------- \r\n");
        
    while (count < MAX_EVENT_QUEUE)
    {
        res = fetchEventQueue(&timestamp, &eventID, &eventStatus);
        if(res < 0)
        {
            break;
        }
        else
        {           
            
            printDebug("<eventReport> [%ld][0x%02X][0x%02X]\r\n",timestamp,eventID,eventStatus);      
            /*                             
            binary_to_date(timestamp, &dt);  
            printRTC(dt); 
            */
            
            if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) )
            {
                printDebug("\r\n<eventReport> === ERROR INFOMATION IN TEMP QUEUE ===\r\n");
                continue;
            }                                                                                            
            
            
            res = ETH_sendEventReportCmd(TYP_RLT,timestamp,eventID,eventStatus);
            if(res < 0)
            {            
                printDebug("<eventReport> ETH_sendEventReportCmd() failed\r\n");      
                // --- Keep in Log --- //
                continue;
            }                                      
            count++;
        } 
    }      
    
    return count;
}
/*============================================================*/

/**************************************************************/
/*********************** Log Management ***********************/
/**************************************************************/
/*============================================================*/
int sensorDataSave(void)
{
    FIL                     file;     
    char                    isFileOpen = 0;
    int                     count = 0;
    int                     res;                  
    unsigned long int       timestamp;
    unsigned int            channel;
    float                   value;       
    
    
    // -- for debug -- //
    //struct rtc_structure    dt;   
    // --------------- //
    
    while (count < MAX_DATA_QUEUE)
    {
        res = fetchDataQueue(&timestamp, &channel, &value);
        if(res < 0)
        {
            break;
        }
        else
        {   
            printDebug("<sensorDataSave> time[%ld],ch[%d],val[%.2f]\r\n",timestamp,channel,value);             
            /*                             
            binary_to_date(timestamp, &dt);  
            printRTC(dt); 
            */
            
            if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) || (channel > (MAX_SENS_CHANNEL-1)) )
            {
                printDebug("\r\n<sensorDataSave> === ERROR INFOMATION IN TEMP QUEUE ===\r\n");
                continue;
            }                                                                                            
                  
            //if(sdStatus == OK && logStatus == OK)         
            if(!isFileOpen)
            {
                res = LOG_openFile(&file,LOGFILE_DATA);  
                if(res < 0)
                {
                    printDebug("<sensorDataSave> LOG_openFile(%s) failed, couldn't save log\r\n",LOGFILE_DATA);
                    continue;
                }              
                isFileOpen = 1;
            }                  
            
            res = LOG_insertData(&file,timestamp,channel,value);
            if(res < 0)
            {            
                printDebug("<sensorDataSave> LOG_insertData() failed\r\n"); 
                continue;
            }        
            count++;
        } 
    }      
    
    if(isFileOpen)
    {
        res = LOG_closeFile(&file);  
        if(res < 0)
        {
            printDebug("<sensorDataSave> LOG_closeFile() failed\r\n");
            return -1;
        }              
    }     
                     
    if(count > 0){
        //isDataLogEmpty  = 0;    
        printDebug("<sensorDataSave> Data Log is not empty\r\n");
    }     
    
    return count;
}
/*============================================================*/
int eventSave(void)
{                                      
    FIL                     file;     
    char                    isFileOpen = 0;
    int                     count = 0;
    int                     res;                  
    unsigned long int       timestamp;
    unsigned char           eventID;
    unsigned char           eventStatus;   
    
    // -- for debug -- //
    //struct rtc_structure    dt;   
    // --------------- //
    
    while (count < MAX_EVENT_QUEUE)
    {
        res = fetchEventQueue(&timestamp, &eventID, &eventStatus);
        if(res < 0)
        {
            break;
        }
        else
        {     
            printDebug("<eventSave> time[%ld],id[0x%02X],stat[0x%02X]\r\n",timestamp,eventID,eventStatus);      
            /*                             
            binary_to_date(timestamp, &dt);  
            printRTC(dt); 
            */
                 
            if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) )
            {
                printDebug("\r\n<eventSave> === ERROR INFOMATION IN TEMP QUEUE ===\r\n");
                continue;
            }                                                                                            
            
            //if(sdStatus == OK && logStatus == OK)         
            if(!isFileOpen)
            {
                res = LOG_openFile(&file,LOGFILE_EVENT);  
                if(res < 0)
                {
                    printDebug("<eventSave> LOG_openFile(%s) failed, couldn't save log\r\n",LOGFILE_EVENT);
                    continue;
                }              
                isFileOpen = 1;
            }  
            
            res = LOG_insertEvent(&file,timestamp,eventID,eventStatus);
            if(res < 0)
            {            
                printDebug("<eventSave> LOG_insertEvent() failed\r\n");      
                continue;
            }                                      
            count++;
        } 
    }   
    
    if(isFileOpen)
    {
        res = LOG_closeFile(&file);  
        if(res < 0)
        {
            printDebug("<eventSave> LOG_closeFile() failed\r\n");
            return -1;
        }              
    }        
                     
    if(count > 0){
        //isEventLogEmpty    = 0;       
        printDebug("<eventSave> Event Log is not empty\r\n");
    }
    
    return count;
}
/*============================================================*/
int logReport(void)
{
    FIL                     file;              
    int                     res;      
    unsigned long int       index;    
    unsigned long int       timestamp;
    unsigned int            channel;
    float                   value;   
    unsigned char           id;
    unsigned char           status;        
                    
    // -- send data log if any exist -- //       
    if((!isDataLogEmpty) && (logDataWait.frameID == 0x00))   // -- if no logDataWaitFrameID, look for some data log to send -- //          
    {                                         
        printDebug("<logReport> Check Data Log Exist\r\n");
        res = LOG_openFile(&file,LOGFILE_DATA);  
        if(res < 0)
        {
            printDebug("<logReport> LOG_openFile(%s) failed\r\n",LOGFILE_DATA);
        }                                                                                              
        else
        {
            res = LOG_readData(&file,&timestamp,&channel,&value,&index);
            if(res < 0)
            {
                printDebug("<logReport> LOG_readData(%s) failed\r\n",LOGFILE_DATA);      
            }  
            else if(res == 0)
            {                                                                
                //isDataLogEmpty      = 1;           
                printDebug("<logReport> LOG_readData() Log empty\r\n");
            }                               
            else
            {
                if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) || (channel > (MAX_SENS_CHANNEL-1)) )
                {
                    printDebug("\r\n<logReport> === ERROR INFOMATION IN %s ===\r\n",LOGFILE_DATA);
                }                        
                else
                {                                                                
                    printDebug("<logReport> DataLog found time[%ld],ch[%d],val[%.2f]\r\n",timestamp,channel,value);    
                    res = ETH_sendDataReportCmd(TYP_LOG,timestamp,channel,value);
                    if(res < 0)
                    {                                            
                        logDataWait.frameID    = 0;
                        logDataWait.logID      = 0;
                        printDebug("<logReport> ETH_sendDataReportCmd() failed\r\n");
                    }            
                    else
                    {                     
                        logDataWait.logID       = index;             
                        printDebug("<logReport> DataLog sent frameID[0x%02X] logID[0x%02X]\r\n",logDataWait.frameID,logDataWait.logID);
                    }
                }
            }             
            
            res = LOG_closeFile(&file);  
            if(res < 0)
            {
                printDebug("<logReport> LOG_closeFile() failed\r\n");
            }      
        }                    
    }   
    
    // -- send event log if any exist -- //                                   
    if((!isEventLogEmpty) && (logEventWait.frameID == 0x00))  // -- if no logEventWaitFrameID, look for some event log to send -- //          
    {                                          
        printDebug("<logReport> Check Event Log Exist\r\n");
        res = LOG_openFile(&file,LOGFILE_EVENT);  
        if(res < 0)
        {
            printDebug("<logReport> LOG_openFile(%s) failed\r\n",LOGFILE_EVENT);
        }                                                                                              
        else
        {
            res = LOG_readEvent(&file,&timestamp,&id,&status,&index);
            if(res < 0)
            {
                printDebug("<logReport> LOG_readEvent() failed\r\n");  
            }      
            else if(res == 0)
            {                                                                       
                //isEventLogEmpty     = 1;
                printDebug("<logReport> LOG_readEvent() Log empty\r\n");
            }                             
            else
            {                                                                       
                if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) )
                {
                    printDebug("\r\n<logReport> === ERROR INFOMATION IN %s ===\r\n",LOGFILE_EVENT);
                }                        
                else
                {                                                                   
                    printDebug("<logReport> EventLog found time[%ld],id[0x%02X],stat[0x%02X]\r\n",timestamp,id,status);    
                    res = ETH_sendEventReportCmd(TYP_LOG,timestamp,id,status);
                    if(res < 0)
                    {                                                
                        logEventWait.frameID    = 0;
                        logEventWait.logID      = 0;
                        printDebug("<logReport> ETH_sendEventReportCmd() failed\r\n");
                    }            
                    else
                    {   
                        logEventWait.logID      = index;                                                  
                        printDebug("<logReport> EventLog sent frameID[0x%02X] logID[0x%02X]\r\n",logEventWait.frameID,logEventWait.logID);
                    }
                }   
            }       
            
            res = LOG_closeFile(&file);  
            if(res < 0)
            {
                printDebug("<logReport> LOG_closeFile(%s) failed\r\n",LOGFILE_EVENT);
            }         
        }                    
    }                
    
    return 0;
}
/*============================================================*/

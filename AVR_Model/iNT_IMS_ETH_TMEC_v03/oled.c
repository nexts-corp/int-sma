#include <stdio.h>
#include <stdlib.h> 
#include <string.h>                                                           
#include <delay.h>
#include "oled.h"
#include "ds1672.h"
#include "status.h"
#include "alarm.h"
#include "sensor.h"
#include "config.h"
#include "timer.h"
#include "debug.h"  

/**************************************************************/
/***************** OLED Service Page Function *****************/
/**************************************************************/
/*============================================================*/
void OLED_showDateTime(int timezone)
{              
    struct  rtc_structure   currentTime;  

    RTC_getDateTime(&currentTime,timezone);  
    printOLED("DT%02d-%02d-%04d",currentTime.date,currentTime.month,currentTime.year); 
    printOLED("TM%02d:%02d",currentTime.hour,currentTime.min);    
    
}
/*============================================================*/
void OLED_showSensorValue(void)
{
    int i;
                   
    for(i=0;i<MAX_SENS_CHANNEL;i++)
    {      
                       
        if( operating_config.sensorEnable[i] ){
                             
            switch (sensor[i].status)
            {
                case SENS_NORMAL            :   printOLED("DV%1d%.1f",i+1,sensor[i].value);
                                                break;
                case SENS_ERR_LOWER_BOUND   :   printOLED("DV%1dERRLB",i+1);
                                                break;
                case SENS_ERR_UPPER_BOUND   :   printOLED("DV%1dERRUB",i+1);
                                                break;
                case SENS_ERR_DISCONNECT    :   printOLED("DV%1dERRNC",i+1);
                                                break;
                case SENS_ERR_SHORT         :   printOLED("DV%1dERRSH",i+1);
                                                break;     
                case SENS_ERR_UNKNOWN       :   printOLED("DV%1dDETECTING",i+1);
                                                break;
                default                     :   printDebug("<OLED_showSensorValue> sensor[%d].status unknown [0x%02X]\r\n",i+1,sensor[i].status);
                                                return;
            }
        }
        else{                    
            printOLED("DV%1dN/A",i+1);                 
        }
    }  
        
    return;
    
}
/*============================================================*/ 
void OLED_showPowerStatus(char pwrSrc,float batLevel)
{

    if(pwrSrc == PWR_NORMAL){                               
        printOLED("CG1");
    }                
    else{                          
        //printDebug("<OLED_showPowerStatus> batt = %f\r\n",batLevel);                                  
        if(batLevel < 3.0){
            printOLED("BL0");       
        }
        else if((batLevel >= 3.0) && (batLevel < 3.3)){    
            printOLED("BL1");       
        }
        else if((batLevel >= 3.3) && (batLevel < 3.7)){     
            printOLED("BL2");       
        }
        else if((batLevel >= 3.7) && (batLevel < 4.0)){      
            printOLED("BL3");       
        }
        else if(batLevel >= 4.0){    
            printOLED("BL4");  
        }   
    }
}
/*============================================================*/
void OLED_showInputStatus(int ch,char state)
{
    if((ch < 0) || (ch >= 8)){  
        printDebug("<OLED_showInputStatus> ERR: ch value out of range.!!\r\n");
        return;
    }                                           
    
    if(din_config[ch].channelEnable != 0x01){
        return;    
    }                   
    
    if((state != DIN_FALLING) && (state != DIN_RISING)){
        //printDebug("<OLED_showInputStatus> ERR: state invalid [0x%02X]!!\r\n",state);
        return;
    }
    
    printOLED("IP%1d%1d",ch,state);
}
/*============================================================*/  
void OLED_showOutputStatus(int ch,char state)
{
    if((ch < 0) || (ch >= 8)){  
        printDebug("<OLED_showOutputStatus> ERR: ch value out of range.!!\r\n");
        return;                           
    }                                        
    
    if(dout_config[ch].channelEnable != 0x01){
        return;    
    }                                       
    
    if((state != LOW) && (state != HIGH)){
        //printDebug("<OLED_showInputStatus> ERR: state invalid [0x%02X]!!\r\n",state);
        return;
    }
    
    printOLED("OP%1d%1d",ch,state);       
}
/*============================================================*/
void OLED_showServerCommStatus(char state)
{
    printOLED("SC%1d",state);  
}
/*============================================================*/
void OLED_showLanStatus(char state)
{
    printOLED("LC%1d",state);
}
/*============================================================*/
void OLED_showBuzzerStatus(char state)
{
    printOLED("BZ%1d",state);
}
/*============================================================*/
void OLED_showMemoryStatus(char state)
{
    printOLED("MS%1d",state);
}
/*============================================================*/
void OLED_showAlarmStatus(void)
{                         
    int     i=0;
    char    flagSens[5],flagData[5],flagBatt,flagMemo;
                   
    for(i=0;i<MAX_SENS_CHANNEL;i++)
    {                             
        if(operating_config.sensorEnable[i]){   

            flagSens[i]         = sensorAlarm[i] ? 1:0;   
                
            if(sensor_config[i].alarmEnable){     
                flagData[i]     = dataAlarm[i] ? 1:0;
            }                                        
            else{
                flagData[i]     = 0;
            }   
        }                                          
        else{
            flagSens[i]         = 0;
            flagData[i]         = 0;
        }                           
        
    }               
     
    if(env_config.batteryLevel > 0){
        flagBatt                = batteryAlarm ? 1:0;
    }                                 
    else{
        flagBatt                = 0;
    }
     
    if(env_config.memoryMin > 0){    
        flagMemo                = memoryAlarm ? 1:0;
    }                                 
    else{
        flagMemo                = 0;
    }   
        
    printOLED("AL%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d",
        flagSens[0],flagSens[1],flagSens[2],flagSens[3],flagSens[4],     
        flagData[0],flagData[1],flagData[2],flagData[3],flagData[4], 
        flagBatt,flagMemo);            
    
    return;
}
/*============================================================*/  

/**************************************************************/
/*************** OLED Low Level Communication *****************/
/**************************************************************/
/*============================================================*/
void printOLED(flash char *fmtstr, ...)
{                          
    TIMER   timeout;
              
    char strcmd[60];
    
    va_list argptr; 

    va_start(argptr, fmtstr);
    vsprintf(strcmd,fmtstr,argptr);
    va_end(argptr);         
                             
//    if ( (strstr(strcmd,"DV") != NULL) ){
//        printDebug("\r\n--%s--\r\n",strcmd);
//    }   
    
    flag_oled_ack = 0;
    
    putchar2(0x01);   
    send_uart(2,strcmd);
    putchar2(0x04);                                      
        
    TIMER_setTimer(&timeout, 3);   
    
    while( (!flag_oled_ack) && !TIMER_checkTimerExceed(timeout));                       
                                        
    if(!flag_oled_ack){
        printDebug("\r\n!!!! OLED Error !!!!\r\n");
    }  
    
}
/*============================================================*/

/**************************************************************/
/****************** OLED Hardare Management *******************/
/**************************************************************/
/*============================================================*/
void OLED_reset(void)
{
    OLED_RESET = 0;            // Active SOLED Reset
    delay_ms(1);
    OLED_RESET = 1;            // Dective SOLED Reset for start use.
    delay_ms(1);
    
}
/*============================================================*/
void init_oled(void)
{   
    printDebug("Initializing OLED.. ");
    flag_oled_ack = 0;
    OLED_reset();                                                                
    printDebug("OK\r\n");
}
/*============================================================*/


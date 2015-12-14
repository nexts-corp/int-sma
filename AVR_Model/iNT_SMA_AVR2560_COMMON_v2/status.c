#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "status.h"
//#include "config.h"
//#include "timer.h"
//#include "debug.h"
//                                                    
///*eeprom*/ char sensorStatus[MAX_SENS_CHANNEL]                  = {SENS_NORMAL,SENS_NORMAL,SENS_NORMAL,SENS_NORMAL,SENS_NORMAL};
///*eeprom*/ char dataStatus[MAX_SENS_CHANNEL]                    = {DATA_NORMAL,DATA_NORMAL,DATA_NORMAL,DATA_NORMAL,DATA_NORMAL};
//char mtncStatus                                             = MTNC_NORMAL;
//char powerSrcStatus                                         = PWR_NORMAL;
//char batteryStatus                                          = BATT_NORMAL;
//char dinStatus[USE_DIN]                                     = {UNKNOWN,UNKNOWN};
//char doutStatus[USE_DIN]                                    = {UNKNOWN,UNKNOWN};
//char serverCommStatus                                       = UNKNOWN; 
//char lanStatus                                              = 0;
//char memoryStatus                                           = MEM_NORMAL;
//    
//float batteryVolt                                           = 0.0;
//
///*
//char dataLowerStatus[MAX_SENS_CHANNEL]           = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//char dataLowerDelayStatus[MAX_SENS_CHANNEL]      = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//char dataLowerExtremeStatus[MAX_SENS_CHANNEL]    = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//char dataUpperStatus[MAX_SENS_CHANNEL]           = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//char dataUpperDelayStatus[MAX_SENS_CHANNEL]      = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//char dataUpperExtremeStatus[MAX_SENS_CHANNEL]    = {UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN};
//*/
//
///****
//*  "sensor.h" -> struct sens_val_st sensor[]
//*
//****/
///*============================================================*/
//int updateMIPStatus(void)
//{
//    // -- depend on menu -- //
//    return 0;
//}
///*============================================================*/
//int updateBatteryStatus(void)
//{
//    static TIMER    t               = 0;               
//    static char     t_flag          = 0;             
//    static char     lastCheck       = 0;
//    char            current_status  = 0;
//    
//    if(MAIN_POWER_CONNECT){
//        powerSrcStatus      =   PWR_NORMAL;        
//        batteryStatus       =   BATT_NORMAL;
//        batteryVolt         =   4.2;       
//        return 0;
//    }   
//    else{
//        powerSrcStatus      =   PWR_DOWN; 
//        batteryVolt         =   readBattery();   
//        
//        if(env_config.batteryLevel == 0)    // -- battery alarm disable --> always BATT_NORMAL -- //
//        {
//            batteryStatus = BATT_NORMAL;
//        }
//        else
//        {          
//            if(batteryVolt < env_config.batteryLevel)
//            {
//                current_status      = BATT_LOW;
//            }
//            else
//            {                             
//                current_status      = BATT_NORMAL;
//            }                     
//            
//            if(lastCheck != current_status)
//            {
//                t_flag              = 0;     
//                lastCheck           = current_status;
//            }                                   
//                
//            if(!t_flag)
//            {                 
//                TIMER_setTimer(&t,10);   
//                t_flag              = 1;         
//            }
//            else
//            {
//                if(TIMER_checkTimerExceed(t))
//                {                        
//                    batteryStatus   = current_status;
//                    t_flag          = 0;
//                }            
//            }   
//        }// -- battery alarm enable -- //       
//        return 1;               
//    }// -- main power disconnect -- //                                                   
//    
//    return 0;
//}
///*============================================================*/
//int updateCommStatus(void)
//{
//    int res;
//    
//    if(!IO_LAN_CONNECT){           
//        lanStatus                       = COMM_LAN_DISCONNECT;
//          
//        serverCommStatus                = UNKNOWN;
//        
//        commLostTimer                   = 0;                // -- re-initial -- //           
//        return 1;
//    }
//    else{                      
//        lanStatus                       = COMM_LAN_CONNECT;     
//                                                                               
//        if((commLostTimer != 0xFFFFFFFF) && (commLostTimer != 0)){     
//            printDebug("<updateCommStatus> LostTimer time left = %ld(%lX)\r\n",(commLostTimer-baseCounter),(commLostTimer-baseCounter));
//        }           
//
//        res = TIMER_checkTimerExceed(commLostTimer);                           
//        if(res < 0){                         
//            serverCommStatus            = UNKNOWN;              // -- unknown -- //
//            return 0;
//        }   
//        else{
//            if(res){                                    
//                if(serverCommStatus != COMM_LOST){                                      
//                    printDebug("<updateCommStatus> Server Down!!!\r\n");          
//                }
//                serverCommStatus        = COMM_LOST;            // -- disconnect -- //         
//                return 1;
//            }                           
//            else{                                       
//                if(serverCommStatus != COMM_NORMAL){                                           
//                    printDebug("<updateCommStatus> Server Up!!!\r\n");         
//                }                                                        
//                serverCommStatus        = COMM_NORMAL;          // -- connect -- //               
//                return 0;
//            }    
//        }                              
//    }                                                               
//}
///*============================================================*/
//int updateMemoryStatus(void)
//{
////    if(env_config.memoryMin == 0)    // -- memory alarm disable --> always MEM_NORMAL -- //
////    {                                   
////            memoryStatus        = MEM_NORMAL; 
////    }
////    else
////    {                                     
//        if(sdFreeSpace == 0)
//        {
//            memoryStatus    = MEM_ERROR;    
//        }
//        else if((int)(sdFreeSpace/1024) < env_config.memoryMin)
//        {
//            memoryStatus    = MEM_LOW;
//        }
//        else
//        {                             
//            memoryStatus    = MEM_NORMAL;                  
//        }                     
////    }// -- memory alarm enable -- //
//                        
//    if( memoryStatus != MEM_NORMAL ){
//        return 1;
//    }
//    else{
//        return 0;
//    }
//}
///*============================================================*/
//int updateSensorStatus(int ch)
//{             
//    if((ch<0) || (ch>=MAX_SENS_CHANNEL)){
//        printDebug("<checkProbeAlarm> sensor channel out of range (%d)\r\n",ch);      
//        return 0;
//    }                                                                           
//                         
//    sensorStatus[ch] = sensor[ch].status;
//                                         
//    if(sensorStatus[ch] == SENS_ERR_UNKNOWN){
//        return 0;
//    }
//    else if(sensorStatus[ch] != SENS_NORMAL){ 
//        //printDebug("<checkProbeAlarm> Channel %d ERR\r\n",ch);  
//        return 1;
//    }          
//    else{                                         
//        //printDebug("<checkProbeAlarm> Channel %d OK\r\n",ch);
//        return 0;                             
//    }                
//}
///*============================================================*/
//int updateDataStatus(int ch)
//{
//    static TIMER t[MAX_SENS_CHANNEL];
//    static char flag_lower[MAX_SENS_CHANNEL] = {0,0,0,0,0};  
//    static char flag_upper[MAX_SENS_CHANNEL] = {0,0,0,0,0}; 
//    int res = 0;
//    
//    if((ch<0) || (ch>=MAX_SENS_CHANNEL)){
//        printDebug("<updateDataStatus> ABORT: sensor channel out of range (%d)\r\n",ch);     
//        return 0;
//    }               
//     
//    if(sensor[ch].status != SENS_NORMAL){                                      
//        //printDebug("<updateDataStatus> ABORT: sensor (%d) error\r\n",ch);
//        return 0;
//    }   
//
//    if(sensor[ch].value < sensor_config[ch].lowerLimit){
//                       
//        if(!flag_lower[ch])
//        {
//            TIMER_setTimer(&t[ch],sensor_config[ch].lowerDelay);
//            flag_lower[ch] = 1;
//        }
//        else
//        {
//            if(TIMER_checkTimerExceed(t[ch]))
//            {                        
//                dataStatus[ch]              |=  DATA_LOWER_LIMIT;
//                //printDebug("<updateDataStatus> Probe %d lowerDelayLimit[%02X]\r\n", ch+1, dataStatus[ch]);   /////
//            } 
//        }                                  
//        //dataStatus[ch]                      |=  DATA_LOWER_LIMIT;                                                                                         
//        res |= 1;                                                                            
//                    
//        //printDebug("<updateDataStatus> Probe %d lowerLimit[%02X] -- sensor[%f] vs config[%f]\r\n",
//        //                ch+1,dataStatus[ch],sensor[ch].value,sensor_config[ch].lowerLimit);
//    }  
//    else{                                                          
//        flag_lower[ch] = 0;
//        dataStatus[ch]                      &=  ~DATA_LOWER_LIMIT;
//        //dataStatus[ch]                      &=  ~DATA_LOWER_DELAY;
//    }           
//                             
//    if(sensor[ch].value < sensor_config[ch].lowerExtreme)
//    {   
//        dataStatus[ch]                      |=  DATA_LOWER_EXTREME;
//        res |= 1;                                                         
//                    
//        //printDebug("<updateDataStatus> Probe %d lowerExtreme[%02X] -- sensor[%f] vs config[%f]\r\n",
//        //                ch+1,dataStatus[ch],sensor[ch].value,sensor_config[ch].lowerExtreme);
//    }                                                  
//    else
//    {                                              
//        dataStatus[ch]                      &=  ~DATA_LOWER_EXTREME;
//    } 
//
//    if(sensor[ch].value > sensor_config[ch].upperLimit)
//    {      
//        if(!flag_upper[ch])
//        {
//            TIMER_setTimer(&t[ch],sensor_config[ch].upperDelay);   
//            flag_upper[ch] = 1;
//        }                                           
//        else
//        {
//            if(TIMER_checkTimerExceed(t[ch]))
//            {                   
//                dataStatus[ch]              |=  DATA_UPPER_LIMIT;
//            }
//        }
//        //dataStatus[ch]                      |=  DATA_UPPER_LIMIT;                                                                              
//        res |= 1;                                                           
//                    
//        //printDebug("<updateDataStatus> Probe %d upperLimit -- sensor[%f] vs config[%f]\r\n",
//        //                ch+1,sensor[ch].value,sensor_config[ch].upperLimit);
//    }                              
//    else
//    {                                 
//        flag_upper[ch] = 0;                
//        dataStatus[ch]                      &=  ~DATA_UPPER_LIMIT;
//        //dataStatus[ch]                      &=  ~DATA_UPPER_DELAY;
//    } 
//                                       
//    if(sensor[ch].value > sensor_config[ch].upperExtreme)
//    {   
//        dataStatus[ch]                      |=  DATA_UPPER_EXTREME;    
//        res |= 1;                                                          
//                    
//        //printDebug("<updateDataStatus> Probe %d upperExtreme -- sensor[%f] vs config[%f]\r\n",
//        //                ch+1,sensor[ch].value,sensor_config[ch].upperExtreme);
//    }                                                  
//    else
//    {                                                   
//        dataStatus[ch]                      &=  ~DATA_UPPER_EXTREME;
//    }               
//                           
//    return res;
//}
///*============================================================*/
//int updateDinStatus(void)
//{
//    static TIMER    t[USE_DIN]                  = {0,0};
//    static char     lastDinCheck[USE_DIN]       = {UNKNOWN,UNKNOWN};   // -- initial with unuse value -- //  
//    static char     steadyState[USE_DIN]        = {0,0};
//    char            currentState                = UNKNOWN;   
//    int             ch                          = 0;      
//    int             ret                         = 0; 
//               
//    for(ch=0;ch<USE_DIN;ch++)
//    {                                     
//        if(din_config[ch].channelEnable == 0x01)           
//        {
//            switch(ch){
//                case 0  : currentState = DIN1;  break;
//                case 1  : currentState = DIN2;  break;       
//                default : printDebug("<updateDinStatus> channel[%d] out of range\r\n",ch); return 0;
//            }
//                 
//            if(currentState != lastDinCheck[ch])   
//            {   
//                TIMER_setTimer(&t[ch],1);                                     
//                lastDinCheck[ch]    = currentState;                                   
//                steadyState[ch]     = 0;
//            }
//            else if(!steadyState[ch])
//            {
//                if(TIMER_checkTimerExceed(t[ch])){  
//
//                    steadyState[ch] = 1;            
//                    
//                    if(currentState){
//                        dinStatus[ch]   = DIN_RISING;
//                    }
//                    else{
//                        dinStatus[ch]   = DIN_FALLING;
//                    }               
//                                          
//                    ret += 1; 
//                }                     
//                else{
//                    steadyState[ch] = 0;
//                }
//            }  
//        }       
//    }                  
//                                    
//    return ret;
//}                                          
///*============================================================*/
//int updateDoutStatus(void)
//{
//    if(d_out[0]){   doutStatus[0]        = DOUT_HIGH; }
//    else{           doutStatus[0]        = DOUT_LOW; }
//    
//    if(d_out[1]){   doutStatus[1]        = DOUT_HIGH; }
//    else{           doutStatus[1]        = DOUT_LOW; }                             
//         
//    return 0;
//}
///*============================================================*/
//int getDeviceState(unsigned short *state)
//{
//    int     i   = 0;
//    
//    *state      = 0x00;
//                       
//    if(isDeviceConfig()){
//        *state |= (1 << 15);
//    }                      
//    else{
//        *state &= ~(1 << 15);
//    }                       
//    
//    if(operating_config.deviceEnable){      
//        *state |= (1 << 14);
//    }                                
//    else{
//        *state &= ~(1 << 14);
//    }                       
//    
//    if(operating_config.mode == MODE_REALTIME){       
//        *state |= (2 << 10);
//    }
//    else{       
//        *state |= (1 << 10);
//    }              
//    
//    if(operating_config.alarmEnable){
//        *state |= (2 << 8);
//    }
//    else{
//        *state |= (1 << 8);
//    }              
//    
//    for(i=0;i<MAX_SENS_CHANNEL;i++)                     
//    {                    
//        if(operating_config.sensorEnable[i]){        
//            *state |= (1 << (4-i) );
//        }
//        else{        
//            *state &= ~(1 << (4-i) );
//        }
//    }                               
//    
//    printDebug("<getDeviceState> state = [0x%04X]\r\n",*state);
//    
//    return 0;
//}
/*============================================================*/

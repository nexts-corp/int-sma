#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <delay.h>
#include "sensor.h"
#include "config.h"
#include "timer.h"
#include "gaussian.h"
#include "debug.h"
#include "ad595.h"
                                                                     
struct sens_val_st      sensor[5];

/******************
Y = mX + c
temp = (epFactor_B * adc) + epFactor_C
******************/                                
                                                                                              
eeprom float            epFactor_A[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };  
eeprom float            epFactor_B[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };
eeprom float            epFactor_C[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };     

eeprom char             epIsPointSet1[5]        = { 0,      0,      0,      0,      0       };
eeprom float            epADCRef1[5]            = { 0.0,    0.0,    0.0,    0.0,    0.0     };
eeprom float            epTempRef1[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };
                                      
eeprom char             epIsPointSet2[5]        = { 0,      0,      0,      0,      0       };
eeprom float            epADCRef2[5]            = { 0.0,    0.0,    0.0,    0.0,    0.0     };
eeprom float            epTempRef2[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };
                                      
eeprom char             epIsPointSet3[5]        = { 0,      0,      0,      0,      0       };
eeprom float            epADCRef3[5]            = { 0.0,    0.0,    0.0,    0.0,    0.0     };
eeprom float            epTempRef3[5]           = { 0.0,    0.0,    0.0,    0.0,    0.0     };

eeprom char             epCalType[5]            = {CAL_DEFAULT,CAL_DEFAULT,CAL_DEFAULT,CAL_DEFAULT,CAL_DEFAULT};  

flash float             epTMECTempSlope         = -0.1114;
flash float             epTMECTempOffset        = 63.73;  

flash float             epPT100TempSlope         = 1.0938;
flash float             epPT100TempOffset        = -208.42;       
                                                                                                
//flash float             epPT100Temp_A           = 0.00005;
//flash float             epPT100Temp_B           = 0.1308;
//flash float             epPT100Temp_C           = -221.71; 

flash float             epPT100Temp_A           = 0.000633;
flash float             epPT100Temp_B           = 0.162124;
flash float             epPT100Temp_C           = -180.886306; 

// Parameter for sensor pt100
flash float             tempTable[15]           = {-80.0, -70.0, -60.0, -50.0, -40.0, -30.0, -20.0, -10.0, 0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0};
//flash float             voltTable[13]           = {0.7276, 0.7734, 0.8176, 0.8604, 0.9019, 0.9420, 0.9810, 1.0188, 1.0556, 1.0912, 1.1259, 1.1596, 1.1924};
flash float             adcTable[15]            = {290.7750, 309.0467, 326.7189, 343.8240, 360.3923, 376.4502, 392.0239, 
                                                    407.1367, 421.8099, 436.0630, 449.9139, 463.3793, 476.4748, 489.2156, 501.6149};

float iTCVoltData[5] = {0.0,0.0,0.0,0.0,0.0};
float iTCADCData[5] = {0.0,0.0,0.0,0.0,0.0};    
/*============================================================*/
void iSensorGetADCValue(){
    long viADCbuffer = 0;
    unsigned int viADCSmoothValue = 0; 
    int i = 0,j = 0;  
    viADCbuffer = 0;
    printDebug("[iSensorGetValue]data[");
    for(i=0;i<5;i++){
        #asm("wdr")
        viADCbuffer = 0;
        for(j=0;j<30;j++){
            viADCbuffer += read_adc(8+i);
        }
        viADCSmoothValue = (unsigned int)(viADCbuffer/30);
        if(i==4){
           printDebug("ch%d: %04d ",i+1,viADCSmoothValue); 
        }else{
           printDebug("ch%d: %04d, ",i+1,viADCSmoothValue); 
        }
        
    }  
    printDebug("]\r\n"); 
    return;
}

void updateSensorADC(int ch)
{
    int adc_raw                                     = 0;        
        
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){     
        adc_raw     = read_adc(8);                             
    }
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){     
        adc_raw     = read_adc(8+ch);                                     
    }
        
    if( fabs(adcData[ch] - adc_raw) < 10){                                                       
        adcData[ch] = (float)((adcData[ch] * 9.9) + (float)(adc_raw * 0.1))/10;           // Temp Sensor Low-Pass Filter 
        //printDebug("<updateSensorADC>        < 10\r\n");  
    }
    else if( fabs(adcData[ch] - adc_raw) < 30){                                                       
        adcData[ch] = (float)((adcData[ch] * 9.5) + (float)(adc_raw * 0.5))/10;           // Temp Sensor Low-Pass Filter  
        //printDebug("<updateSensorADC>                < 30\r\n");   
    }
    else if( fabs(adcData[ch] - adc_raw) < 50){                                                       
        adcData[ch] = (float)((adcData[ch] * 8.5) + (float)(adc_raw * 1.5))/10;           // Temp Sensor Low-Pass Filter  
        //printDebug("<updateSensorADC>                            < 50\r\n");   
    }
    else if( fabs(adcData[ch] - adc_raw) < 100){                                                       
        adcData[ch] = (float)((adcData[ch] * 7.0) + (float)(adc_raw * 3.0))/10;           // Temp Sensor Low-Pass Filter   
        //printDebug("<updateSensorADC>                                    < 100\r\n");  
    }                                           
    else{
        adcData[ch] = (float)((adcData[ch] * 5.0) + (float)(adc_raw * 5.0))/10;           // Temp Sensor Low-Pass Filter   
        //printDebug("<updateSensorADC>                                            >= 100\r\n");  
    }      
    
    return;    
}
/*============================================================*/
int sensorRead(int ch, struct sens_val_st *sensor)
{                  
    static TIMER t[MAX_SENS_CHANNEL]                = {0,0,0,0,0};
    static char lastCheck[MAX_SENS_CHANNEL]         = {0xFF,0xFF,0xFF,0xFF,0xFF};   // -- initial with unuse value -- // 
    static char steadyState[MAX_SENS_CHANNEL]       = {0,0,0,0,0};
    char channelConnect                             = 0;  
    float x_1 = 0.0;   
    float x_2 = 0.0; 
    float y_1 = 0.0; 
    float y_2 = 0.0; 
    
    const float  viZeroSet   = 1.667;   
    
    unsigned long viADCbuffer;   
    int i = 0; 
    float offset = 0.0;
    float sensorRef1 = 0.0,sensorRef2 = 0.0,sensorRef3 = 0.0;
    float stdMeter1 = 0.0,stdMeter2 = 0.0,stdMeter3 = 0.0;        //standrad  meter
    float lowRange = 0.0;  
    float stdMeterRange = 0.0; 
    float factorAA = 0.0,factorBB = 0.0,factorCC = 0.0;      
    float viSensorTempValue = 0.0; 
    float viRawVoltValue = 0.0;   
    float viRiaseUpVoltValue = 0.0;

    if( (ch<0) || (ch>=MAX_SENS_CHANNEL) ){
        printDebug("<sensorRead> ERR: ch value out of range\r\n");
        return -3;
    }            
               
    // ----- Check Sensor Status (Connected-Disconnected) ----- //                                                          
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){    
    /*bongkot comment*/
        SENSOR_SELECT(ch);
        delay_ms(20);                                               // -- for multiplex stable -- // 
        channelConnect = SENSOR_CONNECT;    
        
        //updateSensorADC(ch);
//        if(adcData[ch] < TMC_DISCON_LIMIT){
//            channelConnect  = SENS_ERR_DISCONNECT;
//        }    
//        else{                                
//            channelConnect  = SENS_NORMAL;
//        } 
         viADCbuffer = 0;  
         for(i=0;i<20;i++){
            viADCbuffer += (unsigned long)(read_adc(8+ch));
         }
         viRawVoltValue = iAD595ADCToVolt(viADCbuffer,20.0);    
         //viSensorTempValue = iAD595ADCToVolt(viADCbuffer,20.0);  
         iTCVoltData[ch] = (float)(viRawVoltValue+0.213);
         iTCADCData[ch]  = (float)(viADCbuffer/20.0);
         
         printDebug("[sensorRead1]ch1[%04.2f|%0.2f],ch2[%04.2f|%0.2f],ch3[%04.2f|%0.2f],ch4[%04.2f|%0.2f],ch5[%04.2f|%0.2f]\r\n",iTCADCData[0],iTCVoltData[0],iTCADCData[1],iTCVoltData[1],iTCADCData[2],iTCVoltData[2],iTCADCData[3],iTCVoltData[3],iTCADCData[4],iTCVoltData[4]);
        
         
//         if(viSensorTempValue < TMCP_SHORT_LIMIT){
//             //channelConnect  = SENS_ERR_SHORT;  
//             channelConnect  = SENS_ERR_DISCONNECT;
//         }else if(viSensorTempValue > TMCP_OPEN_LIMIT){
//             channelConnect  = SENS_ERR_DISCONNECT;
//         }else{
//             channelConnect  = SENS_NORMAL;
//         }  

         viRiaseUpVoltValue = (float)(iTCVoltData[ch]-viZeroSet);
         iTCVoltData[ch] = viRiaseUpVoltValue; 
         printDebug("[sensorRead2]ch1[%04.2f|%0.2f],ch2[%04.2f|%0.2f],ch3[%04.2f|%0.2f],ch4[%04.2f|%0.2f],ch5[%04.2f|%0.2f]\r\n",iTCADCData[0],iTCVoltData[0],iTCADCData[1],iTCVoltData[1],iTCADCData[2],iTCVoltData[2],iTCADCData[3],iTCVoltData[3],iTCADCData[4],iTCVoltData[4]);
         if(viRiaseUpVoltValue < TMCP_SHORT_LIMIT){
             //channelConnect  = SENS_ERR_SHORT;  
             channelConnect  = SENS_ERR_DISCONNECT;
         }else if(viRiaseUpVoltValue > TMCP_OPEN_LIMIT){
             channelConnect  = SENS_ERR_DISCONNECT;
         }else{
             channelConnect  = SENS_NORMAL;
         }
                                             
         
        
        /*bongkot test*/
        //channelConnect  = SENS_NORMAL;
        //printDebug("SENSOR:NORMAL\r\n");
        
                                                       
    }                
    else if(TEMP_SEL == TEMP_TMEC){       
        
        updateSensorADC(ch);                    // -- update adcData[ch] -- //
        
        if(adcData[ch] < TMEC_SHORT_LIMIT){
            channelConnect  = SENS_ERR_SHORT;
        }
        else if(adcData[ch] > TMEC_OPEN_LIMIT){  
            channelConnect  = SENS_ERR_DISCONNECT;
        }     
        else{                                
            channelConnect  = SENS_NORMAL;
        }
    }       
    else if(TEMP_SEL == TEMP_PT100){
                            
        updateSensorADC(ch);                    // -- update adcData[ch] -- //
        
        if(adcData[ch] < PT100_SHORT_LIMIT){
            channelConnect  = SENS_ERR_SHORT;
        }
        else if(adcData[ch] > PT100_OPEN_LIMIT){  
            channelConnect  = SENS_ERR_DISCONNECT;
        }     
        else{                                
            channelConnect  = SENS_NORMAL;
        }
    }
    
    /*bongkot test*/ 
    //sensor->status  = SENS_NORMAL; 
    //lastCheck[ch] = channelConnect;
        
    // ----- Confirm Sensor Status Stable ----- //
    if(channelConnect != lastCheck[ch])
    {   
        TIMER_setTimer(&t[ch],5);                                     
        lastCheck[ch]       = channelConnect;                                   
        steadyState[ch]     = 0;
    }
    else if(!steadyState[ch])
    {
        if(TIMER_checkTimerExceed(t[ch])){  
            steadyState[ch] = 1;    
        }      
    } 
    
    /*bongkot test*/  
    //printDebug("SENSOR:channelConnect Status Stable\r\n");  
    //steadyState[ch] = 1;
    
    if(!steadyState[ch])
    {
        //printDebug("<sensorRead> sensor %d state unstable\r\n",ch+1);                               
        sensor->status  = SENS_ERR_UNKNOWN;        
        return -9;
    }       
   
    /*bongkot test*/   
    //printDebug("SENSOR:steadyState ok\r\n"); 
    
    
    // ----- Return if Sensor Data is not available ----- //    
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED))
    {   /*bongkot comment*/
//        if(!channelConnect){
//            sensor->status  = SENS_ERR_DISCONNECT;	
//            return -2;      
//        }                        
        if(channelConnect != SENS_NORMAL){
            sensor->status  = channelConnect;            
            return -2;
        }  
       // updateSensorADC(ch);                            // -- update adcData[ch] -- //
    }                
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){ 
        if(channelConnect != SENS_NORMAL){
            sensor->status  = channelConnect;            
            return -2;
        }                        
        
        updateSensorADC(ch);                            // -- update adcData[ch] -- again ^_^ --- //
    }                        
    
    switch(epCalType[ch]){
        case CAL_SELFCOMPARE    :   // --- Same as CAL_EXTOFFSET --- //
        case CAL_EXTOFFSET      :   if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){  
                                        /*bongkot comment*/
                                        //sensor->value   =   (ADC_CONV_FAC*100 * adcData[ch]) + epFactor_C[ch]; 
                                         //sensor->value   +=    epFactor_C[ch];  
                                         viADCbuffer = 0;  
                                         for(i=0;i<30;i++){
                                            viADCbuffer += read_adc(8+ch);
                                         } 
                                         
                                         viSensorTempValue = iAD595ADCToTemp(viADCbuffer,30);
                                         offset =  epTempRef1[ch] - epADCRef1[ch];   
                                         printDebug("TempRaw(%f), TempRef(%f), Offset(%f)\r\n",viSensorTempValue,epTempRef1[ch],offset);
                                         sensor->value = viSensorTempValue + offset;
                                    }                
                                    else if(TEMP_SEL == TEMP_TMEC){  
                                                          
                                        sensor->value   =   (epTMECTempSlope  * adcData[ch]) + epFactor_C[ch];      
                                    }                      
                                    else if(TEMP_SEL == TEMP_PT100){               
                                        sensor->value   =   (epPT100Temp_A    * adcData[ch] * adcData[ch]) + 
                                                            (epPT100Temp_B    * adcData[ch]) + 
                                                             epFactor_C[ch];  
//                                        sensor->value = (epPT100TempSlope * adcData[ch]) + epFactor_C[ch];   
                                    }               
                                    break;      
        case CAL_EXTLINEAR      :  {   
                                        if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
                                               //sensor->value   =   (epFactor_B[ch]   * adcData[ch]) + epFactor_C[ch];   
                                               
                                               viADCbuffer = 0;  
                                                 for(i=0;i<30;i++){
                                                    viADCbuffer += read_adc(8+ch);
                                                 }                
                                                 
                                                sensorRef1 = iAD595ADCToTemp(epADCRef1[ch],1);
                                                sensorRef2 = iAD595ADCToTemp(epADCRef2[ch],1);
                                                stdMeter1  = epTempRef1[ch]; 
                                                stdMeter2  = epTempRef2[ch];
                                                lowRange   =  sensorRef2 - sensorRef1;
                                                stdMeterRange = stdMeter2 - stdMeter1;  
                                                viSensorTempValue = iAD595ADCToTemp(viADCbuffer,30);
                                               sensor->value = (((viSensorTempValue-sensorRef1)*stdMeterRange)/lowRange)+stdMeter1;    
                                               
                                               
                                               
                                        }else{
                                               sensor->value   =   (epFactor_B[ch]   * adcData[ch]) + epFactor_C[ch];  
                                        }
                                       
                                        break;  
                                    }
        case CAL_EXTPOLYNOMIAL  :   {   if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
                                            viADCbuffer = 0;
                                            for(i=0;i<30;i++){
                                                viADCbuffer += read_adc(8+ch);
                                            }  
                                            sensorRef1 = epADCRef1[ch];
                                            sensorRef2 = epADCRef2[ch];
                                            sensorRef3 = epADCRef3[ch]; 
                                            stdMeter1 = epTempRef1[ch];
                                            stdMeter2 = epTempRef2[ch];
                                            stdMeter3 = epTempRef3[ch];   
                                            viSensorTempValue = iAD595ADCToTemp(viADCbuffer,30);
                                            gaussian(sensorRef1,  sensorRef2,  sensorRef3, stdMeter1, stdMeter2, stdMeter3, &factorAA, &factorBB, &factorCC); 
                                            sensor->value = (factorAA*viSensorTempValue*viSensorTempValue) + (factorBB*viSensorTempValue) + factorCC;      //polynomail formula
            
                                        }else{
                                              sensor->value   =   (epFactor_A[ch]   * adcData[ch] * adcData[ch]) + 
                                                        (epFactor_B[ch]   * adcData[ch]) + 
                                                         epFactor_C[ch]; 
                                        }
                                           
                                        break; 
                                    }     
        default                 :   if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){ 
                                        //sensor->value   =   (ADC_CONV_FAC*100 * adcData[ch]);    
                                         viADCbuffer = 0;  
                                         for(i=0;i<20;i++){
                                            viADCbuffer += (unsigned long)(read_adc(8+ch));
                                         }
                                         viRawVoltValue = iAD595ADCToVolt(viADCbuffer,20.0);  
                                         
                                         sensor->value = iAD595VoltToTemp((float)((viRawVoltValue+0.213)-viZeroSet));        //0.213 *2 = 0.426
                                    }                
                                    else if(TEMP_SEL == TEMP_TMEC){                 
                                        sensor->value   =   (epTMECTempSlope  * adcData[ch]) + epTMECTempOffset;        
                                    }                      
                                    else if(TEMP_SEL == TEMP_PT100){             
                                        sensor->value   =   (epPT100Temp_A    * adcData[ch] * adcData[ch]) + 
                                                            (epPT100Temp_B    * adcData[ch]) + 
                                                             epPT100Temp_C;  
//                                        sensor->value = (epPT100TempSlope * adcData[ch]) + epPT100TempOffset;
//                                        getInterval(adcData[ch], &x_1, &x_2, &y_1, &y_2);
//                                        sensor->value = (((adcData[ch] - x_1) * (y_2 - y_1)) / (x_2 - x_1)) + y_1;
                                                             
                                        // test
                                        //printDebug("ADC: %f, Value: %f \r\n", adcData[ch], sensor->value);
                                        //   
                                    }      
                                    break;
    }                       
    //printDebug("end read sensor.\r\n");
    sensor->status  = SENS_NORMAL;               
    
    return 0;
}                                                               
/*============================================================*/
void getInterval(float adcRaw, float *x1, float *x2, float *y1, float *y2) {

    int i = 0;
    
    for(i = 0; i < 15; i++) {
        if(adcRaw <= adcTable[i]) {
            *x1 = adcTable[i-1];  
            *y1 = tempTable[i-1];
            *x2 = adcTable[i]; 
            *y2 = tempTable[i];
            break;
        }      
    } 
    
    return;
}
/*============================================================*/
int sensorCalibrate1(int p1, int p2)
{
    float p2_raw;
    
    if(operating_config.sensorEnable[p1] != 0x01){
        printDebug("<sensorCalibrate1> ABORT: sensor ch %d is disable\r\n",p1+1);  
        return -1;
    }                                                                      
    if(operating_config.sensorEnable[p2] != 0x01){
        printDebug("<sensorCalibrate1> ABORT: sensor ch %d is disable\r\n",p2+1);  
        return -1;
    }                                                                        
    
    if(sensor[p1].status != SENS_NORMAL){
        printDebug("<sensorCalibrate1> ABORT: sensor ch %d error(0x%02X)\r\n",p1+1,sensor[p1].status);  
        return -1;
    }                                                                        
    if(sensor[p2].status != SENS_NORMAL){
        printDebug("<sensorCalibrate1> ABORT: sensor ch %d error(0x%02X)\r\n",p2+1,sensor[p2].status);  
        return -1;
    }
     
    epCalType[p2]   = CAL_SELFCOMPARE;                      
                              
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){ 
        p2_raw          = (ADC_CONV_FAC*100 * adcData[p2]); 
    }                
    else if(TEMP_SEL == TEMP_TMEC){                    
        p2_raw          = epTMECTempSlope   * adcData[p2];       
    }                      
    else if(TEMP_SEL == TEMP_PT100){               
        p2_raw          = (epPT100Temp_A    * adcData[p2] * adcData[p2]) + 
                          (epPT100Temp_B    * adcData[p2]); 
    }   
                      
    epFactor_A[p2]  = 0;         
    epFactor_B[p2]  = 0;         
    epFactor_C[p2]  = sensor[p1].value - p2_raw;
    
    return 0;
}                                                                  
/*============================================================*/
int sensorCalibrate2(int prbId)
{            
    float p2_raw;      
    float tmpA,tmpB,tmpC; 
    //int i = 0;
    //long viADCbuffer = 0;
             
    if(epIsPointSet1[prbId] == 0){             
        printDebug("<sensorCalibrate2> ABORT: sensor ch %d No set point found\r\n",prbId+1);  
        return -1;
    }                                                                                         
    
    if(epIsPointSet2[prbId] == 0){                  
        printDebug("<sensorCalibrate2> -- CAL_EXTOFFSET --\r\n");
        epFactor_A[prbId]   = 0;         
        epFactor_B[prbId]   = 0;         
                              
        if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){ 
            p2_raw          = (ADC_CONV_FAC*100 * epADCRef1[prbId]);  
            
            
        }                
        else if(TEMP_SEL == TEMP_TMEC){                    
            p2_raw          = epTMECTempSlope   * epADCRef1[prbId];     
        }                      
        else if(TEMP_SEL == TEMP_PT100){               
            p2_raw          = (epPT100Temp_A    * epADCRef1[prbId] * epADCRef1[prbId]) + 
                              (epPT100Temp_B    * epADCRef1[prbId]);  
        }   

        epFactor_C[prbId]   = epTempRef1[prbId] - p2_raw;
        epCalType[prbId]    = CAL_EXTOFFSET;                                                                
        printDebug("<sensorCalibrate2> --C--> %f\r\n", epFactor_C[prbId]);  
        return 0;
    }                                                    
    
    if(epIsPointSet3[prbId] == 0){                                                                                  
        printDebug("<sensorCalibrate2> -- CAL_EXTLINEAR --\r\n");    
        epFactor_A[prbId]   = 0;         
        
        epFactor_B[prbId]   = (epTempRef2[prbId] - epTempRef1[prbId])/(epADCRef2[prbId] - epADCRef1[prbId]);
        printDebug("<sensorCalibrate2> --B--> %f\r\n", epFactor_B[prbId]);                     
          
        epFactor_C[prbId]   = epTempRef2[prbId] - (epFactor_B[prbId] * epADCRef2[prbId]);                                       
        printDebug("<sensorCalibrate2> --C--> %f\r\n", epFactor_C[prbId]);                      
        
        epCalType[prbId]    = CAL_EXTLINEAR;    
    }  
    else{                                                                         
        printDebug("<sensorCalibrate2> -- CAL_EXTPOLYNOMIAL --\r\n"); 
                                                                    
        gaussian(epADCRef1[prbId],  epADCRef2[prbId],  epADCRef3[prbId], 
                 epTempRef1[prbId], epTempRef2[prbId], epTempRef3[prbId], 
                 &tmpA, &tmpB, &tmpC);
                                           
        epFactor_A[prbId]   = tmpA;      
        epFactor_B[prbId]   = tmpB;                   
        epFactor_C[prbId]   = tmpC;                           
                                                                                            
        printDebug("<sensorCalibrate2> --A--> %f\r\n", epFactor_A[prbId]);                     
        printDebug("<sensorCalibrate2> --B--> %f\r\n", epFactor_B[prbId]);                    
        printDebug("<sensorCalibrate2> --C--> %f\r\n", epFactor_C[prbId]); 
                 
        
        
        epCalType[prbId]    = CAL_EXTPOLYNOMIAL;   
    }
    
    return 0;
}                                                
/*============================================================*/
int sensorGetCurrentSetPoint(int ch,int pnt, unsigned char *buf)
{
    unsigned char   valStr[10];                                   
    float           valTmp;
    //signed int      val;               
    int             i;
                
    if(pnt == CALPOINT(1)){      
        if(epIsPointSet1[ch] == 1){      
            valTmp  = epTempRef1[ch];                                               
            printDebug("<sensorGetCurrentSetPoint> tempRef1 = [%f][%f]\r\n",epTempRef1[ch],valTmp);
            ftoa(valTmp,2,valStr);
            //val = (epTempRef1[ch] * 100);   
        }         
        else{                                             
            printDebug("<sensorGetCurrentSetPoint> tempRef1 not set [0.00]\r\n");
            sprintf(valStr,"0.00");                 //val = 0;        
        }   
    }
    else if(pnt == CALPOINT(2)){ 
        if(epIsPointSet2[ch] == 1){      
            valTmp  = epTempRef2[ch];                                               
            printDebug("<sensorGetCurrentSetPoint> tempRef2 = [%f][%f]\r\n",epTempRef2[ch],valTmp);
            ftoa(valTmp,2,valStr);
            //val = (epTempRef1[ch] * 100);   
        }         
        else{                                                          
            printDebug("<sensorGetCurrentSetPoint> tempRef2 not set [0.00]\r\n");
            sprintf(valStr,"0.00");                 //val = 0;        
        }   
        /*
        if(epIsPointSet2[ch] == 1){ val = (epTempRef2[ch] * 100);   printDebug("<sensorGetCurrentSetPoint> tempRef = [%f]\r\n",epTempRef2[ch]);}         
        else{                       val = 0;        }   
        */
    }   
    else if(pnt == CALPOINT(3)){  
        if(epIsPointSet3[ch] == 1){      
            valTmp  = epTempRef3[ch];                                               
            printDebug("<sensorGetCurrentSetPoint> tempRef3 = [%f][%f]\r\n",epTempRef3[ch],valTmp);
            ftoa(valTmp,2,valStr);
            //val = (epTempRef1[ch] * 100);   
        }         
        else{                                             
            printDebug("<sensorGetCurrentSetPoint> tempRef3 not set [0.00]\r\n");
            sprintf(valStr,"0.00");                 //val = 0;        
        }      
        /*
        if(epIsPointSet3[ch] == 1){ val = (epTempRef3[ch] * 100);   printDebug("<sensorGetCurrentSetPoint> tempRef = [%f]\r\n",epTempRef3[ch]);}         
        else{                       val = 0;        }    
        */
    }
    else{         
        printDebug("<sensorGetCurrentSetPoint> invalid paramter pnt->[%d]\r\n",pnt);
        return -1;
    }                                                 
    //printDebug("<sensorGetCurrentSetPoint> read val = [%d]\r\n",val);
    printDebug("<sensorGetCurrentSetPoint> read val(%d) = [%s]\r\n",strlen(valStr),valStr);    
    
    if(valStr[0] == '-'){
        buf[0] = 1;
        if(strlen(valStr) == 7){                          
            buf[1]  = valStr[1] - 0x30;     buf[2]  = valStr[2] - 0x30;     buf[3]  = valStr[3] - 0x30;      
            buf[4]  = valStr[5] - 0x30;     buf[5]  = valStr[6] - 0x30;            
        }
        else if(strlen(valStr) == 6){
            buf[1]  = 0;                    buf[2]  = valStr[1] - 0x30;     buf[3]  = valStr[2] - 0x30;      
            buf[4]  = valStr[4] - 0x30;     buf[5]  = valStr[5] - 0x30;  
        }               
        else if(strlen(valStr) == 5){
            buf[1]  = 0;                    buf[2]  = 0;                    buf[3]  = valStr[1] - 0x30;      
            buf[4]  = valStr[3] - 0x30;     buf[5]  = valStr[4] - 0x30;  
        }                         
        else{         
            printDebug("<sensorGetCurrentSetPoint> out of condition valStr = '%s'",valStr);
            return -1;  
        }
    }
    else{
        buf[0] = 0;
        if(strlen(valStr) == 6){
            buf[1]  = valStr[0] - 0x30;     buf[2]  = valStr[1] - 0x30;     buf[3]  = valStr[2] - 0x30;      
            buf[4]  = valStr[4] - 0x30;     buf[5]  = valStr[5] - 0x30;            
        }
        else if(strlen(valStr) == 5){
            buf[1]  = 0;                    buf[2]  = valStr[0] - 0x30;     buf[3]  = valStr[1] - 0x30;      
            buf[4]  = valStr[3] - 0x30;     buf[5]  = valStr[4] - 0x30;  
        }               
        else if(strlen(valStr) == 4){
            buf[1]  = 0;                    buf[2]  = 0;                    buf[3]  = valStr[0] - 0x30;      
            buf[4]  = valStr[2] - 0x30;     buf[5]  = valStr[3] - 0x30;  
        }                    
        else{                                          
            printDebug("<sensorGetCurrentSetPoint> out of condition valStr = '%s'",valStr);
            return -1;  
        }
    }
    
    printDebug("<sensorGetCurrentSetPoint> buf read");       
    for(i=0;i<6;i++){
        printDebug("[%d]",buf[i]);    
    }                      
    printDebug("\r\n");
    
    return 0;
}
/*============================================================*/
int sensorSetCurrentSetPoint(int ch,int pnt,unsigned char *buf)
{
    float   val=0;    
    long viADCbuffer = 0; 
    int i = 0; 
    
    if(operating_config.sensorEnable[ch] != 0x01){
        printDebug("<sensorSetCurrentSetPoint> ABORT: sensor ch %d is disable\r\n",ch+1);  
        return -1;
    }                                                                       
    
    if(sensor[ch].status != SENS_NORMAL){
        printDebug("<sensorSetCurrentSetPoint> ABORT: sensor ch %d error(0x%02X)\r\n",ch+1,sensor[ch].status);  
        return -1;
    }                                                            
                                                 
    printDebug("<sensorSetCurrentSetPoint> buf: [%d]/[%d][%d][%d][%d][%d]\r\n",
                                        buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);  
                                                 
    val =  (float) (0.0);
    val += (float) (buf[1] * 100);
    val += (float) (buf[2] * 10);
    val += (float) (buf[3]);
    val += (float) (buf[4] * 0.1);
    val += (float) (buf[5] * 0.01);                   
    if(buf[0] == 1){    val *= (float) (-1.0);  }              
    
    printDebug("<sensorSetCurrentSetPoint> sensor %1d, point %1d, set to [%f]",ch+1,pnt,val);
    
    if(pnt == CALPOINT(1)){       
        if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
           viADCbuffer = 0;  
             for(i=0;i<30;i++){
                viADCbuffer += read_adc(8+ch);
             } 
                                             
             epADCRef1[ch] = iAD595ADCToTemp(viADCbuffer,30);
        }else{
           epADCRef1[ch]       = adcData[ch]; 
        }
             
             
                                                           
        epTempRef1[ch]      = val;                                      
        //epADCRef1[ch]       = adcData[ch];                                
        epIsPointSet1[ch]   = 1;                                                                   
        epIsPointSet2[ch]   = 0;                        // -- reset cal_point 2 -- //           
        epIsPointSet3[ch]   = 0;                        // -- reset cal_point 3 -- //
    }
    else if(pnt == CALPOINT(2)){ 
      
        if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
           viADCbuffer = 0;  
             for(i=0;i<30;i++){
                viADCbuffer += read_adc(8+ch);
             } 
                                             
             epADCRef2[ch] = iAD595ADCToTemp(viADCbuffer,30);
        }else{
           epADCRef2[ch]       = adcData[ch]; 
        }
                                
        epTempRef2[ch]      = val;                                                      
        //epADCRef2[ch]       = adcData[ch];          
        epIsPointSet2[ch]   = 1;                                        
        epIsPointSet3[ch]   = 0;                        // -- reset cal_point 3 -- //
    }                
    else if(pnt == CALPOINT(3)){   
        if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
           viADCbuffer = 0;  
             for(i=0;i<30;i++){
                viADCbuffer += read_adc(8+ch);
             } 
                                             
             epADCRef3[ch] = iAD595ADCToTemp(viADCbuffer,30);
        }else{
           epADCRef3[ch]       = adcData[ch]; 
        }
                            
        epTempRef3[ch]      = val;                                                      
        //epADCRef3[ch]       = adcData[ch];            
        epIsPointSet3[ch]   = 1;   
    }
    else{         
        printDebug("<sensorGetCurrentSetPoint> invalid paramter pnt->[%d]\r\n",pnt);
        return -1;
    }                                
    
    return 0;
}
/*============================================================*/

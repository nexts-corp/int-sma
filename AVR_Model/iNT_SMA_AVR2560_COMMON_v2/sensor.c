#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <delay.h>
#include "sensor.h"
#include "config.h"
#include "timer.h"
#include "debug.h"
#include "dht11.h"
#include "calibration.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))


//extern eeprom ST_CAL sensorCalulate;
struct sens_val_st      sensor[5];
extern ST_DHT11 dhtDevice;
ST_DHT11 dhtDevice;
//eeprom float calS1[5];//standrad point 1
//eeprom float calS2[5];//standrad point 2
//eeprom float calS3[5];//standrad point 3
//eeprom float calX1[5];//input sensor point 1
//eeprom float calX2[5];//input sensor point 2
//eeprom float calX3[5];//input sensor point 3

float sensorGetValue(unsigned int channelID){
    printDebug("Sensor ch[%d] is a raw data[%0.2f] , calProcess[%0.2f]\r\n",channelID-1,adcData[channelID-1],calProcessValue(adcData[channelID-1],channelID));
    return calProcessValue(adcData[channelID-1],channelID);
}

int sensorRead(int ch, struct sens_val_st *sensor){
    int dhtRet = 0;
    
    updateSensorADC(ch);
    if(ch==0){  
       sensor->value = calProcessValue(adcData[ch],ch+1);
       //sensor->value = adcData[ch]; 
    }else if(ch==3){
    sensor->value = calProcessValue(adcData[ch],ch+1);
       //sensor->value = adcData[ch];
    }else if(ch==4){ 
       sensor->value = calProcessValue(adcData[ch],ch+1);
       //sensor->value = adcData[ch];
    }else if((ch==1) || (ch==2)){
        dhtRet = dhtDevice.status;
        if(dhtRet == DHT11_SUCCESS){
           sensor->value = calProcessValue(adcData[ch],ch+1);  
           //sensor->value = adcData[ch];
        }else if(dhtRet == DHT11_ERROR_TIMEOUT){       
           sensor->status  = SENS_ERROR;
           //return -2;
        }else if(dhtRet == DHT11_ERROR_CHECKSUM){      
           sensor->status  = SENS_ERROR;
           //return -2;
        }    
    }
       
    sensor->status  = SENS_NORMAL; 
    return 0;
}

void updateSensorADC(int ch)
{
    int adc_raw = 0;
    int dhtRet = 0; 
    float voltCutOff = 0.0; 
    float ACS709offset = 18.5; 
    float buffAmpSensor[60]; 
    float buffVoltSensor[50];  
    float buff420Sensor[10];
    float bufferTemp[2] = {0,0};
    float bufferHumi[2] = {0,0};
    int i=0,j=0;  
    int dht11Reread=0;     
    float ampValue = 0.0;
    float voltValue = 0.0; 
    float mA420Value = 0.0;
        
    adc_raw     = read_adc(8+ch);
    if(ch==0){ 
        //adcData[ch] =   adc_raw;
        //mA420Value = ((((adcData[ch]+5.0/1023.0)*3.30)/3.30)*20.0);  
        
        for(i=0;i<10;i++){
            buff420Sensor[i] = read_adc(8+ch);
            delay_us(10);
        }
        mA420Value = ((average(buff420Sensor,10)+5.0)/1023.0)*20.0;     
        //mA420Value = (((adcData[ch]+5.0/1023.0)*3.30)/165.0);     //R=165 Ohm
       //printDebug("[%d]ADC: %f, mA: %f \r\n", ch, adcData[ch], mA420Value);
       //adcData[ch] = (adcData[ch]+5/1023.0)*3.30;  
       adcData[ch] = mA420Value;
    }else if(ch==3){ 
        //adcData[ch] =   adc_raw; 
        //voltValue = ((((adcData[ch]+5.0)/1023.0)*3.30)/3.30)*1000.0;
        for(i=0;i<50;i++){
            buffVoltSensor[i] = read_adc(8+ch);
            delay_us(10);
        }
        
          
        voltValue = ((average(buffVoltSensor,50)+5.0)/1023.0)*1000.0;
        //printDebug("[%d]ADC: %f, Volt(raw): %f, Volt(AC): %f \r\n", ch, adcData[ch], (adcData[ch]+5.0/1023.0)*3.30, voltValue);
       
       adcData[ch] = voltValue; 
       //adcData[ch] = (adcData[ch]+5/1023)*3.30;
    }else if(ch==4){  
        memset(buffAmpSensor,0,sizeof(buffAmpSensor));
        for(i=0;i<60;i++){
          buffAmpSensor[i] =  read_adc(8+ch);
          delay_ms(1);
        }
        //adcData[ch] = average(buffAmpSensor);     
       // voltCutOff = fabs(((((adcData[ch]+5.0)/1023.0)*3.30)-1.65));
        voltCutOff = fabs(((((voltPeak(buffAmpSensor)+5.0)/1023.0)*3.30)-1.65));
        ampValue = (voltCutOff*1000.0)/18.5; 
        //printDebug("[%d]ADC: %f, Volt(raw): %f, Amp: %f\r\n", ch, adcData[ch], (adcData[ch]-511.0), ampValue);
        
       adcData[ch] =  ampValue;
    }else if((ch==1) || (ch==2)){ 
          
        //dhtRet = DHT11Read(&dhtDevice); 
        
        dht11Reread = 0;
        //memset(&dhtDevice,0,sizeof(dhtDevice));
        for(i=0;i<20;i++){
            
            dhtRet = DHT11Read(&dhtDevice);
            //delay_ms(4);
            //printDebug("[%d]ADC:DHT11:Return (%d). \r\n", ch,dhtRet); 
            if(dhtRet == DHT11_SUCCESS){ 
//                if(ch==1){
//                  bufferTemp[dht11Reread] = dhtDevice.temp;
//                }else if(ch==2){
//                  bufferHumi[dht11Reread] = dhtDevice.humi;
//                }
                  
                bufferTemp[dht11Reread] = dhtDevice.temp;
                bufferHumi[dht11Reread] = dhtDevice.humi;
                
                ++dht11Reread;
            }  
            if(dht11Reread>=2){
               break;
            }
        }
            
        if(dht11Reread>=2){
            if(ch==1){
               adcData[ch] = average(bufferTemp,2);
            }else if(ch==2){
               adcData[ch] = average(bufferHumi,2);
            }
        }else if(dht11Reread>=1){
            if(ch==1){
               adcData[ch] = average(bufferTemp,1);
            }else if(ch==2){
               adcData[ch] = average(bufferHumi,1);
            }
        }
    }
    return;    
}

float voltPeak(float data[]){       //Amp value
     int i = 0;
     float sum=0.0;
     
     for(i=0;i<60;i++){   
       sum = MAX(sum,data[i]);
     }  
     return sum;
}
float average(float data[],int count){      //Volt value
     int i = 0;
     float avg = 0.0, sum=0.0;
     
     //printDebug("raw data.\r\n");
     for(i=0;i<count;i++){   
       //printDebug("%.2f ", data[i]);
       sum += data[i];
     }    
     //printDebug("\r\nMax(%.2f) ", sum);  
     //printDebug("\r\n");
     //return sum;
              
     
     avg =(sum/count);
     return avg;
}


 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <delay.h>
#include "calibration.h"
#include "timer.h"
#include "gaussian.h"
#include "debug.h"
//#include "sensor.h"

//typedef struct{
//   ST_CAL_CHANNEL sChennel[5]; 
//}ST_CAL;
//

ST_CAL_CONFIG sensorCalConfig;
eeprom  ST_CAL sensorCalulate = {
  {
    {{0.0,0.0,0.0},{0.0,0.0,0.0},0},
    {{0.0,0.0,0.0},{0.0,0.0,0.0},0},
    {{0.0,0.0,0.0},{0.0,0.0,0.0},0},
    {{0.0,0.0,0.0},{0.0,0.0,0.0},0},
    {{0.0,0.0,0.0},{0.0,0.0,0.0},0}
  }
};          //improtance

//eeprom float calOffset = 0.0;

void (*pfCalibrate) (ST_CAL_CONFIG calConfig);   //pointer function of calibation set


/**
**@fn name   calGet
**@header   name        type            des
**@param    channelID   unsigned int    channel of sensor = 5(max)
**@return   none        void
**/
void calGet(ST_CAL_CONFIG calConfig){
    unsigned int i=0;
    unsigned int offsetMember       = 1;     //use 1 point
    unsigned int linearMember       = 2;     //use 2 point
    unsigned int polynomailMember   = 3;     //use 3 point   
    if(calConfig.prop=CAL_GET){
        printDebug("<Cal Std ref>ch[%d],formatType:%0.2f\r\n",calConfig.channelID,calConfig.calType);
        switch(calConfig.calType){
            case CAL_EXTOFFSET :{
                for(i=0;i<offsetMember;i++){
                   printDebug("stdRef:%0.2f,sensorRef:%0.2f\r\n",sensorCalulate.sChennel[calConfig.channelID-1].stdRef[i],sensorCalulate.sChennel[calConfig.channelID-1].sensorRef[i]);
                } 
                break;
            }
            case CAL_EXTLINEAR :{  
                for(i=0;i<linearMember;i++){
                   printDebug("stdRef:%0.2f,sensorRef:%0.2f\r\n",sensorCalulate.sChennel[calConfig.channelID-1].stdRef[i],sensorCalulate.sChennel[calConfig.channelID-1].sensorRef[i]);
                }
                break;
            }
            case CAL_EXTPOLYNOMIAL :{
                for(i=0;i<polynomailMember;i++){
                   printDebug("stdRef:%0.2f,sensorRef:%0.2f\r\n",sensorCalulate.sChennel[calConfig.channelID-1].stdRef[i],sensorCalulate.sChennel[calConfig.channelID-1].sensorRef[i]);
                }
                break;
            }
            defualt:{
                break;
            }
        }
    }else{
       printDebug("<calibration>Cal properties:set can not get value.");
    }  
}
void calSet(ST_CAL_CONFIG calConfig){
    if(calConfig.prop=CAL_SET){
        sensorCalulate.sChennel[calConfig.channelID-1].stdRef[calConfig.pointID-1] =  calConfig.pointValue;
        sensorCalulate.sChennel[calConfig.channelID-1].sensorRef[calConfig.pointID-1] = adcData[calConfig.channelID-1];
    }else{
       printDebug("<calibration>Cal properties:get can not set value.");
    } 
}
void calSetConfirm(ST_CAL_CONFIG calConfig){
    if(calConfig.prop=CAL_SET){
        printDebug("<Cal Std ref>ch[%d],formatType:%d\r\n",calConfig.channelID,calConfig.calType);
        switch(calConfig.calType){
            case CAL_EXTOFFSET :{
                sensorCalulate.sChennel[calConfig.channelID-1].calType = CAL_EXTOFFSET;
                break;
            }
            case CAL_EXTLINEAR :{  
                sensorCalulate.sChennel[calConfig.channelID-1].calType = CAL_EXTLINEAR;
                break;
            }
            case CAL_EXTPOLYNOMIAL :{
                sensorCalulate.sChennel[calConfig.channelID-1].calType = CAL_EXTPOLYNOMIAL;
                break;
            }
            case CAL_DEFAULT:{
                sensorCalulate.sChennel[calConfig.channelID-1].calType = CAL_DEFAULT;
                break;
            }
        }
    }else{
       printDebug("<calibration>Cal properties:get can not set value.");
    }
}
void calSetClearPoint(ST_CAL_CONFIG calConfig){
   if(calConfig.prop=CAL_SET){
      sensorCalulate.sChennel[calConfig.channelID-1].stdRef[calConfig.pointID] =  0;
      sensorCalulate.sChennel[calConfig.channelID-1].sensorRef[calConfig.pointID] = 0;
   }else{
      printDebug("<calibration>Cal properties:get can not set value.");
   }
}

float calProcessValue(float sensorValue,unsigned int channelID){
    float offset = 0.0;
    float sensorRef1 = 0.0,sensorRef2 = 0.0,sensorRef3 = 0.0;
    float stdMeter1 = 0.0,stdMeter2 = 0.0,stdMeter3 = 0.0;        //standrad  meter
    float lowRange = 0.0;  
    float stdMeterRange = 0.0; 
    float factorA = 0.0,factorB = 0.0,factorC = 0.0;   
    
    switch(sensorCalulate.sChennel[channelID-1].calType){
        case CAL_EXTOFFSET :{ 
            offset =  sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT1] - sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT1];
            sensorValue += offset;
            break;
        }
        case CAL_EXTLINEAR :{  
            //sensorValue = (((H28-$B$28)*$E$28)/$F$28)+$A$28;
            sensorRef1 = sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT1]; 
            sensorRef2 = sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT2];
            stdMeter1  = sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT1]; 
            stdMeter2  = sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT2];
            lowRange   =  sensorRef2 - sensorRef1;
            stdMeterRange = stdMeter2 - stdMeter1;
            sensorValue = (((sensorValue-sensorRef1)*stdMeterRange)/lowRange)+stdMeter1;
            break;
        }
        case CAL_EXTPOLYNOMIAL :{
            sensorRef1 = sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT1]; 
            sensorRef2 = sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT2];
            sensorRef3 = sensorCalulate.sChennel[channelID-1].sensorRef[CAL_POINT3];
            stdMeter1  = sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT1]; 
            stdMeter2  = sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT2]; 
            stdMeter3  = sensorCalulate.sChennel[channelID-1].stdRef[CAL_POINT3]; 
            
            gaussian(sensorRef1,  sensorRef2,  sensorRef3, stdMeter1, stdMeter2, stdMeter3, &factorA, &factorB, &factorC); 
            sensorValue = (factorA*sensorValue*sensorValue) + (factorB*sensorValue) + factorC;      //polynomail formula
            
            break;
        }
        case CAL_DEFAULT:{
            break;
        }
    }
    return sensorValue;
}
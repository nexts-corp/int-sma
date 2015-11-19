#include "ad595.h"
#include "debug.h"
#include "gaussian.h"

float iAD595ADCToTemp(long viADC_p,int viADCLoop_p){
     int    viADCSmooth =  (int)(viADC_p/viADCLoop_p);           ////(float)(viADCbuffer/30.0); 
     float  viADCToVolt = 0.0;   
     //const float  viZeroSet   = 1.68;   
     const float  viZeroSet   = 1.667; 
     float viVoltUpLevel = 0.0; 
     float ivTempValue = 0.0;
     float factorA = 0.0;   
     float factorB = 0.0;
     float factorC = 0.0;
     
     /////////range 1 Temp C(-10 to -100)  
     float viVoltPoint1_1 = -0.094;   
     float viVoltPoint2_1 = -0.552;
     float viVoltPoint3_1 = -0.876; 
     float viTempOutPoint1_1 = -10.0;   
     float viTempOutPoint2_1 = -60.0;
     float viTempOutPoint3_1 = -100.0;   
     
     /////////range 2 Temp C(-100 to -200)  
     float viVoltPoint1_2 = -0.876;   
     float viVoltPoint2_2 = -1.269;
     float viVoltPoint3_2 = -1.454; 
     float viTempOutPoint1_2 = -100.0;   
     float viTempOutPoint2_2 = -160.0;
     float viTempOutPoint3_2 = -200.0;   
     
     const float viVoltPointTable[5][3]  = {
            {-0.094,-0.189,-0.375},           //-10 to -40
            {-0.375,-0.552,-0.719},           //-40 to -80
            {-0.719,-0.876,-1.021},           //-80 to -120
            {-1.021,-1.152,-1.269},           //-120 to -160
            {-1.269,-1.370,-1.454}            //-160 to -200
     }; 
     
     const float viTempOutPointTable[5][3]  = {
            {-10.0,-20.0,-40.0},           //-10 to -40
            {-40.0,-60.0,-80.0},           //-40 to -80
            {-80.0,-100.0,-120.0},           //-80 to -120
            {-120.0,-140.0,-160.0},           //-120 to -160
            {-160.0,-180.0,-200.0}            //-160 to -200
     };
     
     viADCToVolt = (float)(viADCSmooth/1024.0)*3.30;          //- 0  +     
     viVoltUpLevel = viADCToVolt - viZeroSet;         
    
     
//     if(viVoltUpLevel >= (-0.094)){
//          ivTempValue = (viVoltUpLevel*1000) / 10.0;
//          printDebug("ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-0.094)) && (viVoltUpLevel >= (-0.876)) ){   
//        
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0; 
//        gaussian(viVoltPoint1_1,  viVoltPoint2_1,  viVoltPoint3_1, viTempOutPoint1_1, viTempOutPoint2_1, viTempOutPoint3_1, &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//         printDebug("-L1 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-0.876)) && (viVoltUpLevel >= (-1.454)) ){
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
//        gaussian(viVoltPoint1_2,  viVoltPoint2_2,  viVoltPoint3_2, viTempOutPoint1_2, viTempOutPoint2_2, viTempOutPoint3_2, &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//         printDebug("-L2 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if(viVoltUpLevel < ((-1.454)+10.0)){
//        ivTempValue = (viVoltUpLevel*1000) / 10.0;
//         printDebug("-L3 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp Error\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel);
//     }    
     
     
//     if(viVoltUpLevel >= (-0.094)){
//          ivTempValue = (viVoltUpLevel*1000) / 10.0;
//        //  printDebug("ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-0.094)) && (viVoltUpLevel >= (-0.375)) ){   
//        
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0; 
//        gaussian(viVoltPointTable[0][0],  viVoltPointTable[0][1],  viVoltPointTable[0][2], viTempOutPointTable[0][0], viTempOutPointTable[0][1], viTempOutPointTable[0][2], &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//        // printDebug("-L1 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-0.375)) && (viVoltUpLevel >= (-0.719)) ){
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
//        gaussian(viVoltPointTable[1][0],  viVoltPointTable[1][1],  viVoltPointTable[1][2], viTempOutPointTable[1][0], viTempOutPointTable[1][1], viTempOutPointTable[1][2], &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//        // printDebug("-L2 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-0.719)) && (viVoltUpLevel >= (-1.021)) ){
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
//        gaussian(viVoltPointTable[2][0],  viVoltPointTable[2][1],  viVoltPointTable[2][2], viTempOutPointTable[2][0], viTempOutPointTable[2][1], viTempOutPointTable[2][2], &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//        // printDebug("-L3 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-1.021)) && (viVoltUpLevel >= (-1.269)) ){
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
//        gaussian(viVoltPointTable[3][0],  viVoltPointTable[3][1],  viVoltPointTable[3][2], viTempOutPointTable[3][0], viTempOutPointTable[3][1], viTempOutPointTable[3][2], &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//        // printDebug("-L4 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }else if((viVoltUpLevel < (-1.269)) && (viVoltUpLevel >= (-1.464)) ){        //-1.454 + 20.0   | 20 = +compencess
//        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
//        gaussian(viVoltPointTable[4][0],  viVoltPointTable[4][1],  viVoltPointTable[4][2], viTempOutPointTable[4][0], viTempOutPointTable[4][1], viTempOutPointTable[4][2], &factorA, &factorB, &factorC);
//        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
//        
//         //printDebug("-L5 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
//     }
//     else if(viVoltUpLevel < (-1.464)){
//        ivTempValue = (viVoltUpLevel*1000) / 10.0;
//         //printDebug("-L6 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp Error\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel);
//     } 
//     //printDebug("%f,%f\r\n",viVoltUpLevel,ivTempValue);


     if(viVoltUpLevel >= (-0.094)){
          ivTempValue = (viVoltUpLevel*1000) / 10.0;
        //  printDebug("ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }else if((viVoltUpLevel < (-0.094)) && (viVoltUpLevel >= (-0.375)) ){   
        
        //ivTempValue = (viVoltUpLevel*1000) / 10.0; 
        gaussian(viVoltPointTable[0][0],  viVoltPointTable[0][1],  viVoltPointTable[0][2], viTempOutPointTable[0][0], viTempOutPointTable[0][1], viTempOutPointTable[0][2], &factorA, &factorB, &factorC);
        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
        
        // printDebug("-L1 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }else if((viVoltUpLevel < (-0.375)) && (viVoltUpLevel >= (-0.719)) ){
        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
        gaussian(viVoltPointTable[1][0],  viVoltPointTable[1][1],  viVoltPointTable[1][2], viTempOutPointTable[1][0], viTempOutPointTable[1][1], viTempOutPointTable[1][2], &factorA, &factorB, &factorC);
        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
        
        // printDebug("-L2 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }else if((viVoltUpLevel < (-0.719)) && (viVoltUpLevel >= (-1.021)) ){
        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
        gaussian(viVoltPointTable[2][0],  viVoltPointTable[2][1],  viVoltPointTable[2][2], viTempOutPointTable[2][0], viTempOutPointTable[2][1], viTempOutPointTable[2][2], &factorA, &factorB, &factorC);
        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
        
        // printDebug("-L3 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }else if((viVoltUpLevel < (-1.021)) && (viVoltUpLevel >= (-1.269)) ){
        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
        gaussian(viVoltPointTable[3][0],  viVoltPointTable[3][1],  viVoltPointTable[3][2], viTempOutPointTable[3][0], viTempOutPointTable[3][1], viTempOutPointTable[3][2], &factorA, &factorB, &factorC);
        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
        
        // printDebug("-L4 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }else if((viVoltUpLevel < (-1.269)) && (viVoltUpLevel >= (-1.504)) ){        //-1.454 + 50.0   | 50 = +compencess
        //ivTempValue = (viVoltUpLevel*1000) / 10.0;  
        gaussian(viVoltPointTable[4][0],  viVoltPointTable[4][1],  viVoltPointTable[4][2], viTempOutPointTable[4][0], viTempOutPointTable[4][1], viTempOutPointTable[4][2], &factorA, &factorB, &factorC);
        ivTempValue = (factorA*viVoltUpLevel*viVoltUpLevel) + (factorB*viVoltUpLevel) + factorC;      //polynomail formular
        
         //printDebug("-L5 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp(%f)\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel,ivTempValue);
     }
     else if(viVoltUpLevel < (-1.504)){
        ivTempValue = (viVoltUpLevel*1000) / 10.0;
         //printDebug("-L6 ADC(%d), V(%f) ,V(offset+(-range))(%f) ,Temp Error\r\n",viADCSmooth,viADCToVolt,viVoltUpLevel);
     } 
     //printDebug("%f,%f\r\n",viVoltUpLevel,ivTempValue);
     return ivTempValue;
}

float iAD595ADCToVolt(long viADC_p,int viADCLoop_p){            //Volt Uplevel
     int    viADCSmooth =  (int)(viADC_p/viADCLoop_p);           ////(float)(viADCbuffer/30.0); 
     float  viADCToVolt = 0.0;   
     const float  viZeroSet   = 1.68;  
     float viVoltUpLevel = 0.0; 
     float ivVoltValue = 0.0;
     
     viADCToVolt = (float)(viADCSmooth/1024.0)*3.30;          //- 0  +     
     viVoltUpLevel = viADCToVolt - viZeroSet;         
    
      
     return viVoltUpLevel;
}

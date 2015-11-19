#include "smoothFilter.h"

int indexWrite = 0;
int gItemSize = 0;
unsigned int * smoothBuffer;
iSmoothData_t iSmoothData;

pviSmoothHandle_t pviSmoothSensorHandle;

iSmoothData_t smoothFilter(pviSmoothHandle_t pviSmoothHandle_p,int adcValue_p){
    int * ptrSmoothBuffer;
    iSmoothData_t iSmoothData;  
    iSmoothData.status = 0;
    if(indexWrite<gItemSize){
        iSmoothData.status = 0; 
        ptrSmoothBuffer = pviSmoothHandle_p;
        *ptrSmoothBuffer = adcValue_p;
        indexWrite++; 
    }else if(indexWrite==gItemSize){
       iSmoothData.status = 1; 
       indexWrite = 0;
       gItemSize = 0;
       free(smoothBuffer);
    }
      
    
    return iSmoothData;
}

pviSmoothHandle_t smoothAllocateMem(int itemSize){
   pviSmoothHandle_t  pviSmoothHandle;
   smoothBuffer = (int *)malloc(itemSize);
   if(smoothBuffer!=NULL){
     pviSmoothHandle = smoothBuffer;
     gItemSize = itemSize;
   }else{
     return   pviSmoothHandle = (int *)0;
   } 
   return  pviSmoothHandle;
}
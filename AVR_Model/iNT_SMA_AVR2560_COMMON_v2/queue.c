#include "queue.h"

piQueueHandle_t iQueueCreate(const unsigned int iItemQuantity_p,const unsigned int iItemLength_p){
	iQueue_t * pviQueue;
	
	piQueueHandle_t ipvReturnQueueHandle = NULL;
	pviQueue = (iQueue_t *)malloc(sizeof(iQueue_t));
    if(pviQueue != NULL){
        pviQueue->head = (void *)malloc((iItemQuantity_p*iItemLength_p));
        if(pviQueue->head != NULL){
            pviQueue->itemCount = 0;  //Empty  
            pviQueue->writeTo = pviQueue->head;
            pviQueue->readDataFrom = pviQueue->head;
            pviQueue->tail = pviQueue->head+((iItemQuantity_p*iItemLength_p)-iItemLength_p);
            pviQueue->itemLength = iItemLength_p;  //size of type
            pviQueue->itemQuantity = iItemQuantity_p; //Quantity of item
            ipvReturnQueueHandle = pviQueue;  
            printDebug("[iQueueCreate],itemCount(%d)\r\n",pviQueue->itemCount); 
            //printDebug("[iQueueCreate],itemCount2(%d)\r\n",((iQueue_t *)ipvReturnQueueHandle)->itemCount);
        }else{
            ipvReturnQueueHandle = (int *)(-1); //fail malloc
            printDebug("[iQueueCreate],Queue(Elen size) is not created!.\r\n");
        }
    }else{
       ipvReturnQueueHandle = (int *)(-1); //fail malloc
       printDebug("[iQueueCreate],Queue is not created!.\r\n");
    }
    return ipvReturnQueueHandle;
}

int iQueueSend(piQueueHandle_t const iQueue_p,const void* const piItem_p){
    int ivReturn;
    //int buffTest;
    //iSTData_t idata;
    iQueue_t * const piQueue = (iQueue_t *)iQueue_p;
    //printDebug("[iQueueSend],itemCount(%d)\r\n",((iQueue_t * const)iQueue_p)->itemCount);
    if(piQueue != NULL){
        if(piQueue->itemCount == 0){  
            printDebug("<iQueueSend>status(empty)\r\n");
            piQueue->writeTo = piQueue->head;
            piQueue->readDataFrom = piQueue->head;
            
            piQueue->itemCount++;
            memcpy((void*)piQueue->writeTo,piItem_p,piQueue->itemLength);
            
            
            //test read
//            memcpy(&buffTest,(void*)piQueue->writeTo,piQueue->itemLength);
//            printf("<iQueueSend>data = %d\r\n",buffTest);
//            memcpy(&idata,(void*)piQueue->writeTo,piQueue->itemLength);
//            printf("<iQueueSend>data len(%d)\r\n",idata.length);
            
            piQueue->writeTo += piQueue->itemLength; //address will write data a next.
            ivReturn = 1;
        }else if((piQueue->itemCount > 0) && (piQueue->itemCount < piQueue->itemQuantity) ){
            piQueue->itemCount++;
            memcpy((void*)piQueue->writeTo,piItem_p,piQueue->itemLength);
            
            //test read
//            memcpy(&buffTest,(void*)piQueue->writeTo,piQueue->itemLength);
//            printf("<iQueueSend>data = %d\r\n",buffTest);
//            memcpy(&idata,(void*)piQueue->writeTo,piQueue->itemLength);
//            printf("<iQueueSend>data len(%d)\r\n",idata.length);
            
            piQueue->writeTo += piQueue->itemLength; //address will write data a next.
            ivReturn = 1;
        }else if((piQueue->itemCount) < 0 || (piQueue->itemCount > piQueue->itemQuantity)){ 
            printDebug("<iQueueSend>status(item count is not range)\r\n)"); 
            ivReturn = -1;
            //while(1); //reset wdt
        }else if(piQueue->itemCount == piQueue->itemQuantity){
            piQueue->writeTo = piQueue->head;
            printDebug("<iQueueSend>status(full and wait)\r\n");
            ivReturn = -1;
        }
        //printf("<iQueueSend>item count(%d)\r\n",piQueue->itemCount);
    }else{
        ivReturn = -1; //fail malloc ref = NULL    
    }
    //printf("<iQueueSend>count(%d)\r\n",piQueue->itemCount);  
    printDebug("<iQueueSend>count(%d)\r\n",piQueue->itemCount);
    return ivReturn;
}

int iQueueReceive(piQueueHandle_t iQueue_p,void* const piItem_p){
    int ivReturn;
    //int buffTest;
    iQueue_t * const piQueue = (iQueue_t *)iQueue_p;
    
    if(piQueue != NULL){
        if(piQueue->itemCount == 0){
            //printf("<iQueueReceive>QueueItem is empty it can not read.\r\n");  
            printDebug("<iQueueReceive>QueueItem is empty it can not read.\r\n");
            ivReturn = -1;
        }else if(piQueue->itemCount > 0){
            //printf("<iQueueReceive>QueueItem is exist.\r\n");
            printDebug("<iQueueReceive>QueueItem is exist.\r\n");
            if(piQueue->readDataFrom < piQueue->tail){
                
                piQueue->itemCount--;
                memcpy(piItem_p,(void*)piQueue->readDataFrom,piQueue->itemLength);
                //memcpy((void*)piQueue->writeTo,piItem_p,piQueue->itemLength);
                
//                memcpy(&buffTest,(void*)piQueue->readDataFrom,piQueue->itemLength);
//                printf("<iQueueReceive>data = %d\r\n",buffTest);
                
                piQueue->readDataFrom += piQueue->itemLength;
                ivReturn = 1;
            }else if(piQueue->readDataFrom == piQueue->tail){
                piQueue->itemCount--;
                piQueue->readDataFrom = piQueue->head;
            }
        }
    }else{
        ivReturn = -1; //fail malloc ref = NULL    
    }
    //printf("<iQueueReceive>count(%d)\r\n",piQueue->itemCount); 
    printDebug("<iQueueReceive>count(%d)\r\n",piQueue->itemCount);
    return ivReturn;
}
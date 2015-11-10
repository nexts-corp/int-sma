/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_PacketQueue.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	27-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#ifndef _INT_STM32F4_PACKETQUEUE_H
#define _INT_STM32F4_PACKETQUEUE_H

//#include "main.h"
#include <stdint.h>

#define MAX_PKT_QUEUE  		15
#define MAX_PKT_LEN     	200
                     
typedef struct {                                
    uint8_t			retryCount;    
    uint32_t    retryTime;
    uint16_t    timeout;          
    uint8_t   	packetLen;     
    uint8_t     packetID; 
    uint8_t     packetData[MAX_PKT_LEN];
}PACKET_QUEUE;

/* Global Structure ----------------------------------------------------------*/
extern PACKET_QUEUE 		packet_Queue[MAX_PKT_QUEUE];
/* Global Variable -----------------------------------------------------------*/
extern uint8_t 					_Queue_counter;


void vPKTQUEUE_Initial(PACKET_QUEUE *packetQueue);
uint8_t ucPKTQUEUE_Add(PACKET_QUEUE *packetQueue, uint8_t *queueCounter, uint8_t id, uint8_t *packet, uint8_t length, uint16_t timeout);
uint8_t ucPKTQUEUE_Remove(PACKET_QUEUE *packetQueue, uint8_t *queueCounter, uint8_t id);
void vPKTQUEUE_Retransmit(PACKET_QUEUE *packetQueue, uint8_t *queueCounter);
uint8_t ucPKTQUEUE_GenerateID(PACKET_QUEUE *packetQueue);


#endif /*_INT_STM32F4_PACKETQUEUE_H*/


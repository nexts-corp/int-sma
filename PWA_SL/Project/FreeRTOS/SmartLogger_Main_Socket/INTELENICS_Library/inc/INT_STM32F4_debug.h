/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_debug.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	22-May-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#ifndef _INT_STM32F4_DEBUG_H
#define _INT_STM32F4_DEBUG_H
 
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"
 
 /* Debug Level ---------------------------------------------------------------*/
#define MAJOR_ERR							0x01
#define MINOR_ERR							0x02
#define DESCRIBE							0x08
#define WARNING								0x10
#define INFO									0x20
#define DETAIL								0x40
#define GSM_DESC							0x80
#define ALWAYS								0x01

#define LEV_DEB								0x33

#define SEMPHR_WAIT						2000
#define REP_INTV							30

#define DEBUG_LOG							"DEBUG.LOG"

typedef struct {
	char 				message[256];
	uint32_t		timestamp;
}DEBUG_MESSAGE;
 
extern xSemaphoreHandle 		xSemaphore;
 
void vDEBUG_SendData(uint8_t *data, uint16_t length);
void printDebug(uint8_t Debug_ID, char *fmtstr, ...);
void printDebug_Semphr(uint8_t Debug_ID, char *fmtstr, ...);
void print_payload(uint8_t Debug_ID, const unsigned char *payload, int len);
 
#endif /*_INT_STM32F4_DEBUG_H*/
 
 
 
 


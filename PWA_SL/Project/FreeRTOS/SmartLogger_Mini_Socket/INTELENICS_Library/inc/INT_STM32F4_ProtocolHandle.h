/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolHandle.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	16-June-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef _INT_STM32F4_PROTOCOLHANDLE_H
#define _INT_STM32F4_PROTOCOLHANDLE_H

#include <stdint.h>

#define CONF_INDEX_CNFG					0
#define CONF_INDEX_SPEC					1

#define FUNC_RECV								1
#define FUNC_NOTRECV						0

#define SET_DEFAULT_INFO				(uint8_t)0x11
#define LOAD_INFO								(uint8_t)0x12
#define SET_DEFAULT_CONF				(uint8_t)0x21
#define LOAD_CONF								(uint8_t)0x22

#define LED_ERR_TIME						5

uint8_t vPTCHANDLE_TimeSync(uint8_t *buf);
void vPTCHANDLE_Heartbeat(uint8_t func);
uint8_t ucPTCHANDLE_RemoteCtrl(uint8_t cid, uint8_t *value);
uint8_t vPTCHANDLE_ConfigRead(uint8_t cnfg, uint8_t *confSpec, uint16_t *specLen);
uint8_t ucPTCHANDLE_ConfigWrite(uint8_t *confSpec, uint8_t src);
uint8_t vPTCHANDLE_LogManipulate(uint8_t *date, uint8_t fid);


#endif /*_INT_STM32F4_PROTOCOLHANDLE_H*/

/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolParser.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	21-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_ProtocolParser_H
#define _INT_STM32F4_ProtocolParser_H

#include <stdint.h>
#include "SL_ConfigStruct.h"
#include "INT_STM32F4_ProtocolComm.h"

#define INDEX_FRAMETYPE				21
#define INDEX_TIMESTAMP				22

/* Data&Status Report */
#define INDEX_NUMSTAT					35

/* Event Report */
#define INDEX_EID							35
#define INDEX_EDAT						36

/* Configuration Management */
#define INDEX_ACCD						30
#define INDEX_CNFG						31
#define INDEX_SPEC						32
#define INDEX_UPDATE_TIMSTAMP	31
#define INDEX_UPDATE_CNFG			35
#define INDEX_UPDATE_SPEC			36


uint8_t		ucPTCPARSER_getFrameType(uint8_t *packet);
uint32_t	uiPTCPARSER_getTimestamp(uint8_t *packet);
uint8_t		ucPTCPARSER_IsolateDataPacket(uint8_t *packet, uint8_t packetLen, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data);
uint8_t 	ucPTCPARSER_IsolateEventPacket(uint8_t *packet, uint8_t packetLen, uint8_t *eid, uint32_t *edata);
uint8_t 	ucPTCPARSER_IsolateConfigPacket(uint8_t *packet, uint8_t packetLen, uint8_t *accd, uint32_t *timestamp, uint8_t *cnfg, uint8_t *spec, uint16_t *specLen);

uint8_t 	PTCPARSER_ValidateConfigOperating(STCONFIG_OPERATING *st_config);
uint8_t 	PTCPARSER_ValidateConfigBuzzer(STCONFIG_BUZZER *st_config);
uint8_t 	PTCPARSER_ValidateConfigWorking(STCONFIG_WORKING *st_config);
uint8_t 	PTCPARSER_ValidateConfigNetwork(STCONFIG_NETWORK *st_config);
uint8_t 	PTCPARSER_ValidateConfigDev(STCONFIG_DEV *st_config);


	
#endif /*_INT_STM32F4_ProtocolParser_H*/	


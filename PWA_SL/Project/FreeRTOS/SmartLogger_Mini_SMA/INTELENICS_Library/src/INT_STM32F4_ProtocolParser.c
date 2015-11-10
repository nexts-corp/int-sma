/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolParser.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	21-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "INT_STM32F4_ProtocolParser.h"
#include "INT_STM32F4_debug.h"
#include "SL_ConfigStruct.h"
#include "INT_STM32F4_rtc.h"

/*==========================================================================================*/
/**
  * @brief  This function validate Operating_Configuration.
	* 				if packet config(buf) is correct -> copy config to st_config 
  * @param  
  * @retval Status
  */
uint8_t PTCPARSER_ValidateConfigDev(STCONFIG_DEV *st_config) {
	
	if(st_config->transmitRetry > 10) {
		return 1;
	}
	if((st_config->retransmitInterval < 10) || (st_config->retransmitInterval > 60)) {
		return 2;
	}
	if((st_config->heartbeatInterval < 30) || (st_config->heartbeatInterval > 300)) {
		return 3;
	}
	if(st_config->heartbeatTimeout > 10) {
		return 4;
	}
	if((st_config->adc_Vref <= 0.0) || (st_config->adc_Vref > 5.0)) {
		return 5;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function validate Operating_Configuration.
	* 				if packet config(buf) is correct -> copy config to st_config 
  * @param  
  * @retval Status
  */
uint8_t PTCPARSER_ValidateConfigOperating(STCONFIG_OPERATING *st_config) {

	if((st_config->deviceEnable!=CONF_DISABLE)&&(st_config->deviceEnable!=CONF_ENABLE)) {
		return 1;
	}
	if((st_config->alarmEnable!=CONF_DISABLE)&&(st_config->alarmEnable!=CONF_ENABLE)) {
		return 2;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function validate Buzzer_Configuration.
	* 				if packet config(buf) is correct -> copy config to st_config 
  * @param  
  * @retval Status
  */
uint8_t PTCPARSER_ValidateConfigBuzzer(STCONFIG_BUZZER *st_config) {
	
	if((st_config->buzzerEnable != CONF_DISABLE) && (st_config->buzzerEnable != CONF_ENABLE)) {
		return 1;
	}
	if(st_config->numberOfSrc > BUZZ_MAX_EVSRC) {
		return 2;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function validate Working_Configuration.
	* 				if packet config(buf) is correct -> copy config to st_config 
  * @param  
  * @retval Status
  */
uint8_t PTCPARSER_ValidateConfigWorking(STCONFIG_WORKING *st_config) {

	if((st_config->minBattery < LOWeLIMIT_MINBATT) || (st_config->minBattery > UPpLIMIT_MINBATT)) {
		return 1;
	}
	if((st_config->recordMode != ROLLOVER_MODE) && (st_config->recordMode != STOPWHENFULL_MODE)) {
		return 2;
	}
	if((st_config->transmitInterval < LOWeLIMIT_RECINVL) || (st_config->transmitInterval > UPpLIMIT_RECINVL)) {
		return 3;
	}
	if((st_config->samplingInterval < LOWeLIMIT_SAMPINVL) || (st_config->samplingInterval > UPpLIMIT_SAMPINVL)) {
		return 4;
	}
	if((st_config->commDataLimit <= 0)) {
		return 5;
	}

	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function validate Network_Configuration.
	* 				if packet config(buf) is correct -> copy config to st_config 
  * @param  
  * @retval Status
  */
uint8_t PTCPARSER_ValidateConfigNetwork(STCONFIG_NETWORK *st_config) {
	
	if((st_config->serverAddrType != IP_ADDR) && (st_config->serverAddrType != HOST_NAME)) {
		return 2;
	}
	if((st_config->serverAddrLen < LOWeLIMIT_ADDRLEN) || (st_config->serverAddrLen > UPpLIMIT_ADDRLEN)) {
		return 3;
	}

	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function get FrameType of DataPacket.
  * @param  Data Packet.
  * @retval Frame Type.
  */
uint8_t	ucPTCPARSER_getFrameType(uint8_t *packet) {
	return packet[INDEX_FRAMETYPE];
}
/*==========================================================================================*/
/**
  * @brief  This function get Timestamp of DataPacket.
  * @param  Data Packet.
  * @retval Frame Type.
  */
uint32_t uiPTCPARSER_getTimestamp(uint8_t *packet) {
	
	uint32_t timestamp = 0;
	memcpy(&timestamp, &packet[INDEX_TIMESTAMP], sizeof(uint32_t));
	return timestamp;
}
/*==========================================================================================*/
/**
  * @brief  This function isolate Data&Status Cmd Packet.
  * @param  
  * @retval Status.
  */
uint8_t	ucPTCPARSER_IsolateDataPacket(uint8_t *packet, uint8_t packetLen, uint8_t *numStat, STATUS_SET *status, uint8_t *numData, DATA_SET *data) {
	
	uint8_t 	index 		= 0;
	uint8_t 	statSize 	= 0;
	uint8_t 	dataSize 	= 0;
	uint8_t 	*bufstat, *bufdata;
	
	(*numStat) = 0;
	(*numData) = 0;
	
	index = INDEX_FRAMETYPE;
	
	if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ) {
		printDebug( DETAIL, "<IsolateDataPacket>: Isolate : %d\r\n", packetLen );
		print_payload( DETAIL, packet, packetLen );
		printDebug(DETAIL, "\r\n");
		xSemaphoreGive(xSemaphore);
	}
	
	while(index<packetLen) {
		if(packet[index] == STATREPC) {
			index++;
			statSize = packet[index++];
			memcpy(&bufstat[(*numStat)*statSize], &packet[index], statSize);
			(*numStat)++;
			index += sizeof(STATUS_SET);
		}else if(packet[index] == DATAREPC) {
			index++;
			dataSize = packet[index++];
			memcpy(&bufdata[(*numData)*dataSize], &packet[index], dataSize);
			(*numData)++;
			index += sizeof(DATA_SET);
		}else {
			index++;
		}
	}
	statSize = (*numStat)*sizeof(STATUS_SET);
	memcpy(status, bufstat, statSize);
	dataSize = (*numData)*sizeof(DATA_SET);
	memcpy(data, bufdata, dataSize);
	
	printDebug( DETAIL, "<IsolateDataPacket>: stat : %d\r\n", statSize );
	print_payload( DETAIL, bufstat, statSize );
	printDebug(DETAIL, "\r\n");
	printDebug( DETAIL, "<IsolateDataPacket>: data : %d\r\n", dataSize );
	print_payload( DETAIL, bufdata, dataSize );
	printDebug(DETAIL, "\r\n");
	
// 	*numStat = packet[index++];
// 	statSize = (*numStat)*sizeof(STATUS_SET);
// 	memcpy(status, &packet[index], statSize);
// 	index += statSize;
// 	*numData = packet[index++];
// 	dataSize = (*numData)*sizeof(DATA_SET);
// 	memcpy(data, &packet[index++], dataSize);
// 	if(index > packetLen) {
// 		return 1;
// 	}
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function isolate Event Cmd Packet.
  * @param  
  * @retval Status.
  */
uint8_t ucPTCPARSER_IsolateEventPacket(uint8_t *packet, uint8_t packetLen, uint8_t *eid, uint32_t *edata) {
	
	*eid = packet[INDEX_EID];
	memcpy(edata, &packet[INDEX_EDAT], sizeof(uint32_t));
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function isolate Configuration Cmd Packet.
  * @param  
  * @retval Status.
  */
uint8_t ucPTCPARSER_IsolateConfigPacket(uint8_t *packet, uint8_t packetLen, uint8_t *accd, uint32_t *timestamp, uint8_t *cnfg, uint8_t *spec, uint16_t *specLen) {
	
	int16_t spec_len = 0;
	
	*accd = packet[INDEX_ACCD];
	if(packet[INDEX_ACCD] == CONF_ACCD_UPDATE) {
		memcpy(timestamp, &packet[INDEX_UPDATE_TIMSTAMP], sizeof(uint32_t));
		*cnfg = packet[INDEX_UPDATE_CNFG];
		spec_len = packetLen-20-17;    // PacketLength-HEADER-FID-TYP-TGID-ACCD-TIMESTAMP-CNFG-CHECKSUM
		if(spec_len < 0) {
			return 1;
		}
		*specLen = (uint16_t)spec_len;
		memcpy(spec, &packet[INDEX_UPDATE_SPEC], spec_len);
	}else {
		*cnfg = packet[INDEX_CNFG];
		spec_len = packetLen-20-13;    // PacketLength-HEADER-FID-TYP-TGID-ACCD-CNFG-CHECKSUM
		if(spec_len < 0) {
			return 1;
		}
		*specLen = (uint16_t)spec_len;
		memcpy(spec, &packet[INDEX_SPEC], spec_len);
	}
	
	return 0;
}
/*==========================================================================================*/














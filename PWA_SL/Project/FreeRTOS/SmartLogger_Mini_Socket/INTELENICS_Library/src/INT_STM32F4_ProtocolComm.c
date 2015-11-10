/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolComm.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	21-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	

#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "FreeRTOS.h"

#include "main.h"
#include "stm32f4_SDCard.h"
#include "SD_Task.h"
#include "SL_PinDefine.h"
#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_ProtocolParser.h"
#include "INT_STM32F4_ProtocolHandle.h"
#include "INT_STM32F4_PacketQueue.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_Usart.h"
#include "INT_STM32F4_ConfigSD.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_aes128.h"
#include "INT_STM32F4_rtc.h"

#include "INT_STM32F4_GSM.h"

uint8_t IN420MA_CH[] = {IN420MA_CH1,IN420MA_CH2,IN420MA_CH3,IN420MA_CH4,IN420MA_CH5,IN420MA_CH6};

const uint8_t CONF_CNFG_420SENSORn[] = {CONF_CNFG_420SENSOR1, CONF_CNFG_420SENSOR2, CONF_CNFG_420SENSOR3, CONF_CNFG_420SENSOR4,
																				CONF_CNFG_420SENSOR5, CONF_CNFG_420SENSOR6};

const uint8_t CONF_CNFG_ADCSENSORn[] = {CONF_CNFG_ADCSENSOR1, CONF_CNFG_ADCSENSOR2};

const uint8_t CONF_CNFG_CAL420SENn[] = {CONF_CNFG_CAL420SEN1, CONF_CNFG_CAL420SEN2, CONF_CNFG_CAL420SEN3, CONF_CNFG_CAL420SEN4,
																				CONF_CNFG_CAL420SEN5, CONF_CNFG_CAL420SEN6};

const uint8_t CONF_CNFG_420SENsALn[] = {CONF_CNFG_420SENsAL1,CONF_CNFG_420SENsAL2,CONF_CNFG_420SENsAL3,CONF_CNFG_420SENsAL4,
																				CONF_CNFG_420SENsAL5,CONF_CNFG_420SENsAL6};

const uint8_t CONF_CNFG_ADCSENsALn[] = {CONF_CNFG_420SENsAL1,CONF_CNFG_420SENsAL2};

const uint8_t CONF_CNFG_DINn[] = {CONF_CNFG_DIN1,CONF_CNFG_DIN2,CONF_CNFG_DIN3,CONF_CNFG_DIN4,CONF_CNFG_DIN5,CONF_CNFG_DIN6,
																	CONF_CNFG_DIN7,CONF_CNFG_DIN8,CONF_CNFG_DIN9,CONF_CNFG_DIN10,CONF_CNFG_DIN11,CONF_CNFG_DIN12};

extern xQueueHandle 	Queue_PacketHTTPToGSM;
extern xQueueHandle 	Queue_PacketToGSM;
extern 	xQueueHandle 	Queue_PacketFromGSM;
extern __IO uint32_t 	_heartbeatCounter;
extern __IO uint8_t		_SendRegisCounter;
extern __IO uint8_t		_SendJoinCounter;
extern uint32_t				_JoinPendingTimeout;
extern uint32_t				_RegisPendingTimeout;
extern __IO uint32_t		_GSMCounter;
extern __IO uint8_t			_ConfigCmdFID;

uint8_t 	_PENDING_ID[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t		_DataLog_WaitID = 0x00;
uint8_t		_EventLog_WaitID = 0x00;
uint8_t		_ConfigLog_WaitID = 0x00;

/*==========================================================================================*/
/**
  * @brief  Receive Data Packet
  * @param  None
  * @retval Return protocol function code
  */
PTC_RES ucPROCOMM_ReceivePacket(void) {

	uint8_t 		recvPacket[RX_BUFFER_SIZE1];           
	uint16_t 		size 		= 0;           
	uint16_t 		offset 	= 0;
	uint16_t 		length 	= 0;
	uint16_t 		total 	= 0;
	uint8_t 		i;
				
	size = usUSART_ReadUart1(&recvPacket[0], (RX_BUFFER_SIZE1-1), 5);
	if(size <= 0) {
		/* Not have receive data packet */
		return PTC_PKT_EMPTY;
	}
	else{						/* command GSM module config */
// 		recvPacket[size] 		= '\r';
// 		recvPacket[size+1]	= '\n';
// 		vGSM_SendATcommand( ( char* )recvPacket, size+1 );
// 		recvPacket[size+2]	= '\0';
// 		printDebug(INFO, "Receive Packet(%s): \r\n", recvPacket);
// 		memcpy( ( char* )recvPacket, "", 100 );
	}
	
	if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
		printDebug(INFO, "Receive Packet(%d): \r\n", size);
//		print_payload(INFO, recvPacket, size);
		for(i=0;i<size;i++) {
			printDebug(INFO, "%02X ", recvPacket[i]);
		}
		printDebug(INFO, "\r\n");
		xSemaphoreGive(xSemaphore);
	}
	
	
	
	do {
		offset += strcspn((const char*)&recvPacket[offset], "\x7E");                      				/* seach for header */
		if(offset == size) {                 
			/* Not found Start Delimiter 0x7E */
			break;
		}
		length = (recvPacket[offset+1] & 0xFFFF) << 8;                                       			/* check packet length (MSB) */
		length |= recvPacket[offset+2];                                                      			/* check packet length (LSB) */
		total += length;                                                                     			/* update total analyse */ 
		if (total > size) {                                                                				/* check length validation */
			offset++;
			total = offset;                                                               					/* roll back total analyse */                        
			/* invalid length */
			printDebug_Semphr(INFO, "<PROCOMM_ReceivePacket>: Invalid length %d \r\n", total);
			continue;
		}                             
			
		if(ucPROCOMM_Checksum(&recvPacket[offset+3], length) != recvPacket[offset+length+3]) {   	/* checksum error detection */                  
			/* Incorrect checksum */
			offset++;
			total = offset;                                                               					/* roll back total analyse */  
			printDebug_Semphr(INFO, "<PROCOMM_ReceivePacket>: Incorrect checksum %d\r\n", total);
		}else {                                                                             			/* got a valid packet */     
			xPROCOMM_ProcessPacket(&recvPacket[offset+3], length, DEBUG_PORT);                      /* analyse API-specific Structure */ 
			offset += (length+4);
			total 	= offset; 
		}
	}while(total < size);
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Process INT_COMM_PROTOCOL Data Packet
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_ProcessPacket(uint8_t *buf, uint16_t length, uint8_t source) {

	uint8_t 	frameID;																																/* Frame id */
	uint8_t 	frameType;																															/* Frame type */
	uint16_t 	frameLen;           																										/* Frame data length */
	uint16_t  i;
	uint8_t		configSpec[162]; 					// Reserve for Max Specific_Struct Length (Properties_Config:161 bytes)
	uint16_t	configSpecLen = 0;
	uint8_t 	challenge[16];
	uint8_t		save_regis	= 0;
	uint8_t		res = 0;
	
	PTC_RES					ptc_res;
	ST_LOG_DATA   	log_data;
	ST_LOG_EVENT   	log_event;
	ST_LOG_CONFIG  	log_config;
	
//	if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
//		printDebug(INFO, "Receive Packet(%d): \r\n", length);
//		print_payload(INFO, buf, length);
//		xSemaphoreGive(xSemaphore);
//	}
	
	frameID     = buf[INDEX2_FRAMEID]; 
	frameType   = buf[INDEX2_FRAMETYPE];
	/* 									ProtoVer  SendAddr  RecvAddr  FrameID FrameType */
	frameLen 		= (length  -1    -ADDR_LEN -ADDR_LEN		-1			-1);

	// Check Receiver Address
	if(source == GSM_PORT) {
		if(memcmp(&buf[INDEX2_RECVADDR], &struct_info.serialNumber[0], ADDR_LEN) != 0) {
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(MINOR_ERR, "<PROCOMM_ReceivePacket> Invalid Receiver Address : ");
				for(i = INDEX2_RECVADDR; i < (INDEX2_RECVADDR+ADDR_LEN); i++) {
					printDebug(MINOR_ERR, "%02X ", buf[i]);
				}
				printDebug(MINOR_ERR, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			return PTC_INVAL_ADDR;
		}
	}
		
	switch(frameType) {
	/*------------------------------- Command Packet -------------------------------*/
	case CONFMANC :                                                       						/* [0x4A] Configuration Management CMD */
		printDebug_Semphr(ALWAYS, "-->>> CONFIGURATION_MANAGEMENT CMD : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
		if(buf[INDEX2_CONFIG_ACCD] == CONF_ACCD_READ) {							/* ACCD: READ */
			if(vPTCHANDLE_ConfigRead(buf[INDEX2_CONFIG_CNFG], configSpec, &configSpecLen)) {
				/* Handle Failed */
				printDebug_Semphr(MINOR_ERR, "vPTCHANDLE_ConfigRead Failed. \r\n");
				ptc_res = xPROCOMM_SendConfigManageACK(buf[INDEX2_FRAMEID],CONF_ACCD_READ,buf[INDEX2_CONFIG_CNFG],STAT_ERR,NULL,0,source);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}else {
				/* Handle Success */
				ptc_res = xPROCOMM_SendConfigManageACK(buf[INDEX2_FRAMEID],CONF_ACCD_READ,buf[INDEX2_CONFIG_CNFG],STAT_SUCCESS,configSpec,configSpecLen,source);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}
		}else if(buf[INDEX2_CONFIG_ACCD] == CONF_ACCD_WRITE) {			/* ACCD: WRITE */
			_ConfigCmdFID = frameID;
			res = ucPTCHANDLE_ConfigWrite(&buf[INDEX2_CONFIG_CNFG], source);
			if(res == 0) {
				/* Handle Success */
				ptc_res = xPROCOMM_SendConfigManageACK(buf[INDEX2_FRAMEID],CONF_ACCD_WRITE,buf[INDEX2_CONFIG_CNFG],STAT_SUCCESS,NULL,0,source);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}else if(res == CONF_CNFG_CMD) {
				/* Special Case */ 
			}else {
				/* Handle Failed */
				printDebug_Semphr(MINOR_ERR, "vPTCHANDLE_ConfigWrite Failed. \r\n");
				ptc_res = xPROCOMM_SendConfigManageACK(buf[INDEX2_FRAMEID],CONF_ACCD_WRITE,buf[INDEX2_CONFIG_CNFG],STAT_ERR,NULL,0,source);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}
		}else if(buf[INDEX2_CONFIG_ACCD] == CONF_ACCD_CONFIRM) {		/* ACCD: CONFIRM */
			return PTC_TYP_NREF;
		}else if(buf[INDEX2_CONFIG_ACCD] == CONF_ACCD_UPDATE) {			/* ACCD: UPDATE */
			return PTC_TYP_NREF;
		}else {
			return PTC_TYP_NREF;
		}
		break;
	case LOGMANIC :																																		/* [0x6A] Log Manipulation CMD */
		printDebug_Semphr(ALWAYS, "-->>> LOG_MANIPULATION CMD : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
		if(buf[INDEX2_LOG_ACCD] == LOG_ACCD_READDATA) {							/* ACCD: READ DATA LOG */
			vPTCHANDLE_LogManipulate(&buf[INDEX2_LOG_DLOG], frameID);
		}
		break;
	case NETMAINC :                                                       						/* [0x7A] Network Maintenance CMD */
		printDebug_Semphr(ALWAYS, "-->>> NETWORK_MAINTENANCE CMD : CMD[%02X]\r\n", buf[INDEX2_NETMT_CMD]);
		if(buf[INDEX2_NETMT_CMD] == TIMESYNC) {
			if(vPTCHANDLE_TimeSync(&buf[INDEX2_NETMT_TSYNC])) {
				/* fpPROCOMM_HandleTimeSync Failed */
				printDebug_Semphr(MINOR_ERR, "fpPROCOMM_HandleTimeSync Failed. \r\n");
				return PTC_HANDLE_FAILED;
			}
			ptc_res = xPROCOMM_SendNetworkMaintACK(buf[INDEX2_FRAMEID],buf[INDEX2_NETMT_CMD],STAT_SUCCESS,NULL,0);
			if(ptc_res != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "X<<-- NETWORK_MAINTENANCE ACK Failed Err(PTC_%d) \r\n", ptc_res);
			}
		}else if(buf[INDEX2_NETMT_CMD] == PENDING_APPRV) {
			i = 0;
			if(memcmp(&buf[INDEX2_NETMT_APPPENDING], _PENDING_ID, 8) == 0) {
				if(memcmp(&buf[INDEX2_NETMT_APPSERIAL], &struct_info.serialNumber[0], 8) == 0) {
					_Communication_Status &= ~COMMU_REG_PENDING;		// Update Comm Status
					_Communication_Status |= COMMU_REGISTERED;   		// Update Comm Status
					save_regis = CHECK_REGISTERED;
//					if(!xQueueSend(Queue_SaveRegis, &save_regis, 0)) {
//						printDebug_Semphr(MINOR_ERR, "<PROCOMM_ProcessPacket>: Send Queue_SaveRegis failed. \r\n");
//					}
					printDebug_Semphr(DESCRIBE, "<PROCOMM_ProcessPacket>: Registration Success. \r\n");
					i = 1;
				}
			}
			if(i) {
				ptc_res = xPROCOMM_SendNetworkMaintACK(buf[INDEX2_FRAMEID],buf[INDEX2_NETMT_CMD],STAT_SUCCESS,NULL,0);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- NETWORK_MAINTENANCE ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}else {
				_Communication_Status &= ~COMMU_REG_PENDING;		// Update Comm Status
				printDebug_Semphr(MINOR_ERR, "<PROCOMM_ProcessPacket>: Registration Failed. \r\n");
				ptc_res = xPROCOMM_SendNetworkMaintACK(buf[INDEX2_FRAMEID],buf[INDEX2_NETMT_CMD],STAT_ERR,NULL,0);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- NETWORK_MAINTENANCE ACK Failed Err(PTC_%d) \r\n", ptc_res);
				}
			}
		}else if(buf[INDEX2_NETMT_CMD] == CHAP_CMD) {
			ucTIMER_SetTimer(&_JoinPendingTimeout, 120);			// Set join panding timeout
			_Communication_Status |= COMMU_JOIN_PENDING;   		// Update Comm Status
			memcpy(challenge, &buf[INDEX2_NETMT_CHALLENGE], sizeof(challenge));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(DETAIL, "<PROCOMM_ProcessPacket> CHALLENGE: ");
				for(i = 0; i < sizeof(challenge); i++) {
					printDebug(DETAIL, "%02X ", challenge[i]);
				}
				printDebug(DETAIL, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			
			/* AES-128 Encryption with Private Key */
			vAES_encrypt(challenge, (unsigned char *)_Private_Key);
			
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(DETAIL, "<PROCOMM_ProcessPacket> RESPONSE: ");
				for(i = 0; i < sizeof(challenge); i++) {
					printDebug(DETAIL, "%02X ", challenge[i]);
				}
				printDebug(DETAIL, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			/* Send CHAP Response to Server */
			ptc_res = xPROCOMM_SendNetworkMaintACK(buf[INDEX2_FRAMEID],buf[INDEX2_NETMT_CMD],STAT_SUCCESS,challenge,sizeof(challenge));
			if(ptc_res != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "X<<-- NETWORK_MAINTENANCE ACK Failed Err(PTC_%d) \r\n", ptc_res);
			}
		}else {
			return PTC_TYP_NREF;
		}
		break;	
	case RMDCTRLC :                                                       						/* [0x8A] Remote Device Control CMD  */
		printDebug_Semphr(ALWAYS, "-->>> REMOTE_DEVICE_CONTROL CMD : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
		if(ucPTCHANDLE_RemoteCtrl(buf[INDEX2_REMOTE_CID], &buf[INDEX2_REMOTE_VAL])) {
			/* Handle Failed */
			printDebug_Semphr(MINOR_ERR, "fpPROCOMM_HandleRemoteCtrl Failed. \r\n");
			ptc_res = xPROCOMM_SendRemoteCtrlACK(buf[INDEX2_FRAMEID],buf[INDEX2_REMOTE_CID],STAT_ERR,source);
			if(ptc_res != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "X<<-- REMOTE_DEVICE_CONTROL ACK Failed Err(PTC_%d) \r\n", ptc_res);
			}
		}else {
			/* Handle Success */
			ptc_res = xPROCOMM_SendRemoteCtrlACK(buf[INDEX2_FRAMEID],buf[INDEX2_REMOTE_CID],STAT_SUCCESS,source);
			if(ptc_res != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "X<<-- REMOTE_DEVICE_CONTROL ACK Failed Err(PTC_%d) \r\n", ptc_res);
			}
		}
		break;
			
	/*------------------------------- Acknowledgment Packet -------------------------------*/
	case EVENREPA :                                                        						/* [0x3B] Event Report ACK */
		printDebug_Semphr(ALWAYS, "-->>> EVENT_REPORT ACK : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
		/* If EventLog ACK, Remove log wait id */
		if((_EventLog_WaitID != 0x00) && (_EventLog_WaitID == frameID)) {
			_EventLog_WaitID = 0x00;
			log_event.cmd = LOG_CMD_SETiNDEX;
			if(!xQueueSend(Queue_EventLogToSD, &log_event, 0)) {
				printDebug_Semphr(MINOR_ERR, "<PROCOMM_ProcessPacket>: Send Queue_EventLogToSD failed. \r\n");
			}
		}
		if(ucPKTQUEUE_Remove(packet_Queue, &_Queue_counter, frameID)) {
			/* Remove packet from queue failed */
			return PTC_HANDLE_FAILED;
		}
		break;
	case CONFMANA :                                                        						/* [0x4B] Configuration Management ACK */ 
		printDebug_Semphr(ALWAYS, "-->>> CONFIGURATION_MANAGEMENT ACK : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
		/* If ConfigLog ACK, Remove log wait id */
		if((_ConfigLog_WaitID != 0x00) && (_ConfigLog_WaitID == frameID)) {
			_ConfigLog_WaitID = 0x00;
			log_config.cmd = LOG_CMD_SETiNDEX;
			if(!xQueueSend(Queue_ConfigLogToSD, &log_config, 0)) {
				printDebug_Semphr(MINOR_ERR, "<PROCOMM_ProcessPacket>: Send Queue_ConfigLogToSD failed. \r\n");
			}
		}
		if(ucPKTQUEUE_Remove(packet_Queue, &_Queue_counter, frameID)) {
			/* Remove packet from queue failed */
			return PTC_HANDLE_FAILED;
		}
		break;
	case NETMAINA :                                                        						/* [0x7B] Network Maintenance ACK */        
		printDebug_Semphr(ALWAYS, "-->>> NETWORK_MAINTENANCE ACK : CMD[%02X]\r\n", buf[INDEX2_NETMT_CMD]);
		if(buf[INDEX2_NETMT_CMD] == HEARTBEAT) {
			/* Reset Heartbeat ACK Timer Counter */
			_heartbeatCounter = 0;
			/* Reset GSM Timer Counter */
			_GSMCounter				= 0;
		}else if(buf[INDEX2_NETMT_CMD] == SELF_REGIS) {
			_SendRegisCounter = 0;
			if(buf[INDEX2_NETMT_STAT] == STAT_SUCCESS) {
				memcpy(_PENDING_ID, &buf[INDEX2_NETMT_REGPENDING], sizeof(_PENDING_ID));
				ucTIMER_SetTimer(&_RegisPendingTimeout, 120);		// Set regis panding timeout
				_Communication_Status |= COMMU_REG_PENDING;   	// Update Comm Status
				if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
					printDebug(INFO, "<PROCOMM_ProcessPacket> Pending ID: ");
					for(i = 0; i < sizeof(_PENDING_ID); i++) {
						printDebug(INFO, "%02X ", _PENDING_ID[i]);
					}
					printDebug(INFO, "\r\n");
					xSemaphoreGive(xSemaphore);
				}
			}else {
				printDebug(MINOR_ERR, "<PROCOMM_ProcessPacket> Self-Registration ACK Failed. \r\n");
			}
		}else if(buf[INDEX2_NETMT_CMD] == JOIN_NETWORK) {
			_SendJoinCounter = 0;
			_Communication_Status &= ~COMMU_JOIN_PENDING;   	// Update Comm Status
			if(buf[INDEX2_NETMT_STAT] == STAT_SUCCESS) {
				_Communication_Status |= COMMU_JOINED;   				// Update Comm Status
				LED_CONNSERVER_GREEN;														// Update CONNSERVER LED Indicator
			}else {
				printDebug(MINOR_ERR, "<PROCOMM_ProcessPacket> Join-Network ACK Failed. \r\n");
			}
		}else {
			return PTC_TYP_NREF;
		}
		break;
	case DNSTREPA :                                                        						/* [0x9B] Data&Status Report ACK */  
		printDebug_Semphr(ALWAYS, "-->>> DATA&STATUS_REPORT ACK : FID[%02X]\r\n", buf[INDEX2_FRAMEID]);
	
		/* If DataLog ACK, Remove log wait id */
		if((_DataLog_WaitID != 0x00) && (_DataLog_WaitID == frameID)) {
			_DataLog_WaitID = 0x00;
			log_data.cmd = LOG_CMD_SETiNDEX;
			if(!xQueueSend(Queue_DataLogToSD, &log_data, 0)) {
				printDebug_Semphr(MINOR_ERR, "<PROCOMM_ProcessPacket>: Send Queue_DataLogToSD failed. \r\n");
			}
		}
		
		if(ucPKTQUEUE_Remove(packet_Queue, &_Queue_counter, frameID)) {
			/* Remove packet from queue failed */
			return PTC_HANDLE_FAILED;
		}
		break;
	case PROTOERROR :                                                      						/* [0xEB] Protocol Error Feedback */      
		printDebug_Semphr(ALWAYS, "\r\n-->>> PROTOCOL_ERROR_FEEDBACK_ACK : FRAME ID[%02X] ERROR ID[0x%02X]\r\n\r\n", frameID, buf[31] );                                 
		break;

	default:                      
		printDebug_Semphr(ALWAYS, "\r\n-->>> RECEIVE INT_COMM> Frame Type not define or not reference yet (0x%02X)!!\r\n\r\n", frameType);
//		INT_sendProtocolErrorFeedbackAck(frameID, INVALID_FTYPE);                
		return PTC_TYP_NDEF;
	}
	
	return PTC_OK;     
}
/*==========================================================================================*/
/**
  * @brief  Send Configuration_Management Cmd
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendConfigManageCMD(uint8_t accd, uint8_t recordType, uint32_t timestamp, uint8_t cnfg, void *spec, uint16_t specLen, uint32_t timeout) {
	
	uint8_t 	frameData[MAX_PKT_LEN];
	uint8_t 	frameLen 		= 0; 
	uint8_t 	frameID			= 0;  
	uint8_t 	index 			= 0;
	uint8_t		i;
	PTC_RES		ptc_res;
	
	// Packet size   ***include timestamp in accd:update
	if(accd == CONF_ACCD_UPDATE) {
		// 				FID TYP    TGID   ACCD TIMESTAMP CNFG SPEC_LEN
		frameLen = 1  +1  +ADDR_LEN	 +1  		+4  		+1  +specLen; 
	}else {
		// 				FID TYP    TGID   ACCD CNFG SPEC_LEN
		frameLen = 1  +1  +ADDR_LEN	 +1   +1  +specLen; 
	}

	frameID = ucPKTQUEUE_GenerateID(packet_Queue);
	printDebug_Semphr(ALWAYS, "<<<-- CONFIGURATION_MANAGEMENT CMD : FID[%02X]\r\n", frameID);
	
	/* Add config_log wait frame_id */
	if(recordType == RECORD_LOG) {
		_ConfigLog_WaitID = frameID;
	}
												
	frameData[index++] = frameID;                                           // Frame ID
	frameData[index++] = CONFMANC;                                         	// Frame Type                             
	for(i = 0; i < ADDR_LEN; i++) {                                         /* Source ID */
		frameData[index++]  = struct_info.serialNumber[i];
	}
	frameData[index++] = accd;                                         			// Frame Type  
	if(accd == CONF_ACCD_UPDATE) {																					// Timestamp (Only ACCD:UPDATE)
		memcpy(&frameData[index], &timestamp, sizeof(timestamp));           
		index += sizeof(timestamp);
	}
	frameData[index++] = cnfg;                                         			// Frame Type  
	if(specLen > 0) {
		memcpy(&frameData[index], spec, specLen);                  						// Specific Structure
		index += specLen;  
	}
	
	ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, timeout);
	if(ptc_res != PTC_OK) {               
		return PTC_SEND_FAIL;
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send Log Manipulattion Ack
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendLogManipulateACK(uint8_t frameID, uint8_t accd, uint8_t stat, uint8_t dest) {
	
	uint8_t 	frameData[4];
	uint8_t 	frameLen 		= 0;   
	uint8_t 	index 			= 0;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- LOG_MANIPULATION ACK [%s] : FID[%02X]\r\n", (dest==GSM_PORT)?"GSM":"DEBUG", frameID);

	// 				FID TYP ACCD STAT 
	frameLen = 1  +1  +1   +1;
												
	frameData[index++] = frameID;                                           // Frame ID
	frameData[index++] = LOGMANIA;                                         	// Frame Type   
	frameData[index++] = accd;                                              // ACCD
	frameData[index++] = stat;                                              // STAT
	
	if(dest == GSM_PORT) {
		ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}else if(dest == DEBUG_PORT) {
		ptc_res = xPROCOMM_SendPacketToDebug(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send NetworkMaintenance Ack
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendNetworkMaintACK(uint8_t frameID, uint8_t cmd, uint8_t stat, uint8_t *spec, uint8_t specLen) {
	
	uint8_t 	frameData[MAX_PKT_LEN];
	uint8_t 	frameLen 		= 0;   
	uint8_t 	index 			= 0;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- NETWORK_MAINTENANCE ACK : CMD[%02X]\r\n", cmd);

	// 				FID TYP CMD STAT SPEC_LEN
	frameLen = 1  +1  +1   +1  +specLen;
												
	frameData[index++] = frameID;                                           // Frame ID (Fixed 0xFF)
	frameData[index++] = NETMAINA;                                         	// Frame Type   
	frameData[index++] = cmd;                                              	// CMD
	frameData[index++] = stat;                                              // STAT
	if(specLen > 0) {
		memcpy(&frameData[index], spec, specLen);                  						// Specific Structure
		index += specLen;  
	}
	
	ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, 0);
	if(ptc_res != PTC_OK) {              
		return PTC_SEND_FAIL;
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send RemoteDeviecControl Ack
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendRemoteCtrlACK(uint8_t frameID, uint8_t cid, uint8_t stat, uint8_t dest) {
	
	uint8_t 	frameData[4];
	uint8_t 	frameLen 		= 0;   
	uint8_t 	index 			= 0;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- REMOTE_DEVICE_CONTROL ACK [%s] : FID[%02X]\r\n", (dest==GSM_PORT)?"GSM":"DEBUG", frameID);

	// 				FID TYP CID STAT 
	frameLen = 1  +1  +1   +1;
												
	frameData[index++] = frameID;                                           // Frame ID (Fixed 0xFF)
	frameData[index++] = RMDCTRLA;                                         	// Frame Type   
	frameData[index++] = cid;                                              	// CMD
	frameData[index++] = stat;                                              // STAT
	
	if(dest == GSM_PORT) {
		ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}else if(dest == DEBUG_PORT) {
		
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send ConfigurationManagement Ack
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendConfigManageACK(uint8_t frameID, uint8_t accd, uint8_t cnfg, uint8_t stat, uint8_t *spec, uint16_t specLen, uint8_t dest) {
	
	uint8_t 	frameData[MAX_PKT_LEN];
	uint8_t 	frameLen 		= 0;   
	uint8_t 	index 			= 0;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- CONFIGURATION_MANAGEMENT ACK : FID[%02X] (%s)\r\n", frameID, (dest==GSM_PORT)?"GSM":"DEBUG");

	// 				FID TYP SID ACCD CNFG STAT SPEC_LEN
	frameLen = 1  +1  +8   +1   +1   +1  +specLen;
	
	frameData[index++] = frameID;                                           // Frame ID (Fixed 0xFF)
	frameData[index++] = CONFMANA;                                         	// Frame Type   
	memcpy(&frameData[index], &struct_info.serialNumber[0], ADDR_LEN);                  	// Source ID
	index += ADDR_LEN;
	frameData[index++] = accd;                                              // ACCD
	frameData[index++] = cnfg;                                              // CNFG
	frameData[index++] = stat;                                              // STAT
	if(specLen > 0) {
		memcpy(&frameData[index], spec, specLen);                  						// Specific Structure
		index += specLen;  
	}
	
	if(dest == GSM_PORT) {
		ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}else if(dest == DEBUG_PORT) {
		ptc_res = xPROCOMM_SendPacketToDebug(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send Data&Status Report Cmd
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendDataNStatusReportCMD(uint32_t timestamp, uint8_t recordType, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data, uint32_t timeout, uint8_t dest) {

	uint8_t 	frameData[MAX_PKT_LEN];
	uint8_t		frameID			=	0;
	uint32_t 	frameLen 		= 0;
	uint32_t 	index 			= 0;	
	uint32_t 	i;
	PTC_RES		ptc_res;
	
	frameID = ucPKTQUEUE_GenerateID(packet_Queue);
	printDebug_Semphr(ALWAYS, "<<<-- DATA&STATUS_REPORT CMD [%s] : FID[%02X]\r\n", (dest==GSM_PORT)?"GSM":"DEBUG", frameID);
	
	/* 	      FID TYP TIM SID RTYP NUM_S            STAT_SIZE            NUM_D            DAT_SIZE           */
	frameLen = 1  +1  +4  +8  +1   +1    +(numStatSet*sizeof(STATUS_SET))  +1   +(numDatSet*sizeof(DATA_SET));     

	if((recordType != RECORD_RLT) && (recordType != RECORD_LOG)) {         
		/* Invalid Record Type */
		return PTC_FIELD_INVALID;
	}
	
	/* Add data_log wait frame_id */
	if(recordType == RECORD_LOG) {
		_DataLog_WaitID = frameID;
	}
	 
	frameData[index++] = frameID;												                                 								/* Frame ID */
	frameData[index++] = DNSTREPC;                                         															/* Frame Type */
	memcpy(&frameData[index], &timestamp, sizeof(timestamp));              															/* Timestamp */
	index += sizeof(timestamp);
	
	for(i = 0; i < ADDR_LEN; i++) {                                                														/* Source ID */
		frameData[index++]  = struct_info.serialNumber[i];
	}
																																			 
	frameData[index++] = recordType;                                       															/* Record Type */
	frameData[index++] = numStatSet;                                       															/* Number of status set */
	
	for(i = 0; i < numStatSet; i++) {                                      															/* Status Set */	
		memcpy(&frameData[index], &status[i], sizeof(STATUS_SET));
		index += sizeof(STATUS_SET);
	}
	
	frameData[index++] = numDatSet;                                        															/* Number of data set */
	
	for(i = 0; i < numDatSet; i++) {                                       															/* Data Set */
		memcpy(&frameData[index], &data[i], sizeof(DATA_SET));
		index += sizeof(DATA_SET);
	}
	
	if(dest == GSM_PORT) {
		ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, timeout);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}else if(dest == DEBUG_PORT) {
		ptc_res = xPROCOMM_SendPacketToDebug(struct_info.destSerial, frameData, frameLen, 0);
		if(ptc_res != PTC_OK) {               
			return PTC_SEND_FAIL;
		}
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send Data&Status Report Cmd
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendDataNStatusLog(uint32_t timestamp, uint16_t fTotal, uint16_t fNo, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data, uint32_t timeout) {

	uint8_t 	frameData[MAX_PKT_LEN];
	uint32_t 	frameLen 		= 0;
	uint32_t 	index 			= 0;	
	uint32_t 	i;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- DATA&STATUS_LOG \r\n");
	
	/* 	      FID TYP TIM FTOTAL FNO NUM_S             STAT_SIZE            NUM_D            DAT_SIZE           */
	frameLen = 1  +1  +4   +2    +2   +1    +(numStatSet*sizeof(STATUS_SET))  +1   +(numDatSet*sizeof(DATA_SET));
	 
	frameData[index++] = 0xFF;												                                 									/* Frame ID */
	frameData[index++] = DNSTALOG;                                         															/* Frame Type */
	memcpy(&frameData[index], &timestamp, sizeof(timestamp));              															/* Timestamp */
	index += sizeof(timestamp);
																									
	frameData[index++] = (fTotal >> 8) & 0xFF;                                      										/* Frame Total 	- MSB */
  frameData[index++] = fTotal & 0xFF;                                                									/*             	- LSB */
	frameData[index++] = (fNo >> 8) & 0xFF;                                      												/* Frame Number - MSB */
  frameData[index++] = fNo & 0xFF;                                                										/*             	- LSB */
	frameData[index++] = numStatSet;                                       															/* Number of status set */
	
	for(i = 0; i < numStatSet; i++) {                                      															/* Status Set */	
		memcpy(&frameData[index], &status[i], sizeof(STATUS_SET));
		index += sizeof(STATUS_SET);
	}
	
	frameData[index++] = numDatSet;                                        															/* Number of data set */
	
	for(i = 0; i < numDatSet; i++) {                                       															/* Data Set */
		memcpy(&frameData[index], &data[i], sizeof(DATA_SET));
		index += sizeof(DATA_SET);
	}
	
	ptc_res = xPROCOMM_SendPacketToDebug(struct_info.destSerial, frameData, frameLen, 0);
	if(ptc_res != PTC_OK) {               
		return PTC_SEND_FAIL;
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send Event Report Cmd
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendEventReportCMD(uint32_t timestamp, uint8_t recordType, uint8_t eid, uint32_t edat, uint32_t timeout) {

	uint8_t 	i;
	uint8_t 	frameData[EV_FRAME_SIZE];
	uint8_t 	frameID			= 0;
	uint8_t 	index 			= 0;	 
	PTC_RES		ptc_res;
	
	frameID = ucPKTQUEUE_GenerateID(packet_Queue);
	printDebug_Semphr(ALWAYS, "<<<-- EVENT_REPORT CMD : FID[%02X]\r\n", frameID);

	if((recordType != RECORD_RLT) && (recordType != RECORD_LOG)) {         
		/* Invalid Record Type */
		return PTC_FIELD_INVALID;
	}
	
	/* Add event_log wait frame_id */
	if(recordType == RECORD_LOG) {
		_EventLog_WaitID = frameID;
	}
	 
	frameData[index++] = frameID;                                																				/* Frame ID */
	frameData[index++] = EVENREPC;                                         															/* Frame Type */
	memcpy(&frameData[index], &timestamp, sizeof(timestamp));              															/* Timestamp */
	index += sizeof(timestamp);
	for(i = 0; i < ADDR_LEN; i++) {                                                											/* Source ID */
		frameData[index++]  = struct_info.serialNumber[i];
	}																																		 
	frameData[index++] = recordType;                                       															/* Record Type */
	frameData[index++] = eid;                                       																		/* Event ID */
	memcpy(&frameData[index], &edat, sizeof(edat));              																				/* Event Data */
	index += sizeof(edat);
	
//	if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
//		printDebug(INFO, "Event Data(%d):\r\n", index);
//		for(i = 0; i < EV_FRAME_SIZE; i++) {
//			printDebug(INFO, "%02X ", frameData[i]);
//		}
//		printDebug(INFO, "\r\n");
//		xSemaphoreGive(xSemaphore);
//	}
	
	ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, EV_FRAME_SIZE, timeout);
	if(ptc_res != PTC_OK) {               
		return PTC_SEND_FAIL;
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send Data&Status Report Cmd
  * @param  
  * @retval Return protocol function code
  */
PTC_RES xPROCOMM_SendNetworkMaintCMD(uint8_t cmd, uint8_t *specData, uint8_t specDataLen, uint32_t timeout) {
	
	uint8_t 	frameData[MAX_PKT_LEN];
	uint8_t 	frameLen = 0;   
	uint8_t 	index = 0;
	PTC_RES		ptc_res;
	
	printDebug_Semphr(ALWAYS, "<<<-- NETWORK_MAINTENANCE CMD : CMD[%02X] \r\n", cmd);

	// 				FID TYP CMD   SPEC_DATA
	frameLen = 1  +1  +1  +specDataLen;    
												
	frameData[index++] = 0xFF;                                             	// Frame ID (Fixed 0xFF)
	frameData[index++] = NETMAINC;                                         	// Frame Type                             
	frameData[index++] = cmd;                                              	// CMD
	if(specDataLen > 0) {
		memcpy(&frameData[index], specData, specDataLen);                  		// Specific Structure
		index += specDataLen;  
	}
	
	ptc_res = xPROCOMM_SendPacketToGSM(struct_info.destSerial, frameData, frameLen, timeout);
	if(ptc_res != PTC_OK) {               
		return PTC_SEND_FAIL;
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Initial timer
  * @param  
  * @retval None
  */
PTC_RES xPROCOMM_SendPacketToDebug(uint8_t *recvAddr, uint8_t *data, uint32_t dataLen, uint32_t timeout) {
	
	uint8_t 	sendPacket[MAX_PKT_LEN];
	uint32_t 	packetLen 	= 0;
	uint8_t 	frameID			= 0;
	uint32_t 	frameLen 		= 0;
	uint32_t 	index 			= 0;
//	uint8_t 	res;
	
	/* 				Start Len ProtoVer  SendAddr  RecvAddr  Payload CheckSum   */
	packetLen = 1		+2     +1    +ADDR_LEN +ADDR_LEN +dataLen   +1;
	
	frameLen 	= packetLen - 4;
	frameID 	= data[0];
	
	sendPacket[index++] = 0x7E;                                                           /* Start Delimeter */
	sendPacket[index++] = (frameLen >> 8) & 0xFF;                                      		/* Packet Length - MSB */
  sendPacket[index++] = frameLen & 0xFF;                                                /*               - LSB */
	sendPacket[index++] = PTC_VERSION;                                                   	/* Protocal Version */
	memcpy(&sendPacket[index], &struct_info.serialNumber[0], ADDR_LEN);                   /* Sender Address */
	index += ADDR_LEN;
	memcpy(&sendPacket[index], &recvAddr[0], ADDR_LEN);                                   /* Receiver Address */
	index += ADDR_LEN;
	memcpy(&sendPacket[index], data, dataLen);                                            /* Payload */
	index += dataLen;
	sendPacket[index] = ucPROCOMM_Checksum(&sendPacket[3], frameLen);                			/* Checksum */
	
	/* Send packet to debug port */
	vDEBUG_SendData(sendPacket, packetLen);
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Initial timer
  * @param  
  * @retval None
  */
PTC_RES xPROCOMM_SendPacketToGSM(uint8_t *recvAddr, uint8_t *data, uint32_t dataLen, uint32_t timeout) {
	
	uint8_t 	sendPacket[MAX_PKT_LEN];
	uint32_t 	packetLen 	= 0;
	uint8_t 	frameID			= 0;
	uint32_t 	frameLen 		= 0;
	uint32_t 	index 			= 0;
	uint8_t 	res;
	PACKET_DATA 	packet_data;
	
	/* 				Start Len ProtoVer  SendAddr  RecvAddr  Payload CheckSum   */
	packetLen = 1		+2     +1    +ADDR_LEN +ADDR_LEN +dataLen   +1;
	
	frameLen 	= packetLen - 4;
	frameID 	= data[0];
	
	sendPacket[index++] = 0x7E;                                                           /* Start Delimeter */
	sendPacket[index++] = (frameLen >> 8) & 0xFF;                                      		/* Packet Length - MSB */
  sendPacket[index++] = frameLen & 0xFF;                                                /*               - LSB */
	sendPacket[index++] = PTC_VERSION;                                                   	/* Protocal Version */
	memcpy(&sendPacket[index], &struct_info.serialNumber[0], ADDR_LEN);                   /* Sender Address */
	index += ADDR_LEN;
	memcpy(&sendPacket[index], &recvAddr[0], ADDR_LEN);                                   /* Receiver Address */
	index += ADDR_LEN;
	memcpy(&sendPacket[index], data, dataLen);                                            /* Payload */
	index += dataLen;
	sendPacket[index] = ucPROCOMM_Checksum(&sendPacket[3], frameLen);                			/* Checksum */
	
	if((timeout > 0) && (frameID != 0xFF)) {
		res = ucPKTQUEUE_Add(packet_Queue, &_Queue_counter, frameID, &sendPacket[0], packetLen, timeout);    
		if(res) {               
			return PTC_QUEUE_ERR;
		}
	}
	memcpy(packet_data.packet, sendPacket, packetLen);
	packet_data.length = packetLen;
	
	res = xQueueSend(Queue_PacketToGSM, &packet_data, 0);
	if(res != pdTRUE) {
		printDebug_Semphr(MINOR_ERR, "<PROCOMM_SendPacketToGSM>: Send Queue_PacketToGSM failed. \r\n");
	}
	
	return PTC_OK;
}
/*==========================================================================================*/
/**
  * @brief  Send HTTP Packet
  * @param  
  * @retval Protocol return code
  */
PTC_RES xPROCOMM_SendHTTPPacket(uint32_t timestamp, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data) {
	
	RTC_DateTypeDef 	RTC_DateStruct;
	RTC_TimeTypeDef 	RTC_TimeStruct;
	
	uint8_t 	sendPacket[MAX_PKT_LEN];
	uint8_t		i;
	uint32_t 	packetLen 	= 0;
	
	uint8_t headerHTTP[] = "http://192.168.242.136/sendsmart.php?query=";
	uint8_t		datetime[14];
	uint8_t 	res;
	
	// Value
	float 		ai1 = 0.0, ai2 = 0.0, ai3 = 0.0, ai4 = 0.0;
	float 		io5_counter = 0.0, io5_acc = 0.0, io6_digital = 0.0, accf = 0.0, accr = 0.0;
	uint8_t		csq = 0;
	float			vbatt = 0.0;
	// Null
	uint8_t		ao1 = 0, ao2 = 0, level = 0;
	uint8_t		io1 = 0, io2 = 0, io3 = 0, io4 = 0;
	
	PACKET_DATA 	packet_data;
	
	timestamp += (60*60*7);    // GMT+7
	vRTC_EpochToDateTime(&RTC_DateStruct, &RTC_TimeStruct, timestamp);
	sprintf((char*)datetime, "%04d%02d%02d%02d%02d%02d", 2000+RTC_DateStruct.RTC_Year,
																							RTC_DateStruct.RTC_Month,
																							RTC_DateStruct.RTC_Date,
																							RTC_TimeStruct.RTC_Hours, 
																							RTC_TimeStruct.RTC_Minutes, 
																							RTC_TimeStruct.RTC_Seconds);
												
	for(i = 0; i < numDatSet; i++) {
		if(data[i].channel == IN420MA_CH1) {
			ai1 = data[i].value;
		}else if(data[i].channel == IN420MA_CH2) {
			ai2 = data[i].value;
		}else if(data[i].channel == IN420MA_CH3) {
			ai3 = data[i].value;
		}else if(data[i].channel == IN420MA_CH4) {
			ai4 = data[i].value;
		}else if(data[i].channel == MB_CH1_FLOWRATE) {
			io6_digital = data[i].value;
		}else if(data[i].channel == MB_CH1_POS) {
			accf = data[i].value;
		}else if(data[i].channel == MB_CH1_NEG) {
			accr = data[i].value;
		}else if(data[i].channel == PUL_CH1_FLOWRATE) {
			io5_counter = data[i].value;
		}else if(data[i].channel == PUL_CH1_NETFLOW) {
			io5_acc = data[i].value;
		}
	}
	
	for(i = 0; i < numStatSet; i++) {
		if(status[i].statusType == GSM) {
			csq = status[i].value_int;
		}else if(status[i].statusType == BAT) {
			vbatt = status[i].value_float;
		}
	}
	
	/* Id_name, Pressure, AI2, AI3, AI4, AO1, AO2, io1, io2, io3, io4, Flow(Pulse), Flow(485), CSQ, VBATT, Flow(total), ACCF, ACCR, Level, datetime */
	sprintf((char*)sendPacket	,"%s%s,%0.3f,%0.3f,%0.3f,%0.3f,%d,%d,%d,%d,%d,%d,%0.3f,%0.3f,%d,%0.3f,%0.3f,%0.3f,%0.3f,%d,%s,%d,%d"
														, headerHTTP, struct_ConfigProperties.deviceName, ai1, ai2, ai3, ai4, ao1, ao2, io1, io2, io3, io4
														, io5_counter, io6_digital, csq, vbatt, io5_acc, accf, accr, level, datetime, 0, 0);
					
	packet_data.length = strlen((char*)sendPacket);
	memcpy(packet_data.packet, sendPacket, packet_data.length);
	
	res = xQueueSend(Queue_PacketHTTPToGSM, &packet_data, 0);
	if(res != pdTRUE) {
		printDebug_Semphr(MINOR_ERR, "<PROCOMM_SendHTTPPacket>: Send Queue_PacketToGSM failed. \r\n");
	}
}
/*============================================================================================
Support Function
============================================================================================*/
/**
  * @brief  Data Checksum
  * @param  
  * @retval Checksum Value
  */
uint8_t ucPROCOMM_Checksum(uint8_t buf[], uint32_t len) {

    uint32_t 	i = 0;
    uint8_t 	sum = 0;      
	
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }                 
    return (0xFF - (sum & 0xFF));
}
/*==========================================================================================*/





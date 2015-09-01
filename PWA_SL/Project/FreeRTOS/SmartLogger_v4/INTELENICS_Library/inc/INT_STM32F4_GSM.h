/**
  ******************************************************************************
  * @file    gsm_task.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _GSM_H_
#define _GSM_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "INT_STM32F4_ProtocolComm.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
// 	GSM_STATE_IPINITIAL,								/*! < The TCPIP stack is in idle state. >*/
// 	GSM_STATE_IPSTART,									/*! < The TCPIP stack has been registered. >*/
// 	GSM_STATE_IPCONFIG,									/*! < It has been start-up to activate GPRS/CSD context. >*/
// 	GSM_STATE_IPIND,										/*! < It is activating GPRS/CSD context. >*/
// 	GSM_STATE_GPRSACT,									/*! < GPRS/CSD context has been activated successfully. >*/
// 	GSM_STATE_IPSTATUS,									/*! < The local IP address gas been gotten by the command. AT+QILOCIP >*/
// 	GSM_STATE_TCPCONNECTING,						/*! < It is trying to establish a TCP connection. >*/
// 	GSM_STATE_UDPCONNECTING,						/*! < It is trying to establish a UDP connection. >*/
	GSM_STATE_IPCLOSE,									/*! < The TCP/UDP connection has been closed. >*/
	GSM_STATE_CONNECTOK,								/*! < The TCP/UDP connection has been established successfully. >*/
// 	GSM_STATE_PDPDEACT,									/*! < GPRS/CSD context was deactivated because of unknown reason. >*/
	GSM_STATE_CMDERROR									/*! < Send AT command then an error occurred. >*/
} eGSMState;

typedef enum {
	GSM_CONNECT_OK,											/*! < The TCP/UDP connection has been established successfully. >*/
	GSM_CONNECT_FAIL,										/*! < The TCP/UDP connection has been established fail. >*/
	GSM_CONNECT_ALREADY,								/*! < The TCP/UDP connection has been established already. >*/
	GSM_CONNECT_CMDERROR								/*! < Send AT command then an error occurred. >*/
} eGSMConnectStatus;

typedef enum {
	GSM_ENOERR,													/*! < no error. >*/
	GSM_ECMD,														/*! < AT command error >*/
	GSM_ETIMEDOUT,											/*! < timeout error occurred. >*/
	GSM_ENVMODE													/*! < Mode not vaild. >*/
} eGSMErrorCode;

typedef enum {
	GSM_BYTE_MODE_READ,
	GSM_BYTE_MODE_WRITE
} eGSMByteMode;

typedef enum {
	GSM_RET_OK = 0,
	GSM_RET_PKT_EMPTY,
	GSM_RET_CLOSED
} eGSMReturnCode;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define GSM_Provider_AIS				0x01
#define GSM_Provider_DTAC				0x02
#define GSM_Provider_TRUE				0x03

#define GSM_Flag_Active					0x01
#define GSM_Flag_NotActive			0x00

extern uint8_t flag_GSMReady;

/* Exported functions ------------------------------------------------------- */ 
uint8_t 				ucGSM_Initialize( void );
void 						vGSM_Start( void );
void 						vGSM_Restart( void );
void 						vGSM_PowerOff( void );
uint8_t					ucGSM_ConnectTCP( char *IP ,int port );
void 						vGSM_SendATcommand( volatile uint8_t *s, uint32_t size );
void 						vGSM_CloseConnection( void );
uint8_t 				ucGSM_ReadSignalQuality( void );
void 						vGSM_QueryCreditBalance( uint8_t ucProvider, uint8_t *ucCode );
void 						vGSM_ReadCreditBalance( uint8_t ucProvider );

void 						vGSM_SendHTTPGET( uint8_t* ucURL, uint16_t LenURL );

eGSMReturnCode 	eGSM_receivePacket( void );
eGSMReturnCode 	eGSM_transmitPacket( uint8_t *strPacket, uint32_t len );
eGSMState 			ucGSM_QueryState( void );

#endif

/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

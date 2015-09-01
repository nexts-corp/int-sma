/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolComm.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	21-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#ifndef _INT_STM32F4_PROTOCOLCOMM_H
#define _INT_STM32F4_PROTOCOLCOMM_H

#include "stdint.h"

#define PTC_VERSION			0x40       	/* Protocol Version */
#define ADDR_LEN				8						/* Address length is 64bits */

/* PROTOCOL COMMAND ----------------------------------------------------------*/
#define STATREQC        0x1A        /* Status Request COMMAND */     
#define STATREPC        0x2A        /* Status Report COMMAND */    
#define EVENREPC        0x3A        /* Event Report COMMAND */    
#define CONFMANC        0x4A        /* Configuration Management COMMAND */  
#define DATAREPC        0x5A        /* Data Report COMMAND */               
#define LOGMANIC        0x6A        /* Log Manipulation COMMAND */   
#define NETMAINC        0x7A        /* Network Maintenance COMMAND */   
#define RMDCTRLC        0x8A        /* Remote Device Control COMMAND */   
#define DNSTREPC        0x9A        /* Data&Status Report COMMAND */
#define DNSTALOG        0xAA        /* Data&Status LOG */
#define FMWUPLDC        0xFA        /* Firmware Upload COMMAND (Reserved) */   
                   
/* PROTOCOL ACKNOWLEDGE ------------------------------------------------------*/
#define STATREQA        0x1B        /* Status Request ACKNOWLEDGE */
#define STATREPA        0x2B        /* Status Report ACKNOWLEDGE */
#define EVENREPA        0x3B        /* Event Report ACKNOWLEDGE */
#define CONFMANA        0x4B        /* Configuration Management ACKNOWLEDGE */
#define DATAREPA        0x5B        /* Data Report ACKNOWLEDGE */
#define LOGMANIA        0x6B        /* Log Manipulation ACKNOWLEDGE */
#define NETMAINA        0x7B        /* Network Maintenance ACKNOWLEDGE */
#define RMDCTRLA        0x8B        /* Remote Device Control ACKNOWLEDGE */       
#define DNSTREPA        0x9B        /* Data&Status Report ACKNOWLEDGE */
#define PROTOERROR      0xEB        /* Protocol Error Feedback */
#define FMWUPLDA        0xFB        /* Firmware Upload ACKNOWLEDGE (Reserved) */

/* Network Maintenance -------------------------------------------------------*/
#define TIMESYNC				0x10
#define HEARTBEAT				0x11
#define SELF_REGIS			0x20
#define PENDING_APPRV		0x28
#define JOIN_NETWORK		0x30
#define CHAP_CMD				0x31

/* Data&Status Report --------------------------------------------------------*/
/* Record Type */
#define RECORD_RLT          0x00        /* Real time record */
#define RECORD_LOG          0x01        /* Logged record */

/* Status ID */
#define GSM									0x11				/* GSM Signal */
#define BAT 								0x12 				/* Battery Remaining */
#define BTEMP								0x13 				/* Temperature on board */
#define VALV 								0x21 				/* Butterfly Valve */
#define DIGI 								0x22 				/* Digital I/O status */
#define ALAM1 							0x23 				/* Sensor Alarm Status 1 */
#define ALAM2 							0x24 				/* Sensor Alarm Status 2 */
#define ALAM3 							0x25 				/* Sensor Alarm Status 3 */
/* Value Type */
#define VBIT								0x01				/* Bit Level */
#define VBYTE								0x02				/* Byte Level */
#define VINT								0x03				/* Integer */
#define VFLOAT							0x04				/* Floating Point */

#define MAX_STATUS					8
#define MAX_DATA						13

/* Configuration Management --------------------------------------------------*/
/* Access Direction */
#define CONF_ACCD_READ					0x10
#define CONF_ACCD_WRITE					0x20
#define CONF_ACCD_CONFIRM				0x30
#define CONF_ACCD_UPDATE				0x40
/* Config Group */
#define CONF_CNFG_PROPERTIES		0x10
#define CONF_CNFG_DEV						0x11
#define CONF_CNFG_CMD						0x12
#define CONF_CNFG_OPERATING			0x20
#define CONF_CNFG_BUZZER				0x80
#define CONF_CNFG_WORKING				0x91
#define CONF_CNFG_NETWORK				0x92

#define CONF_CNFG_420SENSOR1		0xA0
#define CONF_CNFG_420SENSOR2		0xA1
#define CONF_CNFG_420SENSOR3		0xA2
#define CONF_CNFG_420SENSOR4		0xA3
#define CONF_CNFG_420SENSOR5		0xA4
#define CONF_CNFG_420SENSOR6		0xA5
extern const uint8_t CONF_CNFG_420SENSORn[];

#define CONF_CNFG_ADCSENSOR1		0xA6
#define CONF_CNFG_ADCSENSOR2		0xA7
extern const uint8_t CONF_CNFG_ADCSENSORn[];

#define CONF_CNFG_CAL420SEN1		0xA8
#define CONF_CNFG_CAL420SEN2		0xA9
#define CONF_CNFG_CAL420SEN3		0xAA
#define CONF_CNFG_CAL420SEN4		0xAB
#define CONF_CNFG_CAL420SEN5		0xAC
#define CONF_CNFG_CAL420SEN6		0xAD
extern const uint8_t CONF_CNFG_CAL420SENn[];

#define CONF_CNFG_CAL420VALVEiN		0xAE
#define CONF_CNFG_CAL420VALVEoUT	0xAF

#define CONF_CNFG_MODBUSSENS		0xB0
#define CONF_CNFG_PULSESENS			0xC0
#define CONF_CNFG_PULSEADJ			0xC1
#define CONF_CNFG_VALVE					0xCF

#define CONF_CNFG_420SENsAL1		0xD0
#define CONF_CNFG_420SENsAL2		0xD1
#define CONF_CNFG_420SENsAL3		0xD2
#define CONF_CNFG_420SENsAL4		0xD3
#define CONF_CNFG_420SENsAL5		0xD4
#define CONF_CNFG_420SENsAL6		0xD5
extern const uint8_t CONF_CNFG_420SENsALn[];

#define CONF_CNFG_ADCSENsAL1		0xD6
#define CONF_CNFG_ADCSENsAL2		0xD7
extern const uint8_t CONF_CNFG_ADCSENsALn[];

#define CONF_CNFG_MODBUSSENsAL	0xE0
#define CONF_CNFG_PULSENsAL			0xF0

#define CONF_CNFG_DIN1					0x40
#define CONF_CNFG_DIN2					0x41
#define CONF_CNFG_DIN3					0x42
#define CONF_CNFG_DIN4					0x43
#define CONF_CNFG_DIN5					0x44
#define CONF_CNFG_DIN6					0x45
#define CONF_CNFG_DIN7					0x46
#define CONF_CNFG_DIN8					0x47
#define CONF_CNFG_DIN9					0x48
#define CONF_CNFG_DIN10					0x49
#define CONF_CNFG_DIN11					0x4A
#define CONF_CNFG_DIN12					0x4B
extern const uint8_t CONF_CNFG_DINn[];

#define STAT_SUCCESS						0x00
#define STAT_ERR								0x01

/* Working Management */
/*  - Recording Mode */
#define UPpLIMIT_MINBATT		(float)14.0	/* Upper Limit MinBatt */
#define LOWeLIMIT_MINBATT		(float)0.0	/* Lower Limit MinBatt */
#define ROLLOVER_MODE				0x00				/* Roll Over Mode */
#define STOPWHENFULL_MODE		0x01				/* Stop When Full Mode */
#define UPpLIMIT_RECINVL		86400				/* Upper Limit Record Interval */
#define LOWeLIMIT_RECINVL		20					/* Lower Limit Record Interval */
#define UPpLIMIT_SAMPINVL		86400				/* Upper Limit Sampling Interval */
#define LOWeLIMIT_SAMPINVL	20					/* Lower Limit Sampling Interval */

/* Network Configuration */
/*  - Server Address Type */
#define IP_ADDR							0x01				/* IP Address */
#define HOST_NAME						0x02				/* Host Name */
#define UPpLIMIT_ADDRLEN		100				/* Upper Limit Sampling Interval */
#define LOWeLIMIT_ADDRLEN		1					/* Lower Limit Sampling Interval */

/* Buzzer Configuration */
#define BUZZ_MAX_EVSRC			25					/* Max event source */

/* Digital I/O */
/*  - Digital I/O Channel */
#define DIGI_OUT1						0x01				/* Digital output channel 1 */
#define DIGI_OUT2						0x02				/* Digital output channel 2 */
#define DIGI_OUT3						0x03				/* Digital output channel 3 */
#define DIGI_OUT4						0x04				/* Digital output channel 4 */
#define DIGI_IN1						0x11				/* Digital input channel 1 */
#define DIGI_IN2						0x12				/* Digital input channel 2 */
#define DIGI_IN3						0x13				/* Digital input channel 3 */
#define DIGI_IN4						0x14				/* Digital input channel 4 */
#define DIGI_IN5						0x15				/* Digital input channel 5 */
#define DIGI_IN6						0x16				/* Digital input channel 6 */
#define DIGI_IN7						0x17				/* Digital input channel 7 */
#define DIGI_IN8						0x18				/* Digital input channel 8 */
#define DIGI_IN9						0x19				/* Digital input channel 9 */
#define DIGI_IN10						0x1A				/* Digital input channel 10 */
#define DIGI_IN11						0x1B				/* Digital input channel 11 */
#define DIGI_IN12						0x1C				/* Digital input channel 12 */

#define DIN_RISING_EDGE			0x01
#define DIN_FALLING_EDGE		0x02
#define DIN_ALL_EDGE				0x03

/* 4-20mA Sensor Configuration */
/*  - Sensor Channel */
#define IN420MA_CH1 				0x01				/* Channel 1 */
#define IN420MA_CH2 				0x02				/* Channel 2 */
#define IN420MA_CH3 				0x03				/* Channel 3 */
#define IN420MA_CH4 				0x04				/* Channel 4 */
#define IN420MA_CH5 				0x05				/* Channel 5 */
#define IN420MA_CH6 				0x06				/* Channel 6 */
#define IN420MA_VALVE 			0x07				/* 4-20mA Input Butterfly Valve */
#define OUT420MA_VALVE 			0x08				/* 4-20mA Output Butterfly Valve */
extern uint8_t IN420MA_CH[];
/*  - Sensor Output */
#define OUT_420MA						0x01				/* 4-20mA */
#define OUT_0to5V						0x02				/* Analog 0-5 Volt */
#define OUT_0to10V					0x03				/* Analog 0-10 Volt */
/*  - Sensor Type */
#define TYP_FLOW						0x01				/* Flow Sensor (output: water flow) */
#define TYP_PRESS_FRONT			0x04				/* Pressure Sensor @front water gate */
#define TYP_PRESS_BEHIND		0x05 				/* Pressure Sensor @behind water gate */
#define TYP_PRESS_END				0x06				/* Pressure Sensor @end of pipe */
#define TYP_PH							0x07				/* PH Sensor */
#define TYP_TFC							0x08				/* Total Free Residual Chlorine Sensor */
#define TYP_TURBID					0x09				/* Turbidity Sensor */
#define TYP_TEMP						0x0A				/* Temperature Sensor */
#define TYP_ULTRASONIC			0x0B				/* Ultrasonic Level */
#define TYP_OTHER						0x0F				/* Other Sensor */
/*  - Data Unit */
#define UNIT_M3pHR					0x01    		/* m³/hr */
#define UNIT_M3							0x02				/* m³ */
#define UNIT_BAR						0x03				/* bar */
#define UNIT_PH							0x04				/* pH */
#define UNIT_MGpL						0x05				/* mg/l */
#define UNIT_FNU						0x06				/* FNU */
#define UNIT_PPM						0x07				/* ppm */
#define UNIT_GpL						0x08				/* g/l */
#define UNIT_CELSIUS				0x09				/* °C */
#define UNIT_METER					0x0A				/* meter */
#define UNIT_PERCENT				0x0B				/* percent (%) */
#define UNIT_MA							0x0C				/* mA */
#define UNIT_VOLT						0x0D				/* Voltage */
#define UNIT_ADC						0x0F				/* ADC Value (12bits) */
#define UNIT_NULL						0xFF				/* Null */
/*  - Sensor Enable */
#define SENSOR_DISABLE			0x00 				/* Disable */
#define SENSOR_ENABLE				0x01				/* Enable */
#define CONF_DISABLE				0x00 				/* Disable */
#define CONF_ENABLE					0x01				/* Enable */

/* PULSE FlowSensor Configuration */
/*	- Volume per Pulse*/
#define VOL_001CU_ID				0x01
#define VOL_01CU_ID					0x02
#define VOL_1CU_ID					0x03
#define VOL_10CU_ID					0x04
#define VOL_0001CU_ID				0x05

/* Other Sensor */
#define MB_CH1_FLOWRATE			0x11				/* MODBUS Flow Sensor Channel1 -> Flow Rate */
#define MB_CH1_POS					0x12				/* MODBUS Flow Sensor Channel1 -> Positive Total NetFlow */
#define MB_CH1_NEG					0x13				/* MODBUS Flow Sensor Channel1 -> Negative Total NetFlow */
#define PUL_CH1_FLOWRATE		0x21				/* PULSE Flow Sensor Channel 1 -> Flow Rate */
#define PUL_CH1_NETFLOW			0x22				/* PULSE Flow Sensor Channel 1 -> Net Flow */

/* Butterfly Valve */
#define VALVE_420MA					0x01				/* Valve interface is 4-20mA */
#define VALVE_MODBUS				0x02				/* Valve interface is MODBUS */

/* Calibration */
#define CAL_USE_DEFAULT			0x01				/* Use default value to calculate */
#define CAL_USE_CALIBRATE		0x02				/* Use calibrate value to calculate */

/* Log Manipulation ----------------------------------------------------------*/
/* Access Direction */
#define LOG_ACCD_READDATA		0x31
#define LOG_ACCD_DATAEMPTY	0xE1

/* Event Report --------------------------------------------------------------*/
#define EV_GSM							0x20
#define EV_BATTERY					0x51
#define EV_MEMORY						0x70

/* EID : Digital Input Event */
#define EV_DIN							0xC0				/* 0xC0 - 0xCB */

/* EID : Data Sensor Event */
#define EV_DATA							0xE0				/* 0xE1 - 0xEB */
#define EV_DATA_ANALOG_1		0xE7        /* Analog Sensor Ch1 */
#define EV_DATA_ANALOG_2		0xE8        /* Analog Sensor Ch2 */
#define EV_DATA_MBSENS			0xE9        /* MODBUS FlowSensor */
#define EV_DATA_PULSESENS		0xEB  			/* PULSE FlowSensor */

/* EID : Sensor Error Event */
#define EV_SENSOR						0xF0  			/* 0xF1 - 0xFA */

#define EV_FRAME_SIZE				20       		/* 1_FID +1_TYP +4_TIM +8_SID +1_RTYP +1_EID +4_EDAT */
#define EV_NORMAL						0x00

/* Battery Event */
#define	EV_BATT_LOW					0x01

/* Memory Event */
#define EV_MEM_LOW							0x01
#define EV_MEM_ERR							0x02
#define EV_MEM_NOTINSTALL				0x03
#define EV_MEM_OCONFIG_ERR			0x10		// Open ConfigFile error
#define EV_MEM_RCONFIG_ERR			0x11		// Read ConfigFile error
#define EV_MEM_WCONFIG_ERR			0x12		// Write ConfigFile error
#define EV_MEM_CCONFIG_ERR			0x14		// Close ConfigFile error

#define EV_MEM_ODATALOG_ERR			0x50		// Open DataLog error
#define EV_MEM_RDATALOG_ERR			0x51		// Read DataLog error
#define EV_MEM_WDATALOG_ERR			0x52		// Write DataLog error
#define EV_MEM_IDATALOG_ERR			0x53		// Set index DataLog error
#define EV_MEM_CDATALOG_ERR			0x54		// Close DataLog error
#define EV_MEM_OEVENTLOG_ERR		0x60		// Open EventLog error
#define EV_MEM_REVENTLOG_ERR		0x61		// Read EventLog error
#define EV_MEM_WEVENTLOG_ERR		0x62		// Write EventLog error
#define EV_MEM_IEVENTLOG_ERR		0x63		// Set index EventLog error
#define EV_MEM_CEVENTLOG_ERR		0x64		// Close EventLog error
#define EV_MEM_OCONFIGLOG_ERR		0x70		// Open ConfigLog error
#define EV_MEM_RCONFIGLOG_ERR		0x71		// Read ConfigLog error
#define EV_MEM_WCONFIGLOG_ERR		0x72		// Write ConfigLog error
#define EV_MEM_ICONFIGLOG_ERR		0x73		// Set index ConfigLog error
#define EV_MEM_CCONFIGLOG_ERR		0x74		// Close ConfigLog error

/* Digital Input Event */
#define EV_DIN_FALLING			0x00
#define EV_DIN_RISING				0x01
#define EV_DIN_UNKNOWN			0xFF

/* Sensor Alarm Event */
#define EV_LOWER						0x01
#define EV_LOWER_EXTREME		0x02
#define EV_UPPER						0x10
#define EV_UPPER_EXTREME		0x20

/* Sensor Error Event */
#define EV_SENSOR_ERR				0x80


/* Protocol Index 
 * in Packet as not include 
 * StartDelimiter, PacketLength and Checksum 
 */
#define INDEX2_RECVADDR				9
#define INDEX2_FRAMEID				17
#define INDEX2_FRAMETYPE			18

/* Configuration Packet Index */
#define INDEX2_CONFIG_ACCD		27
#define INDEX2_CONFIG_CNFG		28

/* Log Manipulation Index */
#define INDEX2_LOG_ACCD				19
#define INDEX2_LOG_DLOG				20							/* Date of Log required */

/* Network Maintenance Packet Index */
#define INDEX2_NETMT_CMD					19					/* Network Maintenance Command index */
#define INDEX2_NETMT_TSYNC				20					/* Network Maintenance TimeSync value index */
#define INDEX2_NETMT_STAT					20					/* Network Maintenance Status ACK index */
#define INDEX2_NETMT_APPPENDING		20					/* Pending ID index(CMD: PendingApproval) */
#define INDEX2_NETMT_APPSERIAL		36					/* Serial Number index(CMD: PendingApproval) */
#define INDEX2_NETMT_REGPENDING		21					/* Pending ID index(CMD: Self-Regis) */
#define INDEX2_NETMT_CHALLENGE		20					/* Challenge index(CMD: CHAP Challenge) */

/* Remote Device Control Packet Index */
#define INDEX2_REMOTE_CID		27
#define INDEX2_REMOTE_VAL		28


/* Source Configuration */
#define DEBUG_PORT					0
#define GSM_PORT						1

/* Remote Device Control ------------------------------------------------------*/
#define CID_DRVDOUT1				0x21				/* DigitalOutput_Ch1 Drive */
#define CID_DRVDOUT2				0x22				/* DigitalOutput_Ch2 Drive */
#define CID_DRVDOUT3				0x23				/* DigitalOutput_Ch3 Drive */
#define CID_DRVDOUT4				0x24				/* DigitalOutput_Ch4 Drive */
#define CID_DRVVALVE				0x60				/* Butterfly Valve Drive */

/* Protocol function return code ---------------------------------------------*/
typedef enum {				
	PTC_OK = 0,								/* [0] Succeed */
	PTC_MALLOC_FAIL,					/* [1] Malloc failed */
	PTC_FIELD_INVALID,				/* [2] Invalid data in field */
	PTC_PKT_EMPTY,						/* [3] Not have data packet */
	PTC_TYP_NDEF,							/* [4] Frame type not define */
	PTC_TYP_NREF,							/* [5] Frame type define but not reference yet */
	PTC_INVAL_MIN_LEN,				/* [6] Receive Frame length < Minimum frame length */
	PTC_HANDLE_FAILED,				/* [7] Handle protocol failed */
	PTC_QUEUE_ERR,						/* [] Incorrect checksum */
	PTC_CHECKSUM_INCORRECT,		/* [] Incorrect checksum */
	PTC_SEND_FAIL,						/* [] Send packet failed */		
	PTC_ACK,
	PTC_INVAL_ADDR
}PTC_RES;

/* STATUS_SET use in Data&Status Report Cmd Packet */
#pragma anon_unions
__packed typedef struct {
	uint8_t 		statusType;
	uint8_t			valueType;
	__packed union {											/* 4bytes */
		uint8_t		value_char[4];
		uint32_t	value_int;
		float 		value_float;
	};
}STATUS_SET;
 
/* DATA_SET use in Data&Status Report Cmd Packet */
__packed typedef struct {
    uint8_t 	channel;
		uint8_t		sensorType;
    uint8_t 	dataUnit;
    float 		value;
}DATA_SET;


/* Function Pointer ----------------------------------------------------------*/
//extern PTC_RES (*fpPROCOMM_SendPacket)(uint8_t *, uint8_t *, uint32_t, uint32_t);
//extern uint16_t (*fpPROCOMM_ReadRawData)(uint8_t *, uint16_t, uint16_t); 
//extern uint8_t (*fpPROCOMM_HandleTimeSync)(uint8_t *);
//extern void (*fpPROCOMM_HandleHeartbeat)(uint8_t);
//extern uint8_t (*fpPROCOMM_HandleConfigWrite)(uint8_t *, uint8_t);
//extern uint8_t (*fpPROCOMM_HandleRemoteCtrl)(uint8_t , uint8_t*);

extern uint8_t		_DataLog_WaitID;
extern uint8_t		_EventLog_WaitID;
extern uint8_t		_ConfigLog_WaitID;

PTC_RES ucPROCOMM_ReceivePacket(void);
PTC_RES xPROCOMM_ProcessPacket(uint8_t *buf, uint16_t length, uint8_t source);

PTC_RES xPROCOMM_SendConfigManageCMD(uint8_t accd, uint8_t recordType, uint32_t timestamp, uint8_t cnfg, void *spec, uint16_t specLen, uint32_t timeout);
PTC_RES xPROCOMM_SendEventReportCMD(uint32_t timestamp, uint8_t recordType, uint8_t eid, uint32_t edat, uint32_t timeout);
PTC_RES xPROCOMM_SendDataNStatusReportCMD(uint32_t timestamp, uint8_t recordType, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data, uint32_t timeout, uint8_t dest);
PTC_RES xPROCOMM_SendNetworkMaintCMD(uint8_t cmd, uint8_t *specData, uint8_t specDataLen, uint32_t timeout);
PTC_RES xPROCOMM_SendDataNStatusLog(uint32_t timestamp, uint16_t fTotal, uint16_t fNo, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data, uint32_t timeout);

PTC_RES xPROCOMM_SendConfigManageACK(uint8_t frameID, uint8_t accd, uint8_t cnfg, uint8_t stat, uint8_t *spec, uint16_t specLen, uint8_t dest);
PTC_RES xPROCOMM_SendNetworkMaintACK(uint8_t frameID, uint8_t cmd, uint8_t stat, uint8_t *spec, uint8_t specLen);
PTC_RES xPROCOMM_SendRemoteCtrlACK(uint8_t frameID, uint8_t cid, uint8_t stat, uint8_t dest);
PTC_RES xPROCOMM_SendLogManipulateACK(uint8_t frameID, uint8_t accd, uint8_t stat, uint8_t dest);
PTC_RES xPROCOMM_SendPacketToDebug(uint8_t *recvAddr, uint8_t *data, uint32_t dataLen, uint32_t timeout);
PTC_RES xPROCOMM_SendPacketToGSM(uint8_t *recvAddr, uint8_t *data, uint32_t dataLen, uint32_t timeout);
uint8_t ucPROCOMM_Checksum(uint8_t buf[], uint32_t len);

PTC_RES xPROCOMM_SendHTTPPacket(uint32_t timestamp, uint8_t numStatSet, STATUS_SET *status, uint8_t numDatSet, DATA_SET *data);


#endif /*_INT_STM32F4_PROTOCOLCOMM_H*/ 
  


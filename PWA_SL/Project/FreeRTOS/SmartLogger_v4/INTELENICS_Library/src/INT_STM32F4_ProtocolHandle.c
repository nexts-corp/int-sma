/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ProtocolHandle.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	16-June-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Sensors_Task.h"
#include "SD_Task.h"
#include "stm32f4_SDCard.h"
#include "SL_ConfigStruct.h"
#include "SL_PinDefine.h"
#include "INT_STM32F4_ProtocolHandle.h"
#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_ProtocolParser.h"
#include "INT_STM32F4_420RClick.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_ConfigSD.h"
#include "INT_STM32F4_24LC512.h"
#include "tm_stm32f4_ds1307.h"

extern STCONFIG_DEV 		struct_DevConfig;
extern __IO uint32_t 		_ADC_TClick;
extern __IO uint32_t		_Transmit_Period;
extern __IO bool				flag_isSDInstall;
extern __IO uint8_t 		_index_pulseData;
extern __IO uint8_t			_PulseData_Counter;
extern __IO uint8_t			_ConfigCmdACK;
extern __IO uint8_t			_LED_ERR_ON;

/* Flag Is Configuration Update from Debug_Port */
bool 	flag_isConfPropUpdate 									= false;
bool 	flag_isConfOperUpdate 									= false;
bool 	flag_isConfBuzzUpdate 									= false;
bool 	flag_isConfWorkUpdate 									= false;
bool 	flag_isConfNetwUpdate 									= false;
bool 	flag_isConf420SensUpdate[MAX_420SENSOR] = {false,false,false,false,false,false};
bool 	flag_isConfADCSensUpdate[MAX_ADCSENSOR] = {false,false};
bool 	flag_isConfMODBUSSensUpdate 						= false;
bool 	flag_isConfPulsSensUpdate 							= false;
bool 	flag_isConf420AlamUpdate[MAX_420SENSOR] = {false,false,false,false,false,false};
bool 	flag_isConfADCAlamUpdate[MAX_ADCSENSOR] = {false,false};
bool 	flag_isConfMODBUSAlamUpdate							= false;
bool 	flag_isConfPulsAlamUpdate 							= false;
bool 	flag_isConfDINUpdate[MAX_DIGItIN] 			= {false,false,false,false,false,false,false,false,false,false,false,false};
bool 	flag_isConfValveUpdate 									= false;

uint32_t DOUTn_BASEADDR[MAX_DOUT] = {DOUT1_BASEADDR, DOUT2_BASEADDR ,DOUT3_BASEADDR, DOUT4_BASEADDR};

extern void vMAIN_PrintConfig(uint8_t printLevel, uint8_t config_id, void *st_config, uint8_t ch);
extern uint8_t ucMAIN_SetDefaultInformation(void);
extern uint8_t ucMAIN_SetDefaultStatus(void);
extern uint8_t ucMAIN_SetDefaultConfig(void);

extern SENSOR_DATA 					_Sensor_Data;


/*==========================================================================================*/
/**
  * @brief  
  * @param  
  * @retval Handle Status
  */	
uint8_t vPTCHANDLE_LogManipulate(uint8_t *date, uint8_t fid) {
	
	char 						day[] = {"00"};
	char 						month[] = {"00"};
	char 						year[] = {"0000"};
	
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	ST_LOG_DATA 		data;
	
	memcpy(&day[0], &date[0], 2);
	memcpy(&month[0], &date[2], 2);
	memcpy(&year[0], &date[4], 4);
	RTC_DateStruct.RTC_Date 		= atoi(day);
	RTC_DateStruct.RTC_Month 		= atoi(month);
	RTC_DateStruct.RTC_Year 		= (atoi(year)-2000);
	RTC_TimeStruct.RTC_Hours 		= 0;
	RTC_TimeStruct.RTC_Minutes 	= 0;
	RTC_TimeStruct.RTC_Seconds 	= 0;
	
	data.cmd 				= LOG_CMD_READ;
	data.fid				= fid;
	data.timestamp 	= uiRTC_DateTimeToEpoch(RTC_DateStruct, RTC_TimeStruct);
	
	printDebug_Semphr(INFO, "<PTCHANDLE_LogManipulate>: Date required %d/%d/%d (%d)\r\n", RTC_DateStruct.RTC_Date,
																																												RTC_DateStruct.RTC_Month,
																																												RTC_DateStruct.RTC_Year+2000,
																																												data.timestamp);
	
	if(!xQueueSend(Queue_DataToSD, &data, 0)) {
		printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_LogManipulate>: Send Queue_DataToSD failed. \r\n");
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function handle Read Properties_Configuration packet.
  * @param  Configuration Specific Structure
  * @retval Handle Status
  */	
uint8_t vPTCHANDLE_ConfigRead(uint8_t cnfg, uint8_t *confSpec, uint16_t *specLen) {
	
	uint8_t 	sen420Ch      = 0;
	uint8_t 	dinCh					= 0;
	uint8_t		sensAlarmCh		= 0;
	
	if(cnfg == CONF_CNFG_PROPERTIES) {																			/*--- Properties Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigProperties, sizeof(STCONFIG_PROPERTIES));
		*specLen = sizeof(STCONFIG_PROPERTIES);
	}else if(cnfg == CONF_CNFG_DEV) {																				/*--- Deverloper Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_DevConfig, sizeof(STCONFIG_DEV));
		*specLen = sizeof(STCONFIG_DEV);
	}else if(cnfg == CONF_CNFG_OPERATING) {																	/*--- Operating Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigOperating,  sizeof(STCONFIG_OPERATING));
		*specLen = sizeof(STCONFIG_OPERATING);
		printDebug_Semphr(INFO, "<ucPTCHANDLE_ConfigRead> %d \r\n", *specLen);
	}else if(cnfg == CONF_CNFG_BUZZER) {																		/*--- Buzzer Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigBuzzer,  sizeof(STCONFIG_BUZZER));
		*specLen = sizeof(STCONFIG_BUZZER);
	}else if(cnfg == CONF_CNFG_WORKING) {																		/*--- Working Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigWorking,  sizeof(STCONFIG_WORKING));
		*specLen = sizeof(STCONFIG_WORKING);
	}else if(cnfg == CONF_CNFG_NETWORK) {																		/*--- Network Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigNetwork, sizeof(STCONFIG_NETWORK));
		*specLen = sizeof(STCONFIG_NETWORK);
	}else if((cnfg&0xF0) == CONF_CNFG_420SENSOR1) {													/*--- Analog Sensor ---*/																																		/*-------- 4-20mA Sensor Config CH1-6 (0xA0-0xA5) */
		/* Index 4-20mA Sensor Configuration */
		sen420Ch = (cnfg&0x0F);
		if(sen420Ch <= 5) {																										/*--- 4-20mA Sensor Config CH1-6 (0xA0-0xA5) ---*/	
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_Config420Sensor[sen420Ch], sizeof(STCONFIG_ANALOGSENSOR));
			*specLen = sizeof(STCONFIG_ANALOGSENSOR);
		}else if((sen420Ch >= 0x06) && (sen420Ch <= 0x07)) {									/*--- ADCSensor Config CH1-2 (0xA6-0xA7) ---*/
			/* Update index */
			sen420Ch -= 0x06;
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_ConfigADCSensor[sen420Ch], sizeof(STCONFIG_ANALOGSENSOR));
			*specLen = sizeof(STCONFIG_ANALOGSENSOR);
		}else if((sen420Ch >= 0x08) && (sen420Ch <= 0x0D)) {									/*--- 4-20mA Sensor Calibration CH1-6 (0xA8-0xAD) ---*/
			/* Update index */
			sen420Ch -= 0x08;
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_Config420Cal[sen420Ch], sizeof(STCONFIG_420CAL));
			*specLen = sizeof(STCONFIG_420CAL);
		}else if(sen420Ch == 0x0E) {																					/*--- 4-20mAInput Valve Calibration (0xAE) ---*/
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_Config420ValveInCal, sizeof(STCONFIG_420CAL));
			*specLen = sizeof(STCONFIG_420CAL);
		}else if(sen420Ch == 0x0F) {																					/*--- 4-20mAOutput Valve Calibration (0xAF) ---*/
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_Config420ValveOutCal, sizeof(STCONFIG_420CAL));
			*specLen = sizeof(STCONFIG_420CAL);
		}
	}else if(cnfg == CONF_CNFG_MODBUSSENS) {																/*--- MODBUS FlowSensor Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR));
		*specLen = sizeof(STCONFIG_MODBUSSENSOR);
	}else if(cnfg == CONF_CNFG_PULSESENS) {																	/*--- Pulse Sensor Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR));
		*specLen = sizeof(STCONFIG_PULSESENSOR);
	}else if(cnfg == CONF_CNFG_PULSEADJ) {
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ));
		*specLen = sizeof(STCONFIG_PULSEADJ);
	}else if((cnfg&0xF0) == CONF_CNFG_DIN1) {																/*--- Digital Input CH1-12 ---*/
		/* Index Digital Input Channel id */
		dinCh = (cnfg&0x0F);
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigDIN[dinCh], sizeof(STCONFIG_DIGITALsIN));
		*specLen = sizeof(STCONFIG_DIGITALsIN);
	}else if((cnfg&0xF0) == CONF_CNFG_420SENsAL1) {													/*--- Analog Sensor ---*/
		/* Arr Index */
		sensAlarmCh = (cnfg&0x0F);
		if(sensAlarmCh <= 0x05) {																							/*-------- 4-20mA Sensor Alarm CH1-6 (0xD0-0xD5) */	
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_Config420SensorAlarm[sensAlarmCh], sizeof(STCONFIG_SENSORALARM));
			*specLen = sizeof(STCONFIG_SENSORALARM);
		}else if((sensAlarmCh >= 0x06) && (sensAlarmCh <= 0x07)) {						/*-------- ADCSensor Config Alarm CH1 (0xD6) */
			sensAlarmCh -= 0x06;
			/* Copy Config_Structure into Config_Specific(Data Packet) */
			memcpy(confSpec, &struct_ConfigADCSensorAlarm[sensAlarmCh], sizeof(STCONFIG_SENSORALARM));
			*specLen = sizeof(STCONFIG_SENSORALARM);
		}
	}else if(cnfg == CONF_CNFG_MODBUSSENsAL) { 															/*--- MODBUS FlowSensor Alarm ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigMODBUSSensorAlarm, sizeof(STCONFIG_SENSORALARM));
		*specLen = sizeof(STCONFIG_SENSORALARM);
	}else if(cnfg == CONF_CNFG_PULSENsAL) {																	/*--- Pulse Sensor Alarm ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigPulseSensorAlarm, sizeof(STCONFIG_SENSORALARM));
		*specLen = sizeof(STCONFIG_SENSORALARM);
	}else if(cnfg == CONF_CNFG_VALVE) {																			/*--- Valve Config ---*/
		/* Copy Config_Structure into Config_Specific(Data Packet) */
		memcpy(confSpec, &struct_ConfigValve, sizeof(STCONFIG_VALVE));
		*specLen = sizeof(STCONFIG_VALVE);
	}else {
		printDebug_Semphr(MINOR_ERR, "<ucPTCHANDLE_ConfigRead> CNFG not define yet. \r\n");
		return 2;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function handle Write Configuration packet.
  * @param  Configuration Specific Structure
  * @retval Handle Status
  */	
uint8_t ucPTCHANDLE_ConfigWrite(uint8_t *confSpec, uint8_t src) {
	
	uint8_t   printLevel 		= INFO;
	uint8_t 	analogIndex  	= 0;
	uint8_t 	dinCh					= 0;
	uint8_t		sensAlarmCh		= 0;
	uint8_t 	validate_res	= 0;
	uint8_t		copy_cmd			= 0;
	float 		tmp_calValue	= 0.0;
	float			tmp_float			= 0.0;
	bool			isFileOpen		= false;
	
	STCONFIG_DEV 					tmp_DevConfig;
	STCONFIG_PROPERTIES		tmp_ConfigProperties;
	STCONFIG_OPERATING		tmp_ConfigOperating;
	STCONFIG_BUZZER				tmp_ConfigBuzzer;
	STCONFIG_WORKING			tmp_ConfigWorking;
	STCONFIG_NETWORK 			tmp_ConfigNetwork; 
	STCONFIG_ANALOGSENSOR tmp_ConfigAnalogSensor;
	STCONFIG_MODBUSSENSOR	tmp_ConfigMODBUSSensor;
	STCONFIG_PULSESENSOR	tmp_ConfigPulseSensor;
	STCONFIG_PULSEADJ			tmp_ConfigPulseADJ;
	STCONFIG_DIGITALsIN 	tmp_ConfigDIN;
	STCONFIG_SENSORALARM 	tmp_ConfigSensorAlarm;
	STCONFIG_VALVE				tmp_ConfigValve;
	STCONFIG_420CAL				tmp_Config420Cal;
	ST_CONFIG_Q						stBuf_conf;
		
	if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_DEV) {																												/*--- Dev Config ---*/
		memcpy(&tmp_DevConfig, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_DEV));															/* Copy to temp config structure */
		vMAIN_PrintConfig(printLevel, DEV_CONF, &tmp_DevConfig, CH_NONE);																			/* Print Config */
		validate_res = PTCPARSER_ValidateConfigDev(&tmp_DevConfig);																						/* Validate config */
		if(validate_res) {
			printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Validate failed(ERR_%d) \r\n", validate_res);
			return 1;
		}else {
			if(EEPROM_WriteBuf(DEVeCONF_BASEADDR, &tmp_DevConfig, sizeof(STCONFIG_DEV))) {										/* Save Configuration into EEPROM */
				printDebug(MAJOR_ERR, "<PTCHANDLE_ConfigWrite>: EEPROM_Write Deverloper_Configuration Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_DevConfig, &tmp_DevConfig, sizeof(STCONFIG_DEV));																	/* Save Configuration into Config_Structure */
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_PROPERTIES) {																						/*--- Properties Config ---*/
		memcpy(&tmp_ConfigProperties, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_PROPERTIES));
		vMAIN_PrintConfig(printLevel, PROPERTIES_CONF, &tmp_ConfigProperties, CH_NONE);
		if(EEPROM_WriteBuf(PROPERTIES_BASEADDR, &tmp_ConfigProperties, sizeof(STCONFIG_PROPERTIES))) {			/* Save Configuration into EEPROM */
			printDebug(MAJOR_ERR, "<PTCHANDLE_ConfigWrite>: EEPROM_Write Properties_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigProperties, &tmp_ConfigProperties, sizeof(STCONFIG_PROPERTIES));								/* Save Configuration into Config_Structure */
			if(src == DEBUG_PORT) {																																							/* If config from debug port, Set flag for update to server */
				flag_isConfPropUpdate = true;
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_CMD) {																									/*--- Config Management ---*/
		switch(confSpec[CONF_INDEX_SPEC]) {
			case SET_DEFAULT_INFO:
				if(ucMAIN_SetDefaultInformation()) {
					_ConfigCmdACK = CONF_ACK_ERR;
				}else {
					_ConfigCmdACK = CONF_ACK_OK;
				}
				break;
			case LOAD_INFO:
				copy_cmd = LOAD_INFO;
				if(!xQueueSend(Queue_CopyFromSD, &copy_cmd, 0)) {
					printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Send Queue_CopyFromSD failed. \r\n");
				}
				break;
			case SET_DEFAULT_CONF:
				if(ucMAIN_SetDefaultConfig()) {
					_ConfigCmdACK = CONF_ACK_ERR;
				}else {
					_ConfigCmdACK = CONF_ACK_OK;
				}
				break;
			case LOAD_CONF:
				copy_cmd = LOAD_CONF;
				if(!xQueueSend(Queue_CopyFromSD, &copy_cmd, 0)) {
					printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Send Queue_CopyFromSD failed. \r\n");
				}
				break;
			default:
				break;
		}
		return CONF_CNFG_CMD;
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_OPERATING) {																						/*--- Operating Config ---*/
		memcpy(&tmp_ConfigOperating, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_OPERATING));									/* Copy to temp config structure */
		vMAIN_PrintConfig(printLevel, OPERATING_CONF, &tmp_ConfigOperating, CH_NONE);
		validate_res = PTCPARSER_ValidateConfigOperating(&tmp_ConfigOperating);																/* Validate Config */
		if(validate_res) {
			printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Validate failed(ERR_%d) \r\n", validate_res);
			return 1;
		}else {
			if(EEPROM_WriteBuf(OPERATING_BASEADDR, &tmp_ConfigOperating, sizeof(STCONFIG_OPERATING))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write Operating_Configuration Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_ConfigOperating, &tmp_ConfigOperating, sizeof(STCONFIG_OPERATING));								/* Save Configuration into Config_Structure */
				if(src == DEBUG_PORT) {																																						/* If config from debug port, Set flag for update to server */
					flag_isConfOperUpdate = true;
				}
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_BUZZER) {																								/*--- Buzzer Config ---*/
		memcpy(&tmp_ConfigBuzzer, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_BUZZER));												/* Copy to temp config structure */
		vMAIN_PrintConfig(printLevel, BUZZER_CONF, &tmp_ConfigBuzzer, CH_NONE);
		validate_res = PTCPARSER_ValidateConfigBuzzer(&tmp_ConfigBuzzer);																			/* Validate Config */
		if(validate_res) {
			printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Validate failed(ERR_%d) \r\n", validate_res);
			return 1;
		}else {
			if(EEPROM_WriteBuf(BUZZER_BASEADDR, &tmp_ConfigBuzzer, sizeof(STCONFIG_BUZZER))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write Buzzer_Configuration Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_ConfigBuzzer, &tmp_ConfigBuzzer, sizeof(STCONFIG_BUZZER));													/* Save Configuration into Config_Structure --*/
				if(src == DEBUG_PORT) {
					flag_isConfBuzzUpdate = true;
				}
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_WORKING) {																							/*--- Working Config ---*/
		memcpy(&tmp_ConfigWorking, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_WORKING));											
		vMAIN_PrintConfig(printLevel, WORKING_CONF, &tmp_ConfigWorking, CH_NONE);
		validate_res = PTCPARSER_ValidateConfigWorking(&tmp_ConfigWorking);																		
		if(validate_res) {
			printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Validate failed(ERR_%d) \r\n", validate_res);
			return 1;
		}else {
			if(EEPROM_WriteBuf(WORKING_BASEADDR, &tmp_ConfigWorking, sizeof(STCONFIG_WORKING))) {					
				printDebug(MAJOR_ERR, "[EEPROM]: Write Working_Configuration Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_ConfigWorking, &tmp_ConfigWorking, sizeof(STCONFIG_WORKING));
				if(src == DEBUG_PORT) {
					flag_isConfWorkUpdate = true;
				}
				_Transmit_Period = 0;				// Reset transmission data time
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_NETWORK) {																							/*--- Network Config ---*/
		memcpy(&tmp_ConfigNetwork, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_NETWORK));
		vMAIN_PrintConfig(printLevel, NETWORK_CONF, &tmp_ConfigNetwork, CH_NONE);
		validate_res = PTCPARSER_ValidateConfigNetwork(&tmp_ConfigNetwork);
		if(validate_res) {
			printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite>: Validate failed(ERR_%d) \r\n", validate_res);
			return 1;
		}else {
			if(EEPROM_WriteBuf(NETWORK_BASEADDR, &tmp_ConfigNetwork, sizeof(STCONFIG_NETWORK))) {
				_LED_ERR_ON = LED_ERR_TIME;
				printDebug(MAJOR_ERR, "[EEPROM]: Write Network_Configuration Error \r\n");
				return 1;
			}else {
				memcpy(&struct_ConfigNetwork, &tmp_ConfigNetwork, sizeof(STCONFIG_NETWORK));
				if(src == DEBUG_PORT) {
					flag_isConfNetwUpdate = true;
				}
			}
		}
	}else if((confSpec[CONF_INDEX_CNFG]&0xF0) == CONF_CNFG_420SENSOR1) {																		/*--- Analog Sensor ---*/
		analogIndex = (confSpec[CONF_INDEX_CNFG]&0x0F); 																											/* Index 4-20mA Sensor Channel id */
		if(analogIndex <= 5) {																																									/*--- 4-20mA Sensor Config CH1-6 (0xA0-0xA5) ---*/	
			memcpy(&tmp_ConfigAnalogSensor, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_ANALOGSENSOR));
			vMAIN_PrintConfig(printLevel, SEN420_CONF, &tmp_ConfigAnalogSensor, analogIndex);
			if(EEPROM_WriteBuf(SEN420CONF_BASEADDR[analogIndex], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write 4-20mASensor_Configuration CH_%d Error \r\n", analogIndex+1);
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_Config420Sensor[analogIndex], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
				if(src == DEBUG_PORT) {
					flag_isConf420SensUpdate[analogIndex] = true;
				}
			}
		}else if((analogIndex >= 0x06) && (analogIndex <= 0x07)) {																							/*--- ADCSensor Config CH1-2 (0xA6-0xA7) ---*/
			analogIndex -= 0x06;																																									/* Update index */
			memcpy(&tmp_ConfigAnalogSensor, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_ANALOGSENSOR));
			vMAIN_PrintConfig(printLevel, SENADC_CONF, &tmp_ConfigAnalogSensor, analogIndex);
			if(EEPROM_WriteBuf(SENADCCONF_BASEADDR[analogIndex], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write ADCSensor_Configuration CH_%d Error \r\n", analogIndex+1);
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_ConfigADCSensor[analogIndex], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
				if(src == DEBUG_PORT) {
					flag_isConfADCSensUpdate[analogIndex] = true;
				}
			}
		}else if((analogIndex >= 0x08) && (analogIndex <= 0x0D)) {																							/*--- 4-20mA Sensor Calibration CH1-6 (0xA8-0xAD) ---*/
			analogIndex -= 0x08;																																									/* Update index */
			tmp_Config420Cal.calculation 	= confSpec[CONF_INDEX_SPEC];
			memcpy(&tmp_calValue, &confSpec[CONF_INDEX_SPEC+1], sizeof(float));
			tmp_Config420Cal.cal_val 			= tmp_calValue;
			tmp_Config420Cal.real_val 		= _val420mA_sensor[analogIndex];
			tmp_float											= (((float)_adc420mA_sensor[analogIndex]*102.4)/(4095.0*4.99));
			tmp_Config420Cal.constant 		= tmp_Config420Cal.cal_val - tmp_float;
			vMAIN_PrintConfig(printLevel, SEN420CAL_CONF, &tmp_Config420Cal, analogIndex);
			if(EEPROM_WriteBuf(SEN420CHnCAL_BASEADDR[analogIndex], &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write 420mASensor_Calibrate CH_%d Error \r\n", analogIndex+1);
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				struct_Config420Cal[analogIndex].calculation 	= tmp_Config420Cal.calculation;
				struct_Config420Cal[analogIndex].cal_val 			= tmp_Config420Cal.cal_val;
				struct_Config420Cal[analogIndex].real_val 		= tmp_Config420Cal.real_val;
				struct_Config420Cal[analogIndex].constant 		= tmp_Config420Cal.constant;
			}
		}else if(analogIndex == 0x0E) {																																					/*--- 4-20mAInput Valve Calibration (0xAE) ---*/
			tmp_Config420Cal.calculation 	= confSpec[CONF_INDEX_SPEC];
			memcpy(&tmp_calValue, &confSpec[CONF_INDEX_SPEC+1], sizeof(float));
			tmp_Config420Cal.cal_val 			= tmp_calValue;
			tmp_Config420Cal.real_val 		= _Valve_mA;
			tmp_float											= (((float)_adc420mA_inputVal*102.4)/(4095.0*4.99));
			tmp_Config420Cal.constant 		= tmp_Config420Cal.cal_val - tmp_float;
			vMAIN_PrintConfig(printLevel, VALVE420INCAL_CONF, &tmp_Config420Cal, CH_NONE);
			if(EEPROM_WriteBuf(VALVE420INCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write In420mAValve_Calibrate Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				/*-- Save Configuration into Config_Structure --*/
				struct_Config420ValveInCal.calculation 	= tmp_Config420Cal.calculation;
				struct_Config420ValveInCal.cal_val 			= tmp_Config420Cal.cal_val;
				struct_Config420ValveInCal.real_val 		= tmp_Config420Cal.real_val;
				struct_Config420ValveInCal.constant 		= tmp_Config420Cal.constant;
			}
		}else if(analogIndex == 0x0F) {																																				/*--- 4-20mAOutput Valve Calibration (0xAF) ---*/
			tmp_Config420Cal.calculation 	= confSpec[CONF_INDEX_SPEC];
			memcpy(&tmp_calValue, &confSpec[CONF_INDEX_SPEC+1], sizeof(float));
			tmp_Config420Cal.cal_val 			= tmp_calValue;
			tmp_Config420Cal.real_val			= 0.0; // un-use
			tmp_float 										= (TCLICK_SLOPE * tmp_Config420Cal.cal_val);
			tmp_Config420Cal.constant 		= _ADC_TClick - tmp_float;
			vMAIN_PrintConfig(printLevel, VALVE420OUTCAL_CONF, &tmp_Config420Cal, CH_NONE);
			if(EEPROM_WriteBuf(VALVE420OUTCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write Out420mAValve_Calibrate Error \r\n");
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				struct_Config420ValveOutCal.calculation 	= tmp_Config420Cal.calculation;
				struct_Config420ValveOutCal.cal_val 			= tmp_Config420Cal.cal_val;
				struct_Config420ValveOutCal.real_val			= tmp_Config420Cal.real_val;
				struct_Config420ValveOutCal.constant 			= tmp_Config420Cal.constant;
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_MODBUSSENS) {																						/*--- MODBUS FlowSensor Config ---*/
		memcpy(&tmp_ConfigMODBUSSensor, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_MODBUSSENSOR));
		vMAIN_PrintConfig(printLevel, SENMODBUS_CONF, &tmp_ConfigMODBUSSensor, CH_NONE);
		if(EEPROM_WriteBuf(SENsMODBUSCONF_BASEADDR, &tmp_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write MODBUSFlowSensor_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigMODBUSSensor, &tmp_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR));
			if(src == DEBUG_PORT) {
					flag_isConfMODBUSSensUpdate = true;
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_PULSESENS) {																						/*--- PULSES FlowSensor Config ---*/
		memcpy(&tmp_ConfigPulseSensor, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_PULSESENSOR));
		vMAIN_PrintConfig(printLevel, SENPULSE_CONF, &tmp_ConfigPulseSensor, CH_NONE);
		if(EEPROM_WriteBuf(SENsPULCONF_BASEADDR, &tmp_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write PULSEFlowSensor_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			delay_ms(300);
			_Sensor_Data.dataPULSENet = 0.0;
			struct_Status.net_flow 		= 0.0;
			tmp_float 								= 0.0;
			if(EEPROM_WriteBuf(NETFLOW_BASEADDR, &tmp_float, sizeof(float))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write Status NetFlow Error (1 times) \r\n");
				delay_ms(500);
				if(EEPROM_WriteBuf(NETFLOW_BASEADDR, &tmp_float, sizeof(float))) {
					printDebug(MAJOR_ERR, "[EEPROM]: Write Status NetFlow Error (2 times) \r\n");
				}
			}
			memcpy(&struct_ConfigPulseSensor, &tmp_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR));
			if(src == DEBUG_PORT) {
				flag_isConfPulsSensUpdate = true;
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_PULSEADJ) {																							/*-- PULSE FlowSensor Value Adjustment --*/
		memcpy(&tmp_ConfigPulseADJ.multiplier_value, &confSpec[CONF_INDEX_SPEC], sizeof(float));
		vMAIN_PrintConfig(printLevel, SENPULSEADJ_CONF, &tmp_ConfigPulseADJ, CH_NONE);
		if(EEPROM_WriteBuf(SENPULSEADJ_BASEADDR, &tmp_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write PULSEFlowValue_Adjustment Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigPulseADJ.multiplier_value, &tmp_ConfigPulseADJ.multiplier_value, sizeof(float));
			/* Reset data_pulse buffer */
			_index_pulseData		= 0;
			_PulseData_Counter 	= 0;	
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_VALVE) {																								/*--- Valve Config ---*/
		memcpy(&tmp_ConfigValve, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_VALVE));
		vMAIN_PrintConfig(printLevel, VALVE_CONF, &tmp_ConfigValve, CH_NONE);
		if(EEPROM_WriteBuf(VALVECONF_BASEADDR, &tmp_ConfigValve, sizeof(STCONFIG_VALVE))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write Valve_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigValve, &tmp_ConfigValve, sizeof(STCONFIG_VALVE));
			if(src == DEBUG_PORT) {
				flag_isConfValveUpdate = true;
			}
		}
	}else if((confSpec[CONF_INDEX_CNFG]&0xF0) == CONF_CNFG_DIN1) {																					/*--- Digital Input CH1-12 ---*/
		dinCh = (confSpec[CONF_INDEX_CNFG]&0x0F);																															/* Index Digital Input Channel id */
		memcpy(&tmp_ConfigDIN, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_DIGITALsIN));
		vMAIN_PrintConfig(printLevel, DIN_CONF, &tmp_ConfigDIN, dinCh);
		if(EEPROM_WriteBuf(DIGItIN_BASEADDR[dinCh], &tmp_ConfigDIN, sizeof(STCONFIG_DIGITALsIN))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write DIN_Configuration CH_%d Error \r\n", dinCh+1);
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigDIN[dinCh], &tmp_ConfigDIN, sizeof(STCONFIG_DIGITALsIN));
			if(src == DEBUG_PORT) {
				flag_isConfDINUpdate[dinCh] = true;
			}
		}
	}else if((confSpec[CONF_INDEX_CNFG]&0xF0) == CONF_CNFG_420SENsAL1) {																		/*--- Analog Sensor Alarm ---*/							
		sensAlarmCh = (confSpec[CONF_INDEX_CNFG]&0x0F);																												/* Arr Index */
		if(sensAlarmCh <= 0x05) {																																								/*--- 4-20mA Sensor Alarm CH1-6 (0xD0-0xD5) ---*/	
			memcpy(&tmp_ConfigSensorAlarm, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_SENSORALARM));
			vMAIN_PrintConfig(printLevel, SEN420ALARM_CONF, &tmp_ConfigSensorAlarm, sensAlarmCh);
			if(EEPROM_WriteBuf(SEN420ALARMn_BASEADDR[sensAlarmCh], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write 4-20mASensorAlarm_Configuration CH_%d Error \r\n", sensAlarmCh+1);
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_Config420SensorAlarm[sensAlarmCh], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
				if(src == DEBUG_PORT) {
					flag_isConf420AlamUpdate[sensAlarmCh] = true;
				}
			}
		}else if((sensAlarmCh >= 0x06) && (sensAlarmCh <= 0x07)) {																							/*--- ADCSensor Config Alarm CH1 (0xD6) ---*/
			sensAlarmCh -= 0x06;																																									/* Update arr index */
			memcpy(&tmp_ConfigSensorAlarm, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_SENSORALARM));
			vMAIN_PrintConfig(printLevel, SENADCALARM_CONF, &tmp_ConfigSensorAlarm, sensAlarmCh);
			if(EEPROM_WriteBuf(SENADCALARMn_BASEADDR[sensAlarmCh], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
				printDebug(MAJOR_ERR, "[EEPROM]: Write ADCSensorAlarm_Configuration CH_%d Error \r\n", sensAlarmCh+1);
				_LED_ERR_ON = LED_ERR_TIME;
				return 1;
			}else {
				memcpy(&struct_ConfigADCSensorAlarm[sensAlarmCh], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
				if(src == DEBUG_PORT) {
					flag_isConfADCAlamUpdate[sensAlarmCh] = true;
				}
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_MODBUSSENsAL) { 																				/*--- MODBUS FlowSensor Alarm ---*/
		memcpy(&tmp_ConfigSensorAlarm, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_SENSORALARM));
		vMAIN_PrintConfig(printLevel, SENMODBUSALARM_CONF, &tmp_ConfigSensorAlarm, CH_NONE);
		if(EEPROM_WriteBuf(SENsMODBUSALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write MODBUSFlowSensorAlarm_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigMODBUSSensorAlarm, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
			if(src == DEBUG_PORT) {
				flag_isConfMODBUSAlamUpdate = true;
			}
		}
	}else if(confSpec[CONF_INDEX_CNFG] == CONF_CNFG_PULSENsAL) {																						/*--- Pulse Sensor Alarm ---*/
		memcpy(&tmp_ConfigSensorAlarm, &confSpec[CONF_INDEX_SPEC], sizeof(STCONFIG_SENSORALARM));
		vMAIN_PrintConfig(printLevel, SENPULSEALARM_CONF, &tmp_ConfigSensorAlarm, CH_NONE);
		if(EEPROM_WriteBuf(SENsPULALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write FlowFlowSensorAlarm_Configuration Error \r\n");
			_LED_ERR_ON = LED_ERR_TIME;
			return 1;
		}else {
			memcpy(&struct_ConfigPulseSensorAlarm, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
			if(src == DEBUG_PORT) {
				flag_isConfPulsAlamUpdate = true;
			}
		}
	}else {
		printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_ConfigWrite> CNFG not define yet. \r\n");
		return 2;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function handle TimeSync packet.
  * @param  
  * @retval Error code.
  */
uint8_t ucPTCHANDLE_RemoteCtrl(uint8_t cid, uint8_t *value) {
	
	uint8_t 				doutCH 				= 0;   		// Index Digital Output
	uint32_t				valve_value 	= 0;			// Butterfly Valve %Open
	uint8_t 				i;
	ST_UPDATE_STAT	up_stat;
	
//	printDebug_Semphr(INFO, "<PTCHANDLE_RemoteCtrl> CID: %02X   VAL: %d \r\n", cid, value[3]);
	
	if((cid&0xF0) == 0x20) {
		doutCH = (cid & 0x0F) - 1;
		/* Validate CID */
		if((cid != 0x2F) && (doutCH >= MAX_DOUT)) {
			return 1;
		}
		/* Validate Value */
		if((value[0] == 1) || (value[0] == 0)) {
			if(cid == 0x2F) {
				for(i = 0; i < MAX_DOUT; i++) {
					GPIO_WriteBit((GPIO_TypeDef*)DOUTn_PORT[i], DOUTn_PIN[i], value[0]);
					if(EEPROM_WriteBuf(DOUTn_BASEADDR[i], &value[0], sizeof(uint8_t))) {
						_LED_ERR_ON = LED_ERR_TIME;
						printDebug(MINOR_ERR, "<PTCHANDLE_RemoteCtrl>: EEPROM_Write DOUT_%d Status Error \r\n", i+1);
					}
				}
			}else {
				GPIO_WriteBit((GPIO_TypeDef*)DOUTn_PORT[doutCH], DOUTn_PIN[doutCH], value[0]);
				if(EEPROM_WriteBuf(DOUTn_BASEADDR[doutCH], &value[0], sizeof(uint8_t))) {
					_LED_ERR_ON = LED_ERR_TIME;
					printDebug(MINOR_ERR, "<PTCHANDLE_RemoteCtrl>: EEPROM_Write DOUT_%d Status Error \r\n", doutCH+1);
				}
			}
		}else {
			return 2;
		}
	}else if(cid == CID_DRVVALVE) {
		memcpy(&valve_value, value, sizeof(uint32_t));
		/* Validate Value */
		if(valve_value <= 100) {
			if(!xQueueSend(Queue_CtrlValve, &valve_value, 0)) {
				printDebug_Semphr(MINOR_ERR, "<PTCHANDLE_RemoteCtrl>: Send Queue_CtrlValve failed. \r\n");
			}else {
				if(EEPROM_WriteBuf(VALVE_BASEADDR, &valve_value, sizeof(uint8_t))) {
					_LED_ERR_ON = LED_ERR_TIME;
					printDebug(MINOR_ERR, "<PTCHANDLE_RemoteCtrl>: EEPROM_Write Valve Status Error \r\n");
				}
			}
		}else {
			return 3;
		}
	}else {
		return 4;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This function handle TimeSync packet.
  * @param  
  * @retval None
  */
uint8_t vPTCHANDLE_TimeSync(uint8_t *buf) {
	
	int32_t timesync = 0;
	uint32_t currentTime = 0;
//	uint8_t i;
	
	/* internal RTC */
	currentTime = uiRTC_GetEpochTime();
	
	memcpy(&timesync, buf, sizeof(uint32_t));
	
	if(timesync <= 0) {
		return 1;
	}
	
	if(abs(timesync-currentTime) > 120) {  						// if difference more than 2 min.
		if(ucRTC_SetDateTimeFromEpoch(timesync)) {
			/* ucRTC_SetDateTimeFromEpoch Failed */
			return 2;
		}else {
			_Transmit_Period = 0;				// Reset transmission data time
			printDebug_Semphr(INFO, "<PTCHANDLE_TimeSync> Setting Date & Time (%d) \r\n", timesync);
		}
		
	}
	return 0;
}
/*==========================================================================================*/


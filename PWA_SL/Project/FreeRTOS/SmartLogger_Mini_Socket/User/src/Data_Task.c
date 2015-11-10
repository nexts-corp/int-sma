/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	DataBuffer_Task.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	22-08-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"
#include "Sensors_Task.h"
#include "Manager_Task.h"
#include "SD_Task.h"
#include "SL_ConfigStruct.h"
#include "SL_PinDefine.h"
#include "stm32f4_SDCard.h"

#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_DataQueue.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_Debug.h"
#include "INT_STM32F4_adc.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_log.h"
#include "tm_stm32f4_ds1307.h"

/* Function Prototype ----------------------------------------------------*/
float fDATBUF_ReadBoardTemp(void);
uint8_t ucDATBUF_AssemDataSet(DATA_SET *struct_data);
uint8_t ucDATBUF_AssemStatusSet(STATUS_SET *struct_status);
void vDATA_ClearData(uint32_t *timestamp, uint8_t *num_stat, STATUS_SET *stat_set, uint8_t *num_dat, DATA_SET *dat_set);
void vDATBUF_ShowData(void);

__IO bool 			flag_1sec_DataTask = false;

extern uint8_t 					ucGSM_RSSI;
extern __IO float				_BatteryRemain;
extern __IO uint32_t		_Transmit_Period;
extern __IO bool				flag_isSDInstall;
extern __IO uint8_t			_LED_SAMP_ON;
extern __IO uint8_t			_LED_SENT_ON;
extern SENSOR_DATA 			_Sensor_Data;


void vDataBuf_Task(void *pvParameters) {
	
	uint8_t 		i, j;
	uint8_t 		numQueue 					= 0;
	uint8_t			num_DatSet 				= 0;
	uint8_t 		num_StatSet 			= 0;
	uint8_t			num_dataQueue 		= 0;
	uint8_t  		printLevel				= INFO;
	uint32_t		timestamp 				= 0;
	uint32_t		http_timestamp 		= 0;
	uint32_t 		dataRepTimer 			= 0;
	uint32_t 		samplingDataTimer = 0;
	uint32_t		num_sec						= 0;
	uint32_t		sendHTTP_Timer		= 0;
	bool 				isSaveLog					= false;
	bool 				isSendHTTP				= false;
	uint32_t		showData					=0;
	
	RTC_DateTypeDef 	RTC_DateStruct;
	RTC_TimeTypeDef 	RTC_TimeStruct;
	
	TM_DS1307_Time_t	time;
	
	STATUS_SET 				SL_Status[MAX_STATUS];
	DATA_SET 					SL_Data[MAX_DATA];
	PTC_RES 					ptcRes;
	ST_LOG_DATA				data_stat;
	ST_LOG_DATA				log_data;
	
	printDebug_Semphr(DESCRIBE, "[DATA_TASK]: Running... \r\n");	
		
	/* Set output pin to 4096 Hz */
//   TM_DS1307_EnableOutputPin(TM_DS1307_OutputFrequency_32768Hz);
	
	ucTIMER_SetTimer(&samplingDataTimer, struct_ConfigWorking.samplingInterval);
	ucTIMER_SetTimer(&dataRepTimer, struct_ConfigWorking.transmitInterval);
	ucTIMER_SetTimer(&showData, 60);				/* Set show data timer */
	
	while(1) {
		
		/* Device Enable -----------------------------------------------------*/
		if(struct_ConfigOperating.deviceEnable) {
			
			/* Every 1 second ----------------------------------------------------*/
			if(flag_1sec_DataTask) {
				flag_1sec_DataTask = false;
				/* Get date & time */
 				vRTC_GetDateTime(&RTC_DateStruct, &RTC_TimeStruct, 0);
				
				timestamp = uiRTC_DateTimeToEpoch(RTC_DateStruct, RTC_TimeStruct);
				
				/* Sampling Data -----------------------------------------------------*/
				/* Amount of second */
				num_sec = RTC_TimeStruct.RTC_Hours * 60 * 60;
				num_sec += RTC_TimeStruct.RTC_Minutes * 60;
				num_sec += RTC_TimeStruct.RTC_Seconds;
				if(num_sec == 0){
					timestamp += 86400;
				}
				
				if((num_sec % struct_ConfigWorking.samplingInterval) == 0) {
					_LED_SAMP_ON 	= 5;																															// Update Sampling LED Indicator
					num_StatSet 	= ucDATBUF_AssemStatusSet(SL_Status);
					num_DatSet 		= ucDATBUF_AssemDataSet(SL_Data);
					/* Add to send data queue */
					num_dataQueue = ucDATAQUEUE_AddDataStat(timestamp, num_StatSet, SL_Status, num_DatSet, SL_Data);
					if(num_dataQueue == 0) {
						printDebug_Semphr(MINOR_ERR, "[DATA_TASK]: Queue Data Full \r\n");
					}
					/* Set TransmissionTime for first time */
					if(_Transmit_Period == 0) {
						_Transmit_Period = timestamp;
					}
					/* Send to SD_Task to save into data_file */
					if(flag_isSDInstall) {
						data_stat.cmd 				= LOG_CMD_WRITE;
						data_stat.timestamp 	= timestamp;
						data_stat.num_status 	= num_StatSet;
						data_stat.num_data 		= num_DatSet;
						memcpy(data_stat.status, SL_Status, (num_StatSet*sizeof(STATUS_SET)));
						memcpy(data_stat.data, SL_Data, (num_DatSet*sizeof(DATA_SET)));
						if(!xQueueSend(Queue_DataToSD, &data_stat, 0)) {
							printDebug_Semphr(WARNING, "[DATA_TASK]: Send Queue_DataToSD failed. \r\n");
						}
					}else {
						printDebug_Semphr(WARNING, "[DATA_TASK]: Couldn't save data, SD_Card not install. \r\n");
					}
					printDebug_Semphr(INFO, "[DATA_TASK]: Sampling [%d], [%d] ", timestamp, num_sec);
					printDebug_Semphr(INFO, "Date: [%02d] %02d-%02d-%04d  ", 	RTC_DateStruct.RTC_WeekDay, 
																																		RTC_DateStruct.RTC_Date, 
																																		RTC_DateStruct.RTC_Month, 
																																		2000+RTC_DateStruct.RTC_Year);
					printDebug_Semphr(INFO, "Time: %02d:%02d:%02d  \r\n", 		RTC_TimeStruct.RTC_Hours, 
																																		RTC_TimeStruct.RTC_Minutes, 
																																		RTC_TimeStruct.RTC_Seconds);
				}
				
				/* Transmission & Record Data&Status ---------------------------------*/
				if((_Transmit_Period != 0) && (timestamp >= _Transmit_Period)) {
					if((timestamp - _Transmit_Period) > 300) {
						_Transmit_Period = 0; 																					// Guess -> TransmitPeriod wrong, Reset TransmitPeriod
					}else {
						_Transmit_Period += struct_ConfigWorking.transmitInterval;			// Update Transmission Period
					}
					numQueue = ucDATAQUEUE_GetDataAmount();
					printDebug_Semphr(INFO, "[DATA_TASK]: Transmission %d record \r\n", numQueue);
					if(numQueue > 0) {
						_LED_SENT_ON = 5;																								// Update Sent LED Indicator
						for(i = 0; i < numQueue; i++) {
							vDATA_ClearData(&timestamp, &num_StatSet, SL_Status, &num_DatSet, SL_Data); 		/* Clear data */
							ucDATAQUEUE_FetchDataStat(&timestamp, &num_StatSet, SL_Status, &num_DatSet, SL_Data);
							http_timestamp = timestamp;
							if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
								printLevel = INFO;
								printDebug(printLevel, "[DATA_TASK]: Record %d:\r\n", i+1);
								for(j = 0; j < num_DatSet; j ++) {
									printDebug(printLevel, "[DATA_TASK]: channel: %02X  ",SL_Data[j].channel);
									printDebug(printLevel, "dataUnit: %02X  ",SL_Data[j].dataUnit);
									printDebug(printLevel, "sensorType: %02X  ",SL_Data[j].sensorType);
									printDebug(printLevel, "value: %0.4f  \r\n",SL_Data[j].value);
								}
								xSemaphoreGive(xSemaphore);
							}
							
							/*-- Send Data&Status Report to Server --*/
							/*-- For Send HTTP --*/
							isSendHTTP = true;
							ucTIMER_SetTimer(&sendHTTP_Timer, 10);
							isSaveLog = false;
							if((_Communication_Status & COMMU_CONNECTED) && (_Communication_Status & COMMU_JOINED)) {						
								ptcRes = xPROCOMM_SendDataNStatusReportCMD(timestamp, RECORD_RLT, num_StatSet, SL_Status, num_DatSet, SL_Data, struct_DevConfig.retransmitInterval, GSM_PORT);
								if(ptcRes != PTC_OK) {
									printDebug_Semphr(MINOR_ERR, "[DATA_TASK]: Send DataPacket failed(PTC_%d) \r\n", ptcRes);
									isSaveLog = true;
								}
							}else {
								isSaveLog = true;
							}
							
							/*-- Save Data into log file --*/
							if(isSaveLog) {
								if(flag_isSDInstall) {
									log_data.cmd 					= LOG_CMD_WRITE;
									log_data.timestamp 		= timestamp;
									log_data.num_status 	= num_StatSet;
									memcpy(log_data.status, SL_Status, (num_StatSet*sizeof(STATUS_SET)));
									log_data.num_data 		= num_DatSet;
									memcpy(log_data.data, SL_Data, (num_DatSet*sizeof(DATA_SET)));					
									if(!xQueueSend(Queue_DataLogToSD, &log_data, 0)) {
										printDebug_Semphr(MINOR_ERR, "[DATA_TASK]: Send Queue_DataLogToSD failed. \r\n");
									}
								}else {
									printDebug_Semphr(WARNING, "[DATA_TASK]: Couldn't save log_data, SD_Card not install. \r\n");
								}
							}
						}
					}
				}
				
				if(isSendHTTP) {
					if(cTIMER_CheckTimerExceed(sendHTTP_Timer)) {
						isSendHTTP = false;
						xPROCOMM_SendHTTPPacket(http_timestamp, num_StatSet, SL_Status, num_DatSet, SL_Data);
					}
				}
				
			} /* Flag 1 sec */
			
			if(cTIMER_CheckTimerExceed(showData)){
				ucTIMER_SetTimer(&showData, 60);
				
				if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
					vDATBUF_ShowData();

					xSemaphoreGive(xSemaphore);
				}
			}
		} /* Device Enable */
		
	} /* End main loop */
} /* End vDataBuf_Task() */
/*==========================================================================================*/
/*==========================================================================================*/
/**
  * @brief  show data.
  * @param  
  * @retval None.
  */
void vDATBUF_ShowData() {
	uint8_t		i;
	uint8_t		printLevel = ALWAYS;
	
	
	/* GSM Signal */
	printDebug(printLevel, "[DATA_TASK]: Signal Quality : %d%%  \r\n",ucGSM_RSSI);
	
	/* Battery Remaining */
	printDebug(printLevel, "[DATA_TASK]: Battery Remaining : %0.2f v \r\n",_BatteryRemain);
	
	/*-- 4-20mA Sensor	--*/
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(struct_Config420Sensor[i].sensorEnable == SENSOR_ENABLE) {
			printDebug(printLevel, "[DATA_TASK]: channel: %02X (4-20mA)	",i+1);
			printDebug(printLevel, "dataUnit: %02X  ",struct_Config420Sensor[i].dataUnit);
			printDebug(printLevel, "sensorType: %02X  ",struct_Config420Sensor[i].sensorType);
			printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.data420mA[i]);
		}
	}
	
	/*-- Analog Sensor (ADC)	--*/
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(struct_ConfigADCSensor[i].sensorEnable == SENSOR_ENABLE) {
			printDebug(printLevel, "[DATA_TASK]: channel: %02X  ",0x30 | (i+1));
			printDebug(printLevel, "dataUnit: %02X  ",struct_ConfigADCSensor[i].dataUnit);
			printDebug(printLevel, "sensorType: %02X  ",struct_ConfigADCSensor[i].sensorType);
			printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.dataADC[i]);
		}
	}
	
	/*-- MODBUS Flow Sensor --*/
	if(struct_ConfigMODBUSSensor.sensor_enable) {
		printDebug(printLevel, "[DATA_TASK]: channel: %02X(FLOWRATE)	",MB_CH1_FLOWRATE);
		printDebug(printLevel, "dataUnit: %02X  ",UNIT_M3pHR);
		printDebug(printLevel, "sensorType: %02X  ",TYP_FLOW);
		printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.dataMODBUSFlow);
		printDebug(printLevel, "[DATA_TASK]: channel: %02X(POS)		",MB_CH1_POS);
		printDebug(printLevel, "dataUnit: %02X  ",UNIT_M3);
		printDebug(printLevel, "sensorType: %02X  ",TYP_FLOW);
		printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.dataMODBUSPosNet);
		printDebug(printLevel, "[DATA_TASK]: channel: %02X(NEG)  ",MB_CH1_NEG);
		printDebug(printLevel, "dataUnit: %02X  ",UNIT_M3);
		printDebug(printLevel, "sensorType: %02X  ",TYP_FLOW);
		printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.dataMODBUSNegNet);
	}
	
	/*-- PULSE Flow Sensor --*/
	if(struct_ConfigPulseSensor.sensor_enable) {
		printDebug(printLevel, "[DATA_TASK]: channel: %02X(FLOWRATE)	",PUL_CH1_FLOWRATE);
		printDebug(printLevel, "dataUnit: %02X  ",UNIT_M3pHR);
		printDebug(printLevel, "sensorType: %02X  ",TYP_FLOW);
		printDebug(printLevel, "value: %0.4f  \r\n",_Sensor_Data.dataPULSEFlow);
		printDebug(printLevel, "[DATA_TASK]: channel: %02X(NETFLOW)	",PUL_CH1_NETFLOW);
		printDebug(printLevel, "dataUnit: %02X  ",UNIT_M3);
		printDebug(printLevel, "sensorType: %02X  ",TYP_FLOW);
		printDebug(printLevel, "value: %0.4f  \r\n",(_Sensor_Data.dataPULSENet + struct_ConfigPulseSensor.net_total));
	}
}

/*==========================================================================================*/
/**
  * @brief  Clear data in memory.
  * @param  
  * @retval None.
  */
void vDATA_ClearData(uint32_t *timestamp, uint8_t *num_stat, STATUS_SET *stat_set, uint8_t *num_dat, DATA_SET *dat_set) {
	
	uint8_t i;
//	uint32_t tmp = 0;
	
	*timestamp 	= 0;
	*num_stat 	= 0;
	*num_dat 		= 0;
	for(i = 0; i < MAX_STATUS; i++) {
		stat_set[i].statusType 	= 0;
		stat_set[i].valueType 	= 0;
		stat_set[i].value_int = 0;
	}
	for(i = 0; i < MAX_DATA; i++) {
		dat_set[i].channel		= 0;
		dat_set[i].dataUnit		= 0;
		dat_set[i].sensorType	= 0;
		dat_set[i].value			= 0.0;
	}
	
}
/*==========================================================================================*/
/**
  * @brief  Assembly data set
  * @param  
  * @retval Number of data set
  */
uint8_t ucDATBUF_AssemDataSet(DATA_SET *struct_data) {
	
	uint8_t		i;
	uint8_t		num_set = 0;

	/*-- 4-20mA Sensor	--*/
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(struct_Config420Sensor[i].sensorEnable == SENSOR_ENABLE) {
			struct_data[num_set].channel 			= i+1;
			struct_data[num_set].sensorType 	= struct_Config420Sensor[i].sensorType;
			struct_data[num_set].dataUnit 		= struct_Config420Sensor[i].dataUnit;
			struct_data[num_set].value 				= _Sensor_Data.data420mA[i];
			num_set++;
		}
	}
	
	/*-- Analog Sensor (ADC)	--*/
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(struct_ConfigADCSensor[i].sensorEnable == SENSOR_ENABLE) {
			struct_data[num_set].channel 			= 0x30 | (i+1);
			struct_data[num_set].sensorType 	= struct_ConfigADCSensor[i].sensorType;
			struct_data[num_set].dataUnit 		= struct_ConfigADCSensor[i].dataUnit;
			struct_data[num_set].value 				= _Sensor_Data.dataADC[i];
			num_set++;
		}
	}
	
	/*-- MODBUS Flow Sensor --*/
	if(struct_ConfigMODBUSSensor.sensor_enable) {
		struct_data[num_set].channel 			= MB_CH1_FLOWRATE;
		struct_data[num_set].sensorType 	= TYP_FLOW;
		struct_data[num_set].dataUnit 		= UNIT_M3pHR;
		struct_data[num_set].value 				= _Sensor_Data.dataMODBUSFlow;
		num_set++;
		struct_data[num_set].channel 			= MB_CH1_POS;
		struct_data[num_set].sensorType 	= TYP_FLOW;
		struct_data[num_set].dataUnit 		= UNIT_M3;
		struct_data[num_set].value 				= _Sensor_Data.dataMODBUSPosNet;
		num_set++;
		struct_data[num_set].channel 			= MB_CH1_NEG;
		struct_data[num_set].sensorType 	= TYP_FLOW;
		struct_data[num_set].dataUnit 		= UNIT_M3;
		struct_data[num_set].value 				= _Sensor_Data.dataMODBUSNegNet;
		num_set++;
	}
	
	/*-- PULSE Flow Sensor --*/
	if(struct_ConfigPulseSensor.sensor_enable) {
		struct_data[num_set].channel 			= PUL_CH1_FLOWRATE;
		struct_data[num_set].sensorType 	= TYP_FLOW;
		struct_data[num_set].dataUnit 		= UNIT_M3pHR;
		struct_data[num_set].value 				= _Sensor_Data.dataPULSEFlow;
		num_set++;
		struct_data[num_set].channel 			= PUL_CH1_NETFLOW;
		struct_data[num_set].sensorType 	= TYP_FLOW;
		struct_data[num_set].dataUnit 		= UNIT_M3;
		struct_data[num_set].value 				= (_Sensor_Data.dataPULSENet + struct_ConfigPulseSensor.net_total);
		num_set++;
	}
	
	return num_set;
}
/*==========================================================================================*/
/**
  * @brief  Get and Assembly status set
  * @param  
  * @retval None
  */
uint8_t ucDATBUF_AssemStatusSet(STATUS_SET *struct_status) {
	
	uint8_t				i 			= 0;
	static float	temp		= 0.0;
	
	/* GSM Signal */
	struct_status[0].statusType 		= GSM;
	struct_status[0].valueType 			= VINT;
	struct_status[0].value_int 			= (uint32_t)ucGSM_RSSI;
	
	/* Battery Remaining */
	struct_status[1].statusType			= BAT;
	struct_status[1].valueType			= VFLOAT;
	struct_status[1].value_float		= _BatteryRemain;
	
	/* Temperature on board */
	struct_status[2].statusType 		= BTEMP;
	struct_status[2].valueType 			= VFLOAT;
	taskDISABLE_INTERRUPTS();
	ucMNG_ReadDS1820(&temp);
	taskENABLE_INTERRUPTS();
	struct_status[2].value_float		= temp;
//	printDebug_Semphr(INFO, "Temperature: %0.2f C \r\n", struct_status[2].value_float);
	
	/* Butterfly Valve */
	struct_status[3].statusType			= VALV;
	struct_status[3].valueType			= VINT;
	struct_status[3].value_int			= 0;
	
	/* Digital I/O status */
	struct_status[4].statusType			= DIGI;
	struct_status[4].valueType			= VBIT;
	// Reset Value
	struct_status[4].value_char[0] = 0;
	struct_status[4].value_char[1] = 0;
	struct_status[4].value_char[2] = 0;
	struct_status[4].value_char[3] = 0;

	for(i = 0; i < MAX_DOUT; i++) {
		struct_status[4].value_char[1] |= ((GPIO_ReadOutputDataBit((GPIO_TypeDef*)DOUTn_PORT[i], DOUTn_PIN[i]) & 0xFF) << i);
	}
	// bit16 - bit23, Digital Input CH9-12
	for(i = 0; i < 4; i++) {
		struct_status[4].value_char[2] |= ((GPIO_ReadInputDataBit((GPIO_TypeDef*)DINn_PORT[i+8], DINn_PIN[i+8]) & 0xFF) << i);
	}
	
	// bit24 - bit31, Digital Input CH1-8
	for(i = 0; i < 8; i++) {
		struct_status[4].value_char[3] |= ((GPIO_ReadInputDataBit((GPIO_TypeDef*)DINn_PORT[i], DINn_PIN[i]) & 0xFF) << i);
	}
	
//	printDebug_Semphr(INFO, "<MNG_GetSLStatus> Digital I/O: %02X %02X %02X %02X \r\n",struct_status[4].value_char[0],struct_status[4].value_char[1],
//																																struct_status[4].value_char[2],struct_status[4].value_char[3]);

	/* Alarm status 1 */
	struct_status[5].statusType 		= ALAM1;
	struct_status[5].valueType 			= VBYTE;
	struct_status[5].value_char[0] 	= _DataStatus_420mA[3];
	struct_status[5].value_char[1] 	= _DataStatus_420mA[2];
	struct_status[5].value_char[2] 	= _DataStatus_420mA[1];
	struct_status[5].value_char[3] 	= _DataStatus_420mA[0];
	/* Alarm status 2 */
	struct_status[6].statusType 		= ALAM2;
	struct_status[6].valueType 			= VBYTE;
	struct_status[6].value_char[0] 	= _DataStatus_ADC[1];
	struct_status[6].value_char[1] 	= _DataStatus_ADC[0];
	struct_status[6].value_char[2] 	= _DataStatus_420mA[5];
	struct_status[6].value_char[3] 	= _DataStatus_420mA[4];
	/* Alarm status 3 */
	struct_status[7].statusType 		= ALAM3;
	struct_status[7].valueType 			= VBYTE;
	struct_status[7].value_char[0] 	= 0x00;
	struct_status[7].value_char[1] 	= 0x00;
	struct_status[7].value_char[2] 	= _DataStatus_PULSEFlow;
	struct_status[7].value_char[3] 	= _DataStatus_MBFlow;
	
	return (uint8_t)MAX_STATUS;
}
/*==========================================================================================*/
/**
  * @brief  Read temperature on board
  * @param  
  * @retval Temperature(c)
  */
float fDATBUF_ReadBoardTemp(void) {
	
	uint16_t 	i;
	uint16_t	adcBuf[18];
	uint16_t	adcMin = 0x0FFF; 
	uint16_t 	adcMax = 0; 
	uint32_t	adcSum = 0;
	float 		volt, tempBoard;
	float 		vref = struct_DevConfig.adc_Vref;
	
	for(i = 0; i < 18; i++) {
		adcBuf[i] = (ADCValue[0]/*>>4*/);
		if(adcBuf[i] < adcMin) {											/* Search the array for MIN value */
			adcMin = adcBuf[i];
		}
		if(adcBuf[i] > adcMax) {											/* Search the array for MAX value */
			adcMax = adcBuf[i];
		}
//		printf("%d  ", adcBuf[i]);
		adcSum += adcBuf[i];
		delay_ms(5);
	}

	adcSum -= adcMin;																/* Remove the MIN value from sum */
	adcSum -= adcMax;																/* Remove the MAX value from sum */
	adcSum >>= 4;
	volt = (adcSum*vref)/4095;
//	printf("adc: %d   volt: %f \r\n", adcSum, volt);
	tempBoard = ((volt - 0.76)/0.0025)+25.0;
	
	return tempBoard;
}
/*==========================================================================================*/






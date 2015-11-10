/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_csvfile.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	29-April-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ff.h"
#include "stm32f4_SDCard.h"

#include "Sensors_Task.h"

#include "INT_STM32F4_csvfile.h"
#include "INT_STM32F4_rtc.h"
#include "SL_ConfigStruct.h"
#include "INT_STM32F4_debug.h"

static uint32_t uiCSVFILE_Index2Addr(uint32_t index);
static uint8_t 	ucCSVFILE_GetDateTimefromStrCSV(char	*str, RTC_DateTypeDef *date, RTC_TimeTypeDef *time);
static int8_t 	cCSVFILE_GetStatusIndex(uint8_t numStat, STATUS_SET *stat_set, uint8_t stat_type);
static int8_t 	cCSVFILE_GetDataIndex(uint8_t numDat, DATA_SET *dat_set, uint8_t sensor_type, uint8_t sensor_ch);
static uint8_t 	ucCSVFILE_GetLoggerIDfromStrCSV(char	*str, char *DataLoggerID);
static float 		vCSVFILE_GetDatafromStrCSV(char	*str, uint8_t	data_number);

extern STCONFIG_PROPERTIES		struct_ConfigProperties;
char data_unit[15][5] = {"NULL","m3/hr","m3","bar","pH","mg/l","FNU","ppm","g/l","C","m","%","mA","volt","adc"};

extern uint8_t 					ucGSM_RSSI;
extern __IO float				_BatteryRemain;
extern SENSOR_DATA 			_Sensor_Data;

/*==========================================================================================*/
/**
  * @brief  Get data from csv file
  * @param 	
  * @retval File function return code
  */
LOGRESULT xCSVFILE_GetData(FIL *file, uint32_t getIndex, uint32_t *timestamp, uint8_t *numStat, STATUS_SET *stat_set, uint8_t *numDat, DATA_SET *dat_set) {
	
	char			strData[CSV_RECORD_SIZE] = "29/10/2013 02:03:45,5542023-MM-01,1.732,62.397,205396,521.609,2587317,0,0,13,14.1";
	char			*str_tmp;
	uint8_t		num_data 		= 0;
	uint8_t		num_status 	= 0;
	uint32_t	lastIndex 	= 0;
	uint32_t 	startAddr 	= 0;
	
	FRESULT		res;
	LOGRESULT	log_res;
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
 	
	char DataLoggerID[20];
	float data_sensor[9];
 	uint8_t i = 0;
	
	/*-- Get last index --*/
	res = xCSVFILE_GetLastIndex(file, &lastIndex);
	if(res != FR_OK) {
		return (LOGRESULT)res;
	}

	if(lastIndex == 0) {
		return LOG_EMPTY;
	}
	if(getIndex >= lastIndex) {
		return LOG_INVALID_INDEX;
	}
	
	/*-- Get address of getIndex --*/
	startAddr = uiCSVFILE_Index2Addr(getIndex);
	printDebug_Semphr(INFO, "%80s<CSVFILE_GetData>: index: %d  Addr: 0x%08X\r\n", "", getIndex, startAddr);
	
	/*-- Read CSV Data from csv file --*/
	res = xSDCARD_ReadData(file, startAddr, strData, CSV_RECORD_SIZE);
	if(res != FR_OK) {
		return (LOGRESULT)res;
	}
	
	printDebug_Semphr(DESCRIBE, "<CSVFILE_GetData>: %s \r\n", strData);
	
	str_tmp = strtok(strData, ",");
	if(ucCSVFILE_GetDateTimefromStrCSV(str_tmp, &date, &time)) {
		return LOG_ERR;
	}
	
	str_tmp = strtok(NULL, ",");
	if(ucCSVFILE_GetLoggerIDfromStrCSV(str_tmp, DataLoggerID)) {
		return LOG_ERR;
	}
	
	while( str_tmp != NULL )
	{
// 		printDebug_Semphr(DESCRIBE," %s\n", str_tmp );
		str_tmp = strtok(NULL, ",");
		
		data_sensor[i] = vCSVFILE_GetDatafromStrCSV(str_tmp, i);
		switch(i){
			case 0 :
				_Sensor_Data.data420mA[0] = data_sensor[i];
				break;
			case 1 :
				_Sensor_Data.dataPULSEFlow = data_sensor[i];
				break;
			case 2 :
				_Sensor_Data.dataPULSENet = data_sensor[i];
				break;
			case 3 :
				_Sensor_Data.dataMODBUSFlow = data_sensor[i];
				break;
			case 4 :
				_Sensor_Data.dataMODBUSNegNet = data_sensor[i];
				break;
			case 5 :
				_Sensor_Data.data420mA[1]	= data_sensor[i];
				break;
			case 6 :
				_Sensor_Data.data420mA[2] = data_sensor[i];
				break;
			case 7 :
				ucGSM_RSSI = data_sensor[i];
				break;
			case 8 :
				_BatteryRemain = data_sensor[i];
				break;
			default :
				break;
		}
		i++;
  }
	
	return LOG_FR_OK;
	
}
/*==========================================================================================*/
/**
  * @brief  Get Data From String CSV.
  * @param 	
  * @retval None.
  */
static float vCSVFILE_GetDatafromStrCSV(char	*str, uint8_t	data_number) {
	
	uint16_t	i = 0;
	float data	= 0.0;
	
// 	switch(data_number){
// 		case 0: 
			data = atof(str);
// 			break;
// 	}
	printDebug_Semphr(DESCRIBE, "<vCSVFILE_GetDatafromStrCSV>: %f \r\n", data);
	return data;
}
/*==========================================================================================*/
/**
  * @brief  Get Date & Time Struct From String CSV.
  * @param 	
  * @retval None.
  */
static uint8_t ucCSVFILE_GetDateTimefromStrCSV(char	*str, RTC_DateTypeDef *date, RTC_TimeTypeDef *time) {
	
	uint16_t	tmp_int = 0;
	char			tmp_str[5];
	
	// Str: 28/04/2015 11:01:00,
	tmp_str[0] 				= str[0];
	tmp_str[1] 				= str[1];
	date->RTC_Date 		= atoi(tmp_str);
	if(str[2] != '/') {
		printDebug_Semphr(MINOR_ERR, "<CSVFILE_GetDateTimefromStrCSV>: Date String Invalid. \r\n");
		return 1;
	}
	tmp_str[0] 				= str[3];
	tmp_str[1] 				= str[4];
	date->RTC_Month 	= atoi(tmp_str);
	if(str[5] != '/') {
		printDebug_Semphr(MINOR_ERR, "<CSVFILE_GetDateTimefromStrCSV>: Date String Invalid. \r\n");
		return 1;
	}
	tmp_str[0] 				= str[6];
	tmp_str[1] 				= str[7];
	tmp_str[2] 				= str[8];
	tmp_str[3] 				= str[9];
	tmp_int 					= atoi(tmp_str);
	date->RTC_Year 		= tmp_int - 2000;
	
	// Clear Str
	tmp_str[0] 				= 0x00;
	tmp_str[1] 				= 0x00;
	tmp_str[2] 				= 0x00;
	tmp_str[3] 				= 0x00;
	tmp_str[4] 				= 0x00;
	
	tmp_str[0] 				= str[11];
	tmp_str[1] 				= str[12];
	time->RTC_Hours 	= atoi(tmp_str);
	if(str[13] != ':') {
		printDebug_Semphr(MINOR_ERR, "<CSVFILE_GetDateTimefromStrCSV>: Time String Invalid. \r\n");
		return 1;
	}
	tmp_str[0] 				= str[14];
	tmp_str[1] 				= str[15];
	time->RTC_Minutes = atoi(tmp_str);
	if(str[16] != ':') {
		printDebug_Semphr(MINOR_ERR, "<CSVFILE_GetDateTimefromStrCSV>: Time String Invalid. \r\n");
		return 1;
	}
	tmp_str[0] 				= str[17];
	tmp_str[1] 				= str[18];
	time->RTC_Seconds = atoi(tmp_str);
	
	printDebug_Semphr(DESCRIBE, "<CSVFILE_GetDateTimefromStrCSV>: %02d/%02d/%04d %02d:%02d:%02d \r\n", 
												date->RTC_Date,date->RTC_Month,date->RTC_Year+2000,time->RTC_Hours,time->RTC_Minutes,time->RTC_Seconds);
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  Get LoggerID From String CSV.
  * @param 	
  * @retval None.
  */
static uint8_t ucCSVFILE_GetLoggerIDfromStrCSV(char	*str, char *strID) {
	
	uint16_t	tmp_int = 0;
	char			tmp_str[20];
	
	tmp_int = strlen(str);
	memcpy(strID, str, tmp_int);
	strID[tmp_int] = '\0';
	
	printDebug_Semphr(DESCRIBE, "<ucCSVFILE_GetLoggerIDfromStrCSV>: %s \r\n", strID);
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  Insert data in csv file
  * @param 	
  * @retval File function return code
  */
FRESULT xCSVFILE_InsertData(FIL *file, uint32_t timestamp, uint8_t numStat, STATUS_SET *stat_set, uint8_t numDat, DATA_SET *dat_set) {
	
	FRESULT			res = FR_OK;
	char				strData[CSV_RECORD_SIZE];
	uint32_t 		index 		= 0;
	uint32_t 		indexCSV 	= 0;
	uint32_t		addr			= 0;
	
	index = ucCSVFILE_AssemCSV(strData, timestamp, numStat, stat_set, numDat, dat_set);
	
//	printf("CSV: %s \r\n", strData);
//	printf("Complete %d bytes.\r\n", index);
	
	/*-- Get last index --*/
	res = xCSVFILE_GetLastIndex(file, &indexCSV);
	if(res != FR_OK) {
		return res;
	}
	
	/*-- Get address of last index --*/
	addr = uiCSVFILE_Index2Addr(indexCSV);
	printDebug_Semphr(INFO, "%80s<CSVFILE_InsertData>: index: %d  Addr: 0x%08X\r\n", "", indexCSV, addr);
	
	/*-- Write data to csv file --*/
	res = xSDCARD_WriteData(file, addr, strData, CSV_RECORD_SIZE);
	if(res != FR_OK) {
		return res;
	}
	
	/*-- Set last index --*/
	res = xCSVFILE_SetLastIndex(file, ++indexCSV);
	if(res != FR_OK) {
		return res;
	}
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Get index of last record
  * @param 	
  * @retval File function return code
  */
FRESULT xCSVFILE_GetLastIndex(FIL *file, uint32_t *index) {
	
	return xSDCARD_ReadData(file, CSV_INDEX_ADDR, index, sizeof(uint32_t));
}
/*==========================================================================================*/
/**
  * @brief  Set index of last record
  * @param 	
  * @retval File function return code
  */
FRESULT xCSVFILE_SetLastIndex(FIL *file, uint32_t index) {
	return xSDCARD_WriteData(file, CSV_INDEX_ADDR, &index, sizeof(uint32_t));
}
/*==========================================================================================*/
/**
  * @brief  Convert index to address in CSV File.
  * @param 	Index.
  * @retval File function return code.
  */
static uint32_t uiCSVFILE_Index2Addr(uint32_t index) {
	return (uint32_t)((index * CSV_RECORD_SIZE) + CSV_INDEX_SIZE);
}
/*==========================================================================================*/
/**
  * @brief  	Assembly data csv
  * @param 	
  * @retval 	Length of string data (bytes)
	* @example 	15/01/2000 09:57:00,SmartLoggerTest02,0.000,0.00,0.00,0.00,0.00,0.000,0.040,0,9.34,33.00,13.966,0.030,0.020,0,0,9.278,0.006,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1
  */
uint8_t ucCSVFILE_AssemCSV(char *strData, uint32_t timestamp, uint8_t numStat, STATUS_SET *stat_set, uint8_t numDat, DATA_SET *dat_set) {
	
	RTC_DateTypeDef 	date;
	RTC_TimeTypeDef 	time;
	
	int8_t	 		i						= 0;
	int8_t	 		j						= 0;
	int16_t	 		k						= 0;
	uint8_t			index				= 0;
	uint8_t			tmp					= 0;
	bool				flag_tmp		= false;
	
	/*-- Put Date&time to string --*/
	vRTC_EpochToDateTime(&date, &time, timestamp+(struct_ConfigProperties.timezone*60*60));
	index += sprintf(strData, "%02d/%02d/%04d %02d:%02d:%02d,", date.RTC_Date, 							// 18 char
																															date.RTC_Month, 
																															date.RTC_Year+2000,
																															time.RTC_Hours,
																															time.RTC_Minutes,
																															time.RTC_Seconds);
	/*-- Put Device Name to string --*/
	index += sprintf(&strData[index], "%s,", struct_ConfigProperties.deviceName);						// Max 40 char
	
	/*-- Put Pressure1 to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_PRESS_FRONT, 0xFF);
	if(i >= 0) {
		index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);												// Max 8 char "123.567,"
	}else {
		index += sprintf(&strData[index], "0,");
	}

	/*-- Put FlowRate_analog to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_FLOW, PUL_CH1_FLOWRATE);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.2f,", dat_set[i].value);											// Max 8 char "1234.67,"
	}else {
		index += sprintf(&strData[index], "0,"); 
	}
	
	/*-- Put FlowAcc_analog to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_FLOW, PUL_CH1_NETFLOW);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.2f,", dat_set[i].value);											// Max 16 char
	}else {
		index += sprintf(&strData[index], "0,"); 
	}
	
	/*-- Put FlowRate_RS485 to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_FLOW, MB_CH1_FLOWRATE);
	if(i >= 0) {
		index += sprintf(&strData[index], "%0.2f,", dat_set[i].value);												// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,"); 
	}

	/*-- Put FlowAcc_RS485 to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_FLOW, MB_CH1_POS);
	if(i >= 0) {
		index += sprintf(&strData[index], "%0.2f,", dat_set[i].value);												// Max 16 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Pressure2 to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_PRESS_BEHIND, 0xFF);											// Max 8 char
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);
	}else { 
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Pressure3 to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_PRESS_END, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,"); 
	}
	
	/*-- Put SignalGSM to string --*/	
	i = cCSVFILE_GetStatusIndex(numStat, stat_set, GSM);
	if(i >= 0) {
		index += sprintf(&strData[index], "%d,", stat_set[i].value_int);    									// Max 4 char
	}else {
		index += sprintf(&strData[index], "0,"); 
	}
	
	/*-- Put BattaryVoltage to string --*/
	i = cCSVFILE_GetStatusIndex(numStat, stat_set, BAT);
	if(i >= 0) {
		index += sprintf(&strData[index], "%0.2f,", stat_set[i].value_float);									// Max 6 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put TempOnBoard to string --*/
	i = cCSVFILE_GetStatusIndex(numStat, stat_set, BTEMP);
	if(i >= 0) {
		index += sprintf(&strData[index], "%0.2f,", stat_set[i].value_float);									// Max 6 char
	}else {
		index += sprintf(&strData[index], "0,"); 
	}
	
	/*-- Put WaterLevel to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_ULTRASONIC, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put pH to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_PH, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Chlorine to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_TFC, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Turbidity to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_TURBID, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Temperture to string --*/
	i = cCSVFILE_GetDataIndex(numDat, dat_set, TYP_TEMP, 0xFF);
	if(i >= 0) {
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
	}else {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put ADC CH1 to string --*/
	flag_tmp = true;
	for(i = 0; i < numDat; i++) {
		if(dat_set[i].channel == 0x31) {
			flag_tmp = false;
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
			break;
		}
	}
	if(flag_tmp) {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put ADC CH2 to string --*/
	flag_tmp = true;
	for(i = 0; i < numDat; i++) {
		if(dat_set[i].channel == 0x32) {
			flag_tmp = false;
			index += sprintf(&strData[index], "%0.3f,", dat_set[i].value);											// Max 8 char
			break;
		}
	}
	if(flag_tmp) {
		index += sprintf(&strData[index], "0,");
	}
	
	/*-- Put Pulse 1,2,3,4 to string (No data) --*/
	index += sprintf(&strData[index], "0,0,0,0,");																					// 8 char
	
	/*-- Put DigitalInput to string --*/	
	i = cCSVFILE_GetStatusIndex(numStat, stat_set, DIGI);
	if(i >= 0) {
		/* Digital input CH1-8 */
		for(j = 0; j < 8; j++) {
			tmp = (stat_set[i].value_char[3] >> j) & 0x01;
			index += sprintf(&strData[index], "%d,", tmp);																			// 16 char
		}
		/* Digital input CH9-12 */
		for(j = 0; j < 4; j++) {
			tmp = (stat_set[i].value_char[2] >> j) & 0x01;
			if(j == 3) {
				index += sprintf(&strData[index], "%d", tmp);																			// 7 char
			}else {
				index += sprintf(&strData[index], "%d,", tmp);
			}
		}
	}else {
		index += sprintf(&strData[index], "0,0,0,0,0,0,0,0,0,0,0,0");
	}

	for(k = index; k < (CSV_RECORD_SIZE-2); k++) {
		strData[k] = 0x20;
	}
	
	strData[CSV_RECORD_SIZE-2] = '\r';
	strData[CSV_RECORD_SIZE-1] = '\n';
	
	printDebug_Semphr(WARNING, "<CSVFILE_AssemCSV>: STR: %s \r\n", strData);
	
	return index;
}
/*==========================================================================================*/
/**
  * @brief  Find status_set
  * @param 	
  * @retval Index of status_set
  */
static int8_t cCSVFILE_GetStatusIndex(uint8_t numStat, STATUS_SET *stat_set, uint8_t stat_type) {
	
	int8_t i;
	
	for(i = 0; i < numStat; i++) {
		if(stat_set[i].statusType == stat_type) {
			return i;
		}
	}
	return -1;
}
/*==========================================================================================*/
/**
  * @brief  Find data_set
  * @param 	
  * @retval Index of data_set
  */
static int8_t cCSVFILE_GetDataIndex(uint8_t numDat, DATA_SET *dat_set, uint8_t sensor_type, uint8_t sensor_ch) {
	
	int8_t i;
	
	for(i = 0; i < numDat; i++) {
		if(dat_set[i].sensorType == sensor_type) {
			if(sensor_ch != 0xFF) {
				if(dat_set[i].channel == sensor_ch) {
					return i;
				}
			}else {
				return i; 
			}
		}
	}
	return -1;
}
/*==========================================================================================*/




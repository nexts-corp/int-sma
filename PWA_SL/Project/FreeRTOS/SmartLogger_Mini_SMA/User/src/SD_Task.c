/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	SDnMB_Task.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	9-September-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Sensors_Task.h"
#include "stm32f4_SDCard.h"
#include "main.h"
#include "SD_Task.h"
#include "SL_PinDefine.h"

#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_ConfigSD.h"
#include "INT_STM32F4_ProtocolHandle.h"
#include "INT_STM32F4_24LC512.h"
#include "INT_STM32F4_csvfile.h"


#define LED_SD_TIME		3

static LOGRESULT xSD_CheckRegis(uint8_t check_regis);
static LOGRESULT xSD_SaveNetFlow(float net_flow);
static void xSD_HandleSaveConfig(ST_CONFIG_Q *bufConf);
static LOGRESULT xSD_HandleStatusQueue(ST_UPDATE_STAT up_stat);
static LOGRESULT xSD_HandleDebugLogQueue(DEBUG_MESSAGE message);
static void vSD_HandleDataQueue(ST_LOG_DATA data_stat);
static void vSD_HandleDataLogQueue(ST_LOG_DATA log_data);
static void vSD_HandleEventLogQueue(ST_LOG_EVENT log_event);
static void vSD_HandleConfigLogQueue(ST_LOG_CONFIG 	log_config);
static bool bSD_HandleCopyFromSD(uint8_t cmd);
void vSDNMB_GetFileName(uint32_t timestamp, char *name);

uint8_t	STAT_DOUTn[] = {STAT_DOUT1, STAT_DOUT2, STAT_DOUT3, STAT_DOUT4};
extern __IO uint8_t	_MemoryStatus;

extern __IO bool			flag_HaveMemEvent;
extern __IO bool 			flag_HaveDataLog;
extern __IO bool 			flag_HaveConfigLog;
extern __IO bool 			flag_HaveEventLog;
extern __IO bool			flag_isSDInstall;
extern __IO uint8_t		_LED_SD_ON;
extern __IO uint8_t		_LED_ERR_ON;
extern __IO uint8_t		_ConfigCmdACK;
extern __IO uint32_t	_Transmit_Period;

extern void vMAIN_PrintConfig (uint8_t printLevel, uint8_t config_id, void *st_config, uint8_t ch);

void vSDnMB_Task(void *pvParameters) {
	
	bool						wdg 							= true;
	uint8_t					check_regis				= 0;
	uint8_t					copy_cmd					= 0;
	uint32_t				checkSDTimer 			= 0;
	uint32_t				wdgTimer 					= 0;
	uint32_t				mem_remain				= 0;
	
	ST_CONFIG_Q			stBuf_Conf;
	ST_LOG_DATA 		data_stat;
	ST_LOG_DATA 		log_data;
	ST_LOG_EVENT		log_event;
	ST_LOG_CONFIG 	log_config;
	DEBUG_MESSAGE		debug_message;
//	ST_UPDATE_STAT	update_stat;
	LOGRESULT				log_res;
	FRESULT					fatfs_res;

	printDebug_Semphr(DESCRIBE, "[SD_TASK]: Running... \r\n");
	
	/* Set Timer */
	ucTIMER_SetTimer(&checkSDTimer, 60);
	ucTIMER_SetTimer(&wdgTimer, 10);
	
	while(1) {
		
		/* Send signal to MNG_Task every 1 sec -----------------------------*/
		if(cTIMER_CheckTimerExceed(wdgTimer)) {
			if(!xQueueSend(Queue_SDTaskWDG, &wdg, 0)) {
				printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Send Queue_SDTaskWDG failed. \r\n");
			}
			ucTIMER_SetTimer(&wdgTimer, 1);
		}
		
		/* Check Memory(SD Card) ---------------------------------------------*/
		if(cTIMER_CheckTimerExceed(checkSDTimer)) {
			taskDISABLE_INTERRUPTS();		/* Disable Interupt */
			fatfs_res = xSDCARD_ReadMemoryRemain(&mem_remain);
			taskENABLE_INTERRUPTS();		/* Enable Interupt */
			if(fatfs_res == FR_OK) {
				flag_isSDInstall = true;
//				printDebug_Semphr(INFO, "[SD_TASK]: Memory Remaining %d kBytes \r\n", mem_remain);
				if(mem_remain < 1024000) {    // < 1 MBytes.  
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_LOW;
					printDebug_Semphr(WARNING, "[SD_TASK]: Low Memory(SD Card) (%d kBytes) \r\n", mem_remain);
				}else {
					_MemoryStatus = EV_NORMAL;
				}
			}else if(fatfs_res == FR_NOT_READY) {
				LED_SD_RED;
				flag_isSDInstall = false;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_NOTINSTALL;
			}else {
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_ERR;
				_LED_SD_ON = LED_SD_TIME;
			}
			ucTIMER_SetTimer(&checkSDTimer, 300);
		}
		
		/* Data ------------------------------------------------------------------*/
		if(xQueueReceive(Queue_DataToSD, &data_stat, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Data. \r\n");
// 			vSD_HandleDataQueue(data_stat);
		}
		
		/* Data Log --------------------------------------------------------------*/
		if(xQueueReceive(Queue_DataLogToSD, &log_data, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Data Log. \r\n");
			vSD_HandleDataLogQueue(log_data);
		}
		
		/* Event Log -------------------------------------------------------------*/
		if(xQueueReceive(Queue_EventLogToSD, &log_event, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Event Log. \r\n");
			vSD_HandleEventLogQueue(log_event);
		}
		
		/* Send signal to MNG_Task every 1 sec -----------------------------*/
		if(cTIMER_CheckTimerExceed(wdgTimer)) {
			if(!xQueueSend(Queue_SDTaskWDG, &wdg, 0)) {
				printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Send Queue_SDTaskWDG failed. \r\n");
			}
			ucTIMER_SetTimer(&wdgTimer, 1);
		}
		
		/* Configuration Log -----------------------------------------------------*/
		if(xQueueReceive(Queue_ConfigLogToSD, &log_config, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Configuration Log. \r\n");
			vSD_HandleConfigLogQueue(log_config);
		}
		
		/* Debug Log -------------------------------------------------------------*/
		if(xQueueReceive(Queue_DebugToSD, &debug_message, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Debug Log. \r\n");
			xSD_HandleDebugLogQueue(debug_message);
		}
		
		/* Load Content From SD to EEPROM ----------------------------------------*/
		if(xQueueReceive(Queue_CopyFromSD, &copy_cmd, 0)) {
			printDebug_Semphr(MINOR_ERR, "[SD_TASK]: Load Content From SD to EEPROM. \r\n");
			if(bSD_HandleCopyFromSD(copy_cmd)) {
				_ConfigCmdACK = CONF_ACK_OK;
			}else {
				_ConfigCmdACK = CONF_ACK_ERR;
			}
		}
		
	}/* Mian Loop */
}/* vSDnMB_Task */
/*==========================================================================================*/



/*==========================================================================================*/
/**
  * @brief 	This function is copy content from SDCard to EEPROM.
  * @param  Command.
  * @retval Is Success.
  */
static bool bSD_HandleCopyFromSD(uint8_t cmd) {
	
	uint8_t				i;
	uint8_t				sent_wdg = 0;
	unsigned char	check_text[8];
	bool					isFailed			= false;
	FRESULT 			fatfs_res;
	FIL 					info_file;
	FIL 					config_file;
	ST_INFO 			tmp_info;
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
	
	taskDISABLE_INTERRUPTS();		/* Disable Interupt */
	switch(cmd) {
		case LOAD_INFO:
		
			fatfs_res = xSDCARD_OpenFile(&info_file, (const char*)INFO_FILE);
			if(fatfs_res == FR_OK) {
				printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Reading Info file.... \r\n");
				fatfs_res = xSDCARD_ReadData(&info_file, SD_USER_BASEADDR, tmp_info.user, sizeof(tmp_info.user));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read User_Name(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: User_Name: %s \r\n", tmp_info.user);
				}
				fatfs_res = xSDCARD_ReadData(&info_file, SD_PASSWORD_BASEADDR, tmp_info.password, sizeof(tmp_info.password));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Password(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Password: %s \r\n", tmp_info.password);
				}
				fatfs_res = xSDCARD_ReadData(&info_file, SD_SERIAL_BASEADDR, tmp_info.serialNumber, ADDR_LEN);
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read My_Serial_Number(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: My_Serial: ");
					for(i = 0; i < ADDR_LEN; i++) {
						printDebug_Semphr(DESCRIBE, "%02X ", tmp_info.serialNumber[i]);
					}
					printDebug_Semphr(DESCRIBE, "\r\n");
				}
				fatfs_res = xSDCARD_ReadData(&info_file, SD_DESTSERIAL_BASEADDR, tmp_info.destSerial, ADDR_LEN);
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Destination_Serial_Number(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Dest_Serial: ");
					for(i = 0; i < ADDR_LEN; i++) {
						printDebug_Semphr(DESCRIBE, "%02X ", tmp_info.destSerial[i]);
					}
					printDebug_Semphr(DESCRIBE, "\r\n");
				}
				fatfs_res = xSDCARD_ReadData(&info_file, SD_FLAGREGIS_BASEADDR, &tmp_info.flagRegistered, sizeof(uint8_t));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Flag_Registered(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Registered: %d \r\n", tmp_info.flagRegistered);
				}
				
				memcpy(struct_info.serialNumber, tmp_info.serialNumber, sizeof(tmp_info.serialNumber));
				memcpy(check_text, tmp_info.serialNumber, sizeof(tmp_info.serialNumber));					// copy serial number to SD Card for text check 
				
				if(!isFailed) {
					for(i=0;i<sizeof(check_text);i++) {
						printDebug(DESCRIBE, "%02X ", check_text[i]);
					}
					printDebug(DESCRIBE, "\r\n");
					
					fatfs_res = xSDCARD_WriteData(&info_file, CHKTEXT_BASEADDR, &check_text, sizeof(check_text));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't write check_text(FAT_%d) \r\n", fatfs_res);
					}else {
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Write check_text. \r\n");
					}
				}
				
				/* Close Information File -------------------------------------------------------*/
				fatfs_res = xSDCARD_CloseFile(&info_file);
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Close Info File error(FAT_%d) \r\n", fatfs_res);
				}else {
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Close Info File. \r\n");
				}
			}else if(fatfs_res == FR_NO_FILE) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: No Infomation File in SDCard. \r\n");
				isFailed |= true;
			}else {
				_LED_SD_ON = LED_SD_TIME;
				printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Open Information File error(FAT_%d) \r\n", fatfs_res);
				isFailed |= true;
			}
			
			if(!isFailed) {
				if(EEPROM_WriteBuf(INFO_BASEADDR, &tmp_info, sizeof(ST_INFO))) {
					_LED_ERR_ON = LED_ERR_TIME;
					printDebug_Semphr(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Information Error \r\n");
					return false;
				}else {
					memcpy(&struct_info, &tmp_info, sizeof(ST_INFO));
					/* Beep Buzzer */
					BUZZER_ON;
					delay_ms(100);
					BUZZER_OFF;
					delay_ms(100);
					BUZZER_ON;
					delay_ms(200);
					BUZZER_OFF;
					return true;
				}
			}else {
				return false;
			}
			break;
		case LOAD_CONF:
			
			memcpy(struct_info.serialNumber, tmp_info.serialNumber, sizeof(tmp_info.serialNumber));
			memcpy(check_text, tmp_info.serialNumber, sizeof(tmp_info.serialNumber));					// copy serial number to SD Card for text check
		
			/* Configuration File ------------------------------------------------------*/
			fatfs_res = xSDCARD_OpenFile(&config_file, (const char*)CONFIG_FILE);
			if(fatfs_res == FR_OK) {
				printDebug_Semphr(INFO, "<SD_HandleCopyFromSD>: Checking file.... \r\n");
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, DEVeCONF_BASEADDR, &tmp_DevConfig, sizeof(STCONFIG_DEV));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Developer_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, DEV_CONF, &tmp_DevConfig, CH_NONE);
					if(EEPROM_WriteBuf(DEVeCONF_BASEADDR, &tmp_DevConfig, sizeof(STCONFIG_DEV))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Deverloper_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_DevConfig, &tmp_DevConfig, sizeof(STCONFIG_DEV));
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, PROPERTIES_BASEADDR, &tmp_ConfigProperties, sizeof(STCONFIG_PROPERTIES));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Properties(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, PROPERTIES_CONF, &tmp_ConfigProperties, CH_NONE);
					if(EEPROM_WriteBuf(PROPERTIES_BASEADDR, &tmp_ConfigProperties, sizeof(STCONFIG_PROPERTIES))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Properties_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigProperties, &tmp_ConfigProperties, sizeof(STCONFIG_PROPERTIES));
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, OPERATING_BASEADDR, &tmp_ConfigOperating, sizeof(STCONFIG_OPERATING));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Operating_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, OPERATING_CONF, &tmp_ConfigOperating, CH_NONE);
					if(EEPROM_WriteBuf(OPERATING_BASEADDR, &tmp_ConfigOperating, sizeof(STCONFIG_OPERATING))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Operating_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigOperating, &tmp_ConfigOperating, sizeof(STCONFIG_OPERATING));
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, BUZZER_BASEADDR, &tmp_ConfigBuzzer, sizeof(STCONFIG_BUZZER));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Buzzer_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, BUZZER_CONF, &tmp_ConfigBuzzer, CH_NONE);
					if(EEPROM_WriteBuf(BUZZER_BASEADDR, &tmp_ConfigBuzzer, sizeof(STCONFIG_BUZZER))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Buzzer_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigBuzzer, &tmp_ConfigBuzzer, sizeof(STCONFIG_BUZZER));
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, WORKING_BASEADDR, &tmp_ConfigWorking, sizeof(STCONFIG_WORKING));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read WorkingManagement_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, WORKING_CONF, &tmp_ConfigWorking, CH_NONE);
					if(EEPROM_WriteBuf(WORKING_BASEADDR, &tmp_ConfigWorking, sizeof(STCONFIG_WORKING))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Working_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigWorking, &tmp_ConfigWorking, sizeof(STCONFIG_WORKING));
						_Transmit_Period = 0;				// Reset transmission data time
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, NETWORK_BASEADDR, &tmp_ConfigNetwork, sizeof(STCONFIG_NETWORK));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Network_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, NETWORK_CONF, &tmp_ConfigNetwork, CH_NONE);
					if(EEPROM_WriteBuf(NETWORK_BASEADDR, &tmp_ConfigNetwork, sizeof(STCONFIG_NETWORK))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Network_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigNetwork, &tmp_ConfigNetwork, sizeof(STCONFIG_NETWORK));
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				for(i = 0; i < MAX_420SENSOR; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, SEN420CONF_BASEADDR[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read 4-20mA Sensor_Configuration Ch_%d (FAT_%d) \r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, SEN420_CONF, &tmp_ConfigAnalogSensor, i);
						if(EEPROM_WriteBuf(SEN420CONF_BASEADDR[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write 4-20mASensor_Configuration CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_Config420Sensor[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
						}
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				for(i = 0; i < MAX_ADCSENSOR; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENADCCONF_BASEADDR[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read ADC Sensor_Configuration Ch_%d (FAT_%d) \r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, SENADC_CONF, &tmp_ConfigAnalogSensor, i);
						if(EEPROM_WriteBuf(SENADCCONF_BASEADDR[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write ADCSensor_Configuration CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_ConfigADCSensor[i], &tmp_ConfigAnalogSensor, sizeof(STCONFIG_ANALOGSENSOR));
						}
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENsMODBUSCONF_BASEADDR, &tmp_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read UART Sensor_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, SENMODBUS_CONF, &tmp_ConfigMODBUSSensor, CH_NONE);
					if(EEPROM_WriteBuf(SENsMODBUSCONF_BASEADDR, &tmp_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write MODBUSFlowSensor_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigMODBUSSensor, &tmp_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR));
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENsPULCONF_BASEADDR, &tmp_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Pulse Sensor_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, SENPULSE_CONF, &tmp_ConfigPulseSensor, CH_NONE);
					if(EEPROM_WriteBuf(SENsPULCONF_BASEADDR, &tmp_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write PULSEFlowSensor_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigPulseSensor, &tmp_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR));
					}
				}
				for(i = 0; i < MAX_DIGItIN; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, DIGItIN_BASEADDR[i], &tmp_ConfigDIN, sizeof(STCONFIG_DIGITALsIN));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read DIN_Configuration CH_%d (FAT_%d) \r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, DIN_CONF, &tmp_ConfigDIN, i);
						if(EEPROM_WriteBuf(DIGItIN_BASEADDR[i], &tmp_ConfigDIN, sizeof(STCONFIG_DIGITALsIN))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write DIN_Configuration CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_ConfigDIN[i], &tmp_ConfigDIN, sizeof(STCONFIG_DIGITALsIN));
						}
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				for(i = 0; i < MAX_420SENSOR; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, SEN420ALARMn_BASEADDR[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Sensor Alarm_Configuration CH_%d (FAT_%d) \r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, SEN420ALARM_CONF, &tmp_ConfigSensorAlarm, i);
						if(EEPROM_WriteBuf(SEN420ALARMn_BASEADDR[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write 4-20mASensorAlarm_Configuration CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_Config420SensorAlarm[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
						}
					}
				}
				for(i = 0; i < MAX_ADCSENSOR; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENADCALARMn_BASEADDR[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read ADC SensorAlarm_Configuration CH_%d (FAT_%d) \r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, SENADCALARM_CONF, &tmp_ConfigSensorAlarm, i);
						if(EEPROM_WriteBuf(SENADCALARMn_BASEADDR[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write ADCSensorAlarm_Configuration CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_ConfigADCSensorAlarm[i], &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
						}
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENsMODBUSALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Uart_Sensor Alarm_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, SENMODBUSALARM_CONF, &tmp_ConfigSensorAlarm, CH_NONE);
					if(EEPROM_WriteBuf(SENsMODBUSALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write MODBUSFlowSensorAlarm_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigMODBUSSensorAlarm, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENsPULALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Pulse_Sensor Alarm_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, SENPULSEALARM_CONF, &tmp_ConfigSensorAlarm, CH_NONE);
					if(EEPROM_WriteBuf(SENsPULALARM_BASEADDR, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write FlowFlowSensorAlarm_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigPulseSensorAlarm, &tmp_ConfigSensorAlarm, sizeof(STCONFIG_SENSORALARM));
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, VALVECONF_BASEADDR, &tmp_ConfigValve, sizeof(STCONFIG_VALVE));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read Valve_Configuration(FAT_%d) \r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, VALVE_CONF, &tmp_ConfigValve, CH_NONE);
					if(EEPROM_WriteBuf(VALVECONF_BASEADDR, &tmp_ConfigValve, sizeof(STCONFIG_VALVE))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Valve_Configuration Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigValve, &tmp_ConfigValve, sizeof(STCONFIG_VALVE));
					}
				}
				for(i = 0; i < MAX_420SENSOR; i++) {
					fatfs_res = xCONFIGSD_ReadConfig(&config_file, SEN420CHnCAL_BASEADDR[i], &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read 420mA_SensorCH_%d Calibrate(FAT_%d)\r\n", i+1, fatfs_res);
						isFailed |= true;
					}else {
						vMAIN_PrintConfig(DESCRIBE, SEN420CAL_CONF, &tmp_Config420Cal, i);
						if(EEPROM_WriteBuf(SEN420CHnCAL_BASEADDR[i], &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
							printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write 420mASensor_Calibrate CH_%d Error \r\n", i+1);
							_LED_ERR_ON = LED_ERR_TIME;
							isFailed |= true;
						}else {
							memcpy(&struct_Config420Cal[i], &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
						}
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, VALVE420INCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read 420mAIn_Valve Calibrate(FAT_%d)\r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, VALVE420INCAL_CONF, &tmp_Config420Cal, CH_NONE);
					if(EEPROM_WriteBuf(VALVE420INCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write In420mAValve_Calibrate Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_Config420ValveInCal, &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
					}
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, VALVE420OUTCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't read 420mAOut_Valve Calibrate(FAT_%d)\r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, VALVE420OUTCAL_CONF, &tmp_Config420Cal, CH_NONE);
					if(EEPROM_WriteBuf(VALVE420OUTCAL_BASEADDR, &tmp_Config420Cal, sizeof(STCONFIG_420CAL))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write Out420mAValve_Calibrate Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_Config420ValveOutCal, &tmp_Config420Cal, sizeof(STCONFIG_420CAL));
					}
				}
				/* Send WDG Signal to MNG_Task */
				if(!xQueueSend(Queue_SDTaskWDG, &sent_wdg, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Send Queue_SDTaskWDG failed. \r\n");
				}
				fatfs_res = xCONFIGSD_ReadConfig(&config_file, SENPULSEADJ_BASEADDR, &tmp_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ));
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(DESCRIBE, "<SD_HandleCopyFromSD>: Couldn't read PULSE FlowAdjustment(FAT_%d)\r\n", fatfs_res);
					isFailed |= true;
				}else {
					vMAIN_PrintConfig(DESCRIBE, SENPULSEADJ_CONF, &tmp_ConfigPulseADJ, CH_NONE);
					if(EEPROM_WriteBuf(SENPULSEADJ_BASEADDR, &tmp_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ))) {
						printDebug(MAJOR_ERR, "<SD_HandleCopyFromSD>: EEPROM_Write PULSE FlowAdjustment Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
						isFailed |= true;
					}else {
						memcpy(&struct_ConfigPulseADJ.multiplier_value, &tmp_ConfigPulseADJ.multiplier_value, sizeof(float));
					}
				}
				
				if(!isFailed) {
					printDebug(DESCRIBE, "[SDCARD]: text = ");
					for(i=0;i<sizeof(check_text);i++) {
						printDebug(DESCRIBE, "%02X ", check_text[i]);
					}
					printDebug(DESCRIBE, "\r\n");
			
					fatfs_res = xSDCARD_WriteData(&config_file, CHKTEXT_BASEADDR, &check_text, sizeof(check_text));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Couldn't write check_text(FAT_%d) \r\n", fatfs_res);
					}else {
						printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Write check_text. \r\n");
						/* Beep Buzzer */
						BUZZER_ON;
						delay_ms(100);
						BUZZER_OFF;
						delay_ms(100);
						BUZZER_ON;
						delay_ms(200);
						BUZZER_OFF;
					}
				}
				
				/* Close Configuration File ------------------------------------------------*/
				fatfs_res = xSDCARD_CloseFile(&config_file);
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug(MINOR_ERR, "<SD_HandleCopyFromSD>: Close Configuration File error(FAT_%d) \r\n", fatfs_res);
				}else {
					printDebug(INFO, "<SD_HandleCopyFromSD>: Close Configuration File. \r\n");
				}
			}else if(fatfs_res == FR_NO_FILE) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: No Configuration File in SDCard. \r\n");
				return false;
			}else {
				_LED_SD_ON = LED_SD_TIME;
				printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: Open Configuration File error(FAT_%d) \r\n", fatfs_res);
				return false;
			}
			break;
		default :
			printDebug_Semphr(MINOR_ERR, "<SD_HandleCopyFromSD>: No CMD [0x%02X] \r\n", cmd);
			break;
	}
	taskENABLE_INTERRUPTS();		/* Enable Interupt */
	
	return true;
}
/*==========================================================================================*/
/**
  * @brief  Handle debug_log queue.
  * @param  LOG_CONFIG Structure.
  * @retval None.
  */
static LOGRESULT xSD_HandleDebugLogQueue(DEBUG_MESSAGE message) {
	
	FIL				debugLog_file;
	FRESULT		fatfs_res;
	LOGRESULT	log_res;
	
	taskDISABLE_INTERRUPTS();		/* Disable Interupt */
	/*-- Open file --*/
	fatfs_res = xSDCARD_OpenFile(&debugLog_file, (const char*)DEBUG_LOG);
	if(fatfs_res == FR_OK) {
		log_res = xLOG_WriteDebugLog(&debugLog_file, message.timestamp, message.message);
		if(log_res != LOG_FR_OK) {
			/* Use INFO cause prevent recursive */
			_LED_SD_ON = LED_SD_TIME;
			printDebug_Semphr(INFO, "<SD_HandleDebugLogQueue>: Write debug_log failed(LOG_%d) \r\n", log_res);
			return log_res;
		}
		
		/*-- Close file --*/
		fatfs_res = xSDCARD_CloseFile(&debugLog_file);
		if(fatfs_res != FR_OK) {
			_LED_SD_ON = LED_SD_TIME;
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDebugLogQueue>: Close %s failed(FAT_%d) \r\n", DEBUG_LOG, fatfs_res);
			return (LOGRESULT)fatfs_res;
		}
	}else {
		/* Use INFO cause prevent recursive */
		_LED_SD_ON = LED_SD_TIME;
		printDebug_Semphr(INFO, "<SD_HandleDebugLogQueue>: Open %s failed(FAT_%d) \r\n", DEBUG_LOG, fatfs_res);
		return (LOGRESULT)fatfs_res;
	}
	taskENABLE_INTERRUPTS();		/* Enable Interupt */
	
	return log_res;
}
/*==========================================================================================*/
/**
  * @brief  Handle data queue.
  * @param  LOG_DATA Structure.
  * @retval None.
  */
static void vSD_HandleDataQueue(ST_LOG_DATA data_stat) {
	
	char			file_name[13];
	char			newLine[]	= "\r\n";
	bool 			isFileOpen 	= false;
	bool			wdg 				= true;
	uint32_t	i;
	uint32_t	last_index	=	0;
	uint32_t	first_index	=	0;
	uint32_t	num_record	=	0;
	
	FIL				csv_file;
	FRESULT		fatfs_res;
	LOGRESULT	log_res;
	PTC_RES		ptc_res;
	
	/*-- Get file name from timestamp --*/
	vSDNMB_GetFileName(data_stat.timestamp, file_name);
	printDebug_Semphr(DETAIL, "<SD_HandleDataQueue>: File_Name: %s (%d)\r\n", file_name, data_stat.timestamp);
	
// 	taskDISABLE_INTERRUPTS();		/* Disable Interupt */
	/*-- Open file --*/
	fatfs_res = xSDCARD_OpenFile(&csv_file, file_name);
	if(fatfs_res == FR_OK) {																																/********** File already exists */
		isFileOpen = true;
		/*-- Read Last_Index --*/
		fatfs_res = xSDCARD_ReadData(&csv_file, CSV_INDEX_ADDR, &last_index, sizeof(uint32_t));
		if(fatfs_res != FR_OK) {
			_LED_SD_ON = LED_SD_TIME;
			printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Read last_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
			delay_ms(SD_STABLE_DELAY);
			/* If couldn't read last index, Set last index as default */
			last_index = 0;
			fatfs_res = xSDCARD_WriteData(&csv_file, CSV_INDEX_ADDR, &last_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write last_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
			}
		}
		/*-- Handle Command --*/
		if(data_stat.cmd == LOG_CMD_WRITE) {																														/****** Command Write */
			/*-- Save data into file --*/
			fatfs_res = xCSVFILE_InsertData(&csv_file, data_stat.timestamp, data_stat.num_status, data_stat.status, data_stat.num_data, data_stat.data);
			if(fatfs_res != FR_OK) {
				_LED_SD_ON = LED_SD_TIME;
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Write %s failed(FAT_%d) \r\n", file_name, fatfs_res);
			}
		}else if(data_stat.cmd == LOG_CMD_READ) {																												 /****** Command Read */
			/*-- Get amount of backup data --*/
			log_res = xLOG_GetIndex(&csv_file, CSV_INDEX_ADDR, &num_record);
			if(log_res == LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Data amount: %d \r\n", num_record);
				for(i = 0; i < num_record; i++) {
					log_res = xCSVFILE_GetData(&csv_file, i, &data_stat.timestamp, &data_stat.num_status, &data_stat.status[0], &data_stat.num_data, &data_stat.data[0]);
					if(log_res == LOG_FR_OK) {						
//						ptc_res = xPROCOMM_SendDataNStatusLog(data_stat.timestamp, (uint16_t)num_record, (uint16_t)(i+1), data_stat.num_status, &data_stat.status[0], data_stat.num_data, &data_stat.data[0], 0);
//						if(ptc_res != PTC_OK) {
//							printDebug_Semphr(MINOR_ERR, "X<<-- DATA&STATUS_LOG Failed(PTC_%d) \r\n", ptc_res);
//						}
					}else {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Get data %s failed(LOG_%d) \r\n", file_name, log_res);
					}
					/*-- Reset SD WDG Counter --*/
					if(!xQueueSend(Queue_SDTaskWDG, &wdg, 0)) {
						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Send Queue_SDTaskWDG failed. \r\n");
					}
					delay_ms(10);
				}
				/*-- Send ACK Success to Debug Port --*/
				ptc_res = xPROCOMM_SendLogManipulateACK(data_stat.fid, LOG_ACCD_READDATA, STAT_SUCCESS, DEBUG_PORT);
				if(ptc_res != PTC_OK) {
					printDebug_Semphr(MINOR_ERR, "X<<-- LOG_MANIPULATION ACK Failed(PTC_%d) \r\n", ptc_res);
				}
			}else {
				/*xLOG_GetIndex error*/
				_LED_SD_ON = LED_SD_TIME;
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Get index %s failed(LOG_%d) \r\n", file_name, log_res);
			}				
		}else {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Undefine command 0x%02X \r\n", data_stat.cmd);
		}
	}else if(fatfs_res == FR_NO_FILE) {																										/********** File dose not exists */
		/*-- Handle Command --*/
		if(data_stat.cmd == LOG_CMD_WRITE) {																														/****** Command Write */
			printDebug(DESCRIBE, "<SD_HandleDataQueue>: Creating file %s ... \r\n", file_name);
			fatfs_res = xSDCARD_CreateFile(&csv_file, file_name, CREATE_ALWAYS);
			if(fatfs_res == FR_OK) {
				/*-- Close file --*/
				fatfs_res = xSDCARD_CloseFile(&csv_file);
				if(fatfs_res != FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Close %s failed(FAT_%d) \r\n", DATA_FILE, fatfs_res);
				}
				/*-- Open file again --*/
				fatfs_res = xSDCARD_OpenFile(&csv_file, file_name);
				if(fatfs_res == FR_OK) {
					isFileOpen = true;
					/* Set Write_Index as default (0x00000000) */
					last_index = 0;
					fatfs_res = xSDCARD_WriteData(&csv_file, CSV_INDEX_ADDR, &last_index, sizeof(uint32_t));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write Index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
					}
					/* Write '\r\n' for first line */
					fatfs_res = xSDCARD_WriteData(&csv_file, CSV_NEWLINE_ADDR, newLine, sizeof(newLine));
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write new_line %s failed(FAT_%d) \r\n", file_name, fatfs_res);
					}
					/*-- Save data_log into file --*/
					fatfs_res = xCSVFILE_InsertData(&csv_file, data_stat.timestamp, data_stat.num_status, data_stat.status, data_stat.num_data, data_stat.data);
					if(fatfs_res != FR_OK) {
						_LED_SD_ON = LED_SD_TIME;
						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Write %s failed(FAT_%d) \r\n", file_name, fatfs_res);
					}
				}else {
					_LED_SD_ON = LED_SD_TIME;
					printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Open %s file failed(FAT_%d) \r\n", file_name, fatfs_res);
				}
			}else {
				_LED_SD_ON = LED_SD_TIME;
				isFileOpen = false;
				printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Create %s file failed(FAT_%d) \r\n", file_name, fatfs_res);
			}
		}else if(data_stat.cmd == LOG_CMD_READ) {																												/****** Command Read */
			printDebug_Semphr(INFO, "<SD_HandleDataQueue>: Not have data. \r\n");
			ptc_res = xPROCOMM_SendLogManipulateACK(data_stat.fid, LOG_ACCD_DATAEMPTY, STAT_SUCCESS, DEBUG_PORT);
			if(ptc_res != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "X<<-- LOG_MANIPULATION ACK Failed(FAT_%d) \r\n", ptc_res);
			}
		}else {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Undefine command 0x%02X \r\n", data_stat.cmd);
		}
	}else {																																										/********** Open file failed */
		_LED_SD_ON = LED_SD_TIME;
		isFileOpen = false;
		printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Open %s file failed(FAT_%d) \r\n", file_name, fatfs_res);
	}
	
	/*-- Close file --*/
	if(isFileOpen) {
		fatfs_res = xSDCARD_CloseFile(&csv_file);
		if(fatfs_res != FR_OK) {
			_LED_SD_ON = LED_SD_TIME;
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Close %s failed(FAT_%d) \r\n", file_name, fatfs_res);
		}
	}
// 	taskENABLE_INTERRUPTS();		/* Enable Interupt */
	
}
/*==========================================================================================*/
///**
//  * @brief  Handle data queue.
//  * @param  LOG_DATA Structure.
//  * @retval None.
//  */
//static void vSD_HandleDataQueue(ST_LOG_DATA data_stat) {
//	
//	char			file_name[13];
//	bool 			isFileOpen 	= false;
//	bool			wdg 				= true;
//	uint32_t	i;
//	uint32_t	last_index	=	0;
//	uint32_t	first_index	=	0;
//	uint32_t	num_record	=	0;
//	
//	FIL				data_file;
//	FRESULT		fatfs_res;
//	LOGRESULT	log_res;
//	PTC_RES		ptc_res;
//	
//	/*-- Get file name from timestamp --*/
//	vSDNMB_GetFileName(data_stat.timestamp, file_name);
//	printDebug_Semphr(DETAIL, "<SD_HandleDataQueue>: File_Name: %s (%d)\r\n", file_name, data_stat.timestamp);
//	
//	taskDISABLE_INTERRUPTS();		/* Disable Interupt */
//	/*-- Open file --*/
//	fatfs_res = xSDCARD_OpenFile(&data_file, file_name);
//	if(fatfs_res == FR_OK) {																																/********** File already exists */
//		isFileOpen = true;
//		/*-- Read Last_Index --*/
//		fatfs_res = xSDCARD_ReadData(&data_file, WRITE_INDEX_ADDR, &last_index, sizeof(uint32_t));
//		if(fatfs_res != FR_OK) {
//			_LED_SD_ON = LED_SD_TIME;
//			printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Read last_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
//			delay_ms(SD_STABLE_DELAY);
//			/* If couldn't read last index, Set last index as default */
//			last_index = 0;
//			fatfs_res = xSDCARD_WriteData(&data_file, WRITE_INDEX_ADDR, &last_index, sizeof(uint32_t));
//			if(fatfs_res != FR_OK) {
//				printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write last_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
//			}
//		}
//		/*-- Handle Command --*/
//		if(data_stat.cmd == LOG_CMD_WRITE) {																														/****** Command Write */
//			/*-- Save data into file --*/
//			log_res = xLOG_SaveData(&data_file, data_stat.timestamp, data_stat.num_status, data_stat.status, data_stat.num_data, data_stat.data);
//			if(log_res != LOG_FR_OK) {
//				_LED_SD_ON = LED_SD_TIME;
//				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Write %s failed(LOG_%d) \r\n", file_name, log_res);
//			}
//		}else if(data_stat.cmd == LOG_CMD_READ) {																												 /****** Command Read */
//			/*-- Get amount of backup data --*/
//			log_res = xLOG_GetIndex(&data_file, WRITE_INDEX_ADDR, &num_record);
//			if(log_res == LOG_FR_OK) {
//				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Data amount: %d \r\n", num_record);
//				for(i = 0; i < num_record; i++) {
//					log_res = xLOG_GetData(&data_file, i, &data_stat.timestamp, &data_stat.num_status, &data_stat.status[0], &data_stat.num_data, &data_stat.data[0]);
//					if(log_res == LOG_FR_OK) {						
//						ptc_res = xPROCOMM_SendDataNStatusLog(data_stat.timestamp, (uint16_t)num_record, (uint16_t)(i+1), data_stat.num_status, &data_stat.status[0], data_stat.num_data, &data_stat.data[0], 0);
//						if(ptc_res != PTC_OK) {
//							printDebug_Semphr(MINOR_ERR, "X<<-- DATA&STATUS_LOG Failed(PTC_%d) \r\n", ptc_res);
//						}
//					}else {
//						_LED_SD_ON = LED_SD_TIME;
//						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Get data %s failed(LOG_%d) \r\n", file_name, log_res);
//					}
//					/*-- Reset SD WDG Counter --*/
//					if(!xQueueSend(Queue_SDTaskWDG, &wdg, 0)) {
//						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Send Queue_SDTaskWDG failed. \r\n");
//					}
//					delay_ms(10);
//				}
//				/*-- Send ACK Success to Debug Port --*/
//				ptc_res = xPROCOMM_SendLogManipulateACK(data_stat.fid, LOG_ACCD_READDATA, STAT_SUCCESS, DEBUG_PORT);
//				if(ptc_res != PTC_OK) {
//					printDebug_Semphr(MINOR_ERR, "X<<-- LOG_MANIPULATION ACK Failed(PTC_%d) \r\n", ptc_res);
//				}
//			}else {
//				/*xLOG_GetIndex error*/
//				_LED_SD_ON = LED_SD_TIME;
//				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Get index %s failed(LOG_%d) \r\n", file_name, log_res);
//			}				
//		}else {
//			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Undefine command 0x%02X \r\n", data_stat.cmd);
//		}
//	}else if(fatfs_res == FR_NO_FILE) {																										/********** File dose not exists */
//		/*-- Handle Command --*/
//		if(data_stat.cmd == LOG_CMD_WRITE) {																														/****** Command Write */
//			printDebug(DESCRIBE, "<SD_HandleDataQueue>: Creating file %s ... \r\n", file_name);
//			fatfs_res = xSDCARD_CreateFile(&data_file, file_name, CREATE_ALWAYS);
//			if(fatfs_res == FR_OK) {
//				/* Set Write_Index as default (0x00000000) */
//				last_index = 0;
//				fatfs_res = xSDCARD_WriteData(&data_file, WRITE_INDEX_ADDR, &last_index, sizeof(uint32_t));
//				if(fatfs_res != FR_OK) {
//					_LED_SD_ON = LED_SD_TIME;
//					printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write last_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
//				}
//				/* Set Read_Index as default (0x00000000) */
//				first_index = 0;
//				fatfs_res = xSDCARD_WriteData(&data_file, READ_INDEX_ADDR, &first_index, sizeof(uint32_t));
//				if(fatfs_res != FR_OK) {
//					_LED_SD_ON = LED_SD_TIME;
//					printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Write first_index %s failed(FAT_%d) \r\n", file_name, fatfs_res);
//				}
//				/*-- Close file --*/
//				fatfs_res = xSDCARD_CloseFile(&data_file);
//				if(fatfs_res != FR_OK) {
//					_LED_SD_ON = LED_SD_TIME;
//					printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Close %s failed(FAT_%d) \r\n", DATA_FILE, fatfs_res);
//				}
//				/*-- Open file again --*/
//				fatfs_res = xSDCARD_OpenFile(&data_file, file_name);
//				if(fatfs_res == FR_OK) {
//					isFileOpen = true;
//					/*-- Save data_log into file --*/
//					log_res = xLOG_SaveData(&data_file, data_stat.timestamp, data_stat.num_status, data_stat.status, data_stat.num_data, data_stat.data);
//					if(log_res != LOG_FR_OK) {
//						_LED_SD_ON = LED_SD_TIME;
//						printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Write %s failed(LOG_%d) \r\n", file_name, log_res);
//					}
//				}else {
//					_LED_SD_ON = LED_SD_TIME;
//					printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Open %s file failed(PTC_%d) \r\n", file_name, fatfs_res);
//				}
//			}else {
//				_LED_SD_ON = LED_SD_TIME;
//				isFileOpen = false;
//				printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Create %s file failed(PTC_%d) \r\n", file_name, fatfs_res);
//			}
//		}else if(data_stat.cmd == LOG_CMD_READ) {																												/****** Command Read */
//			printDebug_Semphr(INFO, "<SD_HandleDataQueue>: Not have data. \r\n");
//			ptc_res = xPROCOMM_SendLogManipulateACK(data_stat.fid, LOG_ACCD_DATAEMPTY, STAT_SUCCESS, DEBUG_PORT);
//			if(ptc_res != PTC_OK) {
//				printDebug_Semphr(MINOR_ERR, "X<<-- LOG_MANIPULATION ACK Failed(PTC_%d) \r\n", ptc_res);
//			}
//		}else {
//			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Undefine command 0x%02X \r\n", data_stat.cmd);
//		}
//	}else {																																										/********** Open file failed */
//		_LED_SD_ON = LED_SD_TIME;
//		isFileOpen = false;
//		printDebug(MINOR_ERR, "<SD_HandleDataQueue>: Open %s file failed(FAT_%d) \r\n", file_name, fatfs_res);
//	}
//	
//	/*-- Close file --*/
//	if(isFileOpen) {
//		fatfs_res = xSDCARD_CloseFile(&data_file);
//		if(fatfs_res != FR_OK) {
//			_LED_SD_ON = LED_SD_TIME;
//			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataQueue>: Close %s failed(FAT_%d) \r\n", file_name, fatfs_res);
//		}
//	}
//	taskENABLE_INTERRUPTS();		/* Enable Interupt */
//	
//}
/*==========================================================================================*/
/**
  * @brief  Handle config_log queue.
  * @param  LOG_CONFIG Structure.
  * @retval None.
  */
static void vSD_HandleConfigLogQueue(ST_LOG_CONFIG log_config) {
	
	uint8_t		i = 0;
	FIL   		configLog_file;
	FRESULT		fatfs_res;
	LOGRESULT	log_res;
	
	taskDISABLE_INTERRUPTS();    	/* Disable Interupt */
	/*-- Open file --*/
	for(i = 0; i < SD_RETRY; i++) {
		fatfs_res = xSDCARD_OpenFile(&configLog_file, (const char*)LOG_CONFIG);
		if(fatfs_res == FR_OK) {
			break;
		}else {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Open %s failed(FAT_%d) \r\n", LOG_CONFIG, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_OCONFIGLOG_ERR;
				return;
			}
		}
		delay_ms(SD_STABLE_DELAY);
	}
	
	if(log_config.cmd == LOG_CMD_READ) {																														/****** Command Read */
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_ReadConfig(&configLog_file, &log_config.timestamp, &log_config.accd, &log_config.cnfg, log_config.spec, &log_config.spec_len);
			if(log_res == LOG_FR_OK) {			
				log_config.cmd = LOG_ACK_OK;
				if(!xQueueSend(Queue_ConfigLogFromSD, &log_config, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Send Queue_ConfigLogFromSD failed. \r\n");
				}else {
					break;
				}
			}else if(log_res == LOG_EMPTY) {
				flag_HaveConfigLog = false;
				break;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Read %s failed(LOG_%d) \r\n", LOG_CONFIG, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_RCONFIGLOG_ERR;
				}
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else if(log_config.cmd == LOG_CMD_WRITE) {																											/****** Command Write */
		/*-- Save data_log into file --*/
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_WriteConfig(&configLog_file, log_config.timestamp, log_config.accd, log_config.cnfg, log_config.spec, log_config.spec_len);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Write %s failed(LOG_%d) \r\n", LOG_CONFIG, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_WCONFIGLOG_ERR;
				}
			}else {
				flag_HaveConfigLog = true;
				break;
			}			
			delay_ms(SD_STABLE_DELAY);
		}
	}else if(log_config.cmd == LOG_CMD_SETiNDEX) {																					/****** Command Set Next Index */
		/*-- Set next read index --*/
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_SetNextReadIndex(&configLog_file, ID_CONFIGLOG);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Set read index %s failed(LOG_%d) \r\n", LOG_CONFIG, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_ICONFIGLOG_ERR;
				}
			}else {
				break;
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else {
		printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Undefine command 0x%02X \r\n", log_config.cmd);
	}
	
	for(i = 0; i < SD_RETRY; i++) {
		/*-- Close file --*/
		fatfs_res = xSDCARD_CloseFile(&configLog_file);
		if(fatfs_res != FR_OK) {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleConfigLogQueue>: Close %s failed(FAT_%d) \r\n", LOG_CONFIG, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_CCONFIGLOG_ERR;
			}
		}else {
			break;
		}
		delay_ms(SD_STABLE_DELAY);
	}
	taskENABLE_INTERRUPTS();    	/* Enable Interupt */
}
/*==========================================================================================*/
/**
  * @brief  Handle event_log queue.
  * @param  LOG_EVENT Structure.
  * @retval None.
  */
static void vSD_HandleEventLogQueue(ST_LOG_EVENT log_event) {
	
	uint8_t		i = 0;
	FIL   		eventLog_file;
	FRESULT		fatfs_res;
	LOGRESULT	log_res;
	
	taskDISABLE_INTERRUPTS();    	/* Disable Interupt */
	/*-- Open file --*/
	for(i = 0; i < SD_RETRY; i++) {
		fatfs_res = xSDCARD_OpenFile(&eventLog_file, (const char*)LOG_EVENT);
		if(fatfs_res == FR_OK) {
			break;
		}else {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Open %s failed(FAT_%d) \r\n", LOG_EVENT, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_OEVENTLOG_ERR;
				return;
			}
		}
		delay_ms(SD_STABLE_DELAY);
	}
		
	if(log_event.cmd == LOG_CMD_READ) {																															/****** Command Read */
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_ReadEvent(&eventLog_file, &log_event.timestamp, &log_event.eid, &log_event.edata);
			if(log_res == LOG_FR_OK) {			
				log_event.cmd = LOG_ACK_OK;
				if(!xQueueSend(Queue_EventLogFromSD, &log_event, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Send Queue_EventLogFromSD failed. \r\n");
				}else {
					break;
				}
			}else if(log_res == LOG_EMPTY) {
				flag_HaveEventLog = false;
				break;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Read %s failed(LOG_%d) \r\n", LOG_EVENT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_REVENTLOG_ERR;
				}
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else if(log_event.cmd == LOG_CMD_WRITE) {																											/****** Command Write */
		/*-- Save data_log into file --*/
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_WriteEvent(&eventLog_file, log_event.timestamp, log_event.eid, log_event.edata);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Write %s failed(LOG_%d) \r\n", LOG_EVENT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_WEVENTLOG_ERR;
				}
			}else {
				flag_HaveEventLog = true;
				break;
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else if(log_event.cmd == LOG_CMD_SETiNDEX) {																					/****** Command Set Next Index */
		for(i = 0; i < SD_RETRY; i++) {
			/*-- Set next read index --*/
			log_res = xLOG_SetNextReadIndex(&eventLog_file, ID_EVENTLOG);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Set read index %s failed(LOG_%d) \r\n", LOG_EVENT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_IEVENTLOG_ERR;
				}
			}else {
				break;
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else {
		printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Undefine command 0x%02X \r\n", log_event.cmd);
	}
	
	/*-- Close file --*/
	for(i = 0; i < SD_RETRY; i++) {
		fatfs_res = xSDCARD_CloseFile(&eventLog_file);
		if(fatfs_res != FR_OK) {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleEventLogQueue>: Close %s failed(FAT_%d) \r\n", LOG_EVENT, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_CEVENTLOG_ERR;
			}
		}else {
			break;
		}
		delay_ms(SD_STABLE_DELAY);
	}
	taskENABLE_INTERRUPTS();    	/* Enable Interupt */
}
/*==========================================================================================*/
/**
  * @brief  Handle data_log queue.
  * @param  LOG_DATA Structure.
  * @retval None.
  */
static void vSD_HandleDataLogQueue(ST_LOG_DATA log_data) {
	
	uint8_t		i, j;
	uint8_t		printLevel    = 0;
	
	FIL   		dataLog_file;
	FRESULT		fatfs_res;
	LOGRESULT	log_res;
	
	taskDISABLE_INTERRUPTS();    	/* Disable Interupt */
	/*-- Open file --*/
	for(i = 0; i < SD_RETRY; i++) {
		fatfs_res = xSDCARD_OpenFile(&dataLog_file, (const char*)LOG_DATAnSTAT);
		if(fatfs_res == FR_OK) {
			break;
		}else {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Open %s failed(FAT_%d) \r\n", LOG_DATAnSTAT, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_ODATALOG_ERR;
				return;
			}
		}
		delay_ms(SD_STABLE_DELAY);
	}
	
	if(log_data.cmd == LOG_CMD_READ) {																															/****** Command Read */
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_ReadData(&dataLog_file, &log_data.timestamp, &log_data.num_status, &log_data.status[0], &log_data.num_data, &log_data.data[0]);
			if(log_res == LOG_FR_OK) {						
				log_data.cmd = LOG_ACK_OK;
				if(!xQueueSend(Queue_DataLogFromSD, &log_data, 0)) {
					printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Send Queue_DataLogFromSD failed. \r\n");
				}else {
					break;
				}
			}else if(log_res == LOG_EMPTY) {
				flag_HaveDataLog = false;
				break;
			}else if(log_res == LOG_ERR) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Log error, Set next index... \r\n");
				delay_ms(SD_STABLE_DELAY);
				/*-- Set next read index --*/
				log_res = xLOG_SetNextReadIndex(&dataLog_file, ID_DATALOG);
				if(log_res != LOG_FR_OK) {
					_LED_SD_ON = LED_SD_TIME;
					printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Set read index %s failed(LOG_%d) \r\n", LOG_DATAnSTAT, log_res);
				}else {
					break;
				}
			}else {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Read %s failed(LOG_%d) \r\n", LOG_DATAnSTAT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_RDATALOG_ERR;
				}
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else if(log_data.cmd == LOG_CMD_WRITE) {																											/****** Command Write */
		/*-- Save data_log into file --*/
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_WriteData(&dataLog_file, log_data.timestamp, log_data.num_status, log_data.status, log_data.num_data, log_data.data);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Write %s failed(LOG_%d) \r\n", LOG_DATAnSTAT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_WDATALOG_ERR;
				}
			}else {
				flag_HaveDataLog = true;
				break;
			}
			delay_ms(SD_STABLE_DELAY);
		}				
	}else if(log_data.cmd == LOG_CMD_SETiNDEX) {																						/****** Command Set Next Index */
		/*-- Set next read index --*/
		for(i = 0; i < SD_RETRY; i++) {
			log_res = xLOG_SetNextReadIndex(&dataLog_file, ID_DATALOG);
			if(log_res != LOG_FR_OK) {
				printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Set read index %s failed(LOG_%d) \r\n", LOG_DATAnSTAT, log_res);
				if((i+1) == SD_RETRY) {
					_LED_SD_ON = LED_SD_TIME;
					flag_HaveMemEvent = true;
					_MemoryStatus = EV_MEM_IDATALOG_ERR;
				}
			}else {
				break;
			}
			delay_ms(SD_STABLE_DELAY);
		}
	}else {
		printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Undefine command 0x%02X \r\n", log_data.cmd);
	}
	
	/*-- Close file --*/
	for(i = 0; i < SD_RETRY; i++) {
		fatfs_res = xSDCARD_CloseFile(&dataLog_file);
		if(fatfs_res != FR_OK) {
			printDebug_Semphr(MINOR_ERR, "<SD_HandleDataLogQueue>: Close %s failed(FAT_%d) \r\n", LOG_DATAnSTAT, fatfs_res);
			if((i+1) == SD_RETRY) {
				_LED_SD_ON = LED_SD_TIME;
				flag_HaveMemEvent = true;
				_MemoryStatus = EV_MEM_CDATALOG_ERR;
			}
		}else {
			break;
		}
		delay_ms(SD_STABLE_DELAY);
	}
	taskENABLE_INTERRUPTS();    	/* Enable Interupt */

}
/*==========================================================================================*/
/**
  * @brief  Get file name from timestamp.
  * @param  
  * @retval None.
  */
void vSDNMB_GetFileName(uint32_t timestamp, char *name) {
	
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	timestamp += (3600*7); 	// GMT+7
	vRTC_EpochToDateTime(&RTC_DateStruct, &RTC_TimeStruct, timestamp);
	
	if(RTC_DateStruct.RTC_Date < 10) {
		sprintf(&name[0], "%d", 0);
		sprintf(&name[1], "%d", RTC_DateStruct.RTC_Date);
	}else {
		sprintf(&name[0], "%d", RTC_DateStruct.RTC_Date);
	}
	if(RTC_DateStruct.RTC_Month < 10) {
		sprintf(&name[2], "%d", 0);
		sprintf(&name[3], "%d", RTC_DateStruct.RTC_Month);
	}else {
		sprintf(&name[2], "%d", RTC_DateStruct.RTC_Month);
	}
	sprintf(&name[4], "%d", (RTC_DateStruct.RTC_Year+2000));
	sprintf(&name[8], "%s", ".csv");
	
	return;
}
/*==========================================================================================*/






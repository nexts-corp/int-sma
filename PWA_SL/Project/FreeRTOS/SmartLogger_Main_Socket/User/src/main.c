/**
  ******************************************************************************
  * @file    main.c
  * @author  Intelenics Embedded Team
  * @version V1.0.0
  * @date    18-March-2014
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "main.h"

#include "SL_PinDefine.h"
#include "SL_ConfigStruct.h"	 
#include "Manager_Task.h"
#include "GSM_Task.h"
#include "Sensors_Task.h"
#include "Log_Task.h"
#include "SD_Task.h"
#include "Modbus_Task.h"

#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_ProtocolParser.h"
#include "INT_STM32F4_ProtocolHandle.h"
#include "INT_STM32F4_420RClick.h"
#include "INT_STM32F4_ConfigSD.h"
#include "INT_STM32F4_DataQueue.h"
#include "INT_STM32F4_OneWire.h"
#include "INT_STM32F4_24LC512.h"
#include "INT_STM32F4_aes128.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_Usart.h"
#include "INT_STM32F4_debug.h"
#include "stm32f4_SDCard.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_adc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_ds1307.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private define ------------------------------------------------------------*/
/* Tasks Priority */
#define GENERAL_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )

/* Queue ---------------------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
xQueueHandle Queue_PacketHTTPToGSM	= NULL;
xQueueHandle Queue_PacketToGSM 			= NULL;
xQueueHandle Queue_PacketFromGSM 		= NULL;
xQueueHandle Queue_CMDToGSM					= NULL;
xQueueHandle Queue_DataToSD					= NULL;
xQueueHandle Queue_DataFromSD				= NULL;
xQueueHandle Queue_DataLogToSD			= NULL;
xQueueHandle Queue_DataLogFromSD		= NULL;
xQueueHandle Queue_EventLogToSD			= NULL;
xQueueHandle Queue_EventLogFromSD		= NULL;
xQueueHandle Queue_ConfigLogToSD		= NULL;
xQueueHandle Queue_ConfigLogFromSD	= NULL;
xQueueHandle Queue_DebugToSD				= NULL;
xQueueHandle Queue_CopyFromSD				= NULL;
xQueueHandle Queue_SDTaskWDG				= NULL;
xQueueHandle Queue_CtrlValve				= NULL;
xQueueHandle Queue_CtrlValveMB			= NULL;

/* Infomation Structure */
ST_INFO 							struct_info;
/* Status Structure */
ST_STATUS 						struct_Status;
/* Structure for system configuration */
STCONFIG_DEV 					struct_DevConfig;
STCONFIG_PROPERTIES		struct_ConfigProperties;
STCONFIG_OPERATING		struct_ConfigOperating;
STCONFIG_BUZZER				struct_ConfigBuzzer;
STCONFIG_WORKING			struct_ConfigWorking;
STCONFIG_NETWORK 			struct_ConfigNetwork; 
STCONFIG_ANALOGSENSOR struct_Config420Sensor[MAX_420SENSOR];
STCONFIG_ANALOGSENSOR struct_ConfigADCSensor[MAX_ADCSENSOR];
STCONFIG_MODBUSSENSOR	struct_ConfigMODBUSSensor;
STCONFIG_PULSESENSOR	struct_ConfigPulseSensor;
STCONFIG_PULSEADJ			struct_ConfigPulseADJ;
STCONFIG_DIGITALsIN 	struct_ConfigDIN[MAX_DIN];
STCONFIG_SENSORALARM 	struct_Config420SensorAlarm[MAX_420SENSOR];
STCONFIG_SENSORALARM 	struct_ConfigADCSensorAlarm[MAX_ADCSENSOR];
STCONFIG_SENSORALARM 	struct_ConfigMODBUSSensorAlarm;
STCONFIG_SENSORALARM 	struct_ConfigPulseSensorAlarm;
STCONFIG_VALVE				struct_ConfigValve;
STCONFIG_420CAL				struct_Config420Cal[MAX_420SENSOR];
STCONFIG_420CAL 			struct_Config420ValveInCal;
STCONFIG_420CAL 			struct_Config420ValveOutCal;

/* Global variables ----------------------------------------------------------*/
__IO float			_BatteryRemain					= 0.0;
__IO bool 			flag_HaveDataLog				= false;
__IO bool 			flag_HaveConfigLog			= false;
__IO bool 			flag_HaveEventLog				= false;
__IO bool				flag_isSDInstall				= false;
__IO bool				flag_HaveMemEvent				= false;

__IO uint8_t 		flag_ReqSensorData 			= 0;   					/* Manager's Task set -> true, Sensor's Task set -> false */
__IO uint8_t		_Communication_Status		= COMMU_NULL;
__IO uint8_t		_OperatingMode					= MODE_FULL;
__IO uint8_t		_BatteryStatus					= EV_NORMAL;
__IO uint8_t		_BatteryStat_old				= 0xFF;
__IO uint8_t		_MemoryStatus 					= EV_NORMAL;
__IO uint8_t		_ConfigCmdACK						= 0;
__IO uint8_t		_ConfigCmdFID						= 0;
__IO uint8_t 		_index_pulseData 				= 0;
__IO uint8_t		_PulseData_Counter			= 0;
__IO uint8_t		_LED_SAMP_ON						= 0;
__IO uint8_t		_LED_SENT_ON						= 0;
__IO uint8_t		_LED_SD_ON							= 0;
__IO uint8_t		_LED_ERR_ON							= 0;
__IO uint16_t 	rx_wr_index3 						= 0;
__IO uint16_t 	rx_rd_index3 						= 0;
__IO uint16_t 	rx_counter3 						= 0;
__IO uint32_t 	_ms_counter 						= 0;
__IO uint32_t 	_us_counter 						= 0;
__IO uint32_t		_GSMCounter							= 0;
__IO uint32_t		_Transmit_Period				= 0;
__IO uint32_t 	_SDWDGCounter 					= 0;

uint8_t 				flag_GSMReady;
uint8_t					print_count = 0;

SENSOR_DATA 	_Sensor_Data;

const char 			firmware_version[] 			= "iNT-SL-0.1a";
const uint32_t 	DINn_PORT[MAX_DIN] 		= {DIN1_PORT, DIN2_PORT, DIN3_PORT, DIN4_PORT, DIN5_PORT, DIN6_PORT,
																						DIN7_PORT, DIN8_PORT, DIN9_PORT, DIN10_PORT, DIN11_PORT, DIN12_PORT};
const uint16_t	DINn_PIN[MAX_DIN] 		= {DIN1_PIN, DIN2_PIN, DIN3_PIN, DIN4_PIN, DIN5_PIN, DIN6_PIN,
																						DIN7_PIN, DIN8_PIN, DIN9_PIN, DIN10_PIN, DIN11_PIN, DIN12_PIN};
const uint32_t 	DOUTn_PORT[MAX_DOUT] 	= {DOUT1_PORT, DOUT2_PORT, DOUT3_PORT, DOUT4_PORT};
const uint16_t	DOUTn_PIN[MAX_DOUT] 	= {DOUT1_PIN, DOUT2_PIN, DOUT3_PIN, DOUT4_PIN};

/* Private function prototypes -----------------------------------------------*/
extern void vTester_Task(void *pvParameters);
extern void vDataBuf_Task(void *pvParameters);
extern float fDATBUF_ReadBoardTemp(void);
static void SL_InitialDigitalInput(void);
static void SL_InitialDigitalOutput(void);
uint8_t ucMAIN_SetDefaultInformation(void);
uint8_t ucMAIN_SetDefaultStatus(void);
uint8_t ucMAIN_SetDefaultConfig(void);
void vMAIN_PrintConfig (uint8_t printLevel, uint8_t config_id, void *st_config, uint8_t ch);


/*==========================================================================================*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void) {
	
	unsigned char				check_text[] 	= "HELLO_WORLD";
	unsigned char				read_text[11];
	float temp;
	bool								index_err 		= false;
	bool								isLoadInfo		= false;
	bool								isLoadConfig	= false;
	bool 								isEEPROMError	= false;
	uint8_t 						res, i;
	uint8_t 						printLevel 		= INFO;
//	uint8_t							tmp_data[255];
	uint8_t							sd_cmd				= 0;
	uint32_t 						free_kBytes 	= 0;
	uint32_t 						write_index 	= 0;
	uint32_t 						read_index 		= 0;

	FIL 								info_file;
	FIL 								config_file;
	FIL 								dataLog_file;
	FIL									eventLog_file;
	FIL									configLog_file;
	FIL									debugLog_file;
	FRESULT 						fatfs_res;
	
	RTC_TimeTypeDef 		RTC_TimeStructure;
	RTC_DateTypeDef 		RTC_DateStructure;
	
//	STCONFIG_PROPERTIES	tmp_ConfigProperties;
	
	/* Initial Digital Input ---------------------------------------------------*/
	SL_InitialDigitalInput();
	
	/* Initial Digital Output --------------------------------------------------*/
	SL_InitialDigitalOutput();
	
	/* Initial USART -----------------------------------------------------------*/
	vUSART_Initial();
	
	/* Initial 1 Wire ----------------------------------------------------------*/
 	vONEWIRE_Init();
	
	/* Initial I2C -------------------------------------------------------------*/
 	EEPROM_InitI2C1();
	
	/* Initial Timer -----------------------------------------------------------*/	
	vTIMER_initialTimer3();
	vTIMER_initialTimer12();
	
	/* Initial RTC -------------------------------------------------------------*/
 	res = ucRTC_Initial();																
 	if(res) {
		printDebug(MINOR_ERR, "[MNG_TASK] RTC Initial Error (0x%02X) \r\n", res);
	}
	
	/* Initial SPI1 ------------------------------------------------------------*/
	v420RCLICK_InitialSPI1();

	/* Initial ADC -------------------------------------------------------------*/
 	TM_ADC_Init(ADC1, AIN1_BATT_CH);
 	TM_ADC_Init(ADC1, AIN2_SOLARCELL_CH);
 	TM_ADC_Init(ADC1, AIN3_ADC1_CH);
 	TM_ADC_Init(ADC1, AIN4_ADC2_CH);
	
	/* Initialize DS1307 -------------------------------------------------------*/
//   TM_DS1307_Init();
	
	delay_ms(2000);
	
	RCC_GetClocksFreq(&RCC_Clocks);
	printDebug(DESCRIBE, "\n\r================= SMART LOGGER  ================= \r\n");
	printDebug(DESCRIBE, "Microcontroller  : STM32F407VGT6 @%d MHz\r\n", RCC_Clocks.SYSCLK_Frequency/1000000);
	printDebug(DESCRIBE, "Kernel           : FreeRTOS_v8.0.1 \r\n");
	printDebug(DESCRIBE, "Firmware Version : %s \r\n\n", firmware_version);
	
	/* Create Semaphore --------------------------------------------------------*/
	xSemaphore 	= xSemaphoreCreateMutex(); // Semaphore of print debug
	if(xSemaphore == NULL) {
		printDebug(MAJOR_ERR, "[SEMPHR]: Create Debug_Semaphore Failed. \n\r");
	}
	
	/* Initial SD Card ---------------------------------------------------------*/
	fatfs_res = xSDCARD_Initial();
	if(fatfs_res != FR_OK) {
		printDebug(MINOR_ERR, "[SD_CARD]: SD Mount Failed(FAT_%d) \r\n", fatfs_res);
	}else {
		printDebug(DESCRIBE, "[SD_CARD]: SD Mount OK. \r\n");
	}
	
	/* Initial all file (SD Card) ----------------------------------------------*/
	fatfs_res = xSDCARD_ReadMemoryRemain(&free_kBytes);
	if(fatfs_res == FR_OK) {
		LED_SD_GREEN;
		flag_isSDInstall = true;
		printDebug(DESCRIBE, "[SD_CARD]: FreeSpace = %d kBytes\r\n", free_kBytes);
		
		/* Check Information File -------------------------------------------*/
		fatfs_res = xSDCARD_OpenFile(&info_file, (const char*)INFO_FILE);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[INFO_FILE]: Checking file.... \r\n");
			fatfs_res = xSDCARD_ReadData(&info_file, CHKTEXT_BASEADDR, &read_text, sizeof(check_text));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[INFO_FILE]: Couldn't read check_text(FAT_%d) \r\n", fatfs_res);
			}else {
				if(memcmp(check_text, read_text, sizeof(check_text)) == 0) {
					printDebug(DESCRIBE, "[INFO_FILE]: CheckText: %s \r\n", read_text);
				}else {
					printDebug(DESCRIBE, "[INFO_FILE]: Load Information \r\n");
					isLoadInfo = true;
				}
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[INFO_FILE]: Not have file in SD Card. \r\n");
		}else {
			printDebug(MINOR_ERR, "[INFO_FILE]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Information File -------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&info_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[INFO_FILE]: Close Info File error(FAT_%d) \r\n", fatfs_res);
		}else {
			printDebug(DESCRIBE, "[INFO_FILE]: Close Info File. \r\n");
		}
		
		/* Check Configuation File ------------------------------------------*/
		fatfs_res = xSDCARD_OpenFile(&config_file, (const char*)CONFIG_FILE);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[CONFIG_FILE]: Checking file.... \r\n");
			memcpy(read_text, "-----------", sizeof(check_text));			 	// clear read_text
			fatfs_res = xSDCARD_ReadData(&config_file, CHKTEXT_BASEADDR, &read_text, sizeof(check_text));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[CONFIG_FILE]: Couldn't read check_text(FAT_%d) \r\n", fatfs_res);
			}else {
				if(memcmp(check_text, read_text, sizeof(check_text)) == 0) {
					printDebug(DESCRIBE, "[CONFIG_FILE]: CheckText: %s \r\n", read_text);
				}else {
					printDebug(DESCRIBE, "[CONFIG_FILE]: Load Configuration \r\n");
					isLoadConfig = true;
				}
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[CONFIG_FILE]: Not have file in SD Card. \r\n");
		}else {
			printDebug(MINOR_ERR, "[CONFIG_FILE]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Configuation File ------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&config_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[CONFIG_FILE]: Close Info File error(FAT_%d) \r\n", fatfs_res);
		}else {
			printDebug(DESCRIBE, "[CONFIG_FILE]: Close Info File. \r\n");
		}
				
		/* Data Log File ----------------------------------------------------*/
		index_err = false;
		fatfs_res = xSDCARD_OpenFile(&dataLog_file, (const char*)LOG_DATAnSTAT);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[DATA_LOG]: Checking file.... \r\n");
			/* Read Insert Index */
			fatfs_res = xSDCARD_ReadData(&dataLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[DATA_LOG]: Couldn't read write_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[DATA_LOG]: Write_index: %d \r\n", write_index);}
			/* Read Send Index */
			fatfs_res = xSDCARD_ReadData(&dataLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[DATA_LOG]: Couldn't read read_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[DATA_LOG]: Read_index: %d \r\n", read_index);}
			/* If couldn't read Write/Read index, Set Write/Read index as default */
			if(index_err) {
				printDebug(DESCRIBE, "[DATA_LOG]: Set index as default. \r\n");
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&dataLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DATA_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&dataLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DATA_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}
			/* Check, If have data log */
			if(read_index != write_index) {
				flag_HaveDataLog = true;
			}else {
				flag_HaveDataLog = false;
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[DATA_LOG]: Not have file in SD Card. \r\n");
			printDebug(DESCRIBE, "[DATA_LOG]: Creating file... \r\n");
			fatfs_res = xSDCARD_CreateFile(&dataLog_file, (const char*)LOG_DATAnSTAT, CREATE_ALWAYS);
			if(fatfs_res == FR_OK) {
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&dataLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DATA_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&dataLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DATA_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}else {
				printDebug(MINOR_ERR, "[DATA_LOG]: Create File failed(FAT_%d) \r\n", fatfs_res);
			}
		}else {
			printDebug(MINOR_ERR, "[DATA_LOG]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Data & Status Log File --------------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&dataLog_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[DATA_LOG]: Close File failed(FAT_%d) \r\n", fatfs_res);
		}else {
			printDebug(DESCRIBE, "[DATA_LOG]: Close File. \r\n");
		}
		
		/* Event Log File ----------------------------------------------------*/
		index_err = false;
		fatfs_res = xSDCARD_OpenFile(&eventLog_file, (const char*)LOG_EVENT);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[EVENT_LOG]: Checking file.... \r\n");
			/* Read Write Index */
			fatfs_res = xSDCARD_ReadData(&eventLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[EVENT_LOG]: Couldn't read write_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[EVENT_LOG]: Write_index: %d \r\n", write_index);}
			/* Read Read Index */
			fatfs_res = xSDCARD_ReadData(&eventLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[EVENT_LOG]: Couldn't read read_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[EVENT_LOG]: Read_index: %d \r\n", read_index);}
			/* If couldn't read Write/Read index, Set Write/Read index as default */
			if(index_err) {
				printDebug(DESCRIBE, "[EVENT_LOG]: Set index as default. \r\n");
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&eventLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[EVENT_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&eventLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[EVENT_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}
			/* Check, If have event log */
			if(read_index != write_index) {
				flag_HaveEventLog = true;
			}else {
				flag_HaveEventLog = false;
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[EVENT_LOG]: Not have file in SD Card. \r\n");
			printDebug(DESCRIBE, "[EVENT_LOG]: Creating file... \r\n");
			fatfs_res = xSDCARD_CreateFile(&eventLog_file, (const char*)LOG_EVENT, CREATE_ALWAYS);
			if(fatfs_res == FR_OK) {
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&eventLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[EVENT_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&eventLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[EVENT_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}else {
				printDebug(MINOR_ERR, "[EVENT_LOG]: Create File failed(FAT_%d) \r\n", fatfs_res);
			}
		}else {
			printDebug(MINOR_ERR, "[EVENT_LOG]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Event Log File --------------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&eventLog_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[EVENT_LOG]: Close File failed(FAT_%d) \r\n", fatfs_res);
		}else {printDebug(DESCRIBE, "[EVENT_LOG]: Close File. \r\n");}
		
		/* Config Log File ----------------------------------------------------*/
		index_err = false;
		fatfs_res = xSDCARD_OpenFile(&configLog_file, (const char*)LOG_CONFIG);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[CONF_LOG]: Checking file.... \r\n");
			/* Read Insert Index */
			fatfs_res = xSDCARD_ReadData(&configLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[CONF_LOG]: Couldn't read write_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[CONF_LOG]: Write_index: %d \r\n", write_index);}
			/* Read Send Index */
			fatfs_res = xSDCARD_ReadData(&configLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[CONF_LOG]: Couldn't read read_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[CONF_LOG]: Read_index: %d \r\n", read_index);}
			/* If couldn't read Write/Read index, Set Write/Read index as default */
			if(index_err) {
				printDebug(DESCRIBE, "[CONF_LOG]: Set index as default. \r\n");
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&configLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[CONF_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&configLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[CONF_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}
			/* Check, If have config log */
			if(read_index != write_index) {
				flag_HaveConfigLog = true;
			}else {
				flag_HaveConfigLog = false;
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[CONF_LOG]: Not have file in SD Card. \r\n");
			printDebug(DESCRIBE, "[CONF_LOG]: Creating file... \r\n");
			fatfs_res = xSDCARD_CreateFile(&configLog_file, (const char*)LOG_CONFIG, CREATE_ALWAYS);
			if(fatfs_res == FR_OK) {
				/* Set Write_Index as default (0x00000000) */
				write_index = 0;
				fatfs_res = xSDCARD_WriteData(&configLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[CONF_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
				/* Set Read_Index as default (0x00000000) */
				read_index = 0;
				fatfs_res = xSDCARD_WriteData(&configLog_file, READ_INDEX_ADDR, &read_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[CONF_LOG]: Write read_index error(FAT_%d) \r\n", fatfs_res);
				}
			}else {
				printDebug(MINOR_ERR, "[CONF_LOG]: Create File failed(FAT_%d) \r\n", fatfs_res);
			}
		}else {
			printDebug(MINOR_ERR, "[CONF_LOG]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Config Log File --------------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&configLog_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[CONF_LOG]: Close File failed(FAT_%d) \r\n", fatfs_res);
		}else {printDebug(DESCRIBE, "[CONF_LOG]: Close File. \r\n");}
		
		/* Debug Log File ----------------------------------------------------*/
		index_err = false;
		fatfs_res = xSDCARD_OpenFile(&debugLog_file, (const char*)DEBUG_LOG);
		if(fatfs_res == FR_OK) {
			printDebug(DESCRIBE, "[DEBUG_LOG]: Checking file.... \r\n");
			/* Read Insert Index */
			fatfs_res = xSDCARD_ReadData(&debugLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
			if(fatfs_res != FR_OK) {
				printDebug(MINOR_ERR, "[DEBUG_LOG]: Couldn't read write_index(FAT_%d) \r\n", fatfs_res);
				index_err |= true;
			}else {printDebug(DESCRIBE, "[DEBUG_LOG]: Write_Addr: 0x%08X \r\n", write_index);}
			/* If couldn't read Write/Read index, Set Write/Read index as default */
			if(index_err) {
				printDebug(DESCRIBE, "[DEBUG_LOG]: Set index as default. \r\n");
				/* Set Write_Index as default (0x00000000) */
				write_index = 0x0A;
				fatfs_res = xSDCARD_WriteData(&debugLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DEBUG_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
			}
		}else if(fatfs_res == FR_NO_FILE) {
			printDebug(DESCRIBE, "[DEBUG_LOG]: Not have file in SD Card. \r\n");
			printDebug(DESCRIBE, "[DEBUG_LOG]: Creating file... \r\n");
			fatfs_res = xSDCARD_CreateFile(&debugLog_file, (const char*)DEBUG_LOG, CREATE_ALWAYS);
			if(fatfs_res == FR_OK) {
				/* Set Write_Index as default (0x00000000) */
				write_index = 0x0A;
				fatfs_res = xSDCARD_WriteData(&debugLog_file, WRITE_INDEX_ADDR, &write_index, sizeof(uint32_t));
				if(fatfs_res != FR_OK) {
					printDebug(MINOR_ERR, "[DEBUG_LOG]: Write write_index error(FAT_%d) \r\n", fatfs_res);
				}
			}else {
				printDebug(MINOR_ERR, "[DEBUG_LOG]: Create File failed(FAT_%d) \r\n", fatfs_res);
			}
		}else {
			printDebug(MINOR_ERR, "[DEBUG_LOG]: Open File failed(FAT_%d) \r\n", fatfs_res);
		}
		/* Close Debug Log File --------------------------------------------------*/
		fatfs_res = xSDCARD_CloseFile(&debugLog_file);
		if(fatfs_res != FR_OK) {
			printDebug(MINOR_ERR, "[DEBUG_LOG]: Close File failed(FAT_%d) \r\n", fatfs_res);
		}else {printDebug(DESCRIBE, "[DEBUG_LOG]: Close File. \r\n");}
		
	}else if(fatfs_res == FR_NOT_READY) {
		LED_SD_RED;
		flag_isSDInstall = false;
		printDebug(DESCRIBE, "[SD_CARD]: SD not installed (not ready) \r\n");
	}else {
		printDebug(DESCRIBE, "[SD_CARD]: Read SD free space failed(FAT_%d) \r\n", fatfs_res);
	}
		
	/* EEPROM ----------------------------------------------------------------*/
	memcpy(read_text, "-----------", sizeof(check_text));			 	// clear read_text
	if(EEPROM_ReadBuf(CHKTEXT_BASEADDR, &read_text[0], sizeof(check_text))) {
		isEEPROMError |= true;
		printDebug(DESCRIBE, "[EEPROM]: Read CheckText Error. \r\n");
	}else {
		if(memcmp(check_text, read_text, sizeof(check_text)) == 0) {
			printDebug(DESCRIBE, "[EEPROM]: CheckText: %s \r\n", read_text);
		}else {
			printDebug(DESCRIBE, "[EEPROM]: Not found config, Set default config... \r\n");
			if(ucMAIN_SetDefaultInformation()) {
				printDebug(DESCRIBE, "[INFO]: Write Infomation Error \r\n");
			}
			if(ucMAIN_SetDefaultStatus()) {
				printDebug(DESCRIBE, "[EEPROM]: Write Configuration Error \r\n");
			}			
			if(ucMAIN_SetDefaultConfig()) {
				printDebug(DESCRIBE, "[EEPROM]: Write Configuration Error \r\n");
			}				
			if(EEPROM_WriteBuf(CHKTEXT_BASEADDR, &check_text[0], sizeof(check_text))) {
				printDebug(DESCRIBE, "[EEPROM]: Write check text error. \r\n");
			}
		}
	}
	
	/* Information -----------------------------------------------------------*/
	printDebug(DESCRIBE, "[EEPROM]: Reading Information.... \r\n");
	if(EEPROM_ReadBuf(INFO_BASEADDR, &struct_info, sizeof(ST_INFO))) {
		isEEPROMError |= true;
		printDebug(DESCRIBE, "[EEPROM]: Read Infomation Error \r\n");
	}else {
		printDebug(DESCRIBE, "[EEPROM]: User_Name: %s \r\n", struct_info.user);
		printDebug(DESCRIBE, "[EEPROM]: Password: %s \r\n", struct_info.password);
		printDebug(DESCRIBE, "[EEPROM]: My_Serial: ");
		for(i = 0; i < ADDR_LEN; i++) {
			printDebug(DESCRIBE, "%02X ", struct_info.serialNumber[i]);
		}
		printDebug(DESCRIBE, "\r\n");
		printDebug(DESCRIBE, "[EEPROM]: Dest_Serial: ");
		for(i = 0; i < ADDR_LEN; i++) {
			printDebug(DESCRIBE, "%02X ", struct_info.destSerial[i]);
		}
		printDebug(DESCRIBE, "\r\n");
		printDebug(DESCRIBE, "[EEPROM]: Registered: %d \r\n", struct_info.flagRegistered);
	}
	
	/* Status ----------------------------------------------------------------*/
	printDebug(DESCRIBE, "[EEPROM]: Reading Status.... \r\n");
	if(EEPROM_ReadBuf(STATUS_BASEADDR, &struct_Status, sizeof(ST_STATUS))) {
		isEEPROMError |= true;
		printDebug(DESCRIBE, "[EEPROM]: Read Status Error \r\n");
	}else {
		printDebug(DESCRIBE, "[EEPROM]: dout1 %d \r\n", struct_Status.dout_1);
		printDebug(DESCRIBE, "[EEPROM]: dout2 %d \r\n", struct_Status.dout_2);
		printDebug(DESCRIBE, "[EEPROM]: dout3 %d \r\n", struct_Status.dout_3);
		printDebug(DESCRIBE, "[EEPROM]: dout4 %d \r\n", struct_Status.dout_4);
		printDebug(DESCRIBE, "[EEPROM]: Valve %d %% \r\n", struct_Status.valve_open);
		printDebug(DESCRIBE, "[EEPROM]: NetFlow %0.4f \r\n", struct_Status.net_flow);
	}

	/* Configuration -----------------------------------------------------------*/
//	printLevel = DETAIL;
		printLevel = DESCRIBE;
	printDebug(DESCRIBE, "[EEPROM]: Read Configuration.... \r\n");
	
	/* >>>> Developer Config <<<< */
	if(EEPROM_ReadBuf(DEVeCONF_BASEADDR, &struct_DevConfig, sizeof(STCONFIG_DEV))) {	
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Deverloper_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, DEV_CONF, &struct_DevConfig, CH_NONE);
	}
	
	/* >>>> Properties_Config <<<< */
	if(EEPROM_ReadBuf(PROPERTIES_BASEADDR, &struct_ConfigProperties, sizeof(STCONFIG_PROPERTIES))) {	
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Properties_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, PROPERTIES_CONF, &struct_ConfigProperties, CH_NONE);		
	}
	
	/* >>>> Operating Config <<<< */
	if(EEPROM_ReadBuf(OPERATING_BASEADDR, &struct_ConfigOperating, sizeof(STCONFIG_OPERATING))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Operating_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, OPERATING_CONF, &struct_ConfigOperating, CH_NONE);
	}
	
	/* >>>> Buzzer Config <<<< */
	if(EEPROM_ReadBuf(BUZZER_BASEADDR, &struct_ConfigBuzzer, sizeof(STCONFIG_BUZZER))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Buzzer_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, BUZZER_CONF, &struct_ConfigBuzzer, CH_NONE);
	}
	
	/* >>>> Working Config <<<< */
	if(EEPROM_ReadBuf(WORKING_BASEADDR, &struct_ConfigWorking, sizeof(STCONFIG_WORKING))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Working_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, WORKING_CONF, &struct_ConfigWorking, CH_NONE);
	}
	
	/* >>>> Network Config <<<< */
	if(EEPROM_ReadBuf(NETWORK_BASEADDR, &struct_ConfigNetwork, sizeof(STCONFIG_NETWORK))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Network_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, NETWORK_CONF, &struct_ConfigNetwork, CH_NONE);
	}
	
	/* >>>> 4-20mA Sensor Config <<<< */
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(EEPROM_ReadBuf(SEN420CONF_BASEADDR[i], &struct_Config420Sensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read 4-20mASensor_Configuration CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, SEN420_CONF, &struct_Config420Sensor[i], i);
		}
	}
	
	/* >>>> ADC Sensor Config <<<< */
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(EEPROM_ReadBuf(SENADCCONF_BASEADDR[i], &struct_ConfigADCSensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read ADCSensor_Configuration CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, SENADC_CONF, &struct_ConfigADCSensor[i], i);
		}
	}
	
	/* >>>> MODBUS FlowSensor Config <<<< */
	if(EEPROM_ReadBuf(SENsMODBUSCONF_BASEADDR, &struct_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read MODBUSFlowSensor_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, SENMODBUS_CONF, &struct_ConfigMODBUSSensor, CH_NONE);
	}
	
	/* >>>> PULSE FlowSensor Config <<<< */
	if(EEPROM_ReadBuf(SENsPULCONF_BASEADDR, &struct_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read PULSEFlowSensor_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, SENPULSE_CONF, &struct_ConfigPulseSensor, CH_NONE);
	}
	
	/* >>>> Digital Input Config CH %d <<<< */
	for(i = 0; i < MAX_DIGItIN; i++) {
		if(EEPROM_ReadBuf(DIGItIN_BASEADDR[i], &struct_ConfigDIN[i], sizeof(STCONFIG_DIGITALsIN))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read DIN_Configuration CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, DIN_CONF, &struct_ConfigDIN[i], i);
		}
	}
	
	/* >>>> 4-20mA_Sensor AlarmConfig <<<< */
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(EEPROM_ReadBuf(SEN420ALARMn_BASEADDR[i], &struct_Config420SensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read 4-20mASensorAlarm_Configuration CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, SEN420ALARM_CONF, &struct_Config420SensorAlarm[i], i);
		}
	}
	
	/* >>>> ADC_Sensor AlarmConfig <<<< */
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(EEPROM_ReadBuf(SENADCALARMn_BASEADDR[i], &struct_ConfigADCSensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read ADCSensorAlarm_Configuration CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, SENADCALARM_CONF, &struct_ConfigADCSensorAlarm[i], i);
		}
	}
	
	/* >>>> MODBUS FlowSensor AlarmConfig <<<< */
	if(EEPROM_ReadBuf(SENsMODBUSALARM_BASEADDR, &struct_ConfigMODBUSSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read MODBUSFlowSensorAlarm_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, SENMODBUSALARM_CONF, &struct_ConfigMODBUSSensorAlarm, CH_NONE);
	}
	
	/* >>>> PULSE FlowSensor Alarm Config <<<< */
	if(EEPROM_ReadBuf(SENsPULALARM_BASEADDR, &struct_ConfigPulseSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read FlowFlowSensorAlarm_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, SENPULSEALARM_CONF, &struct_ConfigPulseSensorAlarm, CH_NONE);
	}
	
	/* >>>> Valve Config <<<< */
	if(EEPROM_ReadBuf(VALVECONF_BASEADDR, &struct_ConfigValve, sizeof(STCONFIG_VALVE))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Valve_Configuration Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, VALVE_CONF, &struct_ConfigValve, CH_NONE);
	}
	
	/* >>>> 4-20mA SensorCH_%d Calibrate <<<< */
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(EEPROM_ReadBuf(SEN420CHnCAL_BASEADDR[i], &struct_Config420Cal[i], sizeof(STCONFIG_420CAL))) {
			isEEPROMError |= true;
			printDebug(MAJOR_ERR, "[EEPROM]: Read 420mASensor_Calibrate CH_%d Error \r\n", i+1);
		}else {
			vMAIN_PrintConfig(printLevel, SEN420CAL_CONF, &struct_Config420Cal[i], i);
		}
	}
	
	/* >>>> 4-20mAInput Valve Calibrate <<<< */
	if(EEPROM_ReadBuf(VALVE420INCAL_BASEADDR, &struct_Config420ValveInCal, sizeof(STCONFIG_420CAL))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read In420mAValve_Calibrate Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, VALVE420INCAL_CONF, &struct_Config420ValveInCal, CH_NONE);
	}
	
	/* >>>> 4-20mAOutput Valve Calibrate <<<< */
	if(EEPROM_ReadBuf(VALVE420OUTCAL_BASEADDR, &struct_Config420ValveOutCal, sizeof(STCONFIG_420CAL))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read Out420mAValve_Calibrate Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, VALVE420OUTCAL_CONF, &struct_Config420ValveOutCal, CH_NONE);
	}
	
	/* >>>> PULSE FlowSensor Value Adjustment <<<< */
	if(EEPROM_ReadBuf(SENPULSEADJ_BASEADDR, &struct_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ))) {
		isEEPROMError |= true;
		printDebug(MAJOR_ERR, "[EEPROM]: Read PULSEFlowValue_Adjustment Error \r\n");
	}else {
		vMAIN_PrintConfig(printLevel, SENPULSEADJ_CONF, &struct_ConfigPulseADJ, CH_NONE);
	}
	
	/*  -------------------------------------------------*/
	if(isEEPROMError) {
		printDebug(MAJOR_ERR, "[EEPROM]: Error.... \r\n");
		_buzzer_2hz = true;
		delay_ms(5000);
		NVIC_SystemReset();
	}
	
	/* Initial Flow accumulate -------------------------------------------------*/
	_Sensor_Data.dataPULSENet 	= 0.0;
	_Sensor_Data.dataPULSENet  += struct_Status.net_flow;
	
	/* Create Queue ------------------------------------------------------------*/
	Queue_PacketHTTPToGSM = xQueueCreate(15, sizeof(PACKET_DATA));
	if(Queue_PacketHTTPToGSM == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_PacketHTTPToGSM was not created. \n\r");
	}
	Queue_PacketToGSM = xQueueCreate(15, sizeof(PACKET_DATA));
	if(Queue_PacketToGSM == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_PacketToGSM was not created. \n\r");
	}
	Queue_PacketFromGSM = xQueueCreate(15, sizeof(PACKET_DATA));
	if(Queue_PacketFromGSM == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_PacketFromGSM was not created. \n\r");
	}
	Queue_CMDToGSM = xQueueCreate(2, sizeof(uint8_t));
	if(Queue_CMDToGSM == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_CMDToGSM was not created. \n\r");
	}
	Queue_DataToSD	=	xQueueCreate(5, sizeof(ST_LOG_DATA));
	if(Queue_DataToSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_DataToSD was not created. \n\r");
	}
	Queue_DataFromSD	=	xQueueCreate(5, sizeof(ST_LOG_DATA));
	if(Queue_DataFromSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_DataFromSD was not created. \n\r");
	}
	Queue_DataLogToSD	=	xQueueCreate(5, sizeof(ST_LOG_DATA));
	if(Queue_DataLogToSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_DataLogToSD was not created. \n\r");
	}
	Queue_DataLogFromSD	=	xQueueCreate(5, sizeof(ST_LOG_DATA));
	if(Queue_DataLogFromSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_DataLogFromSD was not created. \n\r");
	}
	Queue_EventLogToSD	=	xQueueCreate(4, sizeof(ST_LOG_EVENT));
	if(Queue_EventLogToSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_EventLogToSD was not created. \n\r");
	}
	Queue_EventLogFromSD =	xQueueCreate(2, sizeof(ST_LOG_EVENT));
	if(Queue_EventLogFromSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_EventLogFromSD was not created. \n\r");
	}
	Queue_ConfigLogToSD	=	xQueueCreate(2, sizeof(ST_LOG_CONFIG));
	if(Queue_ConfigLogToSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_ConfigLogToSD was not created. \n\r");
	}
	Queue_ConfigLogFromSD	=	xQueueCreate(2, sizeof(ST_LOG_CONFIG));
	if(Queue_ConfigLogFromSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_ConfigLogFromSD was not created. \n\r");
	}
	Queue_DebugToSD	=	xQueueCreate(2, sizeof(DEBUG_MESSAGE));
	if(Queue_DebugToSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_DebugToSD was not created. \n\r");
	}
	Queue_CopyFromSD	=	xQueueCreate(2, sizeof(uint8_t));
	if(Queue_CopyFromSD == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_UpdateStat was not created. \n\r");
	}
	Queue_SDTaskWDG	=	xQueueCreate(1, sizeof(bool));
	if(Queue_SDTaskWDG == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_SDTaskWDG was not created. \n\r");
	}
	Queue_CtrlValve	=	xQueueCreate(2, sizeof(uint32_t));
	if(Queue_CtrlValve == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_CtrlValve was not created. \n\r");
	}
	Queue_CtrlValveMB	=	xQueueCreate(2, sizeof(uint32_t));
	if(Queue_CtrlValveMB == 0) {
		printDebug(MAJOR_ERR, "[X_QUEUE]: Queue_CtrlValveMB was not created. \n\r");
	}

	/* Beep Buzzer */
 	BUZZER_ON;
 	delay_ms(1000);
 	BUZZER_OFF;
	
	printDebug(DESCRIBE, "\r\n***** Initial Complete ***** \r\n");
	vRTC_GetDateTime(&RTC_DateStructure, &RTC_TimeStructure, 7);
	printDebug_Semphr(0x01, "Running @ \r\n");
	printDebug(DESCRIBE, "%02d:%02d:%02d  ",RTC_TimeStructure.RTC_Hours, 
																			RTC_TimeStructure.RTC_Minutes, 
																			RTC_TimeStructure.RTC_Seconds);
	printDebug(DESCRIBE, "%02d/%02d/%04d  ",RTC_DateStructure.RTC_Date, 
																			RTC_DateStructure.RTC_Month, 
																			2000+RTC_DateStructure.RTC_Year);
	ucMNG_ReadDS1820(&temp);
	printDebug(DESCRIBE, "Temp: %0.2f C [DEVICE: %s/ ALARM: %s] \r\n", temp,
																																	(struct_ConfigOperating.deviceEnable==1)?"EN":"DIS",
																																	(struct_ConfigOperating.alarmEnable==1)?"EN":"DIS");
	
	if(isLoadInfo) {
		sd_cmd = LOAD_INFO;
		if(!xQueueSend(Queue_CopyFromSD, &sd_cmd, 0)) {
			printDebug_Semphr(MINOR_ERR, "[MAIN]: Send Queue_CopyFromSD failed. \r\n");
		}
	}
	if(isLoadConfig) {
		sd_cmd = LOAD_CONF;
		if(!xQueueSend(Queue_CopyFromSD, &sd_cmd, 0)) {
			printDebug_Semphr(MINOR_ERR, "[MAIN]: Send Queue_CopyFromSD failed. \r\n");
		}
	}
	
	
//	while(1) {
//		
//		LED_CONNSERVER_RED;
//		LED_CSQ_RED;
//		LED_SAMP_ON;
//		delay_ms(1000);
//		LED_CONNSERVER_GREEN;
//		LED_CSQ_GREEN;
//		LED_SAMP_OFF;
//		delay_ms(1000);
//		
//	}

	xTaskCreate(vManager_Task, (const char*)"Manager_Task", (unsigned short)2048, NULL, GENERAL_TASK_PRIO, NULL);
	xTaskCreate(vDataBuf_Task, (const char*)"DataBuf_Task", (unsigned short)2048, NULL, GENERAL_TASK_PRIO, NULL);
	xTaskCreate(vLog_Task, (const char*)"Log_Task", (unsigned short)2048, NULL, GENERAL_TASK_PRIO, NULL);
	xTaskCreate(vSDnMB_Task, (const char*)"SDnMB_Task", (unsigned short)2048, NULL, GENERAL_TASK_PRIO, NULL);
	xTaskCreate(vGSM_Task, (const char*)"GSM_Task", ( unsigned short )1024, NULL, GENERAL_TASK_PRIO, NULL);
	xTaskCreate(vSensors_Task, (const char*)"Sensors_Task", (unsigned short)512, NULL, GENERAL_TASK_PRIO, NULL);
 	xTaskCreate(vModbus_Task, (const char*)"Modbus_Task", (unsigned short)512, NULL, GENERAL_TASK_PRIO, NULL);

	vTaskStartScheduler();

  for( ;; ); /* Endless Loop */
	
} /* end main() */


/*============================================================================================
 *====================================== Service Function ====================================
 *============================================================================================
 */
/*==========================================================================================*/
/**
  * @brief  This function is delay (msec).
  * @param  Delay number
  * @retval None
  */
void delay_ms(uint32_t delay) {
	
	delay += _ms_counter;
	while(_ms_counter <= delay);
	
}
/*==========================================================================================*/
/**
  * @brief  This function is delay (micro sec).
  * @param  Delay number
  * @retval None
  */
void delay_us(uint32_t delay) {
	
	delay += _us_counter;
	while(_us_counter <= delay);
	
}
/*==========================================================================================*/
/**
  * @brief  This function is print configuration structure.
  * @param  
  * @retval None
  */
void vMAIN_PrintConfig (uint8_t printLevel, uint8_t config_id, void *st_config, uint8_t ch) {
	
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
	
	switch(config_id) {
		case DEV_CONF:
			memcpy(&tmp_DevConfig, st_config, sizeof(STCONFIG_DEV));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Developer Config <<<< \r\n");
				printDebug(printLevel, "TransmitRetry      : %d \r\n", tmp_DevConfig.transmitRetry);
				printDebug(printLevel, "RetransmitInterval : %d \r\n", tmp_DevConfig.retransmitInterval);
				printDebug(printLevel, "HeartbeatInterval  : %d \r\n", tmp_DevConfig.heartbeatInterval);
				printDebug(printLevel, "HeartbeatTimeout   : %d \r\n", tmp_DevConfig.heartbeatTimeout);
				printDebug(printLevel, "ADCVref            : %f \r\n", tmp_DevConfig.adc_Vref);
				printDebug(printLevel, "DebugLevel         : 0x%02X \r\n", tmp_DevConfig.debug_level);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case PROPERTIES_CONF:
			memcpy(&tmp_ConfigProperties, st_config, sizeof(STCONFIG_PROPERTIES));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Properties <<<< \r\n");
				printDebug(printLevel, "Device Type      : %s \r\n", tmp_ConfigProperties.deviceType);
				printDebug(printLevel, "Firmware Version : %s \r\n", tmp_ConfigProperties.firmwareVer);
				printDebug(printLevel, "Device Name      : %s \r\n", tmp_ConfigProperties.deviceName);
				printDebug(printLevel, "Device ID        : %s \r\n", tmp_ConfigProperties.deviceID);
				printDebug(printLevel, "Host Name        : %s \r\n", tmp_ConfigProperties.hostName);
				printDebug(printLevel, "Host ID          : %s \r\n", tmp_ConfigProperties.hostID);
				printDebug(printLevel, "Timezone         : %d \r\n", tmp_ConfigProperties.timezone);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case OPERATING_CONF:
			memcpy(&tmp_ConfigOperating, st_config, sizeof(STCONFIG_OPERATING));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Operating Config <<<< \r\n");
				printDebug(printLevel, "DeviceEn: 0x%02X  ", tmp_ConfigOperating.deviceEnable);
				printDebug(printLevel, "AlarmEn:  0x%02X \r\n", tmp_ConfigOperating.alarmEnable);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case BUZZER_CONF:
			memcpy(&tmp_ConfigBuzzer, st_config, sizeof(STCONFIG_BUZZER));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Buzzer Config <<<< \r\n");
				printDebug(printLevel, "BuzzerEn: 0x%02X  ", tmp_ConfigBuzzer.buzzerEnable);
				printDebug(printLevel, "NumOfEventSrc: %d \r\n", tmp_ConfigBuzzer.numberOfSrc);
				print_payload(printLevel, tmp_ConfigBuzzer.eventSrc, sizeof(tmp_ConfigBuzzer.eventSrc));
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case WORKING_CONF:
			memcpy(&tmp_ConfigWorking, st_config, sizeof(STCONFIG_WORKING));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Working Config <<<< \r\n");
				printDebug(printLevel, "MinimumBattery: %0.2f  ", tmp_ConfigWorking.minBattery);
				printDebug(printLevel, "RecordMode: 0x%02X  ", tmp_ConfigWorking.recordMode);
				printDebug(printLevel, "RecordInterval: %d  ", tmp_ConfigWorking.transmitInterval);
				printDebug(printLevel, "SamplingInterval: %d  ", tmp_ConfigWorking.samplingInterval);
				printDebug(printLevel, "CommDataLimit: %d \r\n", tmp_ConfigWorking.commDataLimit);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case NETWORK_CONF:
			memcpy(&tmp_ConfigNetwork, st_config, sizeof(STCONFIG_NETWORK));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Network Config <<<< \r\n");
				printDebug(printLevel, "ServerPort: %d  ", tmp_ConfigNetwork.serverPort);
				printDebug(printLevel, "AddrType: 0x%02X  ", tmp_ConfigNetwork.serverAddrType);
				printDebug(printLevel, "AddrLength: %d \r\n", tmp_ConfigNetwork.serverAddrLen);
				tmp_ConfigNetwork.serverAddr[tmp_ConfigNetwork.serverAddrLen] = '\0';
				printDebug(printLevel, "Addr: %s \r\n", tmp_ConfigNetwork.serverAddr);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SEN420_CONF:
			memcpy(&tmp_ConfigAnalogSensor, st_config, sizeof(STCONFIG_ANALOGSENSOR));
			if(ch >= MAX_420SENSOR) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> 4-20mA SensorCH_%d Config <<<< \r\n", ch+1);
				printDebug(printLevel, "Output: 0x%02X ", tmp_ConfigAnalogSensor.sensorOutput);
				printDebug(printLevel, "Type: 0x%02X ", tmp_ConfigAnalogSensor.sensorType);
				printDebug(printLevel, "Unit: 0x%02X ", tmp_ConfigAnalogSensor.dataUnit);
				printDebug(printLevel, "SensorEn: 0x%02X ", tmp_ConfigAnalogSensor.sensorEnable);
				printDebug(printLevel, "NumOfCalibrate: %d \r\n", tmp_ConfigAnalogSensor.numCalibrate);
				printDebug(printLevel, "p1(%0.2f, %0.2f), p2(%0.2f, %0.2f), p3(%0.2f, %0.2f), p4(%0.2f, %0.2f) \r\n", 
																tmp_ConfigAnalogSensor.x1,tmp_ConfigAnalogSensor.y1,
																tmp_ConfigAnalogSensor.x2,tmp_ConfigAnalogSensor.y2,
																tmp_ConfigAnalogSensor.x3,tmp_ConfigAnalogSensor.y3,
																tmp_ConfigAnalogSensor.x4,tmp_ConfigAnalogSensor.y4);
				printDebug(printLevel, "p5(%0.2f, %0.2f), p6(%0.2f, %0.2f), p7(%0.2f, %0.2f), p8(%0.2f, %0.2f) \r\n", 
																tmp_ConfigAnalogSensor.x5,tmp_ConfigAnalogSensor.y5,
																tmp_ConfigAnalogSensor.x6,tmp_ConfigAnalogSensor.y6,
																tmp_ConfigAnalogSensor.x7,tmp_ConfigAnalogSensor.y7,
																tmp_ConfigAnalogSensor.x8,tmp_ConfigAnalogSensor.y8);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENADC_CONF:
			memcpy(&tmp_ConfigAnalogSensor, st_config, sizeof(STCONFIG_ANALOGSENSOR));
			if(ch >= MAX_ADCSENSOR) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> ADCSensorCH_%d Config <<<< \r\n", ch+1);
				printDebug(printLevel, "Output: 0x%02X ", tmp_ConfigAnalogSensor.sensorOutput);
				printDebug(printLevel, "Type: 0x%02X ", tmp_ConfigAnalogSensor.sensorType);
				printDebug(printLevel, "Unit: 0x%02X ", tmp_ConfigAnalogSensor.dataUnit);
				printDebug(printLevel, "SensorEn: 0x%02X ", tmp_ConfigAnalogSensor.sensorEnable);
				printDebug(printLevel, "NumOfCalibrate: %d \r\n", tmp_ConfigAnalogSensor.numCalibrate);
				printDebug(printLevel, "p1(%0.2f, %0.2f), p2(%0.2f, %0.2f), p3(%0.2f, %0.2f), p4(%0.2f, %0.2f) \r\n", 
																tmp_ConfigAnalogSensor.x1,tmp_ConfigAnalogSensor.y1,
																tmp_ConfigAnalogSensor.x2,tmp_ConfigAnalogSensor.y2,
																tmp_ConfigAnalogSensor.x3,tmp_ConfigAnalogSensor.y3,
																tmp_ConfigAnalogSensor.x4,tmp_ConfigAnalogSensor.y4);
				printDebug(printLevel, "p5(%0.2f, %0.2f), p6(%0.2f, %0.2f), p7(%0.2f, %0.2f), p8(%0.2f, %0.2f) \r\n", 
																tmp_ConfigAnalogSensor.x5,tmp_ConfigAnalogSensor.y5,
																tmp_ConfigAnalogSensor.x6,tmp_ConfigAnalogSensor.y6,
																tmp_ConfigAnalogSensor.x7,tmp_ConfigAnalogSensor.y7,
																tmp_ConfigAnalogSensor.x8,tmp_ConfigAnalogSensor.y8);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENMODBUS_CONF:
			memcpy(&tmp_ConfigMODBUSSensor, st_config, sizeof(STCONFIG_MODBUSSENSOR));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> MODBUS FlowSensor Config <<<< \r\n");
				printDebug(printLevel, "SlaveID: 0x%02X  ", tmp_ConfigMODBUSSensor.slave_ID);
				printDebug(printLevel, "ScanRate: %d  ", tmp_ConfigMODBUSSensor.scan_rate);
				printDebug(printLevel, "baudrate: 0x%02X  ", tmp_ConfigMODBUSSensor.baudrate);
				printDebug(printLevel, "SensorEn: 0x%02X \r\n", tmp_ConfigMODBUSSensor.sensor_enable);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENPULSE_CONF:
			memcpy(&tmp_ConfigPulseSensor, st_config, sizeof(STCONFIG_PULSESENSOR));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> PULSE FlowSensor Config <<<< \r\n");
				printDebug(printLevel, "PerPulse: 0x%02X  ", tmp_ConfigPulseSensor.vol_per_pulse);
				printDebug(printLevel, "Total: %d  ", tmp_ConfigPulseSensor.net_total);
				printDebug(printLevel, "SensorEn: 0x%02X \r\n", tmp_ConfigPulseSensor.sensor_enable);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case DIN_CONF:
			memcpy(&tmp_ConfigDIN, st_config, sizeof(STCONFIG_DIGITALsIN));
			if(ch >= MAX_DIGItIN) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> DigitalInputCH_%d Config <<<<\r\n", ch+1);
				printDebug(printLevel, "TiggerAlarmEn: 0x%02X  ", tmp_ConfigDIN.trigAlarmEn);
				printDebug(printLevel, "TriggerType: 0x%02X  ", tmp_ConfigDIN.trigAlarmType);
				printDebug(printLevel, "TriggerDelay: %d  \r\n", tmp_ConfigDIN.trigDelay);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SEN420ALARM_CONF:
			memcpy(&tmp_ConfigSensorAlarm, st_config, sizeof(STCONFIG_SENSORALARM));
			if(ch >= MAX_420SENSOR) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> 4-20mASensorCH_%d AlarmConfig <<<< \r\n", ch+1);
				printDebug(printLevel, "AlarmEn: 0x%02X  ", tmp_ConfigSensorAlarm.alarmEnable);
				printDebug(printLevel, "Lower: %0.2f  ", tmp_ConfigSensorAlarm.lower);
				printDebug(printLevel, "LowerTime: %d  ", tmp_ConfigSensorAlarm.lowerTime);
				printDebug(printLevel, "LowerEx: %0.2f  ", tmp_ConfigSensorAlarm.lowerExtreme);
				printDebug(printLevel, "Upper: %0.2f  ", tmp_ConfigSensorAlarm.upper);
				printDebug(printLevel, "UpperTime: %d ", tmp_ConfigSensorAlarm.upperTime);
				printDebug(printLevel, "UpperEx: %0.2f \r\n", tmp_ConfigSensorAlarm.upperExtreme);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENADCALARM_CONF:
			memcpy(&tmp_ConfigSensorAlarm, st_config, sizeof(STCONFIG_SENSORALARM));
			if(ch >= MAX_ADCSENSOR) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> ADCSensorCH_%d AlarmConfig <<<< \r\n", ch+1);
				printDebug(printLevel, "AlarmEn: 0x%02X  ", tmp_ConfigSensorAlarm.alarmEnable);
				printDebug(printLevel, "Lower: %0.2f  ", tmp_ConfigSensorAlarm.lower);
				printDebug(printLevel, "LowerTime: %d  ", tmp_ConfigSensorAlarm.lowerTime);
				printDebug(printLevel, "LowerEx: %0.2f  ", tmp_ConfigSensorAlarm.lowerExtreme);
				printDebug(printLevel, "Upper: %0.2f  ", tmp_ConfigSensorAlarm.upper);
				printDebug(printLevel, "UpperTime: %d ", tmp_ConfigSensorAlarm.upperTime);
				printDebug(printLevel, "UpperEx: %0.2f \r\n", tmp_ConfigSensorAlarm.upperExtreme);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENMODBUSALARM_CONF:
			memcpy(&tmp_ConfigSensorAlarm, st_config, sizeof(STCONFIG_SENSORALARM));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> MODBUS FlowSensor AlarmConfig <<<< \r\n");
				printDebug(printLevel, "AlarmEn: 0x%02X  ", tmp_ConfigSensorAlarm.alarmEnable);
				printDebug(printLevel, "Lower: %0.2f  ", tmp_ConfigSensorAlarm.lower);
				printDebug(printLevel, "LowerTime: %d  ", tmp_ConfigSensorAlarm.lowerTime);
				printDebug(printLevel, "LowerEx: %0.2f  ", tmp_ConfigSensorAlarm.lowerExtreme);
				printDebug(printLevel, "Upper: %0.2f  ", tmp_ConfigSensorAlarm.upper);
				printDebug(printLevel, "UpperTime: %d  ", tmp_ConfigSensorAlarm.upperTime);
				printDebug(printLevel, "UpperEx: %0.2f \r\n", tmp_ConfigSensorAlarm.upperExtreme);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENPULSEALARM_CONF:
			memcpy(&tmp_ConfigSensorAlarm, st_config, sizeof(STCONFIG_SENSORALARM));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> PULSE FlowSensor Alarm Config <<<< \r\n");
				printDebug(printLevel, "AlarmEn: 0x%02X  ", tmp_ConfigSensorAlarm.alarmEnable);
				printDebug(printLevel, "Lower: %0.2f  ", tmp_ConfigSensorAlarm.lower);
				printDebug(printLevel, "LowerTime: %d  ", tmp_ConfigSensorAlarm.lowerTime);
				printDebug(printLevel, "LowerEx: %0.2f  ", tmp_ConfigSensorAlarm.lowerExtreme);
				printDebug(printLevel, "Upper: %0.2f  ", tmp_ConfigSensorAlarm.upper);
				printDebug(printLevel, "UpperTime: %d  ", tmp_ConfigSensorAlarm.upperTime);
				printDebug(printLevel, "UpperEx: %0.2f \r\n", tmp_ConfigSensorAlarm.upperExtreme);
				printDebug(printLevel, "\r\n");				
				xSemaphoreGive(xSemaphore);
			}
			break;
		case VALVE_CONF:
			memcpy(&tmp_ConfigValve, st_config, sizeof(STCONFIG_VALVE));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> Valve Config <<<< \r\n");
				printDebug(printLevel, "Type: 0x%02X  ", tmp_ConfigValve.interface_type);
				printDebug(printLevel, "SlaveID: %d  ", tmp_ConfigValve.slave_ID);
				printDebug(printLevel, "ScanRate: %d  ", tmp_ConfigValve.scan_rate);
				printDebug(printLevel, "En: 0x%02X \r\n", tmp_ConfigValve.enable);		
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SEN420CAL_CONF:
			memcpy(&tmp_Config420Cal, st_config, sizeof(STCONFIG_420CAL));
			if(ch >= MAX_420SENSOR) {
				printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: IndexCh out of range! \r\n", ch+1);
			}
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> 4-20mA SensorCH_%d Calibrate <<<< \r\n", ch+1);
				printDebug(printLevel, "Type: 0x%02X  ", tmp_Config420Cal.calculation);
				printDebug(printLevel, "CalVal: %0.4f  ", tmp_Config420Cal.cal_val);
				printDebug(printLevel, "RealVal: %0.4f  ", tmp_Config420Cal.real_val);
				printDebug(printLevel, "Const: %0.4f \r\n", tmp_Config420Cal.constant);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case VALVE420INCAL_CONF:
			memcpy(&tmp_Config420Cal, st_config, sizeof(STCONFIG_420CAL));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> 4-20mAInput Valve Calibrate <<<< \r\n");
				printDebug(printLevel, "Type: 0x%02X  ", tmp_Config420Cal.calculation);
				printDebug(printLevel, "CalVal: %0.4f  ", tmp_Config420Cal.cal_val);
				printDebug(printLevel, "RealVal: %0.4f  ", tmp_Config420Cal.real_val);
				printDebug(printLevel, "Const: %0.4f \r\n", tmp_Config420Cal.constant);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case VALVE420OUTCAL_CONF:
			memcpy(&tmp_Config420Cal, st_config, sizeof(STCONFIG_420CAL));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> 4-20mAOutput Valve Calibrate <<<< \r\n");
				printDebug(printLevel, "Type: 0x%02X  ", tmp_Config420Cal.calculation);
				printDebug(printLevel, "CalVal: %0.4f  ", tmp_Config420Cal.cal_val);
				printDebug(printLevel, "Const: %0.4f \r\n", tmp_Config420Cal.constant);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		case SENPULSEADJ_CONF:
			memcpy(&tmp_ConfigPulseADJ, st_config, sizeof(STCONFIG_PULSEADJ));
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(printLevel, ">>>> PULSE FlowSensor Value Adjustment <<<< \r\n");
				printDebug(printLevel, "Multiplier: %0.4f \r\n", tmp_ConfigPulseADJ.multiplier_value);
				printDebug(printLevel, "\r\n");
				xSemaphoreGive(xSemaphore);
			}
			break;
		default:
			printDebug_Semphr(MINOR_ERR, "<MAIN_PrintConfig>: Invalid config id \r\n");
			break;
	}
	
}
/*==========================================================================================*/
/**
  * @brief  This function is show FATFS Error Code.
  * @param  Code id
  * @retval None
  */
void fatfs_err (FRESULT rc) {

  const char *str =
                    "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
                    "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
                    "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
                    "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
  FRESULT i;

  for (i = (FRESULT)0; i != rc && *str; i++) {
    while (*str++) ;
  }
  printDebug(MINOR_ERR, "rc=%u FR_%s\n\r", (UINT)rc, str);
//  STM_EVAL_LEDOn(LED6);
//  while(1);
}
/*==========================================================================================*/
/**
  * @brief  This Function is intial digital input.
  * @param  None
  * @retval None
  */
static void SL_InitialDigitalInput(void) {
	
	uint8_t i;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Set DIN_1 for pulse sensor(interupt) */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Pin = DIN1_PIN;
	GPIO_Init((GPIO_TypeDef *)DIN1_PORT, &GPIO_InitStruct);
	
	/* Connect EXTI Line to DIN1 GPIO Pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);

	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 
	
	/* Set other channel */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	for(i = 1; i < MAX_DIN; i++) {
		GPIO_InitStruct.GPIO_Pin = DINn_PIN[i];
		GPIO_Init((GPIO_TypeDef *)DINn_PORT[i], &GPIO_InitStruct);
	}

}
/*==========================================================================================*/
/**
  * @brief  This Function is intial digital output.
  * @param  None
  * @retval None
  */
static void SL_InitialDigitalOutput(void) {
	
	uint8_t i;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* Digital Output Channel 1-4 */
	for(i = 0; i < MAX_DOUT; i++) {
		GPIO_InitStruct.GPIO_Pin = DOUTn_PIN[i];
		GPIO_Init((GPIO_TypeDef *)DOUTn_PORT[i], &GPIO_InitStruct);
	}
	/* Select RF */
	GPIO_InitStruct.GPIO_Pin = SELECT_RF_PIN;
	GPIO_Init(SELECT_RF_PORT, &GPIO_InitStruct);
	/* SDIO BUS Control */
//	GPIO_InitStruct.GPIO_Pin = SDIO_CTRL_PIN;
//	GPIO_Init(SDIO_CTRL_PORT, &GPIO_InitStruct);
	/* XBee Module Reset */
	GPIO_InitStruct.GPIO_Pin = XBEE_RESRT_PIN;
	GPIO_Init(XBEE_RESRT_PORT, &GPIO_InitStruct);
	/* GSM/GPRS Module ON/OFF */
	GPIO_InitStruct.GPIO_Pin = GSM_SWITCH_PIN;
	GPIO_Init(GSM_SWITCH_PORT, &GPIO_InitStruct);
	/* GSM/GPRS Module Emergency OFF */
	GPIO_InitStruct.GPIO_Pin = GSM_EMER_PIN;
	GPIO_Init(GSM_EMER_PORT, &GPIO_InitStruct);
	/* GSM/GPRS Module Power ON/OFF */
	GPIO_InitStruct.GPIO_Pin = GSM_PWR_PIN;
	GPIO_Init(GSM_PWR_PORT, &GPIO_InitStruct);
	
	/* GSM Signal Status */
	GPIO_InitStruct.GPIO_Pin = LED_CSQ_PIN;
	GPIO_Init(LED_CSQ_PORT, &GPIO_InitStruct);
	/* Connect Server Status */
	GPIO_InitStruct.GPIO_Pin = LED_CONNSERVER_PIN;
	GPIO_Init(LED_CONNSERVER_PORT, &GPIO_InitStruct);
	/* Sampling Data */
	GPIO_InitStruct.GPIO_Pin = LED_SAMP_PIN;
	GPIO_Init(LED_SAMP_PORT, &GPIO_InitStruct);
	/* Sent Data */
	GPIO_InitStruct.GPIO_Pin = LED_SENT_PIN;
	GPIO_Init(LED_SENT_PORT, &GPIO_InitStruct);
	/* SD Card Status */
	GPIO_InitStruct.GPIO_Pin = LED_SD_PIN;
	GPIO_Init(LED_SD_PORT, &GPIO_InitStruct);
	/* Error Indicator */
	GPIO_InitStruct.GPIO_Pin = LED_ERR_PIN;
	GPIO_Init(LED_ERR_PORT, &GPIO_InitStruct);
	
	/* Buzzer */
	GPIO_InitStruct.GPIO_Pin = BUZZER_PIN;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
	
	// Select RF
	GPIO_SetBits(SELECT_RF_PORT, SELECT_RF_PIN);
	// Set default
	LED_CSQ_RED;
	LED_CONNSERVER_RED;
	LED_SAMP_OFF;
	LED_SENT_OFF;
	LED_SD_RED;
	LED_ERR_RED;
	
}
/*==========================================================================================*/
/**
  * @brief  This Function is set default value to Struct_Config. 
  * @param  None
  * @retval None
  */
uint8_t ucMAIN_SetDefaultInformation(void) {
	
	/*-- Information --*/
	memcpy(struct_info.user, "Default_User", 12);
	memcpy(struct_info.password, "Default_Password", 16);
	struct_info.serialNumber[0] = 0x99;
	struct_info.serialNumber[1] = 0x99;
	struct_info.serialNumber[2] = 0x99;
	struct_info.serialNumber[3] = 0x99;
	struct_info.serialNumber[4] = 0x99;
	struct_info.serialNumber[5] = 0x99;
	struct_info.serialNumber[6] = 0x99;
	struct_info.serialNumber[7] = 0x99;
	struct_info.destSerial[0] 	= 0xAA;
	struct_info.destSerial[1] 	= 0xAA;
	struct_info.destSerial[2] 	= 0xAA;
	struct_info.destSerial[3] 	= 0xAA;
	struct_info.destSerial[4] 	= 0xAA;
	struct_info.destSerial[5] 	= 0xAA;
	struct_info.destSerial[6] 	= 0xAA;
	struct_info.destSerial[7] 	= 0xAA;
	struct_info.flagRegistered  = 0x00;
	if(EEPROM_WriteBuf(INFO_BASEADDR, &struct_info, sizeof(ST_INFO))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Information Error \r\n");
		return 1;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This Function is set default value to Struct_Config. 
  * @param  None
  * @retval None
  */
uint8_t ucMAIN_SetDefaultStatus(void) {
	
	/*-- Status --*/
	struct_Status.dout_1 				= 0;
	struct_Status.dout_2 				= 0;
	struct_Status.dout_3 				= 0;
	struct_Status.dout_4 				= 0;
	struct_Status.valve_open 		= 0;
	struct_Status.net_flow 			= 0.0;
	if(EEPROM_WriteBuf(STATUS_BASEADDR, &struct_Status, sizeof(ST_STATUS))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Status Error \r\n");
		return 1;
	}
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  This Function is set default value to Struct_Config. 
  * @param  None
  * @retval None
  */
uint8_t ucMAIN_SetDefaultConfig(void) {
	
	uint8_t 	i;
	uint8_t		res = 0;
	
	/*-- Dev Config --*/
	struct_DevConfig.debug_level						= 0x02;
	struct_DevConfig.transmitRetry				 	= 1;
	struct_DevConfig.retransmitInterval 		= 15;
	struct_DevConfig.heartbeatInterval			= 60;
	struct_DevConfig.heartbeatTimeout				= 1;
	struct_DevConfig.adc_Vref								= 3.3;
	if(EEPROM_WriteBuf(DEVeCONF_BASEADDR, &struct_DevConfig, sizeof(STCONFIG_DEV))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Deverloper_Configuration Error \r\n");
		res |= 1;
	}
	/*-- Properties Config --*/
	memcpy(struct_ConfigProperties.deviceType, "Data Logger", 11);
	memcpy(struct_ConfigProperties.firmwareVer, "iNT-SL-Ver0.10", 14);
	memcpy(struct_ConfigProperties.deviceName, "SmartLogger_00", 14);
	memcpy(struct_ConfigProperties.deviceID, "99999999", 8);
	memcpy(struct_ConfigProperties.hostName, "HostName_00", 11);
	memcpy(struct_ConfigProperties.hostID, "AAAAAAAA", 8);
	struct_ConfigProperties.timezone				= 7;
	if(EEPROM_WriteBuf(PROPERTIES_BASEADDR, &struct_ConfigProperties, sizeof(STCONFIG_PROPERTIES))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Properties_Configuration Error \r\n");
		res |= 1;
	}
	/*-- Operating Config --*/
	struct_ConfigOperating.deviceEnable			= 0x01;
	struct_ConfigOperating.alarmEnable			= 0x01;
	if(EEPROM_WriteBuf(OPERATING_BASEADDR, &struct_ConfigOperating, sizeof(STCONFIG_OPERATING))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Operating_Configuration Error \r\n");
		res |= 1;
	}
	/*-- Buzzer Config --*/
	struct_ConfigBuzzer.buzzerEnable				= 0x00;
	struct_ConfigBuzzer.numberOfSrc					= 0;
	if(EEPROM_WriteBuf(BUZZER_BASEADDR, &struct_ConfigBuzzer, sizeof(STCONFIG_BUZZER))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Buzzer_Configuration Error \r\n");
		res |= 1;
	}
//	struct_ConfigBuzzer.eventSrc[0]					= 0xA1;
	/*-- Working Management Config --*/
	struct_ConfigWorking.minBattery					= 11.0;
	struct_ConfigWorking.recordMode					= ROLLOVER_MODE;
	struct_ConfigWorking.transmitInterval		= 60;
	struct_ConfigWorking.samplingInterval		= 60;
	struct_ConfigWorking.commDataLimit			=	100000000;
	if(EEPROM_WriteBuf(WORKING_BASEADDR, &struct_ConfigWorking, sizeof(STCONFIG_WORKING))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Working_Configuration Error \r\n");
		res |= 1;
	}
	/*-- Network Config --*/
	struct_ConfigNetwork.serverPort					= 0;
	struct_ConfigNetwork.serverAddrType			= IP_ADDR;
	struct_ConfigNetwork.serverAddrLen			= 13;
	memcpy(struct_ConfigNetwork.serverAddr, "164.115.25.95", 13);
	if(EEPROM_WriteBuf(NETWORK_BASEADDR, &struct_ConfigNetwork, sizeof(STCONFIG_NETWORK))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Network_Configuration Error \r\n");
		res |= 1;
	}
	/*-- 4-20mA Sensor Config --*/
	for(i = 0; i < MAX_420SENSOR; i++) {
		struct_Config420Sensor[i].sensorOutput	= OUT_420MA;
		struct_Config420Sensor[i].sensorType 		= TYP_OTHER;
		struct_Config420Sensor[i].dataUnit 			= UNIT_MA;
		struct_Config420Sensor[i].sensorEnable 	= SENSOR_DISABLE;
		struct_Config420Sensor[i].numCalibrate	= 0;
		struct_Config420Sensor[i].x1						= 0.0;								
	 	struct_Config420Sensor[i].y1						= 0.0;
		struct_Config420Sensor[i].x2 						= 0.0;
		struct_Config420Sensor[i].y2   					= 0.0;
		struct_Config420Sensor[i].x3						= 0.0;								
	 	struct_Config420Sensor[i].y3						= 0.0;
		struct_Config420Sensor[i].x4 						= 0.0;
		struct_Config420Sensor[i].y4   					= 0.0;
		struct_Config420Sensor[i].x5 						= 0.0;
		struct_Config420Sensor[i].y5   					= 0.0;
		struct_Config420Sensor[i].x6 						= 0.0;
		struct_Config420Sensor[i].y6   					= 0.0;
		struct_Config420Sensor[i].x7 						= 0.0;
		struct_Config420Sensor[i].y7   					= 0.0;
		struct_Config420Sensor[i].x8 						= 0.0;
		struct_Config420Sensor[i].y8   					= 0.0;
		if(EEPROM_WriteBuf(SEN420CONF_BASEADDR[i], &struct_Config420Sensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write 4-20mASensor_Configuration CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- ADC Sensor Config --*/
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		struct_ConfigADCSensor[i].sensorOutput	= OUT_0to5V;
		struct_ConfigADCSensor[i].sensorType 		= TYP_OTHER;
		struct_ConfigADCSensor[i].dataUnit 			= UNIT_ADC;
		struct_ConfigADCSensor[i].sensorEnable 	= SENSOR_DISABLE;
		struct_ConfigADCSensor[i].numCalibrate	= 0;
		struct_ConfigADCSensor[i].x1						= 0.0;								
	 	struct_ConfigADCSensor[i].y1						= 0.0;
		struct_ConfigADCSensor[i].x2 						= 0.0;
		struct_ConfigADCSensor[i].y2   					= 0.0;
		struct_ConfigADCSensor[i].x3						= 0.0;								
	 	struct_ConfigADCSensor[i].y3						= 0.0;
		struct_ConfigADCSensor[i].x4 						= 0.0;
		struct_ConfigADCSensor[i].y4   					= 0.0;
		struct_ConfigADCSensor[i].x5 						= 0.0;
		struct_ConfigADCSensor[i].y5   					= 0.0;
		struct_ConfigADCSensor[i].x6 						= 0.0;
		struct_ConfigADCSensor[i].y6   					= 0.0;
		struct_ConfigADCSensor[i].x7 						= 0.0;
		struct_ConfigADCSensor[i].y7   					= 0.0;
		struct_ConfigADCSensor[i].x8 						= 0.0;
		struct_ConfigADCSensor[i].y8   					= 0.0;
		if(EEPROM_WriteBuf(SENADCCONF_BASEADDR[i], &struct_ConfigADCSensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write ADCSensor_Configuration CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- MODBUS FlowSensor Config --*/
	struct_ConfigMODBUSSensor.slave_ID					= 1;
	struct_ConfigMODBUSSensor.scan_rate					= 60;
	struct_ConfigMODBUSSensor.baudrate					= 0x01;
	struct_ConfigMODBUSSensor.sensor_enable			= SENSOR_DISABLE;
	if(EEPROM_WriteBuf(SENsMODBUSCONF_BASEADDR, &struct_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write MODBUSFlowSensor_Configuration Error \r\n");
		res |= 1;
	}
	/*-- PULSE FlowSensor Config --*/
	struct_ConfigPulseSensor.vol_per_pulse			= 0x01;
	struct_ConfigPulseSensor.net_total					= 0;
	struct_ConfigPulseSensor.sensor_enable			= SENSOR_DISABLE;
	if(EEPROM_WriteBuf(SENsPULCONF_BASEADDR, &struct_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write PULSEFlowSensor_Configuration Error \r\n");
		res |= 1;
	}
	
	/*-- Digital Input Config --*/
	for(i = 0; i < MAX_DIGItIN; i++) {
		struct_ConfigDIN[i].trigAlarmEn 		= 0x00;
		struct_ConfigDIN[i].trigAlarmType 	= 0x02;
		struct_ConfigDIN[i].trigDelay 			= 10;
		if(EEPROM_WriteBuf(DIGItIN_BASEADDR[i], &struct_ConfigDIN[i], sizeof(STCONFIG_DIGITALsIN))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write DIN_Configuration CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- 4-20mA Sensor Alarm --*/
	for(i = 0; i < MAX_420SENSOR; i++) {
		struct_Config420SensorAlarm[i].alarmEnable 				= SENSOR_DISABLE;
		struct_Config420SensorAlarm[i].lower							= 0.0;
		struct_Config420SensorAlarm[i].lowerTime					= 10;
		struct_Config420SensorAlarm[i].lowerExtreme				= 0.0;
		struct_Config420SensorAlarm[i].upper							= 9999.0;
		struct_Config420SensorAlarm[i].upperTime					= 10;
		struct_Config420SensorAlarm[i].upperExtreme				= 9999.0;
		if(EEPROM_WriteBuf(SEN420ALARMn_BASEADDR[i], &struct_Config420SensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write 4-20mASensorAlarm_Configuration CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- ADC Sensor Alarm --*/
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		struct_ConfigADCSensorAlarm[i].alarmEnable 				= SENSOR_DISABLE;
		struct_ConfigADCSensorAlarm[i].lower							= 0.0;
		struct_ConfigADCSensorAlarm[i].lowerTime					= 10;
		struct_ConfigADCSensorAlarm[i].lowerExtreme				= 0.0;
		struct_ConfigADCSensorAlarm[i].upper							= 9999.0;
		struct_ConfigADCSensorAlarm[i].upperTime					= 10;
		struct_ConfigADCSensorAlarm[i].upperExtreme				= 9999.0;
		if(EEPROM_WriteBuf(SENADCALARMn_BASEADDR[i], &struct_ConfigADCSensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write ADCSensorAlarm_Configuration CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- MODBUS FlowSensor Alarm --*/
	struct_ConfigMODBUSSensorAlarm.alarmEnable 					= SENSOR_DISABLE;
	struct_ConfigMODBUSSensorAlarm.lower								= 1.0;
	struct_ConfigMODBUSSensorAlarm.lowerTime						= 20;
	struct_ConfigMODBUSSensorAlarm.lowerExtreme					= 0.0;
	struct_ConfigMODBUSSensorAlarm.upper								= 9999.0;
	struct_ConfigMODBUSSensorAlarm.upperTime						= 20;
	struct_ConfigMODBUSSensorAlarm.upperExtreme					= 9999.0;
	if(EEPROM_WriteBuf(SENsMODBUSALARM_BASEADDR, &struct_ConfigMODBUSSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write MODBUSFlowSensorAlarm_Configuration Error \r\n");
		res |= 1;
	}
	/*-- PULSE FlowSensor Alarm --*/
	struct_ConfigPulseSensorAlarm.alarmEnable 					= SENSOR_DISABLE;
	struct_ConfigPulseSensorAlarm.lower									= 0.0;
	struct_ConfigPulseSensorAlarm.lowerTime							= 10;
	struct_ConfigPulseSensorAlarm.lowerExtreme					= 0.0;
	struct_ConfigPulseSensorAlarm.upper									= 9999.0;
	struct_ConfigPulseSensorAlarm.upperTime							= 10;
	struct_ConfigPulseSensorAlarm.upperExtreme					= 9999.0;
	if(EEPROM_WriteBuf(SENsPULALARM_BASEADDR, &struct_ConfigPulseSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write FlowFlowSensorAlarm_Configuration Error \r\n");
		res |= 1;
	}
	/*-- Butterfly Valve --*/
	struct_ConfigValve.interface_type										= VALVE_420MA;
	struct_ConfigValve.slave_ID													= 2;
	struct_ConfigValve.scan_rate												= 60;
	struct_ConfigValve.enable														= CONF_DISABLE;
	if(EEPROM_WriteBuf(VALVECONF_BASEADDR, &struct_ConfigValve, sizeof(STCONFIG_VALVE))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Valve_Configuration Error \r\n");
		res |= 1;
	}
	/*-- 4-20mA Sensor calibration --*/
	for(i = 0; i < MAX_420SENSOR; i++) {
		struct_Config420Cal[i].calculation								= CAL_USE_DEFAULT;
		struct_Config420Cal[i].cal_val										= 0.0;
		struct_Config420Cal[i].constant										= 0.0;
		if(EEPROM_WriteBuf(SEN420CHnCAL_BASEADDR[i], &struct_Config420Cal[i], sizeof(STCONFIG_420CAL))) {
			printDebug(MAJOR_ERR, "[EEPROM]: Write 420mASensor_Calibrate CH_%d Error \r\n", i+1);
			res |= 1;
		}
	}
	/*-- 4-20mA input valve calibration --*/
	struct_Config420ValveInCal.calculation							= CAL_USE_DEFAULT;
	struct_Config420ValveInCal.cal_val									= 0.0;
	struct_Config420ValveInCal.constant									= 0.0;
	if(EEPROM_WriteBuf(VALVE420INCAL_BASEADDR, &struct_Config420ValveInCal, sizeof(STCONFIG_420CAL))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write In420mAValve_Calibrate Error \r\n");
		res |= 1;
	}
	/*-- 4-20mA output valve calibration --*/
	struct_Config420ValveOutCal.calculation							= CAL_USE_DEFAULT;
	struct_Config420ValveOutCal.cal_val									= 0.0;
	struct_Config420ValveOutCal.constant								= 0.0;
	if(EEPROM_WriteBuf(VALVE420OUTCAL_BASEADDR, &struct_Config420ValveOutCal, sizeof(STCONFIG_420CAL))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write Out420mAValve_Calibrate Error \r\n");
		res |= 1;
	}
	/*-- PULSE Flow Sensor Value Adjustment --*/
	struct_ConfigPulseADJ.multiplier_value 			= 1.0;
	if(EEPROM_WriteBuf(SENPULSEADJ_BASEADDR, &struct_ConfigPulseADJ, sizeof(STCONFIG_PULSEADJ))) {
		printDebug(MAJOR_ERR, "[EEPROM]: Write PULSEFlowValue_Adjustment Error \r\n");
		res |= 1;
	}
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  vApplicationMallocFailedHook.
  * @param  None
  * @retval None
  */
void vApplicationMallocFailedHook( void ) {
	printDebug(MINOR_ERR, "<vApplicationMallocFailedHook> !!! \r\n");
	while(1);
}
/*==========================================================================================*/
/**
  * @brief  vApplicationStackOverflowHook.
  * @param  
  * @retval None
  */
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ) {
	BUZZER_ON;
	printDebug(MINOR_ERR, "<vApplicationStackOverflowHook> %s !!! \r\n", pcTaskName);
	while(1);
}
/*==========================================================================================*/
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

  return ch;
}
/*==========================================================================================*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	printDebug(MINOR_ERR, "<assert_failed> !!!!! assert_failed !!!!! \r\n");
  /* Infinite loop */
  while (1)
  {}
}
#endif
/*==========================================================================================*/

/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*******************END OF FILE****/

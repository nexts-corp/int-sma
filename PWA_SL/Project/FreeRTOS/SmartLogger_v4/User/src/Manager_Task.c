/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Manager_Task.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-February-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>
#include "main.h"
#include "semphr.h"
#include "Manager_Task.h"
#include "Sensors_Task.h"
#include "SD_Task.h"
#include "stm32f4_SDCard.h"
#include "SL_PinDefine.h"
#include "SL_ConfigStruct.h"
#include "INT_STM32F4_PacketQueue.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_DataQueue.h"
#include "INT_STM32F4_rtc.h"
#include "INT_STM32F4_csvfile.h"
#include "INT_STM32F4_adc.h"
#include "INT_STM32F4_420RClick.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_log.h"
#include "INT_STM32F4_watchdog.h"
#include "INT_STM32F4_OneWire.h"
#include "tm_stm32f4_adc.h"
#include "INT_STM32F4_24LC512.h"
#include "INT_STM32F4_ConfigSD.h"
#include "INT_STM32F4_ProtocolHandle.h"
#include "tm_stm32f4_ds1307.h"

float fMNG_ReadBatt(void);
//static void vMNG_SendConfigLog(void);
//static void vMNG_SendEventLog(void);
//static void vMNG_SendDataLog(void);
static PTC_RES xMNG_SendRegis(void);
static PTC_RES xMNG_SendJoin(void);
static void vMNG_TransmitEvent(void);
static void vMNG_EventCapture(void);
static void vMNG_UpdateConfigCapture(void);
static void vMNG_TransmitConfig(void);
bool bMNG_CheckBatteryAlarm(uint8_t *eventcode);
bool bMNG_CheckMemoryStatusAlarm(uint8_t *eventcode);
bool bMNG_CheckSens420StatusAlarm(uint8_t index_ch, uint8_t *eventcode);
bool bMNG_CheckSens420DataAlarm(uint8_t index_ch, uint8_t *eventcode);
bool bMNG_CheckSensADCDataAlarm(uint8_t index_ch, uint8_t *eventcode);
bool bMNG_CheckSensPULSEDataAlarm(uint8_t *eventcode);
bool bMNG_CheckDINStatusAlarm(uint8_t index_ch, uint8_t *eventcode);

static bool bMNG_CheckAlarm(uint8_t *srcList, uint8_t srcLen);
static bool bMNG_CheckAlarmFromSrc(uint8_t alarmSrc);
extern float fDATBUF_ReadBoardTemp(void);

extern xQueueHandle 				Queue_PacketFromGSM;
extern xQueueHandle 				Queue_CMDToGSM;
extern STCONFIG_ANALOGSENSOR 	struct_Config420Sensor[MAX_420SENSOR];

/* Global Variable */
extern __IO uint32_t 	_SDWDGCounter;
__IO uint8_t 		_heartbeatCounter 			= 0;
__IO uint8_t		_din_status[MAX_DIN] 		= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // For check alarm source
__IO uint8_t		_SendRegisCounter				= 0;
__IO uint8_t		_SendJoinCounter				= 0;
uint32_t				_RegisPendingTimeout		=	0;
uint32_t				_JoinPendingTimeout			=	0;
extern __IO float			_BatteryRemain;
extern __IO uint8_t		_BatteryStatus;
extern __IO uint8_t		_BatteryStat_old;
extern uint8_t				_OperatingMode;
extern __IO bool			flag_HaveMemEvent;
extern __IO bool			flag_isSDInstall;
extern __IO uint8_t		_PulseData_Counter;
extern __IO uint8_t		_LED_SAMP_ON;
extern __IO uint8_t		_LED_SENT_ON;
extern __IO uint8_t		_LED_SD_ON;
extern __IO uint8_t		_LED_ERR_ON;

extern bool 	flag_isConfPropUpdate;
extern bool 	flag_isConfOperUpdate;
extern bool 	flag_isConfBuzzUpdate;
extern bool 	flag_isConfWorkUpdate;
extern bool 	flag_isConfNetwUpdate;
extern bool 	flag_isConf420SensUpdate[];
extern bool 	flag_isConfADCSensUpdate[];
extern bool 	flag_isConfMODBUSSensUpdate;
extern bool 	flag_isConfPulsSensUpdate;
extern bool 	flag_isConf420AlamUpdate[];
extern bool 	flag_isConfADCAlamUpdate[];
extern bool 	flag_isConfMODBUSAlamUpdate;
extern bool 	flag_isConfPulsAlamUpdate;
extern bool 	flag_isConfDINUpdate[];
extern bool 	flag_isConfValveUpdate;
extern __IO uint8_t			_MemoryStatus;
extern __IO uint32_t		_GSMCounter;
extern __IO uint8_t			_ConfigCmdACK;
extern __IO uint8_t			_ConfigCmdFID;
extern SENSOR_DATA 			_Sensor_Data;

/*==========================================================================================*/
/**
  * @brief  Manager Task
  * @param  pvParameters not used
  * @retval None
  */
void vManager_Task(void *pvParameters) {
	
	/* Structure Declaration ---------------------------------------------------*/
	RTC_TimeTypeDef 	RTC_TimeStructure;
	RTC_DateTypeDef 	RTC_DateStructure;
	TM_DS1307_Time_t	time;
	PACKET_DATA				receivePacket;
	
	/* Variable Declaration ----------------------------------------------------*/
	uint8_t			tmp_batteryStat		= EV_NORMAL;
	bool				switchMode_flag		= false;
	bool				sd_wdg						= false;
	bool				checkBattUpper		= false;
	bool				checkBattLower		= false;
	bool				flag_isSetTimeLedSamp = false;
	bool				flag_isSetTimeLedSent = false;
	bool				flag_isSetTimeLedSD 	= false;
	bool				flag_isSetTimeLedERR 	= false;
	
	/*-- Timer Variable --*/
	uint32_t		showDateTime 			= 0;
	uint32_t		sendJoinTimer 		= 0;
	uint32_t		heartbeatTimer 		= 0;
	uint32_t		cmd2GSMTimer 			= 0;
	uint32_t		saveNetFlowTimer	= 0;
	uint32_t 		ledSampTimer 			= 0;
	uint32_t 		ledSentTimer 			= 0;
	uint32_t 		ledSDTimer	 			= 0;
	uint32_t 		ledERRTimer	 			= 0;
	uint32_t		readBattTimer			= 0;
	
	uint32_t		switchModeTimer		= 0;
	uint32_t		testTimer					=	0;

	float		temper = 0.0;
	uint16_t	te = 0;
	
	uint32_t 		timestamp;
	uint8_t			res;
	PTC_RES 		ptcRes;
	
	/*-- Variable about File --*/
	uint8_t			cmd_GSM = 0;
	
	/* Initial packet queue ----------------------------------------------------*/
	vPKTQUEUE_Initial(packet_Queue);
	
	/* Initial Independent Watchdog --------------------------------------------*/
 	vWDG_IntialIWDG();
	
	printDebug_Semphr(DESCRIBE, "[MNG_TASK]: Running... \r\n");	
	
	/* Set Timer */
	ucTIMER_SetTimer(&cmd2GSMTimer, 5);
	ucTIMER_SetTimer(&showDateTime, 300);
	ucTIMER_SetTimer(&saveNetFlowTimer, 300);
	
	/* Device Status (DOUT, Valve) ---------------------------------------------*/
	GPIO_WriteBit((GPIO_TypeDef*)DOUT1_PORT, DOUT1_PIN, struct_Status.dout_1);
	GPIO_WriteBit((GPIO_TypeDef*)DOUT2_PORT, DOUT2_PIN, struct_Status.dout_2);
	GPIO_WriteBit((GPIO_TypeDef*)DOUT3_PORT, DOUT3_PIN, struct_Status.dout_3);
	GPIO_WriteBit((GPIO_TypeDef*)DOUT4_PORT, DOUT4_PIN, struct_Status.dout_4);
	if(!xQueueSend(Queue_CtrlValve, &struct_Status.valve_open, 0)) {
		printDebug_Semphr(WARNING, "[MNG_TASK]: Send Queue_CtrlValve failed. \r\n");
	}
	
	_SDWDGCounter = 0;
	
	/* Main Loop ---------------------------------------------------------------*/
	while(1) {

		/* Reload IWDG counter -----------------------------------------------*/
		IWDG_ReloadCounter();
		
		/* SD_Task WDG -------------------------------------------------------*/
		if(xQueueReceive(Queue_SDTaskWDG, &sd_wdg, 0)) {
			_SDWDGCounter = 0;
		}else {
			if(_SDWDGCounter > 10) {
				printDebug_Semphr(DESCRIBE, "[MNG_TASK]: SD_Task is hang!!  Watchdog Resetting... \r\n");
				if(struct_ConfigPulseSensor.sensor_enable == SENSOR_ENABLE) {
					printDebug_Semphr(DESCRIBE, "[MNG_TASK]: BackUp FlowAcc(%0.4f) \r\n", _Sensor_Data.dataPULSENet);
					if(EEPROM_WriteBuf(NETFLOW_BASEADDR, &_Sensor_Data.dataPULSENet, sizeof(float))) {
						printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: EEPROM_Write Save Net Flow Error \r\n");
					}
				}
				_SDWDGCounter = 0;
				NVIC_SystemReset();
			}
		}
		
		/* Send Configuration CNFG_CMD ACK -------------------------------------*/
		if(_ConfigCmdACK == CONF_ACK_OK) {
			printDebug_Semphr(INFO, "[MNG_TASK]: Send Conf ACK OK \r\n");
			ptcRes = xPROCOMM_SendConfigManageACK(_ConfigCmdFID, CONF_ACCD_WRITE, CONF_CNFG_CMD, STAT_SUCCESS, NULL, 0, DEBUG_PORT);
			if(ptcRes != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptcRes);
			}
			_ConfigCmdACK = CONF_ACK_NONE;
		}else if(_ConfigCmdACK == CONF_ACK_ERR) {
			printDebug_Semphr(INFO, "[MNG_TASK]: Send Conf ACK ERR \r\n");
			ptcRes = xPROCOMM_SendConfigManageACK(_ConfigCmdFID, CONF_ACCD_WRITE, CONF_CNFG_CMD, STAT_ERR, NULL, 0, DEBUG_PORT);
			if(ptcRes != PTC_OK) {
				printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: CONFIGURATION_MANAGEMENT ACK Failed Err(PTC_%d) \r\n", ptcRes);
			}
			_ConfigCmdACK = CONF_ACK_NONE;
		}
			
		/* Receive Data Packet From GSM --------------------------------------*/
		if(xQueueReceive(Queue_PacketFromGSM, &receivePacket, 0)) {			
			xPROCOMM_ProcessPacket(receivePacket.packet, receivePacket.length, GSM_PORT);
		}
		
		/* Receive Data Packet From DebugPort --------------------------------*/
 		ptcRes = ucPROCOMM_ReceivePacket();
 		if((ptcRes != PTC_OK) && (ptcRes != PTC_PKT_EMPTY)) {
 			printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Receive Packet Error (%d) \r\n", ptcRes);
 		}
		
		if(_OperatingMode == MODE_FULL) {
			/* Device Join & Maintenance Network -------------------------------*/
			if(_Communication_Status & COMMU_CONNECTED) {
				/*-- Join Network --*/
				if((!(_Communication_Status & COMMU_JOINED)) && (!(_Communication_Status & COMMU_JOIN_PENDING))) {
					if(cTIMER_CheckTimerExceed(sendJoinTimer)) {
						// Re-Connection, If no Join Ack from server
						if(++_SendJoinCounter <= 2) {
							ptcRes = xMNG_SendJoin();
							if(ptcRes != PTC_OK) {
								printDebug_Semphr(WARNING, "[MNG_TASK]: Send Join_Network_CMD Error (%d) \r\n", ptcRes);
							}
						}else {
							printDebug_Semphr(WARNING, "[MNG_TASK]: Communication Lost, Re-Connection \r\n");
							cmd_GSM = GSM_CMD_RECONNECT;
							res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
							if(res != pdTRUE) {
								printDebug_Semphr(WARNING, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
							}
							_SendJoinCounter = 0;
						}
						LED_CONNSERVER_RED;    // Update CONNSERVER LED Indicator
						ucTIMER_SetTimer(&sendJoinTimer, 60);
					}
				}else if(_Communication_Status & COMMU_JOIN_PENDING) {
					if(cTIMER_CheckTimerExceed(_JoinPendingTimeout)) {
						_Communication_Status &= ~COMMU_JOIN_PENDING;
						printDebug_Semphr(WARNING, "[MNG_TASK]: Join pending timeout, Send new join \r\n");
					}
				}
				
				/*-- Keep Conenction(Heartbeat) --*/
				if(_Communication_Status & COMMU_JOINED) {
					if(cTIMER_CheckTimerExceed(heartbeatTimer)) {
						// Re-Connection, If no Heartbeat Ack from server
						if(++_heartbeatCounter <= (struct_DevConfig.heartbeatTimeout)) {
							ptcRes = xPROCOMM_SendNetworkMaintCMD(HEARTBEAT, NULL, 0, 0);
							if(ptcRes != PTC_OK) {
								printDebug_Semphr(WARNING, "[MNG_TASK]: Send Heartbeat Error (%d) \r\n", ptcRes);
							}
						}else {
							printDebug_Semphr(WARNING, "[MNG_TASK]: Communication Lost, Re-Connection \r\n");
							_Communication_Status &= ~COMMU_JOINED;
							cmd_GSM = GSM_CMD_RECONNECT;
							res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
							if(res != pdTRUE) {
								printDebug_Semphr(WARNING, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
							}
							_heartbeatCounter = 0;
							LED_CONNSERVER_RED;    // Update CONNSERVER LED Indicator
						}
						ucTIMER_SetTimer(&heartbeatTimer, struct_DevConfig.heartbeatInterval);
					}
				}
			}else {
				if(cTIMER_CheckTimerExceed(cmd2GSMTimer)) {
					printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: No Connection, Connecting... \r\n");
					_Communication_Status &= ~COMMU_JOIN_PENDING;
					_Communication_Status &= ~COMMU_JOINED;
					cmd_GSM = GSM_CMD_CONNECT;
					res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
					if(res != pdTRUE) {
						printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
					}
					LED_CONNSERVER_RED;    // Update CONNSERVER LED Indicator
					ucTIMER_SetTimer(&cmd2GSMTimer, 60);
				}
			}
		}else {
			_Communication_Status &= ~COMMU_JOIN_PENDING;
			_Communication_Status &= ~COMMU_JOINED;
			LED_CONNSERVER_RED;				// Update CONNSERVER LED Indicator
		}
		
		/* Device Enable -----------------------------------------------------*/
		if(struct_ConfigOperating.deviceEnable) {
			
//			if(cTIMER_CheckTimerExceed(testTimer)) {
//				printDebug_Semphr(INFO, "[MNG_TASK]: 4-20mA CH_1 %0.4f, 4-20mA CH_2 %0.4f \r\n", _Sensor_Data.data420mA[0], _Sensor_Data.data420mA[1]);
//				printDebug_Semphr(INFO, "[MNG_TASK]: Batt %d, Solar %d \r\n", TM_ADC_Read(ADC1, AIN1_BATT_CH), TM_ADC_Read(ADC1, AIN2_SOLARCELL_CH));
//				ucTIMER_SetTimer(&testTimer, 3);
//			}
			
			// Check for reset GSM
			if((_GSMCounter > 600) && (_OperatingMode == MODE_FULL)) {
				_GSMCounter = 0;
				_Communication_Status &= ~COMMU_JOIN_PENDING;
				_Communication_Status &= ~COMMU_JOINED;
				printDebug_Semphr(MAJOR_ERR, "[MNG_TASK]: Resetting GSM Module.. \r\n");
				
				cmd_GSM = GSM_CMD_HWRESET;
				res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
				if(res != pdTRUE) {
					printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
				}
				
//				GPIO_SetBits((GPIO_TypeDef*)DOUT4_PORT, DOUT4_PIN);
//				delay_ms(2000);
//				GPIO_ResetBits((GPIO_TypeDef*)DOUT4_PORT, DOUT4_PIN);
			}
			
			/* Back up value of Water Accumulate --------------------------------------*/
			if(struct_ConfigPulseSensor.sensor_enable == SENSOR_ENABLE) {
				if(cTIMER_CheckTimerExceed(saveNetFlowTimer)) {
					printDebug_Semphr(DESCRIBE, "[MNG_TASK]: BackUp FlowAcc(%0.4f) \r\n", _Sensor_Data.dataPULSENet);
					if(EEPROM_WriteBuf(NETFLOW_BASEADDR, &_Sensor_Data.dataPULSENet, sizeof(float))) {
						printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: EEPROM_Write Save Net Flow Error \r\n");
						_LED_ERR_ON = LED_ERR_TIME;
					}
					ucTIMER_SetTimer(&saveNetFlowTimer, 1800);
				}
			}
			
			/* Read battery remaining -------------------------------------------------*/
			if(cTIMER_CheckTimerExceed(readBattTimer)) {
				/*-- Read Batt --*/
				_BatteryRemain = fMNG_ReadBatt(); 
//  				printDebug_Semphr(DESCRIBE, "[MNG_TASK]: _BatteryRemain(%0.4f) \r\n", _BatteryRemain);
				/*-- Lower --*/
				if(_BatteryRemain <= struct_ConfigWorking.minBattery) {
					if(checkBattLower) {
						_BatteryStatus = EV_BATT_LOW;
					}else {
						checkBattLower = true;
					}
				}else {
					checkBattLower = false;
				}
				/*-- Upper --*/
				if(_BatteryRemain > struct_ConfigWorking.minBattery) {
					if(checkBattUpper) {
						_BatteryStatus = EV_NORMAL;
					}else {
						checkBattUpper = true;
					}
				}else {
					checkBattUpper = false;
				}
				ucTIMER_SetTimer(&readBattTimer, 5);
			}
			
			/* Check battery status for update operating mode --------------------*/
			if(_BatteryStatus != tmp_batteryStat) {
				tmp_batteryStat = _BatteryStatus;
				switchMode_flag = true;
				ucTIMER_SetTimer(&switchModeTimer, 60);    // Countdown 3 min to switch operating mode
			}
			if(switchMode_flag) {
				if(cTIMER_CheckTimerExceed(switchModeTimer)) {
					switchMode_flag = false;
					if(_BatteryStatus == EV_BATT_LOW) {
						_OperatingMode = MODE_SAVING;
						/* Send CMD to GSM */
						cmd_GSM = GSM_CMD_HWPWROFF;
						res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
						if(res != pdTRUE) {
							printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
						}
						printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Enter to saving mode.. \r\n");
					}else {
						_GSMCounter = 0;
						_OperatingMode = MODE_FULL;
						/* Send CMD to GSM */
						cmd_GSM = GSM_CMD_HWPWRON;
						res = xQueueSend(Queue_CMDToGSM, &cmd_GSM, 0);
						if(res != pdTRUE) {
							printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Send Queue_CMDToGSM failed[%d] \r\n", res);
						}
						printDebug_Semphr(MINOR_ERR, "[MNG_TASK]: Return to full mode.. \r\n");
					}
				}
			}
			
			/* Retransmit Data Packet --------------------------------------------*/
			vPKTQUEUE_Retransmit(packet_Queue, &_Queue_counter);
			
			/* Transmission Event Report -----------------------------------------*/
			vMNG_TransmitEvent();
			
			/* Update Config Capture ---------------------------------------------*/
			vMNG_UpdateConfigCapture();
			
			/* Transmission Configuration Management -----------------------------*/
			vMNG_TransmitConfig();
			
			/* Alarm Process -----------------------------------------------------*/
			if(struct_ConfigOperating.alarmEnable) {
				/* Event Capture */
				vMNG_EventCapture();
				
				/* Check Buzzer Working */
				if(struct_ConfigBuzzer.buzzerEnable) {
					if(bMNG_CheckAlarm(struct_ConfigBuzzer.eventSrc, struct_ConfigBuzzer.numberOfSrc)) {
						_buzzer_2hz = true;
					}else {
						_buzzer_2hz = false;
					}
				}else {
					_buzzer_2hz = false;
				}
			}
			
			/* Handle Sampling, Sent LED Indicator -------------------------------*/
			if(_LED_SAMP_ON) {
				if(!flag_isSetTimeLedSamp) {
//					printDebug_Semphr(INFO, "[MNG_TASK]: LED SAMP ON ]]]]]]]] \r\n");
					LED_SAMP_ON;
					ucTIMER_SetTimer(&ledSampTimer, _LED_SAMP_ON);
					flag_isSetTimeLedSamp = true;
				}else {
					if(cTIMER_CheckTimerExceed(ledSampTimer)) {
//						printDebug_Semphr(INFO, "[MNG_TASK]: LED SAMP OFF ]]]]]]]] \r\n");
						LED_SAMP_OFF;
						_LED_SAMP_ON = 0;
						flag_isSetTimeLedSamp = false;
					}
				}
			}
			if(_LED_SENT_ON) {
				if(!flag_isSetTimeLedSent) {
//					printDebug_Semphr(INFO, "[MNG_TASK]: LED SENT ON ]]]]]]]] \r\n");
					LED_SENT_ON;
					ucTIMER_SetTimer(&ledSentTimer, _LED_SENT_ON);
					flag_isSetTimeLedSent = true;
				}else {
					if(cTIMER_CheckTimerExceed(ledSentTimer)) {
//						printDebug_Semphr(INFO, "[MNG_TASK]: LED SENT OFF ]]]]]]]] \r\n");
						LED_SENT_OFF;
						_LED_SENT_ON = 0;
						flag_isSetTimeLedSent = false;
					}
				}
			}
			if(_LED_SD_ON) {
				if(!flag_isSetTimeLedSD) {
//					printDebug_Semphr(INFO, "[MNG_TASK]: LED SD ON ]]]]]]]] \r\n");
					LED_SD_RED;
					ucTIMER_SetTimer(&ledSDTimer, _LED_SD_ON);
					flag_isSetTimeLedSD = true;
				}else {
					if(cTIMER_CheckTimerExceed(ledSDTimer)) {
//						printDebug_Semphr(INFO, "[MNG_TASK]: LED SD OFF ]]]]]]]] \r\n");
						if(flag_isSDInstall) {
							LED_SD_GREEN;
						}
						_LED_SD_ON = 0;
						flag_isSetTimeLedSD = false;
					}
				}
			}
			
			if(_LED_ERR_ON) {
				if(!flag_isSetTimeLedERR) {
//					printDebug_Semphr(INFO, "[MNG_TASK]: LED SD ON ]]]]]]]] \r\n");
					LED_ERR_RED;
					ucTIMER_SetTimer(&ledERRTimer, _LED_ERR_ON);
					flag_isSetTimeLedERR = true;
				}else {
					if(cTIMER_CheckTimerExceed(ledERRTimer)) {
//						printDebug_Semphr(INFO, "[MNG_TASK]: LED SD OFF ]]]]]]]] \r\n");
						LED_ERR_GREEN;
						_LED_ERR_ON = 0;
						flag_isSetTimeLedERR = false;
					}
				}
			}

		}else { /* Device Disable */
			
		}
		
		if(cTIMER_CheckTimerExceed(showDateTime)) {
			ucTIMER_SetTimer(&showDateTime, 300);
			timestamp = uiRTC_GetEpochTime();
			vRTC_GetDateTime(&RTC_DateStructure, &RTC_TimeStructure, 7);		
			
// 			TM_DS1307_GetDateTime(&time);
				
// 			RTC_DateStructure.RTC_Date = time.date;
// 			RTC_DateStructure.RTC_Month	= time.month;
// 			RTC_DateStructure.RTC_WeekDay	= time.day;
// 			RTC_DateStructure.RTC_Year			=	time.year;
// 			
// 			RTC_TimeStructure.RTC_Seconds	= time.seconds;
// 			RTC_TimeStructure.RTC_Minutes	= time.minutes;
// 			RTC_TimeStructure.RTC_Hours		= time.hours;
			
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printDebug(INFO, "[DEVICE: %s/ ALARM: %s]  ",(struct_ConfigOperating.deviceEnable==1)?"EN":"DIS",(struct_ConfigOperating.alarmEnable==1)?"EN":"DIS");
				printDebug(INFO, "Date: [%02d] %02d-%02d-%04d  ", RTC_DateStructure.RTC_WeekDay, 
																													RTC_DateStructure.RTC_Date, 
																													RTC_DateStructure.RTC_Month, 
																													2000+RTC_DateStructure.RTC_Year);
				printDebug(INFO, "Time: %02d:%02d:%02d  ", RTC_TimeStructure.RTC_Hours, 
																									 RTC_TimeStructure.RTC_Minutes, 
																									 RTC_TimeStructure.RTC_Seconds);	
				printDebug(INFO, "Timestamp: %d  ", timestamp);
				taskDISABLE_INTERRUPTS();
				if(ucMNG_ReadDS1820(&temper)) {
					printDebug(INFO, "Temp: %0.2f C \r\n", temper);
				}else {
					printDebug(INFO, "Temp: Err. \r\n");
				}
				taskENABLE_INTERRUPTS();
				xSemaphoreGive(xSemaphore);
			}
		}
	} /* end main loop */
} /* vManager_Task */
/*==========================================================================================*/


/*============================================================================================
 *====================================== Service Function ====================================
 *============================================================================================
 */
/*==========================================================================================*/
/**
  * @brief  Read Battery Voltage.
  * @param  None
  * @retval Battery Voltage.
  */
uint8_t ucMNG_ReadDS1820(float *temp) {
	
	uint16_t 	temperature;
	uint8_t 	resolution;
	int8_t 		digit, minus = 0;
	float 		decimal	= 0.0;
	
	//Reset pulse
	ucONEWIRE_Reset();
	//Skip rom
	vONEWIRE_WriteByte(ONEWIRE_CMD_SKIPROM);
	//Start conversion on all connected devices
	vONEWIRE_WriteByte(DS1820_CMD_CONVERTTEMP);
	
	delay_ms(500);

	//Reset line
	ucONEWIRE_Reset();
	//Skip rom
	vONEWIRE_WriteByte(ONEWIRE_CMD_SKIPROM);
	//Read scratchpad command by onewire protocol
	vONEWIRE_WriteByte(ONEWIRE_CMD_RSCRATCHPAD);
	//First two bytes of scratchpad are temperature values
	temperature = ucONEWIRE_ReadByte() | (ucONEWIRE_ReadByte() << 8);
	//Reset line
	ucONEWIRE_Reset();
	
	// Check error
	if(((temperature >> 8) != 0x00) && ((temperature >> 8) != 0xFF)) {
		return 0;
	}
	// Convert to temperature
	decimal = (float)(temperature * DS1820_RESOLUTION);
	*temp = decimal;
	
	return 1;
}
/*==========================================================================================*/
/**
  * @brief  Read Battery Voltage.
  * @param  None
  * @retval Battery Voltage.
  */
float fMNG_ReadBatt(void) {
	
	uint8_t 	i = 0;
	uint32_t 	adc_sum = 0;
	uint32_t 	adc_value = 0;
	uint32_t 	adc_buf[18];
	uint32_t  adc_max = 0, adc_min = 0x0FFF;
	float			batt_volt = 0.0;
	float 		adc_vref = struct_DevConfig.adc_Vref;
	
	for(i = 0; i < 18; i++) {
		adc_buf[i] = TM_ADC_Read(ADC1, AIN1_BATT_CH);//ADCValue[1]; 																/* Add ADC value to checkup array */
		adc_sum += adc_buf[i];																		/* Sum ADC values */
		if(adc_buf[i] < adc_min) {																/* Search the array for MIN value */
			adc_min = adc_buf[i];
		}
		if(adc_buf[i] > adc_max) {																/* Search the array for MAX value */
			adc_max = adc_buf[i];
		}
		delay_ms(5);
	}
	adc_sum -= adc_min;																					/* Remove the MIN value from sum */
	adc_sum -= adc_max;																					/* Remove the MAX value from sum */
	adc_sum >>= 4;																							/* Shift bits 4 places - divide the sum by 16 (2 to the power of 4) */
	adc_value = (adc_sum + (adc_sum/100));											/* Final value correction ~1% (Shunt resistor tolerance) */
	
	batt_volt = (24.0/adc_vref)*(((float)adc_value/4095.0)*adc_vref);
	
//	printDebug_Semphr(INFO, "ADC: %d, Volt: %0.4f, Batt: %0.4f \r\n", adc_value, ((adc_value/4095.0)*adc_vref), batt_volt);
	
	return batt_volt;
}
/*==========================================================================================*/
/**
  * @brief  Assembly Self-Registration CMD Packet and Send to Server
  * @param  None
  * @retval Protocol return code
  */
static PTC_RES xMNG_SendRegis(void) {
	uint8_t 		index = 0;
	uint8_t			regisPacket[109];     // Include UNAME + PASS + S/N + DNAME + DTYPID
	PTC_RES			res;
	
	memcpy(&regisPacket[index], struct_info.user, sizeof(struct_info.user));
	index += sizeof(struct_info.user);
	memcpy(&regisPacket[index], struct_info.password, sizeof(struct_info.password));
	index += sizeof(struct_info.password);
	memcpy(&regisPacket[index], struct_info.serialNumber, sizeof(struct_info.serialNumber));
	index += sizeof(struct_info.serialNumber);
	memcpy(&regisPacket[index], struct_ConfigProperties.deviceName, 20);
	index += 20;
	regisPacket[index] = 0x01;
	
	res = xPROCOMM_SendNetworkMaintCMD(SELF_REGIS, regisPacket, sizeof(regisPacket), 0);
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Assembly Join-Network CMD Packet and Send to Server
  * @param  None
  * @retval Protocol return code
  */
static PTC_RES xMNG_SendJoin(void) {
	uint8_t 		index = 0;
	uint8_t			joinPacket[13];     // Include UNAME + PASS + S/N + DNAME + DTYPID
	PTC_RES			res;
	
	memcpy(&joinPacket[index], struct_info.serialNumber, sizeof(struct_info.serialNumber));
	index += sizeof(struct_info.serialNumber);
	joinPacket[index] = 0x01;  // Device Type
	
	res = xPROCOMM_SendNetworkMaintCMD(JOIN_NETWORK, joinPacket, sizeof(joinPacket), 0);
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Transmission Event Report
  * @param  None
  * @retval None
  */
static void vMNG_TransmitEvent(void) {
	
	uint8_t 	i;
	uint8_t 	numQueue 	= 0;
	uint8_t		eid				= 0;
	uint32_t 	edata			= 0;
	uint32_t 	timestamp	= 0;
	bool 			isSaveLog	= false;
	PTC_RES 			res;
	ST_LOG_EVENT 	log_event;

	numQueue = ucDATAQUEUE_GetEventAmount();
	for(i = 0; i < numQueue; i++) {
		ucDATAQUEUE_FetchEvent(&timestamp, &eid, &edata);
		if((_Communication_Status & COMMU_CONNECTED) && (_Communication_Status & COMMU_JOINED)) {
			res = xPROCOMM_SendEventReportCMD(timestamp, RECORD_RLT, eid, edata, struct_DevConfig.retransmitInterval);
			if(res != PTC_OK) {
				isSaveLog	= true;
				printDebug_Semphr(MINOR_ERR, "<MNG_TransmitEvent>: Send event report failed(PTC_%d) \r\n", res);
			}
		}else {
			isSaveLog	= true;
		}
		/*-- Save log, If not able be send --*/
		if(isSaveLog) {
			if(flag_isSDInstall) {
				log_event.cmd 			= LOG_CMD_WRITE;
				log_event.timestamp = timestamp;
				log_event.eid 			= eid;
				log_event.edata 		= edata;		
				if(!xQueueSend(Queue_EventLogToSD, &log_event, 0)) {
					printDebug_Semphr(MINOR_ERR, "<MNG_TransmitEvent>: Send Queue_EventLogToSD failed. \r\n");
				}
			}else {
				printDebug_Semphr(WARNING, "<MNG_TransmitEvent>: Couldn't save log_event, SD_Card not install. \r\n");
			}
		}
	}
	
}
/*==========================================================================================*/
/**
  * @brief  Transmission Configuration Management
  * @param  None
  * @retval None
  */
static void vMNG_TransmitConfig(void) {
	
	uint8_t 				i;
	uint8_t					spec[SIZE_MAX_SPEC];
	uint8_t 				numQueue 	= 0;
	uint8_t					accd			= 0;
	uint8_t					cnfg			= 0;
	uint16_t				specLen		= 0;
	uint32_t 				timestamp	= 0;
	bool 						isSaveLog	= false;
	PTC_RES 				ptc_res;
	ST_LOG_CONFIG 	log_config;
	
	numQueue = ucDATAQUEUE_GetConfigAmount();
	for(i = 0; i < numQueue; i++) {
		ucDATAQUEUE_FetchConfig(&accd, &timestamp, &cnfg, spec, &specLen);
		if((_Communication_Status & COMMU_CONNECTED) && (_Communication_Status & COMMU_JOINED)) {
			ptc_res = xPROCOMM_SendConfigManageCMD(accd, RECORD_RLT, timestamp,cnfg, spec, specLen, struct_DevConfig.retransmitInterval);
			if(ptc_res != PTC_OK) {
				isSaveLog	= true;
				printDebug_Semphr(MINOR_ERR, "<MNG_TransmitConfig>: SendConfigManageCMD failed[PTC_%d] \r\n", ptc_res);
			}
		}else {
			isSaveLog	= true;
		}
		/*-- Save log, If not able be send --*/
		if(isSaveLog) {
			if(flag_isSDInstall) {
				log_config.cmd 				= LOG_CMD_WRITE;
				log_config.accd				= accd;
				log_config.timestamp 	= timestamp;
				log_config.cnfg				=	cnfg;
				log_config.spec_len		= specLen;
				memcpy(log_config.spec, spec, specLen);
				if(!xQueueSend(Queue_ConfigLogToSD, &log_config, 0)) {
					printDebug_Semphr(MINOR_ERR, "<MNG_TransmitConfig>: Send Queue_ConfigLogToSD failed. \r\n");
				}
			}else {
				printDebug_Semphr(WARNING, "<MNG_TransmitConfig>: Couldn't save log_config, SD_Card not install. \r\n");
			}
		}
	}
	
}
/*==========================================================================================*/

/**
  * @brief  Capture Configuration(ACCD: UPDATE)
  * @param  None
  * @retval None
  */
static void vMNG_UpdateConfigCapture(void) {
	
	uint8_t 	i;
//	PTC_RES 	ptc_res;
	uint32_t	timestamp = 0;
	
	if(flag_isConfPropUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_PROPERTIES, &struct_ConfigProperties, sizeof(STCONFIG_PROPERTIES))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfPropUpdate = false;
	}
	
	if(flag_isConfOperUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_OPERATING, &struct_ConfigOperating, sizeof(STCONFIG_OPERATING))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfOperUpdate = false;
	}
	
	if(flag_isConfBuzzUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_BUZZER, &struct_ConfigBuzzer, sizeof(STCONFIG_BUZZER))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfBuzzUpdate = false;
	}
	
	if(flag_isConfWorkUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_WORKING, &struct_ConfigWorking, sizeof(STCONFIG_WORKING))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfWorkUpdate = false;
	}
	
	if(flag_isConfNetwUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_NETWORK, &struct_ConfigNetwork, sizeof(STCONFIG_NETWORK))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfNetwUpdate = false;
	}
	
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(flag_isConf420SensUpdate[i]) {
			timestamp = uiRTC_GetEpochTime();
			if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_420SENSORn[i], &struct_Config420Sensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
				printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
			}
			flag_isConf420SensUpdate[i] = false;
		}
	}
	
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(flag_isConfADCSensUpdate[i]) {
			timestamp = uiRTC_GetEpochTime();
			if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_ADCSENSORn[i], &struct_ConfigADCSensor[i], sizeof(STCONFIG_ANALOGSENSOR))) {
				printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
			}
			flag_isConfADCSensUpdate[i] = false;
		}
	}
	
	if(flag_isConfMODBUSSensUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_MODBUSSENS, &struct_ConfigMODBUSSensor, sizeof(STCONFIG_MODBUSSENSOR))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfMODBUSSensUpdate = false;
	}
	
	if(flag_isConfPulsSensUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_PULSESENS, &struct_ConfigPulseSensor, sizeof(STCONFIG_PULSESENSOR))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfPulsSensUpdate = false;
	}
	
	if(flag_isConfValveUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_VALVE, &struct_ConfigValve, sizeof(STCONFIG_VALVE))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfValveUpdate = false;
	}
	
	for(i = 0; i < MAX_DIGItIN; i++) {
		if(flag_isConfDINUpdate[i]) {
			timestamp = uiRTC_GetEpochTime();
			if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_DINn[i], &struct_ConfigDIN[i], sizeof(STCONFIG_DIGITALsIN))) {
				printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
			}
			flag_isConfDINUpdate[i] = false;
		}
	}
	
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(flag_isConf420AlamUpdate[i]) {
			timestamp = uiRTC_GetEpochTime();
			if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_420SENsALn[i], &struct_Config420SensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
				printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
			}
			flag_isConf420AlamUpdate[i] = false;
		}
	}
	
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(flag_isConfADCAlamUpdate[i]) {
			timestamp = uiRTC_GetEpochTime();
			if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_ADCSENsALn[i], &struct_ConfigADCSensorAlarm[i], sizeof(STCONFIG_SENSORALARM))) {
				printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
			}
			flag_isConfADCAlamUpdate[i] = false;
		}
	}
	
	if(flag_isConfMODBUSAlamUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_MODBUSSENsAL, &struct_ConfigMODBUSSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfMODBUSAlamUpdate = false;
	}
	
	if(flag_isConfPulsAlamUpdate) {
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddConfig(CONF_ACCD_UPDATE, timestamp, CONF_CNFG_PULSENsAL, &struct_ConfigPulseSensorAlarm, sizeof(STCONFIG_SENSORALARM))) {
			printDebug_Semphr(MINOR_ERR, "<MNG_UpdateConfigCapture>: Config_Queue full. \r\n");
		}
		flag_isConfPulsAlamUpdate = false;
	}
	
}
/*==========================================================================================*/
/**
  * @brief  Capture Event Report
  * @param  None
  * @retval None
  */
static void vMNG_EventCapture(void) {
	
	uint8_t 	i;
	uint8_t 	eventID 				= 0;
	uint8_t		battEventCode		= 0;
	uint8_t 	memEventCode 		= 0;
	uint8_t 	dataEventCode 	= 0;
	uint8_t 	statusEventCode = 0;
	uint8_t 	dinEventCode 		= 0;
	uint32_t 	timestamp 			= 0;
	
	/*----- Battery Event -----*/
	if(bMNG_CheckBatteryAlarm(&battEventCode)) {
		printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!Battery Code[%02X]!!! \r\n", battEventCode);
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddEvent(timestamp, EV_BATTERY, (uint32_t)battEventCode)) {
			printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
		}
	}
	
	/*----- Memory Event -----*/
//	if(vMNG_CheckMemoryStatusAlarm(&memEventCode)) {
	if(flag_HaveMemEvent) {
		flag_HaveMemEvent = false;
		printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!Memory Code[%02X]!!! \r\n", _MemoryStatus);
		timestamp = uiRTC_GetEpochTime();
		if(!ucDATAQUEUE_AddEvent(timestamp, EV_MEMORY, (uint32_t)_MemoryStatus)) {
			printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
		}
	}
	
	/*----- Sensor Event -----*/
	/* 4-20mA Sensor */
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(struct_Config420Sensor[i].sensorEnable) {
			if(bMNG_CheckSens420StatusAlarm(i, &statusEventCode)) {
				printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!420mASensor_%d Code[%02X]!!! \r\n", i+1, statusEventCode);
				timestamp = uiRTC_GetEpochTime();
				eventID = EV_SENSOR | (i+1);
				if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)statusEventCode)) {
					printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
				}
			}
		}
	}
	/* MODBUS Flow Sensor */
	
	/* PULSE Flow Sensor */
	
	/*----- Data Event -----*/
	/* 4-20mA Sensor */
	for(i = 0; i < MAX_420SENSOR; i++) {
		if(struct_Config420Sensor[i].sensorEnable && struct_Config420SensorAlarm[i].alarmEnable) {
			if(_SensorStatus_420mA[i] == EV_NORMAL) {
				if(bMNG_CheckSens420DataAlarm(i, &dataEventCode)) {
					printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!Data 4-20mASensor_%d Code[%02X]!!! \r\n", i+1, dataEventCode);
					timestamp = uiRTC_GetEpochTime();
					eventID = EV_DATA | (i+1);
					if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dataEventCode)) {
						printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
					}
				}
			}
		}
	}
	/* ADC Sensor */
	for(i = 0; i < MAX_ADCSENSOR; i++) {
		if(struct_ConfigADCSensor[i].sensorEnable && struct_ConfigADCSensorAlarm[i].alarmEnable) {
			if(bMNG_CheckSensADCDataAlarm(i, &dataEventCode)) {
				printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!Data ADCSensor_%d Code[%02X]!!! \r\n", i+1, dataEventCode);
				timestamp = uiRTC_GetEpochTime();
				switch(i) {
					case 0:
						eventID = EV_DATA_ANALOG_1;
						break;
					case 1:
						eventID = EV_DATA_ANALOG_2;
						break;
					default:
						break;
				}
				if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dataEventCode)) {
					printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
				}
			}
		}
	}
	/* MODBUS Flow Sensor */
	
	/* PULSE Flow Sensor */
	if(struct_ConfigPulseSensor.sensor_enable && struct_ConfigPulseSensorAlarm.alarmEnable) {
		if(_PulseData_Counter != 0) {
			if(bMNG_CheckSensPULSEDataAlarm(&dataEventCode)) {
				printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!Data PulseSensor Code[%02X]!!! \r\n", dataEventCode);
				timestamp = uiRTC_GetEpochTime();
				eventID = EV_DATA_PULSESENS;
				if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dataEventCode)) {
					printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
				}
			}
		}
	}
	
	
	/*----- Digital Input Event -----*/
	for(i = 0; i < MAX_DIN; i++) {
		if(struct_ConfigDIN[i].trigAlarmEn) {
			if(bMNG_CheckDINStatusAlarm(i, &dinEventCode)) {
				timestamp = uiRTC_GetEpochTime();
				eventID = EV_DIN | i;
				switch(struct_ConfigDIN[i].trigAlarmType) {
					case DIN_RISING_EDGE:
						if(dinEventCode == EV_DIN_RISING) {
							printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!DIN_%d Rising-Edge[%02X]!!! \r\n", i+1, dinEventCode);
							if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dinEventCode)) {
								printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
							}
						}
						break;
					case DIN_FALLING_EDGE:
						if(dinEventCode == EV_DIN_FALLING) {
							printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!DIN_%d Falling-Edge[%02X]!!! \r\n", i+1, dinEventCode);
							if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dinEventCode)) {
								printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
							}
						}
						break;
					default:
						printDebug_Semphr(INFO, "<MNG_EventCapture>: !!!DIN_%d %s_Edge[%02X]!!! \r\n", i+1, (dinEventCode==EV_DIN_RISING)?"Rising":"Falling", dinEventCode);
						if(!ucDATAQUEUE_AddEvent(timestamp, eventID, (uint32_t)dinEventCode)) {
							printDebug_Semphr(MINOR_ERR, "<MNG_EventCapture>: Add Event Queue Failed !!! \r\n");
						}
						break;
				}
			}
		}
	}
	
}
/*==========================================================================================*/
/**
  * @brief  This Function is check alarm from Alarm_Source
  * @param  Alarm_Source
  * @retval @true 	Alarm is occur
	*					@false 	Alarm is not occur
  */
static bool bMNG_CheckAlarm(uint8_t *srcList, uint8_t srcLen) {
	
	uint8_t 	i;
	bool 			res = false;
	bool 			ret = false;
	
	for(i = 0; i < srcLen; i++) {
		res = bMNG_CheckAlarmFromSrc(srcList[i]);
		ret |= res;
//		printDebug_Semphr(INFO, "<bMNG_CheckAlarm> Src[0x%02X] %d \r\n", srcList[i], res);
	}
	return ret;
}
/*==========================================================================================*/
/**
  * @brief  This Function is check alarm from Alarm_Source
  * @param  Alarm_Source
  * @retval @true 	Alarm is occur
	*					@false 	Alarm is not occur
  */
static bool bMNG_CheckAlarmFromSrc(uint8_t alarmSrc) {
	
	uint8_t 	i;
	bool 			ret = false;

	for(i = 0; i < MAX_420SENSOR; i++) {
		if(struct_Config420SensorAlarm[i].alarmEnable) {
			if(alarmSrc == SRC_420SENSn_EVENT(i+1)) {
				if(_DataStatus_420mA[i] != EV_NORMAL) {
					ret = true;
				}
				break;
			}else if(alarmSrc == SRC_420SENSn_LOWER(i+1)) {
				if(_DataStatus_420mA[i] & EV_LOWER) {
					ret = true;
				}
				break;
			}else if(alarmSrc == SRC_420SENSn_LOWEREXTREME(i+1)) {
				if(_DataStatus_420mA[i] & EV_LOWER_EXTREME) {
					ret = true;
				}
				break;
			}else if(alarmSrc == SRC_420SENSn_UPPER(i+1)) {
				if(_DataStatus_420mA[i] & EV_UPPER) {
					ret = true;
				}
				break;
			}else if(alarmSrc == SRC_420SENSn_UPPEREXTREME(i+1)) {
				if(_DataStatus_420mA[i] & EV_UPPER_EXTREME) {
					ret = true;
				}
				break;
			}else if(alarmSrc == SRC_420SENSn_ERROR(i+1)) {
				
				break;
			}
		}
	}
	
	if(alarmSrc == SRC_DI_EVENT_ANY) {
		for(i = 0; i < MAX_DIN; i++) {
			if(struct_ConfigDIN[i].trigAlarmEn) {
				if(struct_ConfigDIN[i].trigAlarmType == DIN_FALLING_EDGE) {
					if(_din_status[i] == EV_DIN_FALLING) {
						ret = true;
						break;
					}
				}else if(struct_ConfigDIN[i].trigAlarmType == DIN_RISING_EDGE) {
					if(_din_status[i] == EV_DIN_RISING) {
						ret = true;
						break;
					}
				}else if(struct_ConfigDIN[i].trigAlarmType == DIN_ALL_EDGE) {
//					ret = true;
					break;
				}
			}
		}
	}
	
	for(i = 0; i < MAX_DIN; i++) {
		if(alarmSrc == SRC_DI_EVENT(i)) {
			if(struct_ConfigDIN[i].trigAlarmEn) {
				if(struct_ConfigDIN[i].trigAlarmType == DIN_FALLING_EDGE) {
					if(_din_status[i] == EV_DIN_FALLING) {
						ret = true;
					}
				}else if(struct_ConfigDIN[i].trigAlarmType == DIN_RISING_EDGE) {
					if(_din_status[i] == EV_DIN_RISING) {
							ret = true;
					}
				}else if(struct_ConfigDIN[i].trigAlarmType == DIN_ALL_EDGE) {
//					ret = true;
				}
			}
			break;
		}
	}
	
	return ret;
}
/*==========================================================================================*/
/**
  * @brief  Check DigitalInput status alarm.
  * @param  Index_channel
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckDINStatusAlarm(uint8_t index_ch, uint8_t *eventcode) {
	
	static bool				flag_DIN[MAX_DIN]							= {false,false,false,false,false,false,false,false,false,false,false,false};
	static uint32_t 	timerDIN[MAX_DIN]       			= {0,0,0,0,0,0,0,0,0,0,0,0};	
	static 	uint8_t 	dinStatus_old[MAX_DIN] 				= {EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,
																											EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING};
	static 	uint8_t 	dinStatus_lastCheck[MAX_DIN] 	= {EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,
																											EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING};

	if(_DINStatus[index_ch] != dinStatus_old[index_ch]) {
		if(_DINStatus[index_ch] != dinStatus_lastCheck[index_ch]) {
			flag_DIN[index_ch] = false;
			dinStatus_lastCheck[index_ch] = _DINStatus[index_ch];
		}
		if(!flag_DIN[index_ch]) {
			flag_DIN[index_ch] = true;
			ucTIMER_SetTimer(&timerDIN[index_ch], struct_ConfigDIN[index_ch].trigDelay);
		}else {
			if(cTIMER_CheckTimerExceed(timerDIN[index_ch])) {
				*eventcode = _DINStatus[index_ch];
				_din_status[index_ch] = _DINStatus[index_ch];
				dinStatus_old[index_ch] = _DINStatus[index_ch];
				return true;
			}
		}
	}else {
		flag_DIN[index_ch] = false;
	}
	
	return false;																			
}
/*==========================================================================================*/
/**
  * @brief  Check battery status alarm.
  * @param  Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckBatteryAlarm(uint8_t *eventcode) {
	
//	static uint8_t	battStatus_old = 0;
	
	if(_BatteryStatus != _BatteryStat_old) {
//		printDebug_Semphr(INFO, "<MNG_CheckBatteryAlarm>: Battery Stat: 0x%02X \r\n", _BatteryStatus);
		*eventcode = _BatteryStatus;
		_BatteryStat_old = _BatteryStatus;
		return true;
	}
	
	return false;	
}
/*==========================================================================================*/
/**
  * @brief  Check Memory status alarm.
  * @param  Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckMemoryStatusAlarm(uint8_t *eventcode) {
	
	static uint8_t		memoryStatus_old = 0;
	
	if(_MemoryStatus != memoryStatus_old) {
		*eventcode = _MemoryStatus;
		memoryStatus_old = _MemoryStatus;
		return true;
	}
	
	return false;
}
/*==========================================================================================*/
/**
  * @brief  Check Sensor420mA status alarm.
  * @param  Index channel, Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckSens420StatusAlarm(uint8_t index_ch, uint8_t *eventcode) {
	
	static uint8_t		sensorStatus_420ma_old[MAX_420SENSOR] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	if(_SensorStatus_420mA[index_ch] != sensorStatus_420ma_old[index_ch]) {
		*eventcode = _SensorStatus_420mA[index_ch];
		sensorStatus_420ma_old[index_ch] = _SensorStatus_420mA[index_ch];
		return true;
	}
	
	return false;
}
/*==========================================================================================*/
/**
  * @brief  Check Sensor420mA data alarm.
  * @param  Index channel, Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckSens420DataAlarm(uint8_t index_ch, uint8_t *eventcode) {
	
	static uint8_t		dataStatus_420ma_old[MAX_420SENSOR] = {0,0,0,0,0,0};

	if(index_ch >= MAX_420SENSOR) {
		printDebug_Semphr(MINOR_ERR, "<MNG_CheckSens420DataAlarm>: Invalid index sensor(%d) \r\n", index_ch);
		return false;
	}
	
	if(_DataStatus_420mA[index_ch] != dataStatus_420ma_old[index_ch]) {
		*eventcode = _DataStatus_420mA[index_ch];
		dataStatus_420ma_old[index_ch] = _DataStatus_420mA[index_ch];
		return true;
	}
		
	return false;
}
/*==========================================================================================*/
/**
  * @brief  Check SensorADC data alarm.
  * @param  Index channel, Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckSensADCDataAlarm(uint8_t index_ch, uint8_t *eventcode) {
	
	static uint8_t		dataStatus_ADC_old[MAX_ADCSENSOR] = {0,0};

	if(index_ch >= MAX_ADCSENSOR) {
		printDebug_Semphr(MINOR_ERR, "<MNG_CheckSensADCDataAlarm>: Invalid index sensor(%d) \r\n", index_ch);
		return false;
	}
	
	if(_DataStatus_ADC[index_ch] != dataStatus_ADC_old[index_ch]) {
		*eventcode = _DataStatus_ADC[index_ch];
		dataStatus_ADC_old[index_ch] = _DataStatus_ADC[index_ch];
		return true;
	}
		
	return false;
}
/*==========================================================================================*/
/**
  * @brief  Check PULSEFlowSensor data alarm.
  * @param  Index channel, Pass by reference (return event code)
  * @retval @true  if event is occer.
	* 				@false if no event occer.
  */
bool bMNG_CheckSensPULSEDataAlarm(uint8_t *eventcode) {
	
	static uint8_t		dataStatus_pulse_old;
	
	if(_DataStatus_PULSEFlow != dataStatus_pulse_old) {
		*eventcode = _DataStatus_PULSEFlow;
		dataStatus_pulse_old = _DataStatus_PULSEFlow;
		return true;
	}
		
	return false;
}
/*==========================================================================================*/
/**
  * @brief  
  * @param  
  * @retval 
  */
uint8_t DefEvSrc(uint8_t ch) {
	uint8_t val = 0;
	switch(ch) {
		case 1:
			val = 0xA0;
			break;
		case 2:
			val = 0xA6;
			break;
		case 3:
			val = 0xB0;
			break;
		case 4:
			val = 0xB6;
			break;
		case 5:
			val = 0xC0;
			break;
		case 6:
			val = 0xC6;
			break;
		case 7:
			val = 0xD0;
			break;
		case 8:
			val = 0xD6;
			break;
	}
	return val;
}
/*==========================================================================================*/





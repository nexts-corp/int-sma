/**
  ******************************************************************************
  * @file    Modbus_Task.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _MODBUS_TASK_H_
#define _MODUBS_TASK_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "port.h"
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
	uint8_t 	command;
	uint16_t	value;
}MODBUS_VALVE_CMD;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define MB_SENSOR_CH_BUFFTERFLY										( 1 )
#define MB_SENSOR_CH_FLOW													( 1 )

/* ============================= Coil Status =================================*/
#define COIL_ADDR_STAT_Device											0x0001
#define COIL_ADDR_STAT_Alarm											0x0002
#define COIL_ADDR_STAT_Buzzer											0x0003
#define COIL_ADDR_STAT_420MA_CH1									0x0004
#define COIL_ADDR_STAT_420MA_CH2									0x0005
#define COIL_ADDR_STAT_420MA_CH3									0x0006
#define COIL_ADDR_STAT_420MA_CH4									0x0007
#define COIL_ADDR_STAT_MODBUS											0x0008
#define COIL_ADDR_STAT_PULSE											0x0009

/* ========================== Holding Register ===============================*/
/* ------------------- Modbus Setting -----------------*/
#define HOLD_ADDR_MODBUS_SETTING_SlaveID					0x0001
#define HOLD_ADDR_MODBUS_SETTING_BaudRate					0x0002
/* ------------------- 420MA 8 CH ---------------------*/
#define HOLD_ADDR_420MA_CH1_SensorType						0x0003
#define HOLD_ADDR_420MA_CH1_DataUnit							0x0004
#define HOLD_ADDR_420MA_CH1_DataHi								0x0005
#define HOLD_ADDR_420MA_CH1_DataLo								0x0006
#define HOLD_ADDR_420MA_CH2_SensorType						0x0007
#define HOLD_ADDR_420MA_CH2_DataUnit							0x0008
#define HOLD_ADDR_420MA_CH2_DataHi								0x0009
#define HOLD_ADDR_420MA_CH2_DataLo								0x000A
#define HOLD_ADDR_420MA_CH3_SensorType						0x000B
#define HOLD_ADDR_420MA_CH3_DataUnit							0x000C
#define HOLD_ADDR_420MA_CH3_DataHi								0x000D
#define HOLD_ADDR_420MA_CH3_DataLo								0x000E
#define HOLD_ADDR_420MA_CH4_SensorType						0x000F
#define HOLD_ADDR_420MA_CH4_DataUnit							0x0010
#define HOLD_ADDR_420MA_CH4_DataHi								0x0011
#define HOLD_ADDR_420MA_CH4_DataLo								0x0012
/* ---------------- BUTTERFLY-VALVE ------------------*/
#define HOLD_ADDR_VALVE_Status										0x0013
/* ------------------ MODBUS 1 CH --------------------*/
#define HOLD_ADDR_MODBUS_FLOWRATE_DataHi					0x0014
#define HOLD_ADDR_MODBUS_FLOWRATE_DataLo					0x0015
#define HOLD_ADDR_MODBUS_POSTOTAL_DataHi					0x0016
#define HOLD_ADDR_MODBUS_POSTOTAL_DataLo					0x0017
#define HOLD_ADDR_MODBUS_NEGTOTAL_DataHi					0x0018
#define HOLD_ADDR_MODBUS_NEGTOTAL_DataLo					0x0019
/* ------------------- Pulse 1 CH ---------------------*/
#define HOLD_ADDR_PULSE_FLOWRATE_DataHi						0x001A
#define HOLD_ADDR_PULSE_FLOWRATE_DataLo						0x001B
#define HOLD_ADDR_PULSE_POSTOTAL_DataHi						0x001C
#define HOLD_ADDR_PULSE_POSTOTAL_DataLo						0x001D
/* ------------------- Analog 2 CH ---------------------*/
#define HOLD_ADDR_ANALOG_CH1_DataHi								0x001E
#define HOLD_ADDR_ANALOG_CH1_DataLo								0x001F
#define HOLD_ADDR_ANALOG_CH2_DataHi								0x0020
#define HOLD_ADDR_ANALOG_CH2_DataLo								0x0021
/* ------------------- GSM 	Signal --------------------*/
#define HOLD_ADDR_GSM_Singal_Data									0x0022
/* ---------------- Battery Remaining -----------------*/
#define HOLD_ADDR_Battery_Data										0x0023
/* -------------- Temperature on board ----------------*/
#define HOLD_ADDR_Temperature_DataHi							0x0024
#define HOLD_ADDR_Temperature_DataLo							0x0025
/* --------------- Digital I/O status -----------------*/
#define HOLD_ADDR_Digital_Data										0x0026

extern USHORT usRegMasterInputStart;
extern USHORT usRegMasterHoldingStart;

/* Exported functions --------------------------------------------------------*/ 

void vModbus_Task(void * pvParameters);

#endif

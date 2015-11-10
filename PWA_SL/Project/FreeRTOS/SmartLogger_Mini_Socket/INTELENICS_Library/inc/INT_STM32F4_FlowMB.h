/**
  ******************************************************************************
  * @file    INT_STM32F4_FlowMB.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _INT_STM32F4_FLOWMB_H_
#define _INT_STM32F4_FLOWMB_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx.h"
#include "port.h"

#include "mb.h"
#include "mbport.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* ========================== Function Code ================================= */
#define FUNC_CODE_READ_HOLDING_REGISTER			3			// Reads out the contents of the Holding Registers.
#define FUNC_CODE_PRESET_SINGLE_REGISTER		6			// Writes data to individual Holding Registers (16 bit) of the slave.

/* ========================== Holding Register ============================== */
#define REG_HOLDING_OFFSET_Direction						0x0017
#define REG_HOLDING_OFFSET_FlowUnit							0x0042
#define REG_HOLDING_OFFSET_TotalizeUnit					0x0046
#define REG_HOLDING_OFFSET_ModbusAddress				0x0047
#define REG_HOLDING_OFFSET_SensorNo							0x0127
#define REG_HOLDING_OFFSET_POSTotalize					0x0311
#define REG_HOLDING_OFFSET_NEGTotalize					0x0315
#define REG_HOLDING_OFFSET_Damping							0x0189
#define REG_HOLDING_OFFSET_LowCutoff						0x0203
#define REG_HOLDING_OFFSET_URV									0x0209
#define REG_HOLDING_OFFSET_FreqOutput						0x0229
#define REG_HOLDING_OFFSET_FlowRate							0x0253

#define REG_HOLDING_QUANTITY_ONE								0x0001
#define REG_HOLDING_QUANTITY_TWO								0x0002
#define REG_HOLDING_QUANTITY_THREE							0x0003
#define REG_HOLDING_QUANTITY_FOUR								0x0004
/* Exported functions ------------------------------------------------------- */ 
eMBErrorCode eFlowMB_Intialize( UCHAR ucAddrSlave, USHORT usBaudrate );
eMBErrorCode eFlowMB_ReadHolding( UCHAR ucSlaveAddr, USHORT usAddress, USHORT usQuantity );

float ulFlowMB_GetFlowRate( void );
float ulFlowMB_GetPOSTotalize( void );
float ulFlowMB_GetNEGTotalize( void );

eMBErrorCode vFlowMB_Poll( void );

#endif

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

/**
  ******************************************************************************
  * @file    INT_STM32F4_AumaticMB.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef _INT_STM32F4_AUMATICMB_H_
#define _INT_STM32F4_AUMATICMB_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "port.h"
#include "stm32f4xx.h"

#include "mb.h"
#include "mbport.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* ========================== Function Code ================================== */
#define FUNC_CODE_READ_COIL_STATUS					1			// Reads out the status of individual pieces of output bit information from the slave.
#define FUNC_CODE_READ_INPUT_STATUS					2			// Reads out the status of individual pieces of input bit information from the slave.
#define FUNC_CODE_READ_HOLDING_REGISTER			3			// Reads out the contents of the Holding Registers.
#define FUNC_CODE_READ_INPUT_REGISTER				4			// Reads out the contents of the Input Data Registers (16 Bit) from the slave
#define FUCN_CODE_FORCE_SINGLE_COIL					5			// Sets an individual bit in the slave to ON or OFF.
#define FUNC_CODE_PRESET_SINGLE_REGISTER		6			// Writes data to individual Holding Registers (16 bit) of the slave.
#define FUNC_CODE_PRESET_MULTIPLE_COILS			15		// Sets several consecutive bits in the slave to ON or OFF.
#define FUNC_CODE_PRESET_MULTIPLE_REGISTER	16		// Writes data into consecutive Holding Registers.

/* ========================== Register Input ================================== */
#define INPUT_OFFSET_Signals								0x03E8
#define INPUT_OFFSET_ActualPosition					0x03E9
#define INPUT_OFFSET_Status									0x03EA
#define INPUT_OFFSET_IntermAndDINinput			0x03EB
#define INPUT_OFFSET_InputAIN1							0x03EC
#define INPUT_OFFSET_Torque									0x03ED
#define INPUT_OFFSET_Remote									0x03EE
#define INPUT_OFFSET_Fault									0x03EF
#define INPUT_OFFSET_Warming12							0x03F0
#define INPUT_OFFSET_Warming34							0x03F1
#define INPUT_OFFSET_InputAIN2							0x03F2
#define INPUT_OFFSET_FailureAnaMainten			0x03F3
#define INPUT_OFFSET_FunctionCheck					0x03F6
#define INPUT_OFFSET_StatusFieldbus					0x03F7

/* ========================================== Status Function ==================================================== */
#define STAT_FUN_EndPOPEN 									0x0000					// Bit: End p. OPEN (Byte 1)
#define STAT_FUN_EndPCLOSE 									0x0001					// Bit: End p. CLOSED (Byte 1)
#define STAT_FUN_SetPointReached 						0x0002					// Bit: Setpoint reached (Byte 1)
#define STAT_FUN_NotReadyRemote							0x0003					// Bit: Not ready REMOTE (Byte 1, collective signal 04)
#define STAT_FUN_RunOPEN 										0x0004					// Bit: Running OPEN (Byte 1)
#define STAT_FUN_RunCLOSE										0x0005					// Bit: Running CLOSE (Byte 1)
#define STAT_FUN_Warnings										0x0006 					// Bit: Warnings (Byte 1, collective signal 02)
#define STAT_FUN_Fault											0x0007 					// Bit: Fault (Byte 1, collective signal 03)
#define STAT_FUN_ThermalFault								0x0008 					// Bit: Thermal fault (Byte 2)
#define STAT_FUN_PhaseFault									0x0009 					// Bit: Phase fault (Byte 2)
#define STAT_FUN_SelSWRemote								0x000A 					// Bit: Sel. sw. REMOTE (Byte 2)
#define STAT_FUN_SelSWLocal									0x000B 					// Bit: Sel. sw. LOCAL (Byte 2)
#define STAT_FUN_LimitSWOpen								0x000C 					// Bit: Limit sw. OPEN (Byte 2)
#define STAT_FUN_LimitSWClose								0x000D 					// Bit: Limit sw. CLOSED (Byte 2)
#define STAT_FUN_TorqueSWOpen								0x000E 					// Bit: Torque sw.OPEN (Byte 2)
#define STAT_FUN_TorqueSWClose							0x000F 					// Bit: Torque sw.CLOSE (Byte 2)
#define STAT_FUN_ActualPosition_HIGH				0x0010					// Actual position high byte (position transmitter)
#define STAT_FUN_ActualPosition_LOW					0x0018					// Actual position low byte (position transmitter)

// #define STAT_FUN_NotReadyRemote 	0x0020 					// Bit: Not ready REMOTE (Byte 5, collective signal 04)
// #define STAT_FUN_Warnings 				0x0021 					// Bit: Warnings (Byte 5, collective signal 02)
// #define STAT_FUN_Fault 					0x0022 					// Bit: Fault (Byte 5, collective signal 03)

#define STAT_FUN_MaintenanceREQU 						0x0023 					// Bit: Maintenance requ. (Byte 5, collective signal 09)
#define STAT_FUN_OutOfSpec 									0x0024 					// Bit: Out of spec. (Byte 5, collective signal 07)
#define STAT_FUN_FunctionCheck 							0x0025 					// Bit: Function check (Byte 5, collective signal 08)
#define STAT_FUN_Failure 										0x0026 					// Bit: Failure (Byte 5, collective signal 10)
#define STAT_FUN_DeviceOK 									0x0027 					// Bit: Device ok (Byte 5, collective signal 05)
#define STAT_FUN_OpPauseActive 							0x0028 					// Bit: Op. pause active (Byte 6)
#define STAT_FUN_InIntermPosition 					0x0029 					// Bit: In interm. position (Byte 6)
#define STAT_FUN_StartStepMode 							0x002A 					// Bit: Start step mode (Byte 6)
#define STAT_FUN_Actuator 									0x002C 					// Bit: Actuator running (Byte 6)
#define STAT_FUN_HandwheelOPER 							0x002D 					// Bit: Handwheel oper. (Byte 6)
#define STAT_FUN_RunningREMOTE 							0x002E 					// Bit: Running REMOTE (Byte 6)
#define STAT_FUN_RunningLOCAL 							0x002F 					// Bit: Running LOCAL (Byte 6)
#define STAT_FUN_IntermPos1 								0x0030 					// Bit: Interm. pos. 1 (Byte 7)
#define STAT_FUN_IntermPos2 								0x0031 					// Bit: Interm. pos. 2 (Byte 7)
#define STAT_FUN_IntermPos3 								0x0032 					// Bit: Interm. pos. 3 (Byte 7)
#define STAT_FUN_IntermPos4 								0x0033 					// Bit: Interm. pos. 4 (Byte 7)
#define STAT_FUN_IntermPos5 								0x0034 					// Bit: Interm. pos. 5 (Byte 7)
#define STAT_FUN_IntermPos6 								0x0035 					// Bit: Interm. pos. 6 (Byte 7)
#define STAT_FUN_IntermPos7 								0x0036 					// Bit: Interm. pos. 7 (Byte 7)
#define STAT_FUN_IntermPos8 								0x0037 					// Bit: Interm. pos. 8 (Byte 7)
#define STAT_FUN_InputDIN1 									0x0038 					// Bit: Input DIN 1 (Byte 8)
#define STAT_FUN_InputDIN2 									0x0039 					// Bit: Input DIN 2 (Byte 8)
#define STAT_FUN_InputDIN3 									0x003A 					// Bit: Input DIN 3 (Byte 8)
#define STAT_FUN_InputDIN4 									0x003B 					// Bit: Input DIN 4 (Byte 8)
#define STAT_FUN_InputDIN5 									0x003C 					// Bit: Input DIN 5 (Byte 8)
#define STAT_FUN_InputDIN6 									0x003D 					// Bit: Input DIN 6 (Byte 8)
#define STAT_FUN_InputAIN1_HIGH							0x0040					// Input AIN1 (high-byte)
#define STAT_FUN_InputAIN1_LOW							0x0048					// Input AIN1 (low-byte)
#define STAT_FUN_Torque_HIGH								0x0050					// Torque (high byte) – (byte 11)
#define STAT_FUN_Torque_LOW									0x0058					// Torque (low byte) – (byte 12)
#define STAT_FUN_WrongOperCMD 							0x0060 					// Bit: Wrong oper. cmd (Byte 13)
#define STAT_FUN_SelSWNotREMOTE 						0x0061 					// Bit: Sel. sw. not REMOTE (Byte 13)
#define STAT_FUN_InterlockActive 						0x0062 					// Bit: Interlock active (Byte 13)
#define STAT_FUN_LocalSTOP 									0x0063 					// Bit: Local STOP (Byte 13)
#define STAT_FUN_EMCYStopActive 						0x0064 					// Bit: EMCY stop active (Byte 13)
#define STAT_FUN_EMCYBehavACT 							0x0065 					// Bit: EMCY behav. act. (Byte 13)
#define STAT_FUN_FailStaeFieldbus 					0x0066 					// Bit: FailState fieldbus (Byte 13)
#define STAT_FUN_IOinterface 								0x0067 					// Bit: I/O interface (Byte 13)
#define STAT_FUN_Disabled 									0x006B 					// Bit: Disabled (Byte 14)
#define STAT_FUN_Interlock 									0x006C 					// Bit: Interlock by-pass (Byte 14)
#define STAT_FUN_HandwheelActive 						0x006D 					// Bit: Handwheel active (Byte 14)
#define STAT_FUN_ServiceActive 							0x006E 					// Bit: Service active (Byte 14)
#define STAT_FUN_PVSTActive 								0x006F 					// Bit: PVST active (Byte 14)
#define STAT_FUN_ConfigERROR 								0x0070 					// Bit: Configuration error (Byte 15)
#define STAT_FUN_MainsQuality 							0x0071 					// Bit: Mains quality (Byte 15)
#define STAT_FUN_TorqusFaultOPEN 						0x0074 					// Bit: Torque fault OPEN (Byte 15)
#define STAT_FUN_TorqusFaultCLOSE 					0x0075 					// Bit: Torque fault CLOSE (Byte 15)
#define STAT_FUN_InternalERROR 							0x0076 					// Bit: Internal error (Byte 15)
#define STAT_FUN_NoReaction 								0x0077 					// Bit: No reaction (Byte 15)
#define STAT_FUN_CongfigRemoteERR 					0x007E 					// Bit: Config. error REMOTE (Byte 16)
#define STAT_FUN_IncorrectPhase 						0x007F 					// Bit: Incorrect phase seq (Byte 16)
#define STAT_FUN_TorqueWarnCLOSE 						0x0084 					// Bit: Torque warn. CLOSE (Byte 17)
#define STAT_FUN_TorqueWarnOPEN							0x0085 					// Bit: Torque warn. OPEN (Byte 17)
#define STAT_FUN_WrnControlsTemp 						0x0088 					// Bit: Wrn controls temp (Byte 18)
#define STAT_FUN_WrnGearboxTemp 						0x0089 					// Bit: Wrn gearbox temp (Byte 18)
#define STAT_FUN_WrnMotorTemp 							0x008A 					// Bit: Wrn motor temp (Byte 18)
#define STAT_FUN_24VExternal 								0x008B 					// Bit: 24 V DC, external (Byte 18)
#define STAT_FUN_RTCButtonCell 							0x008D 					// Bit: RTC button cell (Byte 18)
#define STAT_FUN_RTCNotSet 									0x008E 					// Bit: RTC not set (Byte 18)
#define STAT_FUN_ConfigWarning 							0x008F 					// Bit: Config. warning (Byte 18)
#define STAT_FUN_WrnFOCbudget 							0x0090 					// Bit: Wrn FOC budget (Byte 19)
#define STAT_FUN_WrnFOcables 								0x0091 					// Bit: Wrn FO cables (Byte 19)
#define STAT_FUN_WrninputAIN2 							0x0092 					// Bit: Wrn input AIN 2 (Byte 19)
#define STAT_FUN_WrninputAIN1 							0x0093 					// Bit: Wrn input AIN 1 (Byte 19)
#define STAT_FUN_InternalWarning 						0x0094 					// Bit: Internal warning (Byte 19)
#define STAT_FUN_WrnOnTiStarts 							0x0095 					// Bit: WrnOnTiStarts (Byte 19)
#define STAT_FUN_WrnOnTiRunning 						0x0096 					// Bit: WrnOnTiRunning (Byte 19)
#define STAT_FUN_timeWarning 								0x0097 					// Bit: Op. time warning (Byte 19)
#define STAT_FUN_WrnSetpointPos 						0x009A 					// Bit: WrnSetpointPos (Byte 20)
#define STAT_FUN_WrnFOCconnection 					0x009C 					// Bit: Wrn FOC connection (Byte 20)
#define STAT_FUN_Failurebehav 							0x009D 					// Bit: Failure behav. act. (Byte 20)
#define STAT_FUN_PVSTabort 									0x009E 					// Bit: PVST abort (Byte 20)
#define STAT_FUN_PVSTerror 									0x009F 					// Bit: PVST error (Byte 20)
#define STAT_FUN_InputAIN2_HIGH							0x00A0					// Input AIN2 (high-byte) – (byte 21)
#define STAT_FUN_InputAIN2_LOW							0x00A8					// Input AIN2 (low-byte) – (byte 22)
/* ============================================================================================= */
/* ==================================== Holding Register ====================================== */
#define REG_OUTPUT_OFFSET_Commands						0x03E8
#define REG_OUTPUT_OFFSET_SetPointPosition		0x03E9
#define REG_OUTPUT_OFFSET_AddCommandAndInterm	0x03EA
#define REG_OUTPUT_OFFSET_DigitalOutput				0x03EB
#define REG_OUTPUT_OFFSET_Prozessistwert			0x03EC
#define REG_OUTPUT_OFFSET_OutputAOUT1					0x03ED
#define REG_OUTPUT_OFFSET_OutputAOUT2					0x03EE

/* ======= Command ====== */
#define COMMAND_OPEN 													0x01
#define COMMAND_CLOSE													0x02

/* Exported functions ------------------------------------------------------- */ 
eMBErrorCode eValveMB_Intialize( UCHAR ucAddrSlave, USHORT usBaudrate );
eMBErrorCode eValveMB_SetPointPosition( UCHAR ucSlaveAddr, USHORT usValue );
eMBErrorCode eValveMB_CommandDirection( UCHAR ucSlaveAddr, UCHAR ucCommand );

eMBErrorCode eValveMB_ReadInputData( UCHAR ucSlaveAddr, USHORT usOffset, UCHAR ucQuantity );
eMBErrorCode eValveMB_ReadHolding( UCHAR ucSlaveAddr, USHORT usAddress, USHORT usQuantity );
eMBErrorCode eValveMB_Poll( void );

#endif

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

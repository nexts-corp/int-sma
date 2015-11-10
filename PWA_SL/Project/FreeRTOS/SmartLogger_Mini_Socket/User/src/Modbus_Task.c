/**
  ******************************************************************************
  * @file    gsm_task.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include "Modbus_Task.h"
#include "Manager_Task.h"
#include "Sensors_Task.h"
#include "stm32f4_discovery.h"

#include "mbSlave.h"
#include "mbSlaveport.h"

#include "SL_PinDefine.h"

#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_Timer.h"

#include "INT_STM32F4_ValveMB.h"
#include "INT_STM32F4_FlowMB.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MB_SLAVE_ADDRESS		0x01
#define MB_SLAVE_BAUDRATE		USART_Baudrate_115200

#define REG_HOLDING_START 	0x0001 
#define REG_HOLDING_NREGS 	0x0027
#define REG_COILS_START 		0x0001 
#define REG_COILS_NREGS 		0x0020

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

static USHORT   usRegCoilsStart = REG_COILS_START;
static BOOL			xRegCoils[REG_COILS_NREGS];

extern float fDATBUF_ReadBoardTemp(void);
extern uint8_t ucGSM_RSSI;

uint8_t MBValve_connectstatus;
uint8_t MBFlow_connectstatus;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void vMBSlave_AssemData( void );
static void vMBSlave_Initial( void );

extern SENSOR_DATA 	_Sensor_Data;

/**
  * @brief  task control Modbus master and slave.
  * @param  pvParameters not used
  * @retval None
  */
void vModbus_Task( void * pvParameters )
{
	eMBErrorCode estatus = MB_ENOERR;
	
	uint8_t		ucBaudRateFlowflag				=	0;
	uint8_t		ucBaudRateValveflag				=	0;
	
	uint32_t	ulSetPoint;
	
	uint32_t		ulUpdateRegisterTimer 	= 0;
	uint32_t		ulSacnRateTimer_Valve	 	= 0;
	uint32_t		ulSacnRateTimer_Flow		= 0;
	
	BOOL			ucInitStatus_Valve			= 0;
	BOOL			ucInitStatus_Flow				= 0;
	
 	vMBSlave_Initial( );
	
 	printDebug_Semphr(DESCRIBE, "[MODUBS_TASK]: Running... \r\n");
	
	/*------------------- Set Timer -----------------*/
	ucTIMER_SetTimer( &ulUpdateRegisterTimer, 10 );
	ucTIMER_SetTimer( &ulSacnRateTimer_Valve, 5 );
 	ucTIMER_SetTimer( &ulSacnRateTimer_Flow, 5 );
	
	
// 	struct_ConfigMODBUSSensor.sensor_enable = SENSOR_ENABLE;
// 	struct_ConfigMODBUSSensor.scan_rate = 10;
// 	struct_ConfigMODBUSSensor.baudrate = USART_Baudrate_9600;
//	
//	struct_ConfigValve.interface_type = VALVE_MODBUS;
//	struct_ConfigValve.enable = SENSOR_ENABLE;
	
  while( 1 ) {
		eMBSlavePoll();
		vFlowMB_Poll();
	
		if( cTIMER_CheckTimerExceed( ulUpdateRegisterTimer ) ) {		
			if( struct_ConfigMODBUSSensor.sensor_enable == SENSOR_ENABLE ) {
				/* get flow data from register */
				_Sensor_Data.dataMODBUSFlow	= ulFlowMB_GetFlowRate( );
				printDebug_Semphr( DETAIL, "[MODBUS_MT]: ulFlowRate : %.03f\r\n", _Sensor_Data.dataMODBUSFlow );
				_Sensor_Data.dataMODBUSPosNet	= ulFlowMB_GetPOSTotalize( );
				printDebug_Semphr( DETAIL, "[MODBUS_MT]: POS Total : %.01f\r\n", _Sensor_Data.dataMODBUSPosNet );
				_Sensor_Data.dataMODBUSNegNet	= ulFlowMB_GetNEGTotalize( );
				printDebug_Semphr( DETAIL, "[MODBUS_MT]: NEG Total : %.01f\r\n", _Sensor_Data.dataMODBUSNegNet );
			}
			else if( struct_ConfigValve.interface_type == VALVE_MODBUS && struct_ConfigValve.enable == SENSOR_ENABLE ) {
				/* get Setpoint from register */
				_ValveStatus	= (uint32_t)ulValveMB_GetSetPoint( );
				if( _ValveStatus > 1000 ) {
					_ValveStatus = -1;
				}
				printDebug_Semphr( DETAIL, "[MODBUS_MT]: Set Point : %d\r\n", _ValveStatus );
			}
			vMBSlave_AssemData();
			ucTIMER_SetTimer( &ulUpdateRegisterTimer, 10 );
		}
		
		/*------------------ MODBUS Butterfly Valve ------------------*/
		if( struct_ConfigValve.interface_type == VALVE_MODBUS && struct_ConfigValve.enable == SENSOR_ENABLE ) {
			/*	wait command from Manager Task	*/
			if( xQueueReceive( Queue_CtrlValveMB, &ulSetPoint, 0 ) == pdTRUE ) {
					/* Send Set Point Position */
					eValveMB_SetPointPosition( struct_ConfigValve.slave_ID, ulSetPoint );
			}
			
			if( cTIMER_CheckTimerExceed( ulSacnRateTimer_Valve ) ) {
				if( ucInitStatus_Valve == false ) {
					estatus = eValveMB_Intialize( struct_ConfigValve.slave_ID, struct_ConfigMODBUSSensor.baudrate );
					if( estatus != MB_ENOERR ) {
						printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Initialize modbus error (%d)\r\n", estatus );
					}
					ucBaudRateValveflag = struct_ConfigMODBUSSensor.baudrate;
					ucInitStatus_Valve = true;
				}
				
				/* Read Actual Position */
				estatus = eValveMB_ReadInputData( struct_ConfigValve.slave_ID, INPUT_OFFSET_ActualPosition, 0x01 );
				if( estatus != MB_ENOERR && estatus != MB_ERECVOK ) {
					MBValve_connectstatus = 0;
					printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Read Position Fail (%d)\r\n", estatus );
				}
				else {
					MBValve_connectstatus = 1;
				}
				
// 				ucTIMER_SetTimer( &ulSacnRateTimer_Valve, struct_ConfigValve.scan_rate );
				ucTIMER_SetTimer( &ulSacnRateTimer_Valve, 10 );
			}
		}
		else {
			/* new Initilize Modbus about butterfly Valve */
			ucInitStatus_Valve = false;
		}
		
		/*------------------- MODBUS Flow sensor ------------------------*/
		if( struct_ConfigMODBUSSensor.sensor_enable == SENSOR_ENABLE ) {
			if( cTIMER_CheckTimerExceed( ulSacnRateTimer_Flow ) ) {
				if( ucInitStatus_Flow == false ) {
					estatus = eFlowMB_Intialize( struct_ConfigMODBUSSensor.slave_ID, struct_ConfigMODBUSSensor.baudrate );
					if( estatus != MB_ENOERR ) {
						printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Initialize modbus error (%d)\r\n", estatus );
					}
					ucInitStatus_Flow = true;
					ucBaudRateFlowflag = struct_ConfigMODBUSSensor.baudrate;
				}
				
				/*-------- Read data sensor --------*/
				estatus = eFlowMB_ReadHolding( struct_ConfigMODBUSSensor.slave_ID, REG_HOLDING_OFFSET_FlowRate, 0x02 );
				if( estatus != MB_ENOERR && estatus != MB_ERECVOK ) {
					MBFlow_connectstatus = 0;
					_Sensor_Data.dataMODBUSFlow = 0;
					printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Read Flow rate Fail (%d)\r\n", estatus );
				}
				else {
					MBFlow_connectstatus = 1;
				}
				delay_ms( 100 );
				estatus = eFlowMB_ReadHolding( struct_ConfigMODBUSSensor.slave_ID, REG_HOLDING_OFFSET_POSTotalize, 0x02 );
				if( estatus != MB_ENOERR && estatus != MB_ERECVOK ) {
					MBFlow_connectstatus = 0;
					_Sensor_Data.dataMODBUSPosNet = 0;
					printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Read POS Totalize Fail (%d)\r\n", estatus );
				}
				else {
					MBFlow_connectstatus = 1;
				}
				delay_ms( 100 );
				estatus = eFlowMB_ReadHolding( struct_ConfigMODBUSSensor.slave_ID, REG_HOLDING_OFFSET_NEGTotalize, 0x02 );
				if( estatus != MB_ENOERR && estatus != MB_ERECVOK ) {
					MBFlow_connectstatus = 0;
					_Sensor_Data.dataMODBUSNegNet = 0;
					printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Read NEG Totalize Fail (%d)\r\n", estatus );
				}
				else {
					MBFlow_connectstatus = 1;
				}
				/*----------------------------------*/
				
				ucTIMER_SetTimer( &ulSacnRateTimer_Flow, struct_ConfigMODBUSSensor.scan_rate );
			}
		}
		else {
			/* new Initilize Modbus about Flow sensor */
			ucInitStatus_Flow = false;
		}
		
		/* check new baudrate config */
		if( ucBaudRateFlowflag != struct_ConfigMODBUSSensor.baudrate ){ucInitStatus_Flow = false;}
		if( ucBaudRateValveflag != struct_ConfigMODBUSSensor.baudrate ){ucInitStatus_Valve = false;}
	}
}

/* -------------------------------------------- Function Support ----------------------------------------------- */
static void vMBSlave_Initial( void ) {
	
	eMBErrorCode estatus = MB_ENOERR;
	
	/* Initalize modbus slave rtu */
	estatus  =  eMBSlaveInit ( MB_RTU , 
											MB_SLAVE_ADDRESS ,  	/* address slave device */ 
											0 , 									/* port */
											MB_SLAVE_BAUDRATE ,		/* exchange speed (baudrate) */ 
											MB_SLAVE_PAR_NONE     /* no parity */ 
											);
	if( estatus != MB_ENOERR ) {
		printDebug_Semphr( MINOR_ERR, "[MODBUS_TASK]: Intialize modbus slave fail\r\n");
	}
	
	/* Enable modbus */
	estatus = eMBSlaveEnable( );
	if( estatus != MB_ENOERR ) {
		printDebug_Semphr( MINOR_ERR, "[MODBUS_TASK]: Enable modbus fail\r\n");
	}
	
	xMBSlavePortEventPost( SLAVE_EV_FRAME_RECEIVED );
}
/*===================================================================================================*/
static void vMBSlave_AssemData( void ) {
	
	UCHAR i = 0, HOLD_ADDR_OFFSET_Data = 3;
	UCHAR ucBiData[4];
	
	float ulReadTemp;
	
	usRegHoldingBuf[HOLD_ADDR_MODBUS_SETTING_SlaveID] = MB_SLAVE_ADDRESS;
	usRegHoldingBuf[HOLD_ADDR_MODBUS_SETTING_BaudRate] = MB_SLAVE_BAUDRATE;
	/* Assembly Sensor data to holding register */
	/* 4-20mA 4 ch */
	while( i < 4 ) {
		memcpy( ucBiData, &_Sensor_Data.data420mA[i], 4 );
		
		usRegHoldingBuf[HOLD_ADDR_OFFSET_Data++] = ( USHORT )( struct_Config420Sensor[i].sensorType );
		usRegHoldingBuf[HOLD_ADDR_OFFSET_Data++] = ( USHORT )( struct_Config420Sensor[i].dataUnit );
		usRegHoldingBuf[HOLD_ADDR_OFFSET_Data++] = ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];	// data high
		usRegHoldingBuf[HOLD_ADDR_OFFSET_Data++] = ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];	// data low
		i++;
	}
	
	/* Butterfly Valve */
	usRegHoldingBuf[HOLD_ADDR_VALVE_Status] = _ValveStatus;
	
	/* modbus Valve and Flow */
	memcpy( ucBiData, &_Sensor_Data.dataMODBUSFlow, 4 );
	usRegHoldingBuf[HOLD_ADDR_MODBUS_FLOWRATE_DataHi] = ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];	// data high
	usRegHoldingBuf[HOLD_ADDR_MODBUS_FLOWRATE_DataLo] = ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];	// data low
	memcpy( ucBiData, &_Sensor_Data.dataMODBUSPosNet, 4 );
	usRegHoldingBuf[HOLD_ADDR_MODBUS_POSTOTAL_DataHi] = ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];	// data high
	usRegHoldingBuf[HOLD_ADDR_MODBUS_POSTOTAL_DataLo] = ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];	// data low
	memcpy( ucBiData, &_Sensor_Data.dataMODBUSNegNet, 4 );
	usRegHoldingBuf[HOLD_ADDR_MODBUS_NEGTOTAL_DataHi] = ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];	// data high
	usRegHoldingBuf[HOLD_ADDR_MODBUS_NEGTOTAL_DataLo] = ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];	// data low
	
	/* Pulse 1 ch*/
 	memcpy( ucBiData, &_Sensor_Data.dataPULSEFlow, 4 );
	usRegHoldingBuf[HOLD_ADDR_PULSE_FLOWRATE_DataHi] 		= ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];
	usRegHoldingBuf[HOLD_ADDR_PULSE_FLOWRATE_DataLo] 		= ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];
	memcpy( ucBiData, &_Sensor_Data.dataPULSENet, 4 );
	usRegHoldingBuf[HOLD_ADDR_PULSE_POSTOTAL_DataHi] 		= ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];
	usRegHoldingBuf[HOLD_ADDR_PULSE_POSTOTAL_DataLo] 		= ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];
	
	/* GSM Signal */
	usRegHoldingBuf[HOLD_ADDR_GSM_Singal_Data] = ucGSM_RSSI;
	
	/* Battery */
	usRegHoldingBuf[HOLD_ADDR_Battery_Data] = struct_ConfigWorking.minBattery;
	
	/* Tempurature on Board */
	ulReadTemp = fDATBUF_ReadBoardTemp();
	memcpy( ucBiData, &ulReadTemp, 4 );
	usRegHoldingBuf[HOLD_ADDR_Temperature_DataHi] 		= ( USHORT )( ucBiData[3] << 8 ) + ucBiData[2];
	usRegHoldingBuf[HOLD_ADDR_Temperature_DataLo] 		= ( USHORT )( ucBiData[1] << 8 ) + ucBiData[0];
	
	/*===================================================================================================*/
	
	/* Assembly status data to coil status */
	xRegCoils[COIL_ADDR_STAT_Device] 		= struct_ConfigOperating.deviceEnable;
	xRegCoils[COIL_ADDR_STAT_Alarm]			= struct_ConfigOperating.alarmEnable;
	xRegCoils[COIL_ADDR_STAT_Buzzer]		= struct_ConfigBuzzer.buzzerEnable;
	xRegCoils[COIL_ADDR_STAT_420MA_CH1]	= struct_Config420Sensor[0].sensorEnable;
	xRegCoils[COIL_ADDR_STAT_420MA_CH2]	= struct_Config420Sensor[1].sensorEnable;
	xRegCoils[COIL_ADDR_STAT_420MA_CH3]	= struct_Config420Sensor[2].sensorEnable;
	xRegCoils[COIL_ADDR_STAT_420MA_CH4]	= struct_Config420Sensor[3].sensorEnable;
 	xRegCoils[COIL_ADDR_STAT_MODBUS] 		= struct_ConfigMODBUSSensor.sensor_enable;
	xRegCoils[COIL_ADDR_STAT_PULSE]			= struct_ConfigPulseSensor.sensor_enable;
	
}
/*===================================================================================================*/
/*======================================== Modbus Slave =============================================*/
/*===================================================================================================*/
eMBErrorCode eMBSlaveRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	
	usRegHoldingBuf[0]++;
 	printDebug_Semphr( DETAIL, "usAddress : %d\r\nusNRegs :%d\r\neMode :%d\r\n", usAddress, usNRegs, eMode);
	
	if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) ) {
		iRegIndex = ( int )( usAddress - usRegHoldingStart );
		switch( eMode ) {
			case  MB_REG_READ :
				while( usNRegs > 0 ) {
					* pucRegBuffer ++  =  (  UCHAR  )  (  usRegHoldingBuf [ iRegIndex ]  >>  8  ); 
					* pucRegBuffer ++  =  (  UCHAR  )  (  usRegHoldingBuf [ iRegIndex ]  &  0xFF  ); 
					iRegIndex ++ ; 
					usNRegs -- ; 
        }
				break ; 
			case  MB_REG_WRITE :
				while( usNRegs > 0 ) {
					usRegHoldingBuf [ iRegIndex ]  =  * pucRegBuffer ++  <<  8 ; 
					usRegHoldingBuf [ iRegIndex ]  |=  * pucRegBuffer ++ ; 
					iRegIndex ++ ; 
					usNRegs -- ; 
        } 
        break ; 
		} 
	} 
	else { 
		printDebug_Semphr( INFO, "[MODBUS_SL]: illegal register address\r\n" );
		eStatus = MB_ENOREG ; 
	} 
	return eStatus ; 
}
/*===================================================================================================*/
eMBErrorCode eMBSlaveRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	
 	UCHAR		ucEndAddr = usAddress + usNCoils ;
	UCHAR		ucStartAddr = usAddress;
	UCHAR		ucNBytes, i=0, j=0;
	if( ( usNCoils & 0x0007 ) != 0 ) {
		ucNBytes = ( UCHAR )( usNCoils / 8 + 1 );
	}
	else {
		ucNBytes = ( UCHAR )( usNCoils / 8 );
	}
	if( ( usAddress >= REG_COILS_START ) && ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS ) ) {
		switch( eMode ) {
			case MB_REG_READ :
				printDebug_Semphr( DETAIL, "[MODBUS_SL]: Read %d,%d\r\n", ucStartAddr, ucNBytes );
				while( i < ucNBytes ) {
					* pucRegBuffer = * pucRegBuffer & 0x00 ;
					for( j=0; j<8; j++ ) {
						if( ucStartAddr < ucEndAddr ) {
  						printDebug_Semphr( DETAIL, "%d ", xRegCoils[ucStartAddr - 1] );
							* pucRegBuffer = * pucRegBuffer | xRegCoils[ucStartAddr - 1] << j;
						}
						else {
 							printDebug_Semphr( DETAIL, "%d ", 0 );
						}
						ucStartAddr++;
					}
					printDebug_Semphr( DETAIL, "[MODBUS_SL]: 0x%X\r\n", * pucRegBuffer );
					pucRegBuffer++;
					i++;
				}
				break;
			case MB_REG_WRITE :
				printDebug_Semphr( DETAIL, "[MODBUS_SL]: Write \r\n" );
				break;
		}
	}
	else { 
		printDebug_Semphr( MINOR_ERR, "[MODBUS_SL]: illegal register address\r\n" );
		eStatus = MB_ENOREG ; 
	} 
	
   return eStatus;
}
/*===================================================================================================*/
eMBErrorCode eMBSlaveRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete ) {
    return MB_ENOREG;
}
/*===================================================================================================*/
eMBErrorCode eMBSlaveRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs ) {
    return MB_ENOREG;
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

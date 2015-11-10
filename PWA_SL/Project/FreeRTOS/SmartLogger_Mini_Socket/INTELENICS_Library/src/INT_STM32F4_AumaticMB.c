/**
  ******************************************************************************
  * @file    INT_STM32F4_AumaticMB.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "INT_STM32F4_AumaticMB.h"
#include "main.h"
#include "stdio.h"

#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_debug.h"

/* Global variable -----------------------------------------------------------*/
extern uint32_t usRegMasterInputBuf[0x400];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
  * @brief  Initial modbus for interface with butterfly valve.
  * @param  ucAddrSlave		: address slave device
						usBaudrate		: exchange speed
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eValveMB_Intialize( UCHAR ucAddrSlave, USHORT usBaudrate ) {
	eMBErrorCode estatus = MB_ENOERR;
	
	/* Initalize modbus rtu */
	estatus  =  eMBInit ( MB_RTU , 
											ucAddrSlave ,     /* address slave device */ 
											0 , 							/* port */
											115200 ,      		/* exchange speed (baudrate) */ 
											MB_PAR_NONE       /* no parity */ 
											);
	if( estatus != MB_ENOERR ) {
		printDebug( MINOR_ERR, "[MODBUS_MT]: Initalize modbus error\r\n" );
		return estatus;
	}
	
	/* Enable modbus */
	estatus = eMBEnable( );
	return estatus;
}
/*===================================================================================*/
/**
  * @brief  The setpoint position is transmitted via bytes 3 and 4.
  * @param  usValue : position(value: 0-1000 )
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eValveMB_SetPointPosition( UCHAR ucSlaveAddr, USHORT usValue ) {
	eMBErrorCode 	eMBStatus = MB_ENOERR;
	USHORT				reg[2];
	uint8_t 			ucTimeout = 5;
	
	reg[0] = 0x0400;
	reg[1] = usValue;
	
	eMBStatus = eMBDefinitionWriteMultipleHoldingRegister( ucSlaveAddr, REG_OUTPUT_OFFSET_Commands, 0x02, reg );
	if( eMBStatus != MB_ENOERR ) {
		printDebug( MINOR_ERR, "[MODBUS_MT]: SetPointPosition Definition Error!!\r\n" );
		return eMBStatus;
	}
	
	while(ucTimeout-- > 0) {
		/* call modbus poll */
		eMBStatus = eValveMB_Poll();
		if( eMBStatus == MB_ERECVOK ) {
			return eMBStatus;
		}
		delay_ms( 100 );
	}
	
	printDebug( WARNING, "[MODBUS_MT]: timeout error occurred!!\r\n");
	return MB_ETIMEDOUT;
}
/*===================================================================================*/
/**
  * @brief  Operation command in direction OPEN or CLOSE.
  * @param  None
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eValveMB_CommandDirection( UCHAR ucSlaveAddr, UCHAR ucCommand ) {
	eMBErrorCode 	eMBStatus = MB_ENOERR;
	uint8_t 			ucTimeout = 5;
	
	/* check operation command */
	if( ucCommand == COMMAND_OPEN ) {
		eMBStatus = eMBDefinitionWriteHoldingRegister( ucSlaveAddr, REG_OUTPUT_OFFSET_Commands, 0x0100);		// 0x01 is Fieldbus OPEN
		if( eMBStatus != MB_ENOERR ) {
			printDebug( MINOR_ERR, "[MODBUS_MT]: CommandDirection Definition Error!!\r\n");
			return eMBStatus;
		}
	}
	else if( ucCommand == COMMAND_CLOSE ) {
		eMBStatus = eMBDefinitionWriteHoldingRegister( ucSlaveAddr, REG_OUTPUT_OFFSET_Commands, 0x0200);		// 0x02 is Fieldbus CLOSE
		if( eMBStatus != MB_ENOERR )
		{
			printDebug( MINOR_ERR, "[MODBUS_MT]: CommandDirection Definition Error!!\r\n");
			return eMBStatus;
		}
	}
	else {
		/* Command invalid */
		printDebug( MINOR_ERR, "[MODBUS_MT]: Command control valve invalid!!\r\n");
		return eMBStatus;
	}
 	
	while(ucTimeout-- > 0) {
		/* call modbus poll */
		eMBStatus = eValveMB_Poll();
		if( eMBStatus == MB_ERECVOK ) {
			return eMBStatus;
		}
		delay_ms( 100 );
	}

	printDebug( WARNING, "[MODBUS_MT]: timeout error occurred!!\r\n");
	return MB_ETIMEDOUT;
}
/*===================================================================================*/
/**
  * @brief  Reading input data from the actuator using register functions.
  * @param  usOffset 	: Offset or Address of register.
						ucQuality : Quality of registers.
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eValveMB_ReadInputData( UCHAR ucSlaveAddr, USHORT usOffset, UCHAR ucQuantity ) {
	eMBErrorCode 	eMBStatus = MB_ENOERR;
	uint8_t 			ucTimeout = 5;
	
	eMBStatus = eMBDefinitionReadInputRegister( ucSlaveAddr, usOffset, ucQuantity );
	if( eMBStatus != MB_ENOERR ) {
		printDebug( MINOR_ERR, "[MODBUS_MT]: ReadInputData Definition Error\r\n");
		return eMBStatus;
	}
	
 	while(ucTimeout-- > 0) {
		/* call modbus poll */
		eMBStatus = eValveMB_Poll();
		if( eMBStatus == MB_ERECVOK ) {
			return eMBStatus;
		}
		delay_ms( 100 );
	}

	printDebug( WARNING, "[MODBUS_MT]: timeout error occurred!!\r\n");
	return MB_ETIMEDOUT;
}
/*===================================================================================*/
/**
  * @brief  Operation read holding register.
  * @param  ucSlaveAddr		: Address slave device
						usAddress			: Register Address
						usQuantity		: Quantity of register
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eValveMB_ReadHolding( UCHAR ucSlaveAddr, USHORT usAddress, USHORT usQuantity ) {
	eMBErrorCode 	eMBStatus = MB_ENOERR;
	uint8_t 			ucTimeout = 5;
	
	eMBStatus = eMBDefinitionReadHoldingRegister( ucSlaveAddr, usAddress, usQuantity );
	if( eMBStatus != MB_ENOERR ) {
		printDebug( MINOR_ERR, "[MODBUS_MT]: ReadHolding Definition Error\r\n");
		return eMBStatus;
	}
	
	while(ucTimeout-- > 0) {
		/* call modbus poll */
		eMBStatus = eValveMB_Poll();
		if( eMBStatus == MB_ERECVOK ) {
			return eMBStatus;
		}
		delay_ms( 100 );
	}
	
	printDebug( WARNING, "[MODBUS_MT]: timeout error occurred!!\r\n");
	return MB_ETIMEDOUT;
}
/*===================================================================================*/
/**
  * @brief  function to call eMBMasterPoll.
  * @param  None
  * @retval None
  */
eMBErrorCode eValveMB_Poll( void ) {
	eMBErrorCode eStatus;
	eStatus = eMBMasterPoll();
	
	return eStatus;
}
/*===================================================================================================*/
/**
  * @brief  Get current position.
  * @param  None :
  * @retval current position value.
  */
ULONG ulValveMB_GetNEGTotalize( void ) {
	ULONG ulSetPoint;
	
	printDebug( INFO, "[MODBUS_MT]: ulSetPoint[%d] : 0x%x, 0x%x\r\n",INPUT_OFFSET_ActualPosition , usRegMasterInputBuf[INPUT_OFFSET_ActualPosition], usRegMasterInputBuf[INPUT_OFFSET_ActualPosition+1] );
	ulSetPoint = ( usRegMasterInputBuf[INPUT_OFFSET_ActualPosition] << 16 )
								+ ( usRegMasterInputBuf[INPUT_OFFSET_ActualPosition+1] );
	
	return ulSetPoint;
}
/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/


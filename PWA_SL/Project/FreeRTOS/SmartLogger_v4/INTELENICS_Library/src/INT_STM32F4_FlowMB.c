/**
  ******************************************************************************
  * @file    INT_STM32F4_AumaticMB.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "INT_STM32F4_FlowMB.h"
#include "main.h"
#include "stdio.h"

#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_debug.h"

/* Global variable -----------------------------------------------------------*/
extern uint32_t usRegMasterHoldingBuf[0x400];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*==========================================================================================*/
/**
  * @brief  Initial modbus for interface with control flow sensor.
  * @param  ucAddrSlave		: address slave device
						usBaudrate		: exchange speed
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eFlowMB_Intialize( UCHAR ucAddrSlave, USHORT usBaudrate ) {
	eMBErrorCode estatus = MB_ENOERR;
	
// 	printDebug_Semphr( MINOR_ERR, "[MODBUS_MT]: Initalize modbus %d\r\n", usBaudrate );
	/* Initalize modbus rtu */
	estatus  =  eMBInit ( MB_RTU , 
											ucAddrSlave ,     /* address slave device */ 
											0 , 							/* port */
											usBaudrate ,      /* exchange speed (baudrate) */ 
											MB_PAR_NONE       /* no parity */ 
											);
	if( estatus != MB_ENOERR ) {
		return estatus;
	}
	
	/* Enable modbus */
	estatus = eMBEnable( );
	return estatus;
}
/*===================================================================================================*/
/**
  * @brief  function to call eMBMasterPoll.
  * @param  None
  * @retval None
  */
eMBErrorCode vFlowMB_Poll( void ) {	
	eMBErrorCode eStatus;
	eStatus = eMBMasterPoll();
	
	return eStatus;
}
/*===================================================================================================*/
/**
  * @brief  Operation read holding register.
  * @param  ucSlaveAddr		: Address slave device
						usAddress			: Register Address
						usQuantity		: Quantity of register
  * @retval eMBErrorCode 	: Modbus error code
  */
eMBErrorCode eFlowMB_ReadHolding( UCHAR ucSlaveAddr, USHORT usAddress, USHORT usQuantity ) {
	eMBErrorCode 	estatus = MB_ENOERR;
	uint8_t ucTimeout = 5;
	
// 	usRegMasterHoldingStart = usAddress;
	estatus = eMBDefinitionReadHoldingRegister( ucSlaveAddr, usAddress, usQuantity );
	if( estatus != MB_ENOERR ) {
		/* read holding register fail */
		return estatus;
	}
	
	while(ucTimeout-- > 0) {
		/* call modbus poll */
		estatus = vFlowMB_Poll();
		if( estatus == MB_ERECVOK ) {
			return estatus;
		}
		delay_ms( 100 );
	}
	
	return MB_ETIMEDOUT;
}

/*===================================================================================================*/
/**
  * @brief  Get Flow rate value.
  * @param  None :
  * @retval Flow rate value.
  */
float ulFlowMB_GetFlowRate( void ) {
	ULONG ulFlowRate;
	
	ulFlowRate = ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_FlowRate] << 16 )
							 + ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_FlowRate+1] );
	
	return *(float *)&ulFlowRate;
}
/*===================================================================================================*/
/**
  * @brief  Get POS totalize value.
  * @param  None :
  * @retval POS Totalize value.
  */
float ulFlowMB_GetPOSTotalize( void ) {
	ULONG ulPOSTotalize;
	
	ulPOSTotalize = ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_POSTotalize] << 16 )
									+ ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_POSTotalize+1] );
	
	return ulPOSTotalize;
}
/*===================================================================================================*/
/**
  * @brief  Get NEG Totalize value.
  * @param  None :
  * @retval NEG Totalize value.
  */
float ulFlowMB_GetNEGTotalize( void ) {
	ULONG ulNEGTotalize;
	
	ulNEGTotalize = ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_NEGTotalize] << 16 )
									+ ( usRegMasterHoldingBuf[REG_HOLDING_OFFSET_NEGTotalize+1] );
	
	return ulNEGTotalize;
}
/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/


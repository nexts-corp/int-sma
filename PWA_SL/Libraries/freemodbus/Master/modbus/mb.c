/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mb.c,v 1.28 2010/06/06 13:54:40 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"

#include "INT_STM32F4_debug.h"

#if MB_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_ASCII_ENABLED == 1
// #include "mbascii.h"
#endif
#if MB_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#include <stm32f4xx.h>
#endif

/* ----------------------- Static variables ---------------------------------*/
#define REG_MASTER_HOLDING_START 	0x0001 
#define REG_MASTER_HOLDING_NREGS 	0x0300
#define REG_MASTER_COILS_START 		0x0001 
#define REG_MASTER_COILS_NREGS 		0x0020
#define REG_MASTER_INPUT_START 		0x0001 
#define REG_MASTER_INPUT_NREGS 		0x0020

USHORT   					usRegMasterHoldingStart = REG_MASTER_HOLDING_START;
__IO uint32_t   	usRegMasterHoldingBuf[REG_MASTER_HOLDING_NREGS];

USHORT   					usRegMasterCoilsStart = REG_MASTER_COILS_START;
__IO BOOL					xRegMasterCoils[REG_MASTER_COILS_NREGS];

USHORT   					usRegMasterInputStart = REG_MASTER_INPUT_START;
__IO uint32_t   	usRegMasterInputBuf[REG_MASTER_INPUT_NREGS];

// UCHAR 		*ucMBSendFrame;
static UCHAR		ucMBSendFrame[255];
static USHORT   usLength;
static UCHAR    ucMBAddress;
static UCHAR 		ucMBSlaveAddress;
//static ULONG 		ulMBAddress;
//static UCHAR 		ucMBQuantily;
static eMBMode  eMBCurrentMode;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend peMBFrameSendCur;
static pvMBFrameStart pvMBFrameStartCur;
static pvMBFrameStop pvMBFrameStopCur;
static peMBFrameReceive peMBFrameReceiveCur;
static pvMBFrameClose pvMBFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBPortCBTimerExpired ) ( void );

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress = ucSlaveAddress;
			
        switch ( eMode )
        {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            pvMBFrameStartCur = eMBRTUStart;
            pvMBFrameStopCur = eMBRTUStop;
            peMBFrameSendCur = eMBRTUSend;
            peMBFrameReceiveCur = eMBRTUReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;

            eStatus = eMBRTUInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
#if MB_ASCII_ENABLED > 0
        case MB_ASCII:
//             pvMBFrameStartCur = eMBASCIIStart;
//             pvMBFrameStopCur = eMBASCIIStop;
//             peMBFrameSendCur = eMBASCIISend;
//             peMBFrameReceiveCur = eMBASCIIReceive;
//             pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
//             pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
//             pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
//             pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;
// 				
//             eStatus = eMBASCIIInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
        default:
            eStatus = MB_EINVAL;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBCurrentMode = eMode;
                eMBState = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}

#if MB_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBTCPDoInit( ucTCPPort ) ) != MB_ENOERR )
    {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBPortEventInit(  ) )
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        pvMBFrameStartCur = eMBTCPStart;
        pvMBFrameStopCur = eMBTCPStop;
        peMBFrameReceiveCur = eMBTCPReceive;
        peMBFrameSendCur = eMBTCPSend;
        pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBTCPPortClose : NULL;
        ucMBAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
    }
    return eStatus;
}
#endif

eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}


eMBErrorCode
eMBClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur != NULL )
        {
            pvMBFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

/*================================ Polling Master and Slave ==================================*/
eMBErrorCode
eMBMasterPoll( void )
{
    static UCHAR    *ucMBFrame;
// 		static UCHAR		ucMBSendFrame[255];
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
//     static USHORT   usLength;
    static eMBException eException;

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;
	
    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
            eStatus = peMBFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == ucMBAddress ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) ||
										( ucRcvAddress == ucMBSlaveAddress ))
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );
                }
            }
            break;

        case EV_EXECUTE:
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
									break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
									ucMBFrame[MB_PDU_FUNC_OFF] = MB_PDU_RECV;
									eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );
									break;
                }
            }
						
						if( eException != MB_EX_NONE )
						{
							eException = ucMBFrame[MB_PDU_EXCE_OFF];
							printDebug_Semphr( MINOR_ERR, "[MODBUS]: Exception Codes(%d)\r\n", eException );
							return MB_ENOREG;
						}
						return MB_ERECVOK;

        case EV_FRAME_SENT:
//  						ucMBSlaveAddress = 0xF7;
// 				
// // 						printf("sendFrame : %d %d", ucMBSendFrame[1] , ucMBSendFrame[2]);
// 						for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
//             {
// 							/* No more function handlers registered. Abort. */
// 							if( xFuncHandlers[i].ucFunctionCode == 0 )
// 							{
// 								break;
// 							}
// 							else if( xFuncHandlers[i].ucFunctionCode == ucMBFunctioncode )
// 							{
// 								eException = xFuncHandlers[i].pxHandler( ucMBSendFrame, &usLength );
// 								break;
// 							}
//             }
//   					eStatus = peMBFrameSendCur( ucMBSlaveAddress, ucMBSendFrame, usLength );
							
            break;
        }
    }
    return MB_ENOERR;
}
/*==========================================================================================*/
/*=================================== Coil Status ==========================================*/
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionReadCoils( UCHAR ucSlaveAddress, ULONG ulAddress, USHORT usQuantity)
{
	eMBErrorCode    eStatus  = MB_ENOERR;
	usLength = 0;
	
	ucMBSendFrame[usLength++] = MB_PDU_SEND;
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
			eStatus = MB_EINVAL;
			return eStatus;
	}
	ucMBSlaveAddress = ucSlaveAddress;
	
// 	ucMBFunctioncode = MB_FUNC_READ_COILS;
	usRegMasterCoilsStart = ulAddress;
	
	ucMBSendFrame[usLength++] = ulAddress;
	
	ucMBSendFrame[usLength++] = usQuantity;
	
	/* Port Event Frame Sent */
 	xMBPortEventPost( EV_FRAME_SENT );
	
	return eStatus;
}
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionWriteSingleCoil( UCHAR ucSlaveAddress, ULONG ulAddress, eMBStateCoil eStateCoil)
{
	eMBErrorCode    eStatus  = MB_ENOERR;
	usLength = 0;
	
	ucMBSendFrame[usLength++] = MB_PDU_SEND;
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
		/* illegal argument */
		eStatus = MB_EINVAL;
		return eStatus;
	}
	ucMBSlaveAddress = ucSlaveAddress;
	
// 	ucMBFunctioncode = MB_FUNC_WRITE_SINGLE_COIL;
	usRegMasterCoilsStart = ulAddress;
	
	ucMBSendFrame[usLength++] = ulAddress;
	
	/* Check status coil */
	if( eStateCoil == MB_STAT_COIL_ON || eStateCoil == MB_STAT_COIL_OFF )
	{
		ucMBSendFrame[usLength++] = eStateCoil;
	}
	else
	{
		/* illegal argument */
		eStatus = MB_EINVAL;
		return eStatus;
	}
	
	/* Port Event Frame Sent */
 	xMBPortEventPost( EV_FRAME_SENT );
	
	return eStatus;
}
/*==========================================================================================*/
/*=================================== Input Register =====================================*/
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionReadInputRegister( UCHAR ucSlaveAddress, ULONG ulAddress, UCHAR ucQuantity) {
	eMBErrorCode    eStatus  = MB_ENOERR;
	
	usLength = 0;
	
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
			eStatus = MB_EINVAL;
			return eStatus;
	}
	usRegMasterInputStart = ulAddress;
	
	/* First byte contains the function code. */
	ucMBSendFrame[usLength++] = MB_FUNC_READ_INPUT_REGISTER;
	
	/* Next 2 bytes contains the address */
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress & 0xFF );
	
	/* Next 2 bytes contains the quantity */
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity & 0xFF );
	
	/* Send frame data */
	eStatus = peMBFrameSendCur( ucSlaveAddress, ucMBSendFrame, usLength );
	
	return eStatus;
}
/*==========================================================================================*/
/*=================================== Holding Register =====================================*/
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionWriteHoldingRegister( UCHAR ucSlaveAddress, ULONG ulAddress, USHORT usRegWrite) {
	eMBErrorCode    eStatus  = MB_ENOERR;
	usLength = 0;
	
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
			eStatus = MB_EINVAL;
			return eStatus;
	}
	
	/* First byte contains the function code. */
	ucMBSendFrame[usLength++] = MB_FUNC_WRITE_REGISTER;
	
	usRegMasterInputStart = ulAddress;
	
	/* Next 2 bytes contains the address */
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress & 0xFF );
	
	/* Next 2 bytes contains the quantity */
	ucMBSendFrame[usLength++] = ( UCHAR )( usRegWrite >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( usRegWrite & 0xFF );
	
	/* Send frame data */
	eStatus = peMBFrameSendCur( ucSlaveAddress, ucMBSendFrame, usLength );
	
	return eStatus;
}
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionWriteMultipleHoldingRegister( UCHAR ucSlaveAddress, ULONG ulAddress, UCHAR ucQuantity , USHORT *usRegWrite)
{
	eMBErrorCode	eStatus = MB_ENOERR;
	USHORT				usRegCount;
	USHORT				i = 0;
	
	usLength = 0;
	
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
			eStatus = MB_EINVAL;
			return eStatus;
	}
	
	/* First byte contains the function code. */
	ucMBSendFrame[usLength++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	
	usRegMasterHoldingStart = ulAddress;
	
	/* Next 2 bytes contains the address */
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress & 0xFF );
	
	/* Next 2 bytes contains the quantity */
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity & 0xFF );
	
	usRegCount = ucQuantity * 2;
	/* Next byte contains the quantity */
	ucMBSendFrame[usLength++] = usRegCount;
	
	while( i < usRegCount / 2 )
	{
		ucMBSendFrame[usLength++] = ( UCHAR )( usRegWrite[i] >> 8 );
		ucMBSendFrame[usLength++] = ( UCHAR )( usRegWrite[i] & 0xFF );
		i++;
	}
	
	/* Send frame data */
	eStatus = peMBFrameSendCur( ucSlaveAddress, ucMBSendFrame, usLength );
	
	return eStatus;
}
/*==========================================================================================*/
eMBErrorCode
eMBDefinitionReadHoldingRegister( UCHAR ucSlaveAddress, ULONG ulAddress, UCHAR ucQuantity) {
	eMBErrorCode    eStatus  = MB_ENOERR;
	
	usLength = 0;
	
	/* check preconditions */
	if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
			( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
	{
			eStatus = MB_EINVAL;
			return eStatus;
	}
	/* First byte contains the function code. */
	ucMBSendFrame[usLength++] = MB_FUNC_READ_HOLDING_REGISTER;
	
	usRegMasterHoldingStart = ulAddress;
	
	ulAddress = ulAddress - 1;
	
	/* Next 2 bytes contains the address */
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ulAddress & 0xFF );
	
	/* Next 2 bytes contains the quantity */
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity >> 8 );
	ucMBSendFrame[usLength++] = ( UCHAR )( ucQuantity & 0xFF );
	
	/* Send frame data */
	eStatus = peMBFrameSendCur( ucSlaveAddress, ucMBSendFrame, usLength );
	
	return eStatus;
}
/*===================================================================================================*/
/*======================================= Modbus Master =============================================*/
/*===================================================================================================*/

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	USHORT					usBuffer[2];
	ULONG						ulBuff;
	int             i=0;

	usAddress = usRegMasterHoldingStart;
	printDebug( DETAIL, "[MODBUS_MT]: usAddress : %d\r\nusNRegs :%d\r\neMode :%d\r\n", usAddress, usNRegs, eMode);

	if ( usNRegs <= REG_MASTER_HOLDING_NREGS ) {
		switch( eMode ) {
			case MB_REG_READ :
				while( i < usNRegs ) {
					usRegMasterHoldingBuf[usAddress+i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
					i++;
				}
// 				if( MB_SENSOR_CH == 0 ) {
// 					/*------------- Read Flow Rate ---------------*/
// 					if( usAddress == REG_HOLDING_OFFSET_FlowRate ) {
// 						while( i < usNRegs ) {
// 							usBuffer[i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
// 							i++;
// 						}
// 						printDebug( INFO, "[MODBUS_MT]: ulFlowRate : 0x%x, 0x%x\r\n", usBuffer[0], usBuffer[1] );
// 						ulBuff 											= ( usBuffer[0] << 16 ) + ( usBuffer[1] );
// 						_Sensor_Data.dataMODBUSFlow	= *(float *)&ulBuff;
// 						printDebug( INFO, "[MODBUS_MT]: ulFlowRate : %.03f, 0x%04x\r\n", _Sensor_Data.dataMODBUSFlow );
// 					}
// 					/*------------- Read POS Totalize ---------------*/
// 					else if( usAddress == REG_HOLDING_OFFSET_POSTotalize ) {
// 						while( i < usNRegs ) {
// 							usBuffer[i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
// 							i++;
// 						}
// 						printDebug( INFO, "[MODBUS_MT]: POS Total : 0x%x, 0x%x\r\n", usBuffer[0], usBuffer[1] );
// 						ulBuff 												= ( usBuffer[0] << 16 ) + ( usBuffer[1] );
// 						_Sensor_Data.dataMODBUSPosNet	= ulBuff;
// 						printDebug( INFO, "[MODBUS_MT]: POS Total : %.01f\r\n", _Sensor_Data.dataMODBUSPosNet );
// 					}
// 					/*------------- Read NEG Totalize ---------------*/
// 					else if( usAddress == REG_HOLDING_OFFSET_NEGTotalize ) {
// 						while( i < usNRegs ) {
// 							usBuffer[i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
// 							i++;
// 						}
// 						printDebug( INFO, "[MODBUS_MT]: NEG Total : 0x%x, 0x%x\r\n", usBuffer[0], usBuffer[1] );
// 						ulBuff 												= ( usBuffer[0] << 16 ) + ( usBuffer[1] );
//  						_Sensor_Data.dataMODBUSNegNet	= ulBuff;
// 						printDebug( INFO, "[MODBUS_MT]: NEG Total : %.01f\r\n", _Sensor_Data.dataMODBUSNegNet );
// 					}
// 				}
// 				else if( MB_SENSOR_CH == 1 ) {
// 					printDebug( INFO, "[MODBUS_MT]: Operation BUTTERFLY OK!!\r\n" );
// 				}
				break ; 
			case MB_REG_WRITE :				
				break ;
		}
	}
	else {
		printDebug( INFO, "illegal register address\r\n" );
		eStatus  =  MB_ENOREG ;
	}
	return  eStatus ;
}
/*===================================================================================================*/
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usNCoils, UCHAR * ucCoilsData, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus 		= MB_ENOERR;
	USHORT					usCoilIndex;
	USHORT					usCoilCNT		= 0;
	int							i;
	
	usCoilIndex = usRegMasterCoilsStart;
	if( ( usNCoils > REG_MASTER_COILS_NREGS / 8 ) ) {
		switch( eMode ) {
			case MB_REG_READ :
				while ( usNCoils > 0 ) {
					printDebug( INFO, "Coil %2d-%2d (%03d): ", usCoilIndex, (usCoilIndex+7), ucCoilsData[usCoilCNT] );
					for( i=0; i<8; i++ ) {
						if( usCoilIndex < usRegMasterCoilsStart ) {
							printDebug( INFO, "0 " );
						}
						else {
							xRegMasterCoils[usCoilIndex] = ( BOOL )( ucCoilsData[usCoilCNT] & 0x01 );
							ucCoilsData[usCoilCNT] = ucCoilsData[usCoilCNT] >> 1;
							printDebug( INFO, "%d ", xRegMasterCoils[usCoilIndex] );
						}
						usCoilIndex++;
					}
					printDebug( INFO, "\r\n" );
					usNCoils--;
					usCoilCNT++;
				}
				break;
				
			case MB_REG_WRITE :
				break;
		}
	}
	return eStatus;
}
/*===================================================================================================*/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete ) {
    return MB_ENOREG;
}
/*===================================================================================================*/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	USHORT					usBuffer[2];
	UCHAR						i=0;
	ULONG						ulBuffSetPoint;
	
	usAddress = usRegMasterInputStart;
	printDebug( DETAIL, "[MODBUS_MT]: usAddress : %d  usNRegs :%d\r\n", usAddress, usNRegs );
	
	while( i < usNRegs ) {
		usRegMasterInputBuf[usAddress+i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
		i++;
	}
// 	if( MB_SENSOR_CH == 0 ) {

// 	}
// 	else if( MB_SENSOR_CH == 1 ) {
// 		if( usAddress == INPUT_OFFSET_ActualPosition ) {
// 			while( i < usNRegs ) {
// 				usBuffer[i] = ( pucRegBuffer[i*2] << 8 ) + ( pucRegBuffer[(i*2)+1] & 0xFF );
// 				i++;
// 			}
// 			printDebug( DETAIL, "[MODBUS_MT]: Setpoint : 0x%x, 0x%x\r\n", usBuffer[0], usBuffer[1] );
//  			ulBuffSetPoint 	= ( usBuffer[1] << 16 ) + ( usBuffer[0] );
// 			ulSetPoint 			= ulBuffSetPoint / 10.0;
// 			printDebug( INFO, "[MODBUS_MT]: Setpoint : %.01f\r\n", ulSetPoint );
// 		}
// 	}
					
	return eStatus;
}
/*===================================================================================================*/
/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

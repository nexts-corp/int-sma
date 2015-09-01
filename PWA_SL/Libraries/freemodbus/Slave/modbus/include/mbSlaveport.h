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
 * File: $Id: mbport.h,v 1.19 2010/06/06 13:54:40 wolti Exp $
 */

#ifndef _MB_SLAVE_PORT_H
#define _MB_SLAVE_PORT_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Type definitions ---------------------------------*/

typedef enum
{
    SLAVE_EV_READY,                   /*!< Startup finished. */
    SLAVE_EV_FRAME_RECEIVED,          /*!< Frame received. */
    SLAVE_EV_EXECUTE,                 /*!< Execute function. */
    SLAVE_EV_FRAME_SENT               /*!< Frame sent. */
} eMBSlaveEventType;

/*! \ingroup modbus
 * \brief Parity used for characters in serial mode.
 *
 * The parity which should be applied to the characters sent over the serial
 * link. Please note that this values are actually passed to the porting
 * layer and therefore not all parity modes might be available.
 */
typedef enum
{
    MB_SLAVE_PAR_NONE,                /*!< No parity. */
    MB_SLAVE_PAR_ODD,                 /*!< Odd parity. */
    MB_SLAVE_PAR_EVEN                 /*!< Even parity. */
} eMBSlaveParity;

/* ----------------------- Supporting functions -----------------------------*/
BOOL            xMBSlavePortEventInit( void );

BOOL            xMBSlavePortEventPost( eMBSlaveEventType eEvent );

BOOL            xMBSlavePortEventGet(  /*@out@ */ eMBSlaveEventType * eEvent );

/* ----------------------- Serial port functions ----------------------------*/

BOOL            xMBSlavePortSerialInit( UCHAR ucPort, ULONG ulBaudRate,
                                   UCHAR ucDataBits, eMBSlaveParity eParity );

void            vMBSlavePortClose( void );

void            xMBSlavePortSerialClose( void );

void            vMBSlavePortSerialEnable( BOOL xRxEnable, BOOL xTxEnable );

BOOL            xMBSlavePortSerialGetByte( CHAR * pucByte );

BOOL            xMBSlavePortSerialPutByte( CHAR ucByte );

/* ----------------------- Timers functions ---------------------------------*/
BOOL            xMBSlavePortTimersInit( USHORT usTimeOut50us );

void            xMBSlavePortTimersClose( void );

void            vMBSlavePortTimersEnable( void );

void            vMBSlavePortTimersDisable( void );

void            vMBSlavePortTimersDelay( USHORT usTimeOutMS );

/* ----------------------- Callback for the protocol stack ------------------*/

/*!
 * \brief Callback function for the porting layer when a new byte is
 *   available.
 *
 * Depending upon the mode this callback function is used by the RTU or
 * ASCII transmission layers. In any case a call to xMBPortSerialGetByte()
 * must immediately return a new character.
 *
 * \return <code>TRUE</code> if a event was posted to the queue because
 *   a new byte was received. The port implementation should wake up the
 *   tasks which are currently blocked on the eventqueue.
 */
extern          BOOL( *pxMBSlaveFrameCBByteReceived ) ( void );

extern          BOOL( *pxMBSlaveFrameCBTransmitterEmpty ) ( void );

extern          BOOL( *pxMBSlavePortCBTimerExpired ) ( void );

/* ----------------------- TCP port functions -------------------------------*/
BOOL            xMBSlaveTCPPortInit( USHORT usTCPPort );

void            vMBSlaveTCPPortClose( void );

void            vMBSlaveTCPPortDisable( void );

BOOL            xMBSlaveTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength );

BOOL            xMBSlaveTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength );

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif

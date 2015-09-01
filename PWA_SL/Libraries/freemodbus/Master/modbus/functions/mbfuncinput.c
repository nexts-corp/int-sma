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
 * File: $Id: mbfuncinput.c,v 1.10 2007/09/12 10:15:56 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_RECV_READ_BYTECNT_OFF    	( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_RECV_READ_DATA_OFF        ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_SEND_READ_ADDR_OFF				( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_SEND_READ_BYTECNT_OFF    	( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               	( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX         	( 0x007D )

#define MB_PDU_FUNC_READ_RSP_BYTECNT_OFF    ( MB_PDU_DATA_OFF )

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/
#if MB_FUNC_READ_INPUT_ENABLED > 0

eMBException
eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen )
{
	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;
	
	static USHORT   usRegAddress;
	USHORT          usRegCount;
	UCHAR          *pucFrameCur;

	USHORT					usPDUStatus;
	USHORT					usByteCount;
	
	usPDUStatus = pucFrame[MB_PDU_FUNC_OFF];
	
	if( usPDUStatus == MB_PDU_RECV )
	{
		usByteCount = ( USHORT )( pucFrame[MB_PDU_FUNC_RECV_READ_BYTECNT_OFF] );

		/* Check if the byte counter to read is valid. If not
		 * return Modbus illegal data value exception. 
		 */
		if( ( usByteCount >= 1 ) && ( usByteCount < MB_PDU_FUNC_READ_REGCNT_MAX ) )
		{
			/* set the number of bytes */
			usByteCount = usByteCount / 2;

			/* Make callback to read the register values */
			eRegStatus = eMBRegInputCB( &pucFrame[MB_PDU_FUNC_RECV_READ_DATA_OFF], usRegAddress, usByteCount );
		}
		else
		{
			/* Can't be receiver protocol read input register request because the byte count is incorrect. */
			eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}/* End if */
	else if( usPDUStatus == MB_PDU_SEND )
	{
 		usRegAddress	= ( pucFrame[MB_PDU_FUNC_SEND_READ_ADDR_OFF] << 8 ) + pucFrame[MB_PDU_FUNC_SEND_READ_ADDR_OFF+1];
 		usByteCount		= ( pucFrame[MB_PDU_FUNC_SEND_READ_BYTECNT_OFF] << 8 ) + pucFrame[MB_PDU_FUNC_SEND_READ_BYTECNT_OFF+1];
		
		if( usByteCount >= 1 && usByteCount <= MB_PDU_FUNC_READ_REGCNT_MAX )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;
			
			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_READ_INPUT_REGISTER;			
			*usLen += 1;
			
			/* Next byte contains the string address high and low */
			*pucFrameCur++ = ( UCHAR )pucFrame[MB_PDU_FUNC_SEND_READ_ADDR_OFF];
			*pucFrameCur++ = ( UCHAR )pucFrame[MB_PDU_FUNC_SEND_READ_ADDR_OFF+1];
			*usLen += 2;
			
			/* Next byte contains the quantity of registers high and low */
			*pucFrameCur++ = ( UCHAR )pucFrame[MB_PDU_FUNC_SEND_READ_BYTECNT_OFF];
			*pucFrameCur++ = ( UCHAR )pucFrame[MB_PDU_FUNC_SEND_READ_BYTECNT_OFF+1];
			*usLen += 2;
		}
		else
		{
			/* Can't be send protocol read input register request because the byte count is incorrect. */
			*usLen = 0;
			eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}/* End else if */
	else
	{
		/* Can't be send or receive protocol read input register request because the status is incorrect. */
		eStatus = MB_EX_ILLEGAL_DATA_VALUE;
	}
	
	return eStatus;
}

#endif

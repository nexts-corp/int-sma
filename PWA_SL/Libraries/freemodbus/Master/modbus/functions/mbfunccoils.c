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
 * File: $Id: mbfunccoils.c,v 1.8 2007/02/18 23:47:16 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_READ_COILCNT_OFF        ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_COILDATA_OFF       ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_COILCNT_MAX        ( 0x07D0 )

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_VALUE_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE              ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF      ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF    ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN      ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX   ( 0x07B0 )

#define MB_PDU_FUCN_SEND_READ_ADDR_OFF			( MB_PDU_DATA_OFF )
#define MB_PDU_FUCN_SEND_READ_COILCNT_OFF		( MB_PDU_DATA_OFF + 1 )

#define MB_PDU_FUCN_SEND_WRITE_ADDR_OFF			( MB_PDU_DATA_OFF )
#define MB_PDU_FUCN_SEND_WRITE_STAT_OFF			( MB_PDU_DATA_OFF	+ 1)
/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_READ_COILS_ENABLED > 0

eMBException
eMBFuncReadCoils( UCHAR * pucFrame, USHORT * usLen )
{
	USHORT					usRegStatus;
	USHORT          usRegAddress;
	USHORT					usCoilWriteCount;
	USHORT          usCoilReadCount;
	UCHAR           ucNBytes;
	UCHAR					 	ucCoilsData[MB_PDU_FUNC_READ_COILCNT_MAX];
	UCHAR          *pucFrameCur;

	int i=0;
	
	/* Variable about error code and exception */
	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	usRegStatus = pucFrame[MB_PDU_FUNC_OFF];
	
	if( usRegStatus == MB_PDU_RECV )
	{
		usCoilReadCount = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF] );

		while( i < usCoilReadCount )
		{
			ucCoilsData[i] = ( UCHAR )( pucFrame[MB_PDU_FUNC_READ_COILDATA_OFF + i] );
			i++;
		}
			
		printf("ByteCount : %d (%d)\r\n", usCoilReadCount, ucCoilsData[0]);
		/* Check if the number of registers to read is valid. If not
		 * return Modbus illegal data value exception. 
		 */
		if( ( usCoilReadCount >= 1 ) && ( usCoilReadCount < MB_PDU_FUNC_READ_COILCNT_MAX ) )
		{
			eRegStatus = eMBRegCoilsCB( pucFrame, usCoilReadCount, ucCoilsData, MB_REG_READ );

			/* If an error occured convert it into a Modbus exception. */
			if( eRegStatus != MB_ENOERR )
			{
					eStatus = prveMBError2Exception( eRegStatus );
			}
			else
			{
					/* The response contains the function code, the starting address
					 * and the quantity of registers. We reuse the old values in the 
					 * buffer because they are still valid. */
					*usLen += ucNBytes;;
			}
		}
		else
		{
				/* Can't be a valid read coil register request because the length
				 * is incorrect. */
				eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}
	else if( usRegStatus == MB_PDU_SEND )
	{
		usRegAddress 			= pucFrame[MB_PDU_FUCN_SEND_READ_ADDR_OFF];
		usCoilWriteCount	= pucFrame[MB_PDU_FUCN_SEND_READ_COILCNT_OFF];
		
		if( usCoilWriteCount >= 1 && usCoilWriteCount <= MB_PDU_FUNC_READ_COILCNT_MAX )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;
		
			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_READ_COILS;			
			*usLen += 1;
			
			*pucFrameCur++ = ( UCHAR )( usRegAddress >> 8 );
			*pucFrameCur++ = ( UCHAR )( usRegAddress & 0xFF );
			*usLen += 2;
			
			*pucFrameCur++ = ( UCHAR )( usCoilWriteCount >> 8 );
			*pucFrameCur++ = ( UCHAR )( usCoilWriteCount & 0xFF );
			*usLen += 2;
		}
		else
		{
				eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}
	return eStatus;
}

#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException
eMBFuncWriteCoil( UCHAR * pucFrame, USHORT * usLen )
{
	USHORT          usRegAddress;
// 	UCHAR           ucBuf[2];
	
	USHORT					usRegStatus;
	eMBStateCoil		eStateCoil;
	UCHAR          *pucFrameCur;
	
	/* Variable about error code and exception */
	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	/*-----------------------------------------*/
	
	usRegStatus = pucFrame[MB_PDU_FUNC_OFF];
	
	if( usRegStatus == MB_PDU_RECV )
	{
		if( *usLen == ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
		{
			usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF] << 8 );
			usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1] );
			
			printf( "Write Coil : addr %d ,", usRegAddress );
			
			if( ( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF + 1] == 0x00 ) &&
					( ( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF ) ||
						( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0x00 ) ) )
			{
				if( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF )
				{
					printf( "State ON\r\n" );
				}
				else
				{
					printf( "State OFF\r\n" );
				}
			}
			else
			{
				eStatus = MB_EX_ILLEGAL_DATA_VALUE;
			}
		}/* End if usLen */
		else
		{
			/* Can't be a valid write coil register request because the length
			 * is incorrect. */
			eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}/* End if usRegStatus */
	else if( usRegStatus == MB_PDU_SEND )
	{
		usRegAddress 	= pucFrame[MB_PDU_FUCN_SEND_WRITE_ADDR_OFF];
		eStateCoil		= pucFrame[MB_PDU_FUCN_SEND_WRITE_STAT_OFF];
		
		/* Set the current PDU data pointer to the beginning. */
		pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
		*usLen = MB_PDU_FUNC_OFF;
		
		/* First byte contains the function code. */
		*pucFrameCur++ = MB_FUNC_WRITE_SINGLE_COIL;			
		*usLen += 1;
		
		/* Next two byte contains the coil address high and low */
		*pucFrameCur++ = ( UCHAR )( usRegAddress >> 8 );
		*pucFrameCur++ = ( UCHAR )( usRegAddress & 0xFF );
		*usLen += 2;
		
		if( eStateCoil == MB_STAT_COIL_ON )
		{
			/* If state coil ON next two byte contains the 0xFF and 0x00 */
			*pucFrameCur++ = ( UCHAR )( 0xFF );
			*pucFrameCur++ = ( UCHAR )( 0x00 );
			*usLen += 2;
		}
		else if( eStateCoil == MB_STAT_COIL_OFF )
		{
			/* If state coil OFF next two byte contains the 0x00 and 0x00 */
			*pucFrameCur++ = ( UCHAR )( 0x00 );
			*pucFrameCur++ = ( UCHAR )( 0x00 );
			*usLen += 2;
		}
		else
		{
			/* Can't be a valid write coil register request because 
			 * the status coil is incorrect. */
			eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}/* End eles if */
	return eStatus;
}

#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException
eMBFuncWriteMultipleCoils( UCHAR * pucFrame, USHORT * usLen )
{
    USHORT          usRegAddress;
    USHORT          usCoilCnt;
    UCHAR           ucByteCount;
    UCHAR           ucByteCountVerify;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen > ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8 );
        usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1] );
        usRegAddress++;

        usCoilCnt = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF] << 8 );
        usCoilCnt |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF + 1] );

        ucByteCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];

        /* Compute the number of expected bytes in the request. */
        if( ( usCoilCnt & 0x0007 ) != 0 )
        {
            ucByteCountVerify = ( UCHAR )( usCoilCnt / 8 + 1 );
        }
        else
        {
            ucByteCountVerify = ( UCHAR )( usCoilCnt / 8 );
        }

        if( ( usCoilCnt >= 1 ) &&
            ( usCoilCnt <= MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX ) &&
            ( ucByteCountVerify == ucByteCount ) )
        {
//             eRegStatus =
//                 eMBRegCoilsCB( &pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF],
//                                usRegAddress, usCoilCnt, MB_REG_WRITE );

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            {
                /* The response contains the function code, the starting address
                 * and the quantity of registers. We reuse the old values in the 
                 * buffer because they are still valid. */
                *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid write coil register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif

#endif

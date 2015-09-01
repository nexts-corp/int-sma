/*
 * FreeModbus Library: STM32F4xx Port
 * Copyright (C) 2013 Alexey Goncharov <a.k.goncharov@ctrl-v.biz>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stm32f4xx_conf.h"

#include "port.h"
#include "string.h"
#include "stm32f4_discovery.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        USART_ITConfig( USART6, USART_IT_RXNE, ENABLE );
    }
    else
    {
        USART_ITConfig( USART6, USART_IT_RXNE, DISABLE );
    }

    if ( xTxEnable )
    {
        USART_ITConfig( USART6, USART_IT_TXE, ENABLE );

#ifdef RTS_ENABLE
			RTS_HIGH;
#endif
    }
    else
    {
        USART_ITConfig( USART6, USART_IT_TXE, DISABLE );
    }
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	NVIC_InitTypeDef        NVIC_InitStructure;
	GPIO_InitTypeDef        GPIO_InitStructure;
	USART_InitTypeDef       USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	if( ulBaudRate == USART_Baudrate_9600 )USART_InitStructure.USART_BaudRate = 9600;
	else if( ulBaudRate == USART_Baudrate_14400 )USART_InitStructure.USART_BaudRate = 14400;
	else if( ulBaudRate == USART_Baudrate_19200 )USART_InitStructure.USART_BaudRate = 19200;
	else if( ulBaudRate == USART_Baudrate_38400 )USART_InitStructure.USART_BaudRate = 38400;
	else if( ulBaudRate == USART_Baudrate_56000 )USART_InitStructure.USART_BaudRate = 56000;
	else if( ulBaudRate == USART_Baudrate_57600 )USART_InitStructure.USART_BaudRate = 57600;
	else if( ulBaudRate == USART_Baudrate_115200 )USART_InitStructure.USART_BaudRate = 115200;

	// Customizing the parity (default - his net)
	switch( eParity )
	{
	case MB_PAR_NONE:
			USART_InitStructure.USART_Parity = USART_Parity_No;
			break;
	case MB_PAR_ODD:
			USART_InitStructure.USART_Parity = USART_Parity_Odd;
			break;
	case MB_PAR_EVEN:
			USART_InitStructure.USART_Parity = USART_Parity_Even;
			break;
	default:
			USART_InitStructure.USART_Parity = USART_Parity_No;
			break;
	};
	
	USART_DeInit( USART6 );
	
	// COM6 set Tx_pin = C6 , Rx_pin = C7 connect to USART6 ( Modbus Master )
	STM_EVAL_COMInit(COM6, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;           // канал
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // приоритет
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // приоритет субгруппы
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // включаем канал
	NVIC_Init(&NVIC_InitStructure);                             // инициализируем

	USART_Cmd( USART6, ENABLE );

	vMBPortSerialEnable( TRUE, TRUE );

#ifdef RTS_ENABLE
	RTS_INIT;
#endif

	return TRUE;
}

/* =========== function for Modbus Slave =========== */
BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    USART_SendData( USART6, (uint16_t) ucByte );
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR) USART_ReceiveData( USART6 );
    return TRUE;
}

BOOL xMBPortSerialClearByte( CHAR * pucByte )
{
    strcpy(( CHAR *) pucByte, "");
    return TRUE;
}

void USART6_IRQHandler( void )
{
    if ( USART_GetITStatus( USART6, USART_IT_RXNE ) != RESET )
    {
        USART_ClearITPendingBit( USART6, USART_IT_RXNE );
        pxMBFrameCBByteReceived();

    }
    if ( USART_GetITStatus( USART6, USART_IT_TXE ) != RESET )
    {
        USART_ClearITPendingBit( USART6, USART_IT_TXE );
        pxMBFrameCBTransmitterEmpty();
    }
}


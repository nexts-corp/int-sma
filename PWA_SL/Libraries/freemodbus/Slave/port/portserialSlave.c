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
#include "stm32f4_discovery.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mbSlave.h"
#include "mbport.h"

void vMBSlavePortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );
    }
    else
    {
        USART_ITConfig( USART2, USART_IT_RXNE, DISABLE );
    }

    if ( xTxEnable )
    {
        USART_ITConfig( USART2, USART_IT_TXE, ENABLE );

#ifdef RTS_ENABLE
        RTS_HIGH;
#endif
    }
    else
    {
        USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
    }
}

BOOL xMBSlavePortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBSlaveParity eParity )
{
	NVIC_InitTypeDef        NVIC_InitStructure;
//	GPIO_InitTypeDef        GPIO_InitStructure;
	USART_InitTypeDef       USART_InitStructure;
//	USART_ClockInitTypeDef  USART_ClockInitStructure;

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
	
	USART_DeInit( USART2 );
	
	// COM4 set Tx_pin = A2 , Rx_pin = A3 connect to USART2 ( Modbus slave )
	STM_EVAL_COMInit(COM2, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           // канал
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // приоритет
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // приоритет субгруппы
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // включаем канал
	NVIC_Init(&NVIC_InitStructure);                             // инициализируем

	USART_Cmd( USART2, ENABLE );

	vMBSlavePortSerialEnable( TRUE, TRUE );

#ifdef RTS_ENABLE
    RTS_INIT;
#endif
    return TRUE;
}

BOOL xMBSlavePortSerialPutByte( CHAR ucByte )
{
    USART_SendData( USART2, (uint16_t) ucByte );
    return TRUE;
}

BOOL xMBSlavePortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR) USART_ReceiveData( USART2 );
    return TRUE;
}

void USART2_IRQHandler( void )
{
    if ( USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
    {
        USART_ClearITPendingBit( USART2, USART_IT_RXNE );
        pxMBSlaveFrameCBByteReceived();

    }
    if ( USART_GetITStatus( USART2, USART_IT_TXE ) != RESET )
    {
        USART_ClearITPendingBit( USART2, USART_IT_TXE );
        pxMBSlaveFrameCBTransmitterEmpty();
    }
}


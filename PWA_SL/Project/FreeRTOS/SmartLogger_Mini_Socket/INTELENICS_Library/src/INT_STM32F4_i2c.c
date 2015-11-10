/**
  ******************************************************************************
  * @file    INT_STM32F4_i2c.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
#include "INT_STM32F4_i2c.h"

uint32_t I2C_Timeout;
uint32_t I2C_INT_Clocks[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

/*==========================================================================================*/
/**
  * @brief  Initialize I2C. 
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
						pinspack: Pins used
  * @retval None
  */
void vI2C_Initial( I2C_TypeDef* I2Cx, I2C_PinsPack_t pinspack, uint32_t clockSpeed ) {
	I2C_InitTypeDef I2C_InitStruct;
	
	if( I2Cx ==I2C1 ) {
		/* Enable clock */
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );
		/* Enable pins */
		I2C1_InitPins( pinspack );
		
		/* Check clock */
		if ( clockSpeed < I2C_INT_Clocks[0] ) {
			I2C_INT_Clocks[0] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = I2C_INT_Clocks[0];
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C1_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = I2C1_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = I2C1_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = I2C1_ACK;
		I2C_InitStruct.I2C_DutyCycle = I2C1_DUTY_CYCLE;
	}
	
	/* InitializeI2C */
	I2C_Init( I2Cx, &I2C_InitStruct );
	/* EnableI2C */
	I2C_Cmd( I2Cx, ENABLE );
}
/*==========================================================================================*/
/**
  * @brief  Initialize GPIO I2C1. 
  * @param  pinspack: I2C_PinsPack_1:
	* 									- SCL: PB6
	* 									- SDA: PB7
	* 									I2C_PinsPack_2:
	* 									- SCL: PB8
	* 									- SDA: PB9
  * @retval None
  */
void I2C1_InitPins( I2C_PinsPack_t pinspack ) {
	GPIO_InitTypeDef GPIO_InitDef;
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitDef.GPIO_OType = GPIO_OType_OD;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	
	if ( pinspack == I2C_PinsPack_1 ) {
		//                      SCL          SDA
		GPIO_InitDef.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource6, GPIO_AF_I2C1 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_I2C1 );
	} else if ( pinspack == I2C_PinsPack_2 ) {
		//                      SCL          SDA
		GPIO_InitDef.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_I2C1 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource9, GPIO_AF_I2C1 );
	} else if ( pinspack == I2C_PinsPack_3 ) {
		//                      SCL          SDA
		GPIO_InitDef.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
		
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource6, GPIO_AF_I2C1 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource9, GPIO_AF_I2C1 );
	}
	GPIO_Init( GPIOB, &GPIO_InitDef );
}
/*==========================================================================================*/
/**
  * @brief  Read single byte from slave. 
  * @param  I2Cx: I2C used.
	* 				address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used.
	*					reg: register to read from.
  * @retval Data from slave returned.
  */
uint8_t I2C_Read( I2C_TypeDef*I2Cx, uint8_t address, uint8_t reg ) {
	uint8_t received_data;
	I2C_Start( I2Cx, address,I2C_Direction_Transmitter, 0 );
	I2C_WriteData( I2Cx, reg );
	I2C_Stop( I2Cx );
	I2C_Start( I2Cx, address,I2C_Direction_Receiver, 0 );
	received_data = I2C_ReadNack( I2Cx );
	return received_data;
}
/*==========================================================================================*/
/**
  * @brief  Write single byte from slave. 
  * @param  I2Cx: I2C used.
	* 				address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used.
	*					reg: register to write from.
  * @retval None.
  */
void I2C_Write( I2C_TypeDef*I2Cx, uint8_t address, uint8_t reg, uint8_t data ) {
	I2C_Start( I2Cx, address,I2C_Direction_Transmitter, 0 );
	I2C_WriteData( I2Cx, reg );
	I2C_WriteData( I2Cx, data );
	I2C_Stop( I2Cx );
}
/*==========================================================================================*/
/**
  * @brief  Read multi bytes from slave. 
  * @param  I2Cx: I2C used.
	* 				address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used.
	*					reg: register to read from.
	*					*data: pointer to data array to store data from slave.
	*					count: how many bytes will be read.
  * @retval None
  */
void I2C_ReadMulti( I2C_TypeDef*I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count ) {
	uint8_t i;
	I2C_Start( I2Cx, address,I2C_Direction_Transmitter, 1 );
	I2C_WriteData( I2Cx, reg );
	I2C_Stop( I2Cx );
	I2C_Start( I2Cx, address,I2C_Direction_Receiver, 1 );
	for ( i = 0; i < count; i++ ) {
		if ( i == ( count - 1 ) ) {
			//Last byte
			data[i] = I2C_ReadNack( I2Cx );
		} else {
			data[i] = I2C_ReadAck( I2Cx );
		}
	}
}
/*==========================================================================================*/
/**
  * @brief  Write multi bytes from slave. 
  * @param  I2Cx: I2C used.
	* 				address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used.
	*					reg: register to write to.
	*					*data: pointer to data array to store data from slave.
	*					count: how many bytes will be written.
  * @retval None
  */
void I2C_WriteMulti( I2C_TypeDef*I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count ) {
	uint8_t i;
	I2C_Start( I2Cx, address,I2C_Direction_Transmitter, 0 );
	I2C_WriteData( I2Cx, reg );
	for ( i = 0; i < count; i++ ) {
		I2C_WriteData( I2Cx, data[i] );
	}
	I2C_Stop( I2Cx );
}
/*==========================================================================================*/
/**
  * @brief  I2C Start condition. 
  * @param  I2Cx: I2C used.
	* 				address: slave address.
	*					direction: master to slave or slave to master.
	*					ack: ack enabled or disabled.
  * @retval 0: OK
	*					1: Timeout
  */
int16_t I2C_Start( I2C_TypeDef*I2Cx, uint8_t address, uint8_t direction, uint8_t ack ) {
	I2C_GenerateSTART( I2Cx, ENABLE );
	
	I2C_Timeout = I2C_TIMEOUT;
	while ( !I2C_GetFlagStatus( I2Cx,I2C_FLAG_SB ) && I2C_Timeout ) {
		if ( --I2C_Timeout == 0x00 ) {
			return 1;
		}
	}

	if ( ack ) {
		I2C_AcknowledgeConfig( I2C1, ENABLE );
	}
	
	I2C_Send7bitAddress( I2Cx, address, direction );

	if ( direction ==I2C_Direction_Transmitter ) {
		I2C_Timeout = I2C_TIMEOUT;
		while ( !I2C_GetFlagStatus( I2Cx,I2C_FLAG_ADDR ) && I2C_Timeout ) {
			if ( --I2C_Timeout == 0x00 ) {
				return 1;
			}
		}
	} else if ( direction ==I2C_Direction_Receiver ) {
		I2C_Timeout = I2C_TIMEOUT;
		while ( !I2C_CheckEvent( I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) && I2C_Timeout ) {
			if ( --I2C_Timeout == 0x00 ) {
				return 1;
			}
		}
	}
	I2Cx->SR2;
	
	return 0;
}

/*==========================================================================================*/
/**
  * @brief  Write to slave. 
  * @param  I2Cx: I2C used.
	*					data: data to be sent.
  * @retval None.
  */
void I2C_WriteData( I2C_TypeDef*I2Cx, uint8_t data ) {
	I2C_Timeout = I2C_TIMEOUT;
	while ( !I2C_GetFlagStatus( I2Cx,I2C_FLAG_TXE ) && I2C_Timeout ) {
		I2C_Timeout--;
	}
	I2C_SendData( I2Cx, data );
}

/*==========================================================================================*/
/**
  * @brief  Read byte with ack. 
  * @param  I2Cx: I2C used.
  * @retval data read.
  */
uint8_t I2C_ReadAck( I2C_TypeDef*I2Cx ) {
	uint8_t data;
	I2C_AcknowledgeConfig( I2Cx, ENABLE );
	
	I2C_Timeout = I2C_TIMEOUT;
	while ( !I2C_CheckEvent( I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED ) && I2C_Timeout ) {
		I2C_Timeout--;
	}

	data =I2C_ReceiveData( I2Cx );
	return data;
}

/*==========================================================================================*/
/**
  * @brief  Read byte without ack. 
  * @param  I2Cx: I2C used.
  * @retval data read.
  */
uint8_t I2C_ReadNack( I2C_TypeDef*I2Cx ) {
	uint8_t data;
	
	I2C_AcknowledgeConfig( I2Cx, DISABLE );
	
	I2C_GenerateSTOP( I2Cx, ENABLE );
	
	I2C_Timeout = I2C_TIMEOUT;
	while ( !I2C_CheckEvent( I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED ) && I2C_Timeout ) {
		I2C_Timeout--;
	}

	data =I2C_ReceiveData( I2Cx );
	return data;
}

/*==========================================================================================*/
/**
  * @brief  Stop condition. 
  * @param  I2Cx: I2C used
  * @retval 0: OK
	*					1: Timeout
  */
uint8_t I2C_Stop( I2C_TypeDef*I2Cx ) {	
	I2C_Timeout = I2C_TIMEOUT;
	while ( ( ( !I2C_GetFlagStatus( I2Cx,I2C_FLAG_TXE ) ) || ( !I2C_GetFlagStatus( I2Cx,I2C_FLAG_BTF ) ) ) && I2C_Timeout ) {
		if ( --I2C_Timeout == 0x00 ) {
			return 1;
		}
	}
	
	I2C_GenerateSTOP( I2Cx, ENABLE );
	
	return 0;
}

/*==========================================================================================*/
/**
  * @brief  Checks if device is connected to I2C bus. 
  * @param  I2Cx: I2C used.
	* 				address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used.
  * @retval Returns 1 if device is connected, or 0 if not.
  */
uint8_t I2C_IsDeviceConnected( I2C_TypeDef*I2Cx, uint8_t address ) {
	uint8_t connected = 0;
	if ( !I2C_Start( I2Cx, address,I2C_Direction_Transmitter, 1 ) ) {
		connected = 1;
	}
	I2C_Stop( I2Cx );
	
	return connected;
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

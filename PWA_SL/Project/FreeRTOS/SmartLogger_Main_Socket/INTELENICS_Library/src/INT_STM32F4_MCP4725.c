/**
  ******************************************************************************
  * @file    INT_STM32F4_AumaticMB.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
#include <string.h>
#include "stm32f4xx.h"
#include "SL_PinDefine.h"
#include "SL_ConfigStruct.h"
#include "main.h"
#include "INT_STM32F4_MCP4725.h"
#include "INT_STM32F4_debug.h"

/*==========================================================================================*/
/**
  * @brief  This funcion initializes the I2C1 peripheral
  * @param  
  * @retval None
  */
void MCP4725_InitI2C1(void) {
		
		GPIO_InitTypeDef 		GPIO_InitStruct; 		// this is for the GPIO pins used as I2C1SDA and I2C1SCL
		GPIO_InitTypeDef 		GPIO_Output;
		I2C_InitTypeDef 		I2C_InitStruct; 		// this is for the I2C1 initilization
	
		/* enable APB1 peripheral clock for I2C1 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
		/* enable the peripheral clock for the pins used by 
		* I2C1, PB6 for I2C SCL and PB9 for I2C1_SDL
		*/
		/* GPIOB clock enable */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
		/* This sequence sets up the I2C1SDA and I2C1SCL pins 
		* so they work correctly with the I2C1 peripheral
		*/
		GPIO_InitStruct.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN; // Pins 6 (I2C1_SCL) and 9 (I2C1_SDA) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			// the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;		// this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// this defines the output type as open drain mode (as opposed to push pull)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pullup resistors on the IO pins
		GPIO_Init(GPIOB, &GPIO_InitStruct);					// now all the values are passed to the GPIO_Init() function which sets the GPIO registers
	
		/* The I2C1_SCL and I2C1_SDA pins are now connected to their AF
		* so that the USART1 can take over control of the 
		* pins
		*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1); //
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
		/* Configure I2C1 */
		/* I2C DeInit */
		I2C_DeInit(I2C1);
    
		/* Enable the I2C peripheral */
		I2C_Cmd(I2C1, ENABLE);
 
		/* Set the I2C structure parameters */
		I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
		I2C_InitStruct.I2C_OwnAddress1 = 0xEE;
		I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStruct.I2C_ClockSpeed = 30000;
  
		/* Initialize the I2C peripheral w/ selected parameters */
		I2C_Init(I2C1, &I2C_InitStruct);
}
/*==========================================================================================*/
/**
  * @brief  This funcion initializes the I2C1 peripheral
  * @param  
  * @retval None
  */
uint8_t MCP4725_Write(uint16_t Addr, uint32_t Data) {
	
	uint32_t 	timeout = I2C_TIMEOUT_MAX;
	uint8_t 	upper_Data,	lower_Data;
	
	upper_Data = Data >> 4;
	lower_Data = Data%16 << 4;
	
	/* Generate the Start Condition */
	I2C_GenerateSTART(I2C1, ENABLE);
// 	printDebug_Semphr(INFO, "[SENSOR_TASK]: 0x%X = 0x%X, 0x%X\r\n", Data, upper_Data, lower_Data);
	
	/* Test on I2C1 EV5, Start trnsmitted successfully and clear it */
	timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;	
	}

	/* Send Memory device slave Address for write */
	I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Transmitter);

	/* Test on I2C1 EV6 and clear it */
	timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}

 	/* Send Data */
	I2C_SendData(I2C1, MCP4726_CMD_WRITEDAC);

	/* Test on I2C1 EV8 and clear it */
	timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
	
	/* Send I2C1 location address MSB */
	I2C_SendData(I2C1, upper_Data);
	
	/* Test on I2C1 EV8 and clear it */
	timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}

	/* Send I2C1 location address LSB */
	I2C_SendData(I2C1, lower_Data);

	/* Test on I2C1 EV8 and clear it */
	timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}

	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	/* If operation is OK, return 0 */
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  Sets the output voltage to a fraction of source vref.
  * @param  Value : 0-4095
  * @retval None
  */
uint8_t MCP4725_SetVoltage(uint16_t Value) {
	if( MCP4725_Write( MCP4725_ADDRESS, Value ) == 0xff ){
		printDebug_Semphr( MINOR_ERR, "[SENSOR_TASK]: MCP4725 write error\r\n" );
		return 0xff;
	}
	
	/* If operation is OK, return 0 */
	return 0;
}
/*==========================================================================================*/

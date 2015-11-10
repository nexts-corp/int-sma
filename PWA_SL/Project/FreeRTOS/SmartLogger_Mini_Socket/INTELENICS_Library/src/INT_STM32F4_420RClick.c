/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_420RClick.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	26-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "main.h"
#include "SL_PinDefine.h"     // Pin & Port
#include "Sensors_Task.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_420RClick.h"
#include "INT_STM32F4_ProtocolComm.h"

const uint32_t 	RCLICK_PORT_CSn[MAX_420SENSOR] = {RCLICK_PORT_CS1, RCLICK_PORT_CS2, RCLICK_PORT_CS3,
																									RCLICK_PORT_CS4, RCLICK_PORT_CS5, RCLICK_PORT_CS6};
const uint16_t	RCLICK_PIN_CSn[MAX_420SENSOR] = {RCLICK_PIN_CS1, RCLICK_PIN_CS2, RCLICK_PIN_CS3,
																									RCLICK_PIN_CS4, RCLICK_PIN_CS5, RCLICK_PIN_CS6};

__IO uint32_t 	_ADC_TClick = 0;

/*==========================================================================================*/
/**
  * @brief  Initial SPI1 for interface with 4-20mA R Click board
  * @param  
  * @retval None
  */
void v420RCLICK_InitialSPI1(void) {
	
	GPIO_InitTypeDef 	GPIO_InitStruct;
	SPI_InitTypeDef 	SPI_InitStruct;
	
	/* Enable peripheral clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* Enable clock for used IO pins */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	/* Configure pins used by SPI1
	 * PB3 = SCK
	 * PB4 = MISO
	 * PB5 = MOSI
	 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_SPI_SCK | RCLICK_SPI_MISO | RCLICK_SPI_MOSI;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RCLICK_SPI_PORT, &GPIO_InitStruct);
	
	/* Connect SPI1 pins to SPI alternate function */
	GPIO_PinAFConfig(RCLICK_SPI_PORT, RCLICK_SCK_AF, GPIO_AF_SPI1);
	GPIO_PinAFConfig(RCLICK_SPI_PORT, RCLICK_MOSI_AF, GPIO_AF_SPI1);
	GPIO_PinAFConfig(RCLICK_SPI_PORT, RCLICK_MISO_AF, GPIO_AF_SPI1);
	
	/* Configure the chip select pin, 4-20mA Receiver 1 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS1, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 2 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS2;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS2, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 3 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS3;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS3, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 4 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS4;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS4, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 5 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS5;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS5, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 6 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_CS6;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_CS6, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 7 */
	GPIO_InitStruct.GPIO_Pin = RCLICK_PIN_VALiN;
	GPIO_Init((GPIO_TypeDef*)RCLICK_PORT_VALiN, &GPIO_InitStruct);
	/* Configure the chip select pin, 4-20mA Receiver 8 */
	GPIO_InitStruct.GPIO_Pin = TCLICK_PIN_VALVE;
	GPIO_Init((GPIO_TypeDef*)TCLICK_PORT_VALVE, &GPIO_InitStruct);
	
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS1, RCLICK_PIN_CS1); 							//set CS1 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS2, RCLICK_PIN_CS2); 							//set CS2 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS3, RCLICK_PIN_CS3); 							//set CS3 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS4, RCLICK_PIN_CS4); 							//set CS4 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS5, RCLICK_PIN_CS5); 							//set CS5 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CS6, RCLICK_PIN_CS6); 							//set CS6 high
	GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_VALiN, RCLICK_PIN_VALiN); 					//set CS7 high
	GPIO_SetBits((GPIO_TypeDef*)TCLICK_PORT_VALVE, TCLICK_PIN_VALVE); 					//set CS8 high
	
	/* configure SPI1 in Mode 0 
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction 					= SPI_Direction_2Lines_FullDuplex; 							/* set to full duplex mode, seperate MOSI and MISO lines */
	SPI_InitStruct.SPI_Mode 							= SPI_Mode_Master;     													/* transmit in master mode, NSS pin has to be always high */
	SPI_InitStruct.SPI_DataSize 					= SPI_DataSize_8b; 															/* one packet of data is 8 bits wide */
	SPI_InitStruct.SPI_CPOL 							= SPI_CPOL_Low;        													/* clock is low when idle */
	SPI_InitStruct.SPI_CPHA 							= SPI_CPHA_1Edge;      													/* data sampled at first edge */
	SPI_InitStruct.SPI_NSS 								= SPI_NSS_Soft | SPI_NSSInternalSoft_Set; 			/* set the NSS management to internal and pull internal NSS high */
	SPI_InitStruct.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_256; 										/* SPI frequency is APB2 frequency / 4 */
	SPI_InitStruct.SPI_FirstBit 					= SPI_FirstBit_MSB;															/* data is transmitted MSB first */
	SPI_Init(SPI1, &SPI_InitStruct); 
	
	/* Enable SPI1 */
	SPI_Cmd(SPI1, ENABLE);
}
/*==========================================================================================*/
/**
  * @brief  This funtion is used to Transmit(T-Click) and Receive(R-Click) data
  * @param  
  * @retval Read data
  */
uint8_t uc420RClick_ReadWriteData(SPI_TypeDef* SPIx, uint16_t writeData) {

	SPIx->DR = writeData; 																					/* write data to be transmitted to the SPI data register */
	while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); 												/* wait until transmit complete */
	while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); 											/* wait until receive complete */
	while( SPIx->SR & SPI_I2S_FLAG_BSY ); 													/* wait until SPI is not busy anymore */
	return SPIx->DR; 																								/* return received data from SPI data register */
}
/*==========================================================================================*/
/**
  * @brief  This funtion is used to get dataADC from R-Click Board and average,filter data
  * @param  
  * @retval ADC data
  */
uint16_t us420RClick_GetADCValue(uint8_t ch) {
	
	uint32_t 	ADC_sum = 0;
	uint16_t 	ADCValue = 0;
	uint16_t 	ADC_buff[18];
	uint16_t  ADCmax = 0, ADCmin = 0x0FFF;
	uint8_t 	i = 0;
	uint8_t 	adc_h, adc_l;
	uint8_t		ch_index = ch-1;
	
	for(i = 0; i < 18; i++) {
		if(ch == IN420MA_VALVE) {																															/*-- Butterfly Valve --*/
			GPIO_ResetBits((GPIO_TypeDef*)RCLICK_PORT_VALiN, RCLICK_PIN_VALiN);										/* Set SPI->CS low	*/
			adc_h = uc420RClick_ReadWriteData(SPI1, 0x12) & 0x1F;            											/* Get upper 4 bits of ADC value */
			adc_l = uc420RClick_ReadWriteData(SPI1, 0x12);                   											/* Get upper 8 bits of ADC value */
			GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_VALiN, RCLICK_PIN_VALiN);											/* Set SPI->CS high */
		}else {																																								/*-- Sensor --*/
			GPIO_ResetBits((GPIO_TypeDef*)RCLICK_PORT_CSn[ch_index], RCLICK_PIN_CSn[ch_index]);		/* Set SPI->CS low	*/
			adc_h = uc420RClick_ReadWriteData(SPI1, 0x12) & 0x1F;            											/* Get upper 4 bits of ADC value */
			adc_l = uc420RClick_ReadWriteData(SPI1, 0x12);                   											/* Get upper 8 bits of ADC value */
			GPIO_SetBits((GPIO_TypeDef*)RCLICK_PORT_CSn[ch_index], RCLICK_PIN_CSn[ch_index]);			/* Set SPI->CS high */
		}
			
		ADC_buff[i] = ( ( adc_h  << 8 ) | adc_l) >> 1; 																					/* Add ADC value to checkup array */
		ADC_sum += (uint32_t)ADC_buff[i];																												/* Sum ADC values */
		if(ADC_buff[i] < ADCmin) {																															/* Search the array for MIN value */
			ADCmin = ADC_buff[i];
		}
		if(ADC_buff[i] > ADCmax) {																															/* Search the array for MAX value */
			ADCmax = ADC_buff[i];
		}
		delay_ms(5);
	}
	
	ADC_sum -= ADCmin;																																				/* Remove the MIN value from sum */
	ADC_sum -= ADCmax;																																				/* Remove the MAX value from sum */
	
	ADC_sum >>= 4;																																						/* Shift bits 4 places - divide the sum by 16 (2 to the power of 4) */
	ADCValue = (uint16_t)(ADC_sum + (ADC_sum/100));																						/* Final value correction ~1% (Shunt resistor tolerance) */
	
	return ADCValue;
}
/*==========================================================================================*/
/**
  * @brief  
  * @param  Valve open percent.
  * @retval None.
  */
void v420TClick_ControlValve(uint32_t percent) {
	
  uint8_t 	temp			=	0;
	float  		mAValue		= 0.0;
  uint32_t 	adcValue 	= 0;
	
	/*-- Assert parameter --*/
	if(percent > 100) {
		printDebug_Semphr(MINOR_ERR, "<420TClick_ControlValve>: Valve open out of range %d \r\n", percent);
		return;
	}
	
	// Convert % to mA value
	mAValue = (0.16 * (float)percent) + 4.0;
	// Convert mA to ADC value
	if(struct_Config420ValveOutCal.calculation == CAL_USE_CALIBRATE) {
		adcValue = (uint32_t)(TCLICK_SLOPE * mAValue) + struct_Config420ValveOutCal.constant;
		_ADC_TClick = adcValue;
	}else {
		adcValue = (uint32_t)(TCLICK_SLOPE * mAValue);
		_ADC_TClick = adcValue;
	}
		
	printDebug_Semphr(INFO, "<420TClick_ControlValve>: ValveOpen: %d%%, ADC: %d \r\n", percent, adcValue);
		
	// Command to T Click
	GPIO_ResetBits((GPIO_TypeDef*)TCLICK_PORT_VALVE, TCLICK_PIN_VALVE);      		// Set CS_Pin low, Select 4-20mA chip
	// Send High Byte
	temp = (adcValue >> 8) & 0x0F;    																					// Store valueTemp[11..8] to temp[3..0]
	temp |= 0x30;                      																					// Define DAC setting, see MCP4921 datasheet
	uc420RClick_ReadWriteData(SPI1, temp);                  										// Send high byte via SPI
	// Send Low Byte
	temp = adcValue;                  																					// Store valueTemp[7..0] to temp[7..0]
	uc420RClick_ReadWriteData(SPI1, temp);                  										// Send low byte via SPI
	GPIO_SetBits((GPIO_TypeDef*)TCLICK_PORT_VALVE, TCLICK_PIN_VALVE);        		// Set CS_Pin high, Deselect DAC chip
}
/*==========================================================================================*/



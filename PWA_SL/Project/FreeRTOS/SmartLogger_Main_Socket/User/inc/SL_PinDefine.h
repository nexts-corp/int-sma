/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	SL_PinDefine.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	19-June-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#ifndef _SL_PINDEFINE_H
#define _SL_PINDEFINE_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"

/* Digital Input --------------------------------------------------------------------*/
#define MAX_DIN								12
#define DIN1_PORT							(uint32_t)GPIOE							
#define DIN1_PIN							GPIO_Pin_3				/* Digital Input Channel 1 */
#define DIN2_PORT							(uint32_t)GPIOE							
#define DIN2_PIN							GPIO_Pin_4				/* Digital Input Channel 2 */
#define DIN3_PORT							(uint32_t)GPIOE							
#define DIN3_PIN							GPIO_Pin_5				/* Digital Input Channel 3 */
#define DIN4_PORT							(uint32_t)GPIOE						
#define DIN4_PIN							GPIO_Pin_6				/* Digital Input Channel 4 */
#define DIN5_PORT							(uint32_t)GPIOE
#define DIN5_PIN							GPIO_Pin_7				/* Digital Input Channel 5 */
#define DIN6_PORT							(uint32_t)GPIOE
#define DIN6_PIN							GPIO_Pin_8				/* Digital Input Channel 6 */
#define DIN7_PORT							(uint32_t)GPIOE
#define DIN7_PIN							GPIO_Pin_9				/* Digital Input Channel 7 */
#define DIN8_PORT							(uint32_t)GPIOE
#define DIN8_PIN							GPIO_Pin_10				/* Digital Input Channel 8 */
#define DIN9_PORT							(uint32_t)GPIOE
#define DIN9_PIN							GPIO_Pin_11				/* Digital Input Channel 9 */
#define DIN10_PORT						(uint32_t)GPIOE
#define DIN10_PIN							GPIO_Pin_12				/* Digital Input Channel 10 */
#define DIN11_PORT						(uint32_t)GPIOE
#define DIN11_PIN							GPIO_Pin_13				/* Digital Input Channel 11 */
#define DIN12_PORT						(uint32_t)GPIOE
#define DIN12_PIN							GPIO_Pin_14				/* Digital Input Channel 12 */
extern const uint32_t 				DINn_PORT[MAX_DIN];
extern const uint16_t					DINn_PIN[MAX_DIN];

/* Digital Output --------------------------------------------------------------------*/
#define MAX_DOUT							4
#define DOUT1_PORT						(uint32_t)GPIOB							
#define DOUT1_PIN							GPIO_Pin_11				/* Digital Output Channel 1 */
#define DOUT2_PORT						(uint32_t)GPIOB							
#define DOUT2_PIN							GPIO_Pin_12				/* Digital Output Channel 2 */
#define DOUT3_PORT						(uint32_t)GPIOB							
#define DOUT3_PIN							GPIO_Pin_13				/* Digital Output Channel 3 */
#define DOUT4_PORT						(uint32_t)GPIOB						
#define DOUT4_PIN							GPIO_Pin_14				/* Digital Output Channel 4 */
extern const uint32_t 				DOUTn_PORT[MAX_DOUT];
extern const uint16_t					DOUTn_PIN[MAX_DOUT];
#define SELECT_RF_PIN					GPIO_Pin_0				/* GSM/GPRS Module ON/OFF */
#define SELECT_RF_PORT				GPIOD
#define XBEE_RESRT_PORT				GPIOD		
#define XBEE_RESRT_PIN				GPIO_Pin_5				/* XBee Module Reset */
#define GSM_SWITCH_PORT				GPIOD		
#define GSM_SWITCH_PIN				GPIO_Pin_6				/* GSM/GPRS Module ON/OFF */
#define GSM_EMER_PORT					GPIOD		
#define GSM_EMER_PIN					GPIO_Pin_7				/* GSM/GPRS Module Emergency OFF */
#define GSM_PWR_PORT					GPIOB		
#define GSM_PWR_PIN						GPIO_Pin_14				/* GSM/GPRS Module Power */

#define LED_CSQ_PORT					GPIOD		
#define LED_CSQ_PIN						GPIO_Pin_12				/* GSM Signal Status */
#define LED_CSQ_RED						GPIO_SetBits((GPIO_TypeDef*)LED_CSQ_PORT, LED_CSQ_PIN)
#define LED_CSQ_GREEN					GPIO_ResetBits((GPIO_TypeDef*)LED_CSQ_PORT, LED_CSQ_PIN)
#define LED_CONNSERVER_PORT		GPIOD		
#define LED_CONNSERVER_PIN		GPIO_Pin_13				/* Connect Server Status */
#define LED_CONNSERVER_RED		GPIO_SetBits((GPIO_TypeDef*)LED_CONNSERVER_PORT, LED_CONNSERVER_PIN)
#define LED_CONNSERVER_GREEN	GPIO_ResetBits((GPIO_TypeDef*)LED_CONNSERVER_PORT, LED_CONNSERVER_PIN)
#define LED_SAMP_PORT					GPIOD
#define LED_SAMP_PIN					GPIO_Pin_11				/* Sampling Data */
#define LED_SAMP_ON						GPIO_SetBits((GPIO_TypeDef*)LED_SAMP_PORT, LED_SAMP_PIN)
#define LED_SAMP_OFF					GPIO_ResetBits((GPIO_TypeDef*)LED_SAMP_PORT, LED_SAMP_PIN)
#define LED_SENT_PORT					GPIOD		
#define LED_SENT_PIN					GPIO_Pin_10				/* Sent Data */
#define LED_SENT_ON						GPIO_SetBits((GPIO_TypeDef*)LED_SENT_PORT, LED_SENT_PIN)
#define LED_SENT_OFF					GPIO_ResetBits((GPIO_TypeDef*)LED_SENT_PORT, LED_SENT_PIN)
#define LED_SD_PORT						GPIOD		
#define LED_SD_PIN						GPIO_Pin_14				/* SD Card Status */
#define LED_SD_RED						GPIO_SetBits((GPIO_TypeDef*)LED_SD_PORT, LED_SD_PIN)
#define LED_SD_GREEN					GPIO_ResetBits((GPIO_TypeDef*)LED_SD_PORT, LED_SD_PIN)
#define LED_ERR_PORT					GPIOA		
#define LED_ERR_PIN						GPIO_Pin_1				/* Error Indicator */
#define LED_ERR_RED						GPIO_SetBits((GPIO_TypeDef*)LED_ERR_PORT, LED_ERR_PIN)
#define LED_ERR_GREEN					GPIO_ResetBits((GPIO_TypeDef*)LED_ERR_PORT, LED_ERR_PIN)

#define BUZZER_PORT						GPIOD		
#define BUZZER_PIN						GPIO_Pin_15				/* Buzzer */
#define BUZZER_ON							GPIO_SetBits((GPIO_TypeDef*)BUZZER_PORT, BUZZER_PIN)
#define BUZZER_OFF						GPIO_ResetBits((GPIO_TypeDef*)BUZZER_PORT, BUZZER_PIN)
extern __IO bool _buzzer_2hz;

/* One Wire --------------------------------------------------------------------------*/
#define ONEWIRE_RCC						RCC_AHB1Periph_GPIOD
#define ONEWIRE_PORT					GPIOD		
#define ONEWIRE_PIN						GPIO_Pin_1				/* 1 Wire */

#define ONEWIRE_DHT11_RCC			RCC_AHB1Periph_GPIOE
#define ONEWIRE_DHT11_PORT		GPIOE		
#define ONEWIRE_DHT11_PIN			GPIO_Pin_13				/* 1 Wire DHT11 Temparetuer Sensor*/
/* I2C -------------------------------------------------------------------------------*/
#define I2C_SCL_PORT					GPIOB	
#define I2C_SCL_PIN						GPIO_Pin_8
#define I2C_SDA_PORT					GPIOB		
#define I2C_SDA_PIN						GPIO_Pin_9

/* Analog Input ----------------------------------------------------------------------*/
#define AIN1_BATT_CH					ADC_Channel_10
#define AIN1_BATT_PORT				GPIOC
#define AIN1_BATT_PIN					GPIO_Pin_0				/* Analog Input Channel 1 (Battery Voltage) */
#define AIN2_SOLARCELL_CH			ADC_Channel_11
#define AIN2_SOLARCELL_PORT		GPIOC
#define AIN2_SOLARCELL_PIN		GPIO_Pin_1				/* Analog Input Channel 1 (Solar Cell Voltage) */
#define AIN3_ADC1_CH					ADC_Channel_12
#define AIN3_ADC1_PORT				GPIOC
#define AIN3_ADC1_PIN					GPIO_Pin_2				/* Analog Input Channel 1 (0-5 VDC) */
#define AIN4_ADC2_CH					ADC_Channel_13
#define AIN4_ADC2_PORT				GPIOC
#define AIN4_ADC2_PIN					GPIO_Pin_3				/* Analog Input Channel 1 (0-10 VDC) */

#define AIN1_ACVOLT_CH				ADC_Channel_10
#define AIN1_ACVOLT_PORT			GPIOC
#define AIN1_ACVOLT_PIN				GPIO_Pin_0				/* Analog Input Channel 1 (AC Voltoage Sensor) */
#define AIN2_CURRENT_CH				ADC_Channel_11
#define AIN2_CURRENT_PORT			GPIOC
#define AIN2_CURRENT_PIN			GPIO_Pin_1				/* Analog Input Channel 1 (Current Sensor) */

/* Pin for 4-20mA RClick Board (SPI1) ------------------------------------------------*/
#define RCLICK_SPI_PORT				GPIOA							/* GPIO PORT for SPI1->SCK, SPI1->MISO and SPI1->MOSI */
#define RCLICK_SPI_SCK				GPIO_Pin_5				/* SPI1->SCK */
#define RCLICK_SCK_AF					GPIO_PinSource5		/* SPI1->SCK for config Alternate Function */
#define RCLICK_SPI_MISO				GPIO_Pin_6				/* SPI1->MISO */
#define RCLICK_MISO_AF				GPIO_PinSource6		/* SPI1->MISO for config Alternate Function */
#define RCLICK_SPI_MOSI				GPIO_Pin_7				/* SPI1->MOSI */
#define RCLICK_MOSI_AF				GPIO_PinSource7		/* SPI1->MOSI for config Alternate Function */
//#define RClick_PORT_CS(n) 		(RClick_PIN_CS##n)
//#define RClick_PIN_CS(n)			(RClick_PIN_CS##n)
#define RCLICK_PORT_CS1				(uint32_t)GPIOA		/* GPIO PORTB for CS1 Pin */
#define RCLICK_PIN_CS1				GPIO_Pin_8						/* SPI2->CS, Chip Select for Board RCLICK_1*/
#define RCLICK_PORT_CS2				(uint32_t)GPIOB		/* GPIO PORT for CS2 Pin */
#define RCLICK_PIN_CS2				GPIO_Pin_10						/* SPI2->CS, Chip Select for Board RClick_2 */
#define RCLICK_PORT_CS3				(uint32_t)GPIOA		/* GPIO PORT for CS3 Pin */
#define RCLICK_PIN_CS3				GPIO_Pin_10						/* SPI2->CS, Chip Select for Board RClick_3 */
#define RCLICK_PORT_CS4				(uint32_t)GPIOC		/* GPIO PORT for CS4 Pin */
#define RCLICK_PIN_CS4				GPIO_Pin_4						/* SPI2->CS, Chip Select for Board RClick_4 */
#define RCLICK_PORT_CS5				(uint32_t)GPIOC		/* GPIO PORT for CS5 Pin */
#define RCLICK_PIN_CS5				GPIO_Pin_5						/* SPI2->CS, Chip Select for Board RClick_5 */
#define RCLICK_PORT_CS6				(uint32_t)GPIOB		/* GPIO PORT for CS6 Pin */
#define RCLICK_PIN_CS6				GPIO_Pin_0						/* SPI2->CS, Chip Select for Board RClick_6 */
#define RCLICK_PORT_VALiN			(uint32_t)GPIOB		/* GPIO PORT for CS7 Pin */
#define RCLICK_PIN_VALiN			GPIO_Pin_1						/* SPI2->CS, Chip Select for Board RClick_7 */
#define TCLICK_PORT_VALVE			(uint32_t)GPIOB		/* GPIO PORT for CS8 Pin */
#define TCLICK_PIN_VALVE			GPIO_Pin_2						/* SPI2->CS, Chip Select for Board RClick_8 */

#endif /*_SL_PINDEFINE_H*/


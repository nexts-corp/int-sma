/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_OneWire.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	02-December-2014
  * @brief   
  ******************************************************************************
  * @attention
  *	Port from 1 Wire Library Tilen Majerle (tm_stm32f4_onewire.c)
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_ONEWIRE_H
#define _INT_STM32F4_ONEWIRE_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "SL_PinDefine.h"
#include "main.h"

#define ONEWIRE_LOW				ONEWIRE_PORT->BSRRH = ONEWIRE_PIN;
#define ONEWIRE_HIGH			ONEWIRE_PORT->BSRRL = ONEWIRE_PIN;
#define ONEWIRE_INPUT			OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN; GPIO_Init(ONEWIRE_PORT, &OneWire_GPIO_InitDef);
#define ONEWIRE_OUTPUT		OneWire_GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; GPIO_Init(ONEWIRE_PORT, &OneWire_GPIO_InitDef);

/*-- OneWire commands --*/
#define ONEWIRE_CMD_RSCRATCHPAD			0xBE // Read Scratchpad
#define ONEWIRE_CMD_WSCRATCHPAD			0x4E // Write Scratchpad
#define ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define ONEWIRE_CMD_RECEEPROM				0xB8
#define ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define ONEWIRE_CMD_SEARCHROM				0xF0
#define ONEWIRE_CMD_READROM					0x33
#define ONEWIRE_CMD_MATCHROM				0x55
#define ONEWIRE_CMD_SKIPROM					0xCC

//DS18B20 read temperature command
#define DS1820_CMD_CONVERTTEMP			0x44 	//Convert temperature
#define DS1820_RESOLUTION						0.5

void vONEWIRE_Init(void);
void vONEWIRE_WriteBit(uint8_t bit);
void vONEWIRE_WriteByte(uint8_t byte);
void vONEWIRE_ResetSearch(void);
void vONEWIRE_TargetSetup(uint8_t family_code);
void vONEWIRE_FamilySkipSetup(void);
void vONEWIRE_Select(uint8_t addr[]);
void vONEWIRE_SelectWithPointer(uint8_t *ROM);
void vONEWIRE_GetFullROM(uint8_t *firstIndex);
uint8_t ucONEWIRE_Reset(void);
uint8_t ucONEWIRE_ReadBit(void);
uint8_t ucONEWIRE_ReadByte(void);
uint8_t ucONEWIRE_First(void);
uint8_t ucONEWIRE_Next(void);
uint8_t ucONEWIRE_Search(uint8_t command);
uint8_t ucONEWIRE_GetROM(uint8_t index);
int iONEWIRE_Verify(void);


#endif /*_INT_STM32F4_ONEWIRE_H*/


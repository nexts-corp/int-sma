/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_24LC512.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	23-December-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef _INT_STM32F4_MCP4725_H_
#define _INT_STM32F4_MCP4725_H_

#include <stdint.h>

#define I2C_TIMEOUT_MAX 				10000
#define MCP4725_ADDRESS                 (0xC0)   // 1100000x - Assumes A0 is GND and A2,A1 are 0 (MCP4725A0T-E/CH)
#define MCP4725_READ                    (0x01)
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

void MCP4725_InitI2C1(void);
uint8_t MCP4725_Write(uint16_t Addr, uint32_t Data);
uint8_t MCP4725_SetVoltage(uint16_t Value);

#endif /*_INT_STM32F4_24LC512_H_*/


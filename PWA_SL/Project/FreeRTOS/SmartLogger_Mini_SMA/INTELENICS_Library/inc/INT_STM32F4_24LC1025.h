/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_24LC512.h
  * @author  	ThanutSri
  * @version 	V1.0.0
  * @date    	23-December-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef _INT_STM32F4_24LC1025_H_
#define _INT_STM32F4_24LC1025_H_

#include <stdint.h>

#define I2C_TIMEOUT_MAX 				10000
#define MEM_DEVICE_WRITE_ADDR 	0xA0
#define MEM_DEVICE_READ_ADDR 		0xA1

//typedef struct {
//	uint32_t		base_addr;
//	uint32_t		config_length;
//	uint8_t			config_data[161];    // Size for max config_data (Properties)
//}ST_CONFIG_Q;

void EEPROM_InitI2C1(void);
uint8_t EEPROM_Write(uint16_t Addr, uint8_t Data);
uint8_t EEPROM_Read(uint16_t Addr);
uint8_t EEPROM_SequentRead(uint16_t Addr, uint8_t *data, uint16_t num);
uint8_t	EEPROM_WriteBuf(uint32_t baseAddr, void *buf, uint32_t size);
uint8_t	EEPROM_ReadBuf(uint32_t baseAddr, void *buf, uint32_t size);


#endif /*_INT_STM32F4_24LC512_H_*/


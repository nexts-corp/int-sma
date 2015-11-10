/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_OneWire.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	02-December-2014
  * @brief   
  ******************************************************************************
  * @attention
  *	Port from 1 Wire Library Tilen Majerle (tm_stm32f4_onewire.c)
  ******************************************************************************
  */

#include "INT_STM32F4_OneWire.h"

GPIO_InitTypeDef OneWire_GPIO_InitDef;

// global search state
uint8_t _OneWire_ROM_NO[8];
uint8_t _OneWire_LastDiscrepancy;
uint8_t _OneWire_LastFamilyDiscrepancy;
uint8_t _OneWire_LastDeviceFlag;


/*==========================================================================================*/
/**
 * Initialize OneWire bus
 *
 */
void vONEWIRE_Init(void) {
	
	RCC_AHB1PeriphClockCmd(ONEWIRE_RCC, ENABLE);
	
	OneWire_GPIO_InitDef.GPIO_Pin 		= ONEWIRE_PIN;
	OneWire_GPIO_InitDef.GPIO_Mode 		= GPIO_Mode_OUT;
	OneWire_GPIO_InitDef.GPIO_OType 	= GPIO_OType_PP;
	OneWire_GPIO_InitDef.GPIO_PuPd 		= GPIO_PuPd_UP;
	OneWire_GPIO_InitDef.GPIO_Speed 	= GPIO_Speed_50MHz;
	
	GPIO_Init(ONEWIRE_PORT, &OneWire_GPIO_InitDef);
}
/*==========================================================================================*/
/**
 * Reset OneWire bus
 * 
 * Sends reset command for OneWire
 */
uint8_t ucONEWIRE_Reset(void) {
	
	uint8_t i;
	
	// Line low, and wait 480us
	ONEWIRE_LOW;
	ONEWIRE_OUTPUT;
	delay_us(480);
	
	// Release line and wait for 80us
	ONEWIRE_INPUT;
	delay_us(60);
	
	// Check bit value
	i = GPIO_ReadInputDataBit(ONEWIRE_PORT, ONEWIRE_PIN);
	delay_us(420);
	// Return value of presence pulse, 0 = OK, 1 = ERROR
	return i;
}
/*==========================================================================================*/
uint8_t ucONEWIRE_ReadBit(void) {
	uint8_t bit = 0;
	// Line low
	ONEWIRE_LOW;
	ONEWIRE_OUTPUT;
	delay_us(1);
	
	// Release line
	ONEWIRE_INPUT;
	delay_us(15);
	
	// Read line value
	if (GPIO_ReadInputDataBit(ONEWIRE_PORT, ONEWIRE_PIN)) {
		bit = 1;
	}
	
	// Wait 45us to complete 60us period
	delay_us(45);
	
	return bit;
}
/*==========================================================================================*/
uint8_t ucONEWIRE_ReadByte(void) {
	
	uint8_t i = 8, byte = 0;
	
	while (i--) {
		byte >>= 1;
		byte |= (ucONEWIRE_ReadBit() << 7);
	}
	
	return byte;
}
/*==========================================================================================*/
void vONEWIRE_WriteBit(uint8_t bit) {
	
	// Line low
	ONEWIRE_LOW;
	ONEWIRE_OUTPUT;
	delay_us(1);
	
	if (bit) {
		ONEWIRE_INPUT;
	}
	
	// Wait for 60 us and release the line
	delay_us(60);
	ONEWIRE_INPUT;
}
/*==========================================================================================*/
void vONEWIRE_WriteByte(uint8_t byte) {
	
	uint8_t i = 8;
	
	while (i--) {
		// LSB bit is first
		vONEWIRE_WriteBit(byte & 0x01);
		byte >>= 1;
	}
}
/*==========================================================================================*/
uint8_t ucONEWIRE_First(void) {
	vONEWIRE_ResetSearch();
	return ucONEWIRE_Search(ONEWIRE_CMD_SEARCHROM);
}
/*==========================================================================================*/
uint8_t ucONEWIRE_Next(void) {
   // leave the search state alone
   return ucONEWIRE_Search(ONEWIRE_CMD_SEARCHROM);
}
/*==========================================================================================*/
void vONEWIRE_ResetSearch(void) {
	// reset the search state
	_OneWire_LastDiscrepancy = 0;
	_OneWire_LastDeviceFlag = 0;
	_OneWire_LastFamilyDiscrepancy = 0;
}
/*==========================================================================================*/
uint8_t ucONEWIRE_Search(uint8_t command) {
	
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit;
	uint8_t rom_byte_mask, search_direction;

	// initialize for search
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;

	// if the last call was not the last one
	if (!_OneWire_LastDeviceFlag) {
		// 1-Wire reset
		if (ucONEWIRE_Reset()) {
			// reset the search
			_OneWire_LastDiscrepancy = 0;
			_OneWire_LastDeviceFlag = 0;
			_OneWire_LastFamilyDiscrepancy = 0;
			return 0;
		}

		// issue the search command 
		vONEWIRE_WriteByte(command);  

		// loop to do the search
		do {
			// read a bit and its complement
			id_bit = ucONEWIRE_ReadBit();
			cmp_id_bit = ucONEWIRE_ReadBit();

			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1)) {
				break;
			} else {
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit) {
					search_direction = id_bit;  // bit write value for search
				} else {
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (id_bit_number < _OneWire_LastDiscrepancy) {
						search_direction = ((_OneWire_ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					} else {
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == _OneWire_LastDiscrepancy);
					}
					
					// if 0 was picked then record its position in LastZero
					if (search_direction == 0) {
						last_zero = id_bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9) {
							_OneWire_LastFamilyDiscrepancy = last_zero;
						}
					}
				}

				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction == 1) {
					_OneWire_ROM_NO[rom_byte_number] |= rom_byte_mask;
				} else {
					_OneWire_ROM_NO[rom_byte_number] &= ~rom_byte_mask;
				}
				
				// serial number search direction write bit
				vONEWIRE_WriteBit(search_direction);

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0) {
					//docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

		// if the search was successful then
		if (!(id_bit_number < 65)) {
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			_OneWire_LastDiscrepancy = last_zero;

			// check for last device
			if (_OneWire_LastDiscrepancy == 0) {
				_OneWire_LastDeviceFlag = 1;
			}

			search_result = 1;
		}
	}

	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !_OneWire_ROM_NO[0]) {
		_OneWire_LastDiscrepancy = 0;
		_OneWire_LastDeviceFlag = 0;
		_OneWire_LastFamilyDiscrepancy = 0;
		search_result = 0;
	}

	return search_result;
}
/*==========================================================================================*/
int iONEWIRE_Verify(void) {
	
	unsigned char rom_backup[8];
	int i,rslt,ld_backup,ldf_backup,lfd_backup;

	// keep a backup copy of the current state
	for (i = 0; i < 8; i++)
	rom_backup[i] = _OneWire_ROM_NO[i];
	ld_backup = _OneWire_LastDiscrepancy;
	ldf_backup = _OneWire_LastDeviceFlag;
	lfd_backup = _OneWire_LastFamilyDiscrepancy;

	// set search to find the same device
	_OneWire_LastDiscrepancy = 64;
	_OneWire_LastDeviceFlag = 0;

	if (ucONEWIRE_Search(ONEWIRE_CMD_SEARCHROM)) {
		// check if same device found
		rslt = 1;
		for (i = 0; i < 8; i++) {
			if (rom_backup[i] != _OneWire_ROM_NO[i]) {
				rslt = 1;
				break;
			}
		}
	} else {
		rslt = 0;
	}

	// restore the search state 
	for (i = 0; i < 8; i++) {
		_OneWire_ROM_NO[i] = rom_backup[i];
	}
	_OneWire_LastDiscrepancy = ld_backup;
	_OneWire_LastDeviceFlag = ldf_backup;
	_OneWire_LastFamilyDiscrepancy = lfd_backup;

	// return the result of the verify
	return rslt;
}
/*==========================================================================================*/
void vONEWIRE_TargetSetup(uint8_t family_code) {
	
   uint8_t i;

	// set the search state to find SearchFamily type devices
	_OneWire_ROM_NO[0] = family_code;
	for (i = 1; i < 8; i++) {
		_OneWire_ROM_NO[i] = 0;
	}
	_OneWire_LastDiscrepancy = 64;
	_OneWire_LastFamilyDiscrepancy = 0;
	_OneWire_LastDeviceFlag = 0;
}
/*==========================================================================================*/
void vONEWIRE_FamilySkipSetup(void) {
	
	// set the Last discrepancy to last family discrepancy
	_OneWire_LastDiscrepancy = _OneWire_LastFamilyDiscrepancy;
	_OneWire_LastFamilyDiscrepancy = 0;

	// check for end of list
	if (_OneWire_LastDiscrepancy == 0) {
		_OneWire_LastDeviceFlag = 1;
	}
}
/*==========================================================================================*/
uint8_t ucONEWIRE_GetROM(uint8_t index) {
	return _OneWire_ROM_NO[index];
}
/*==========================================================================================*/
void vONEWIRE_Select(uint8_t addr[]) {
	
	uint8_t i;
	
	vONEWIRE_WriteByte(ONEWIRE_CMD_MATCHROM);
	for (i = 0; i < 8; i++) {
		vONEWIRE_WriteByte(*(addr + i));
	}
}
/*==========================================================================================*/
void vONEWIRE_SelectWithPointer(uint8_t *ROM) {
	
	uint8_t i;
	
	vONEWIRE_WriteByte(ONEWIRE_CMD_MATCHROM);
	for (i = 0; i < 8; i++) {
		vONEWIRE_WriteByte(*(ROM + i));
	}
}
/*==========================================================================================*/
void vONEWIRE_GetFullROM(uint8_t *firstIndex) {
	
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		*(firstIndex + i) = _OneWire_ROM_NO[i];
	}
}
/*==========================================================================================*/




















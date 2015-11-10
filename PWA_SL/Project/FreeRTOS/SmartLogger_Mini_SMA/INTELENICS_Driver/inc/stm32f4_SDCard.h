/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	stm32f4_SDCard.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	4-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _STM32F4_SDCARD_H
#define _STM32F4_SDCARD_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "ff.h"

#define CREATE_ALWAYS		0x01
#define CREATE_NEW			0x02
#define SEMPHR_WAIT2		10000

FRESULT xSDCARD_Initial(void);
FRESULT xSDCARD_CreateFile(FIL *file, const char *filename, uint8_t createMode);
FRESULT xSDCARD_OpenFile(FIL *file, const char *filename);
FRESULT xSDCARD_CloseFile(FIL *file);
FRESULT xSDCARD_ReadMemoryRemain(uint32_t *free_kbytes);
FRESULT xSDCARD_WriteData(FIL *file, uint32_t startAddr, void *data, uint32_t length);
FRESULT xSDCARD_ReadData(FIL *file, uint32_t startAddr, void *data, uint32_t length);
	
	
#endif /*_STM32F4_SDCARD_H*/


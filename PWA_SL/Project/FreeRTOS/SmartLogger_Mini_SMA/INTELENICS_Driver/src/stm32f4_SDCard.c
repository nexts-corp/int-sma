/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	stm32f4_SDCard.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	4-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdint.h>
#include "stm32f4_SDCard.h"
#include "SL_PinDefine.h"

extern void delay_ms(uint32_t delay);

FATFS filesystem;		/* volume lable */
FATFS *pFilesystem = &filesystem;

/*==========================================================================================*/
/**
  * @brief  Initial SD Card
  * @param  None
  * @retval File function return code
  */
FRESULT xSDCARD_Initial(void) {
	
	FRESULT		res;
	
	/* mount the filesystem */
  res = f_mount(0, &filesystem);
	if(res != FR_OK) {
		/* Could not open filesystem */
		return res;
	}
	delay_ms(10);
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Create file in SD Card
  * @param 	
  * @retval File function return code
  */
FRESULT xSDCARD_CreateFile(FIL *file, const char *filename, uint8_t createMode) {
	
	FRESULT		res;
	
	if(createMode == CREATE_ALWAYS) {			
		/* Replace old file if exist */
		res = f_open(file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	}else {															
		/* Keep old file if exist */			
		res = f_open(file, filename, FA_CREATE_NEW | FA_WRITE);
	}
	
	if(res != FR_OK) {
		/* File is already exist, or other f_open error */
		return res;
	}
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Open file in SD Card
  * @param 	
  * @retval File function return code
  */
FRESULT xSDCARD_OpenFile(FIL *file, const char *filename) {
	
	FRESULT		res;
	
	res = f_open(file, filename, FA_WRITE | FA_READ);
	if(res != FR_OK) {
		/* f_open Error */
		return res;
	}
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Close file in SD Card
  * @param 	
  * @retval File function return code
  */
FRESULT xSDCARD_CloseFile(FIL *file) {
	
	FRESULT		res;
	
	res = f_close(file);
	if(res != FR_OK) {
		/* f_close Error */
		return res;
	}
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Read memory remaining
  * @param 	
  * @retval File function return code
  */
FRESULT xSDCARD_ReadMemoryRemain(uint32_t *free_kbytes) {
	
	FRESULT res;
	unsigned long free_clusters;
	
	res = f_getfree(0, &free_clusters, &pFilesystem);
	if(res != FR_OK) {
		/* f_getfree Error */
		return res;
	}
	*free_kbytes = free_clusters*pFilesystem->csize/2;
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Write data to file in SD Card
  * @param  
  * @retval File function return code
	*					*** ADD 0xFF indicate Bytes_Write and Actual_Bytes_Write not equal 
  */
FRESULT xSDCARD_WriteData(FIL *file, uint32_t startAddr, void *data, uint32_t length) {
	
	FRESULT		res;                      
  uint32_t 	actual_write;                                                                                                     
    
  res = f_lseek(file, startAddr);
	if(res != FR_OK) {
		/*f_lseek error*/
		return res;
	}             
  res = f_write(file, data, length, &actual_write);  
  if(res != FR_OK){      
		/*f_write error*/
		return res;
	}            
    
  if(actual_write != length){
		/**/   
    return 0xFF;                                                                
	}                                                                    

  return res;
}
/*==========================================================================================*/
/**
  * @brief  Read data from file in SD Card
  * @param 
  * @retval File function return code
	*					*** ADD 0xFF indicate Bytes_Read and Actual_Bytes_Read not equal 
  */
FRESULT xSDCARD_ReadData(FIL *file, uint32_t startAddr, void *data, uint32_t length) {
	
	FRESULT 	res;
	uint32_t 	actual_read;
	
	res = f_lseek(file, startAddr);
	if(res != FR_OK) {
		/*f_lseek error*/
		return res;
	}
	res = f_read(file, data, length, &actual_read);
	if(res != FR_OK) {
		/*f_read error*/
		return res;
	}
	
	if(actual_read != length){
		/**/   
    return 0xFF;                                                                
	}
	
	return res;
}
/*==========================================================================================*/	









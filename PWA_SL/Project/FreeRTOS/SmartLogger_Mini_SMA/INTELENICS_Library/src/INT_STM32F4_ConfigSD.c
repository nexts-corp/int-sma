/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_ConfigSD.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	11-April-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "INT_STM32F4_ConfigSD.h"
#include "stm32f4_SDCard.h"
#include "INT_STM32F4_ProtocolComm.h"

const uint32_t SEN420CONF_BASEADDR[] 	= {SEN420CH1CONF_BASEADDR, SEN420CH2CONF_BASEADDR, SEN420CH3CONF_BASEADDR, SEN420CH4CONF_BASEADDR,
																				SEN420CH5CONF_BASEADDR, SEN420CH6CONF_BASEADDR};

const uint32_t SENADCCONF_BASEADDR[] 	= {SENADCCH1CONF_BASEADDR, SENADCCH2CONF_BASEADDR};

const uint32_t DIGItIN_BASEADDR[] 		= {DIGItIN1_BASEADDR,DIGItIN2_BASEADDR,DIGItIN3_BASEADDR,DIGItIN4_BASEADDR,
																				 DIGItIN5_BASEADDR,DIGItIN6_BASEADDR,DIGItIN7_BASEADDR,DIGItIN8_BASEADDR,
																				 DIGItIN9_BASEADDR,DIGItIN10_BASEADDR,DIGItIN11_BASEADDR,DIGItIN12_BASEADDR};

const uint32_t SEN420ALARMn_BASEADDR[] = {SEN420CH1ALARM_BASEADDR, SEN420CH2ALARM_BASEADDR, SEN420CH3ALARM_BASEADDR, SEN420CH4ALARM_BASEADDR,
																					SEN420CH5ALARM_BASEADDR, SEN420CH6ALARM_BASEADDR};

const uint32_t SENADCALARMn_BASEADDR[] = {SENADCCH1ALARM_BASEADDR, SENADCCH2ALARM_BASEADDR};

const uint32_t SEN420CHnCAL_BASEADDR[] = {SEN420CH1CAL_BASEADDR, SEN420CH2CAL_BASEADDR, SEN420CH3CAL_BASEADDR, SEN420CH4CAL_BASEADDR,
																					SEN420CH5CAL_BASEADDR, SEN420CH6CAL_BASEADDR};

/*==========================================================================================*/
/**
  * @brief  Write Device Serial in Serial File
  * @param 	
  * @retval File function return code
  */
//FRESULT xCONFIGSD_WriteMySerial(FIL *file, uint32_t baseAddr, uint8_t *serial) {

//	return xSDCARD_WriteData(file, baseAddr, serial, ADDR_LEN);
//}
/*==========================================================================================*/
/**
  * @brief  Read Device Serial in Serial File
  * @param 	
  * @retval File function return code
  */
//FRESULT xCONFIGSD_ReadMySerial(FIL *file, uint32_t baseAddr, uint8_t *serial) {

//	return xSDCARD_ReadData(file, baseAddr, serial, ADDR_LEN);
//}
/*==========================================================================================*/
/**
  * @brief  Write Configuration in Config File
  * @param 	
  * @retval File function return code
  */
FRESULT xCONFIGSD_WriteConfig(FIL *file, uint32_t baseAddr, void *pStruct, uint32_t struct_size) {
	
	FRESULT 	res;

	res = xSDCARD_WriteData(file, baseAddr, pStruct, struct_size);
	if(res != FR_OK) { return res; } 							/* Read data error */
	
	return res;
}
/*==========================================================================================*/
/**
  * @brief  Read Configuration in Config File
  * @param 	
  * @retval File function return code
  */
FRESULT xCONFIGSD_ReadConfig(FIL *file, uint32_t baseAddr, void *pStruct, uint32_t struct_size) {
	
	FRESULT 	res;

	res = xSDCARD_ReadData(file, baseAddr, pStruct, struct_size);
	if(res != FR_OK) { return res; } 							/* Read data error */
	
	return res;
}
/*==========================================================================================*/
	


	
	
	

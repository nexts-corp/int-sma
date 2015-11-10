/**
  ******************************************************************************
  * @file    gsm_task.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "main.h"
#include "gsm_task.h"
#include "Manager_Task.h"

#include "SL_PinDefine.h"

#include "INT_STM32F4_GSM.h"
#include "INT_STM32F4_Usart.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_Protocolcomm.h"

#include "INT_STM32F4_SC16IS750.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
extern xQueueHandle Queue_PacketHTTPToGSM;
extern xQueueHandle Queue_PacketToGSM;
extern xQueueHandle Queue_PacketFromGSM;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t ucHeartBeatcmd[24] = {  0x7E, 0x00, 0x14, 0x40, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99
														, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA ,0xFF, 0x7A, 0x11, 0x1D};
uint8_t ucHeartBeatlen = 24;

char* URL_SMA = "http://smasmartdevice.appspot.com/smasmartdevice";

extern xSemaphoreHandle xSDCARD_Semphr;
uint8_t ucGSM_RSSI;

uint8_t	_serialnumber_GSM[15];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Control GSM module task
  * @param  pvParameters not used
  * @retval None
  */
void vGSM_Task( void * pvParameters )
{
	/* Structure Declaration ---------------------------------------------------*/
	PACKET_DATA				sFormatPacketHTTP_GSM;
	
	eGSMErrorCode			errcode;
	/* Variable Declaration ----------------------------------------------------*/
	/* Variable about Queue */
	uint8_t pdstatus_PacketToGSM;
	uint8_t	ucCMDStatus;
	
	/* Variable about return */
	eGSMReturnCode		eGSMReturnResult = GSM_RET_OK;
	eGSMConnectStatus eGSMStatus = GSM_CONNECT_FAIL;
	
	/*-- Variable about Check state --*/
	uint8_t			ucResetCount = 0;
	
	uint32_t	readSignalTimer = 0;
	
	ucTIMER_SetTimer(&readSignalTimer, 60);
	
	printDebug_Semphr(DESCRIBE, "[GSM_TASK]: Running... \r\n");
	
	GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN );
	
	/* Start GSM Module */
 	vGSM_Start( );
		
	/* Get IMEI */
	if(ucGSM_GetIMEI((uint8_t*)_serialnumber_GSM)){_Communication_Status |= COMMU_CONNECTED;}											// set flag when Connect success
	else{while(1);}
	
	/* wait for register networks */
	delay_ms(2000);
	
  while( 1 ) {			
		
		if(cTIMER_CheckTimerExceed(readSignalTimer)) {
			ucTIMER_SetTimer(&readSignalTimer, 300);
			ucGSM_RSSI = ucGSM_ReadSignalQuality( );
			printDebug_Semphr( INFO, "[GSM_TASK]: Signal Quality = %d%%\r\n", ucGSM_RSSI );
			if(ucGSM_RSSI < 20) {
				LED_CSQ_RED;
			}else {
				_Communication_Status |= COMMU_CONNECTED;
				LED_CSQ_GREEN;
			}
		}
		
		/*===============================================================================================
		=====================================	Receive Packet	==========================================*/
		eGSMReturnResult = eGSM_receivePacket( );
		if( eGSMReturnResult == GSM_RET_CLOSED ) {
			_Communication_Status &= ~COMMU_CONNECTED;
		}
 		else if( eGSMReturnResult != GSM_RET_OK && eGSMReturnResult != GSM_RET_PKT_EMPTY ) {
			printDebug_Semphr( WARNING, "[GSM_TASK]: Receive Packet Error( %d ) \r\n", eGSMReturnResult );
 		}
		
		/*===============================================================================================
		======================================	Report HTTP	=============================================*/
		/*	wait Packet from Manager Task	*/
		pdstatus_PacketToGSM = xQueueReceive( Queue_PacketHTTPToGSM, &sFormatPacketHTTP_GSM, 100 );
		if( pdstatus_PacketToGSM == pdTRUE ) {
			errcode = vGSM_SendHTTPOST( (uint8_t*)URL_SMA, strlen(URL_SMA), sFormatPacketHTTP_GSM.packet, sFormatPacketHTTP_GSM.length);
			if(errcode == GSM_ENOERR) {
				_Communication_Status |= COMMU_CONNECTED;											// set flag when Connect success
			}
			delay_ms(100);
		}
		
		/*===============================================================================================
			=====================================	Command from Manager	=================================*/
		if( xQueueReceive( Queue_CMDToGSM, &ucCMDStatus, 100 ) ) {
			/* Read Signal Quality from GSM module */
			ucGSM_RSSI = ucGSM_ReadSignalQuality( );
			printDebug_Semphr( WARNING, "[GSM_TASK]: Signal Quality = %d%%\r\n", ucGSM_RSSI );
			if(ucGSM_RSSI < 20) {
				LED_CSQ_RED;
			}else {
				LED_CSQ_GREEN;
				_Communication_Status |= COMMU_CONNECTED;
			}
			
			if( ucCMDStatus == GSM_CMD_HWRESET ) {
				_Communication_Status &= ~COMMU_CONNECTED;
				vGSM_Restart( );
				
				/*==============================================*/
			}else if( ucCMDStatus == GSM_CMD_HWPWROFF ) {
				vGSM_PowerOff( );
				_Communication_Status &= ~COMMU_CONNECTED;
			}else if( ucCMDStatus == GSM_CMD_HWPWRON ) {
				vGSM_Start( );
			}
			else {
				printDebug_Semphr( MINOR_ERR, "[GSM_TASK]: Command not invalid\r\n", eGSMStatus );
			}
		}
	}
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

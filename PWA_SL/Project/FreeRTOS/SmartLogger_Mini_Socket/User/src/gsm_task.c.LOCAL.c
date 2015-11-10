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
extern xQueueHandle Queue_PacketToGSM;
extern xQueueHandle Queue_PacketFromGSM;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char* IP_ADDRESS = {"202.44.34.86"};
int 	port = 50000 ;

uint8_t ucHeartBeatcmd[24] = {  0x7E, 0x00, 0x14, 0x40, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99
														, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA ,0xFF, 0x7A, 0x11, 0x1D};
uint8_t ucHeartBeatlen = 24;

uint8_t ucTestpktcmd[100] = { 0x7e, 0x00, 0x60, 0x40, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0xaa, 0xaa, 0xaa, 0xaa
, 0xaa, 0xaa, 0xaa, 0xaa, 0x1c, 0x9a, 0x1b, 0x7d, 0x0d, 0x54, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99
, 0x99, 0x99, 0x08, 0x02, 0x11, 0x03, 0x50, 0x00, 0x00, 0x00, 0x12, 0x04, 0x00, 0x00, 0xaa, 0x42
, 0x13, 0x04, 0xa3, 0x7a, 0xa8, 0x45, 0x21, 0x03, 0x32, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00
, 0x0f, 0xff, 0x23, 0x02, 0x01, 0x02, 0x03, 0x04, 0x24, 0x02, 0x01, 0x02, 0x03, 0x04, 0x25, 0x02
, 0x01, 0x02, 0x03, 0x04, 0x02, 0x01, 0x01, 0x0c, 0x9e, 0xc5, 0xa5, 0x41, 0x02, 0x04, 0x0c, 0x9e
, 0xc5, 0xa5, 0x41, 0xd5 };

uint8_t ucTestpktlen = 100;

extern xSemaphoreHandle xSDCARD_Semphr;
uint8_t ucGSM_RSSI;
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
	PACKET_DATA				sFormatPacket_GSM;
	
	/* Variable Declaration ----------------------------------------------------*/
	/* Variable about Queue */
	uint8_t pdstatus_PacketToGSM;
	uint8_t	ucCMDStatus;
	
	char a[50];
	int len;
	
	/* Variable about return */
	eGSMReturnCode		eGSMReturnResult = GSM_RET_OK;
	eGSMConnectStatus eGSMStatus = GSM_CONNECT_FAIL;
	
	/*-- Variable about Check state --*/
	uint8_t			ucResetCount = 0;
	
	/* Start GSM Module */
	vGSM_Start( );
	
	printDebug_Semphr(DESCRIBE, "[GSM_TASK]: Running... \r\n");	
	
 	SC16IS750_init();
	SC16IS750_writeString("Hello123456789\r\n");
  while( 1 ) {			
		
		len = SC16IS750_readString( a, 256);
		if( len > 0 )printDebug_Semphr( INFO, "[TEST]: %s\r\n", a );
		/*===============================================================================================
		======================================	Report	=============================================*/
		/*	wait Packet from Manager Task	*/
		pdstatus_PacketToGSM = xQueueReceive( Queue_PacketToGSM, &sFormatPacket_GSM, 100 );
		if( pdstatus_PacketToGSM == pdTRUE ) {
//		if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ){
//			printDebug( INFO, "[GSM_TASK]: Rcv from MNG : %d\r\n", sFormatPacket_GSM.length );
//			print_payload( INFO, sFormatPacket_GSM.packet, sFormatPacket_GSM.length );
//			printDebug( INFO, "\r\n" );
//			xSemaphoreGive( xSemaphore );
//		}	
			
			eGSMReturnResult = eGSM_transmitPacket( sFormatPacket_GSM.packet, sFormatPacket_GSM.length );
			if( eGSMReturnResult != PTC_OK ) {
				printDebug_Semphr( DETAIL,"[GSM_TASK]: Send Packet Error( %d ) \r\n", eGSMReturnResult );
				flag_GSMReady = GSM_Flag_NotActive;
			}
			delay_ms(100);
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
			=====================================	Command from Manager	=================================*/
		if( xQueueReceive( Queue_CMDToGSM, &ucCMDStatus, 100 ) ) {
			/* Read Signal Quality from GSM module */
			ucGSM_RSSI = ucGSM_ReadSignalQuality( );
			printDebug_Semphr( INFO, "[GSM_TASK]: Signal Quality = %d%%\r\n", ucGSM_RSSI );
			if( ucGSM_RSSI == 0 ) {
				ucResetCount++;
				if( ucResetCount == 2 ) {
					GPIO_ResetBits( GSM_SWITCH_PORT, GSM_SWITCH_PIN );
					delay_ms( 1000 );
					GPIO_SetBits( GSM_SWITCH_PORT, GSM_SWITCH_PIN );
					ucResetCount = 0;
				}
			}
			else {
				ucResetCount = 0;
			}
			/* Operation connection */
			if( ucCMDStatus == GSM_CMD_CONNECT ) {
				printDebug_Semphr( DETAIL, "[GSM_TASK]: %s:%d\r\n", struct_ConfigNetwork.serverAddr, struct_ConfigNetwork.serverPort );
				eGSMStatus = ucGSM_ConnectTCP((char*)struct_ConfigNetwork.serverAddr, struct_ConfigNetwork.serverPort);
				if( eGSMStatus == GSM_CONNECT_OK ) {											// GSM module connect to server
					printDebug_Semphr( INFO, "[GSM_TASK]: ===== CONNECT OK ===== \r\n");
					_Communication_Status |= COMMU_CONNECTED;											// set flag when Connect success
				}
				else {
					_Communication_Status &= ~COMMU_CONNECTED;
					printDebug_Semphr( WARNING, "[GSM_TASK]: ===== CONNECT FAIL(%d) ===== \r\n", eGSMStatus );
				}
			}
			/* Operation close connection */
			else if( ucCMDStatus == GSM_CMD_CLOSE ) {
				vGSM_CloseConnection( );
				_Communication_Status &= ~COMMU_CONNECTED;
			}
			/* Operation reconnection */
			else if( ucCMDStatus == GSM_CMD_RECONNECT ) {
				vGSM_CloseConnection( );
				printDebug_Semphr( DETAIL, "[GSM_TASK]: %s:%d\r\n", struct_ConfigNetwork.serverAddr, struct_ConfigNetwork.serverPort );
				eGSMStatus = ucGSM_ConnectTCP((char*)struct_ConfigNetwork.serverAddr, struct_ConfigNetwork.serverPort);
				if( eGSMStatus == GSM_CONNECT_OK ) {											// GSM module connect to server
					printDebug_Semphr( INFO, "[GSM_TASK]: ===== CONNECT OK ===== \r\n");
					_Communication_Status |= COMMU_CONNECTED;											// set flag when Connect success
				}
				else {

					_Communication_Status &= ~COMMU_CONNECTED;
					printDebug_Semphr( WARNING, "[GSM_TASK]: ===== CONNECT FAIL(%d) ===== \r\n", eGSMStatus );
				}
			}
			else {
				printDebug_Semphr( MINOR_ERR, "[GSM_TASK]: Command not invalid\r\n", eGSMStatus );
			}
		}
	}
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

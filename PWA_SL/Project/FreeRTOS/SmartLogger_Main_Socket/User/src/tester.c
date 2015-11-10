/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Tester.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	27-February-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdio.h>
#include "main.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_ProtocolParser.h"

extern xQueueHandle 			Queue_PacketToGSM;
extern xQueueHandle 			Queue_PacketFromGSM;

void vTester_Task(void *pvParameters) {
	
	uint8_t res, i, frameType;
	PACKET_DATA packet_data;
	uint8_t		num_stat, num_data;
	STATUS_SET status[MAX_STATUS];
	DATA_SET data[MAX_DATA];
	uint32_t timestamp;
	uint8_t printLevel = DETAIL;
	/*-- Variable about Protocol --*/
	PTC_RES 		protocolResult;
	
	printDebug_Semphr(DESCRIBE, "[TESTER_TASK]: Running... \r\n");
	
	while(1) {
		
	/* Receive Data Packet -----------------------------------------------*/
//	protocolResult = ucPROCOMM_ReceivePacket();
//	if((protocolResult != PTC_OK) && (protocolResult != PTC_PKT_EMPTY)) {
//		printDebug_Semphr(MINOR_ERR, "Receive Packet Error (%d) \r\n", protocolResult);
//	}
		
	/* Receive Data&Status from SensorTask -------------------------------*/
		if(xQueueReceive(Queue_PacketToGSM, &packet_data, 0)) {
			
			frameType = ucPTCPARSER_getFrameType(packet_data.packet);
			timestamp = uiPTCPARSER_getTimestamp(packet_data.packet);
			if(frameType == DNSTREPC) {
				
			}
			
			if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
				printLevel = INFO;
				printDebug(printLevel, "Send Packet(%d): \r\n", packet_data.length);
				print_payload(printLevel, packet_data.packet, packet_data.length);
//				for(i = 0; i < packet_data.length; i++) {
//					printDebug(printLevel, "%02X ", packet_data.packet[i]);
//				}
				printDebug(printLevel, "\r\n");
				
				xSemaphoreGive(xSemaphore);
			}
		}
	}
	
}




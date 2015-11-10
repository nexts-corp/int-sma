/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_debug.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	22-May-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "SL_ConfigStruct.h"
#include "SL_PinDefine.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_rtc.h"

static void print_hex_ascii_line(const unsigned char *payload, int len, int offset);

xSemaphoreHandle xSemaphore;

/*==========================================================================================*/
/**
  * @brief  Send data to debug port
  * @param  Data
  * @retval None
  */
void vDEBUG_SendData(uint8_t *data, uint16_t length) {
	
	uint16_t i;

	if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
//		printDebug( INFO, "<DEBUG_SendData>: Send : %d \r\n", length );
//		print_payload( INFO, data, length );
//		printDebug(INFO, "\r\n");
//		for(i = 0; i < length; i++) {
//			printDebug(INFO, "%02X", data[i]);
//		}
//		printDebug(INFO, "\r\n");
	
		for(i = 0; i < length; i++) {
			/* e.g. write a character to the USART */
			USART_SendData(USART1, data[i]);
			/* Loop until the end of transmission */
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
		xSemaphoreGive(xSemaphore);
	}
	
}
/*==========================================================================================*/
/**
  * @brief  Print Debug
  * @param  Debug id
  * @retval None
  */
void printDebug(uint8_t debug_ID, char *fmtstr, ...) {
	
	char textBuffer[256];
	va_list argptr; 

	if((struct_DevConfig.debug_level & debug_ID) || (debug_ID == DESCRIBE)) {   	/* DESCRIBE print every times */
		va_start(argptr, fmtstr);
		vsprintf(textBuffer,fmtstr,argptr);
		printf("%s", textBuffer);
		va_end(argptr);   
	}
	
	return;
}
/*==========================================================================================*/
/**
  * @brief  Print Debug with Semaphore mutex
  * @param  Debug id
  * @retval None
  */
void printDebug_Semphr(uint8_t debug_ID, char *fmtstr, ...) {
	
	char 					textBuffer[256];
	va_list 			argptr; 
	DEBUG_MESSAGE	debug_log;
	
	if(xSemaphoreTake(xSemaphore, SEMPHR_WAIT)) {
		if((struct_DevConfig.debug_level & debug_ID) || (debug_ID == DESCRIBE)) {    	/* DESCRIBE print every times */
			va_start(argptr, fmtstr);
			vsprintf(textBuffer,fmtstr,argptr);
			printf("%s", textBuffer);
			va_end(argptr);   
		}
		xSemaphoreGive(xSemaphore);
	}
	/*-- Write message error to log file --*/
	if(debug_ID == MAJOR_ERR /*|| (debug_ID == MINOR_ERR)*/) {
		debug_log.timestamp = uiRTC_GetEpochTime();
		memcpy(debug_log.message, textBuffer, sizeof(debug_log.message));
		xQueueSend(Queue_DebugToSD, &debug_log, 0);
	}
	
	return;        
}
/*==========================================================================================*/
/**
  * @brief  Print Payload (Hex and Asscii)
  * @param  Debug id
  * @retval None
  */
void print_payload(uint8_t Debug_ID, const unsigned char *payload, int len)
{

    int len_rem = len;
    int line_width = 16;            // number of bytes per line //
    int line_len;
    int offset = 0;                    // zero-based offset counter //
    const unsigned char *ch = payload;

		if(struct_DevConfig.debug_level & Debug_ID) {
			if (len <= 0)
					return;

			// data fits on one line //
			if (len <= line_width) {
					print_hex_ascii_line(ch, len, offset);
					return;
			}
			// data spans multiple lines //
			for ( ;; ) {
					// compute current line length //
					line_len = line_width % len_rem;
					// print line //
					print_hex_ascii_line(ch, line_len, offset);
					// compute total remaining //
					len_rem = len_rem - line_len;
					// shift pointer to remaining bytes to print //
					ch = ch + line_len;
					// add offset //
					offset = offset + line_width;
					// check if we have line width chars or less //
					if (len_rem <= line_width) {
							// print last line and get out //
							print_hex_ascii_line(ch, len_rem, offset);
							break;
					}
			}
		}

    return;
}            
/*==========================================================================================*/
/**
  * @brief 	
  * @param  
  * @retval None
  */
static void print_hex_ascii_line(const unsigned char *payload, int len, int offset) {

    int i;
    int gap;
    const unsigned char *ch;

    // offset //                      
    printf("%05d   ", offset);               
    
    
    // hex //                                                                                                      
    ch = payload;
    for(i = 0; i < len; i++) {                            
        printf("%02x ", *ch);                    
        
        ch++;
        // print extra space after 8th byte for visual aid //
        if (i == 7){                            
            printf(" ");                                     
            
        }
    }
    // print space to handle line less than 8 bytes //
    if (len < 8){                            
        printf(" ");                                              
        
    }
    
    // fill hex gap with spaces if not full line //
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");                                           
            
        }
    }
    printf("   ");                                                      
    
    
    // ascii (if printable) //
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch)){
            printf("%c", *ch);                                           
            
        }
        else{
            printf(".");                                                 
            
        }
        ch++;
    }

    printf("\r\n");                                                        
    

return;
}
/*==========================================================================================*/

  




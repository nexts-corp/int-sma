/**
  ******************************************************************************
  * @file    INT_STM32F4_GSM.c
  * @author  Intelenics Application Team
  ******************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "main.h"
#include "INT_STM32F4_GSM.h"
#include "INT_STM32F4_Usart.h"
#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_PacketQueue.h"

#include "FreeRTOS.h"

#include "SL_PinDefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NBUF 							( 30 )

#define INDEX_FRAMETYPE		( 21 )
#define INDEX_FRAMEDATA		( 22 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern xQueueHandle Queue_PacketFromGSM;
extern uint8_t _serialnumber_GSM[15];

uint32_t uiNByte = 0;

int	GSM_Delay_Start;

/* Private function prototypes -----------------------------------------------*/
static eGSMErrorCode 			ucGSM_waitForOk( uint8_t* str, uint16_t time_wait );
static eGSMConnectStatus ucGSM_waitForConnect( uint16_t time_wait );
static uint8_t 						ucGSM_checksum( uint8_t buf[],uint16_t ulLength );
static uint8_t						ucGSM_escapes( const uint8_t* ucSrc, uint8_t* ucRetSrc, uint32_t ulLength );

/*============================================================================================*/
/**
  * @brief  Initialize GSM module
  * @param  None
  * @retval None
  */
uint8_t ucGSM_Initialize( void ) {
	printf( "ATE0\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"ATE0\r\n",6 );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// Set the context 0 as FGCNT
	printf( (const char *)"AT+QIFGCNT=0\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIFGCNT=0\r\n",14 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	//Set APN.
	printf( "AT+QICSGP=1,\"www.dtac.co.th\"\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QICSGP=1,\"www.dtac.co.th\"\r\n",31 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// 0 disables the function of MUXIP. 1 enables the function.
	printf( "AT+QIMUX=0\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIMUX=0\r\n",12 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// Set the session mode as non-transparent.
	printf( "AT+QIMODE=0\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIMODE=0\r\n",13 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// Use IP address to establish TCP/UDP session.
	printf( "AT+QIDNSIP=0\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIDNSIP=0\r\n",14 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// Register the TCP/IP stack.
	printf( "AT+QIREGAPP\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIREGAPP\r\n",13 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	// Activate FGCNT.
	printf( "AT+QIACT\r\n" );
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIACT\r\n",10 );
	//len = read_uart0( buf,10,interchar );
	ucGSM_waitForOk( (uint8_t *)"OK",1000 );
	
	return 0;
}
/*============================================================================================*/ 
/** * @brief Power off GSM module. 
		* @param None * @retval None 
*/ 
void vGSM_PowerOff( void ) { 
	printDebug_Semphr( INFO, "<GSM_PowerOff>: GSM Power Off!!! \r\n" ); /* check GSM module status */ 
	vGSM_SendATcommand( ( uint8_t* )"AT\r\n", 4 ); 
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 50 ) == GSM_ENOERR ) { /* Power off GSM module */ 
		GPIO_SetBits( GSM_PWR_PORT, GSM_PWR_PIN ); 
		delay_ms( 1000 ); GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN ); 
	} 
}

/*============================================================================================*/
/**
  * @brief  Start GSM module.
  * @param  None
  * @retval None
  */
void vGSM_Start( void ) {
	uint8_t i=0;
	
#if GSM_USE_UC20
	GSM_Delay_Start = 1000;
	
	while(i<11) {
		if( GPIO_ReadInputDataBit( GSM_STATUS_PORT, GSM_STATUS_PIN ) == 0 ){
			/* GSM module off */
			printDebug_Semphr( INFO,"[GSM_TASK]: AT error\r\n" );
			
			/* start module wait 2s */
			GPIO_SetBits( GSM_PWR_PORT, GSM_PWR_PIN );
			delay_ms( GSM_Delay_Start );
			GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN );
		}else {	
			/* GSM module on */
			printDebug_Semphr( INFO,"[GSM_TASK]: AT OK\r\n" );
			printDebug_Semphr( INFO, "<GSM_Start>: GSM Power On!!! \r\n" );
			break;
		}
		i++;
		delay_ms( 5000 );
		eGSM_receivePacket();
	}
	
#endif
	
#if GSM_USE_M10
	GSM_Delay_Start	= 2500;
	
	while(i<11) {
		/* check GSM module status */
		vGSM_SendATcommand( ( uint8_t* )"AT\r\n", 4 );
		if ( ucGSM_waitForOk( (uint8_t *)"OK", 200 ) != GSM_ENOERR ) {
			/* GSM module off */
			printDebug_Semphr( INFO,"[GSM_TASK]: AT error\r\n" );
			
			/* start module wait 2s */
			GPIO_SetBits( GSM_PWR_PORT, GSM_PWR_PIN );
			delay_ms( GSM_Delay_Start );
			GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN );
		}
		else {
			_Communication_Status |= COMMU_CONNECTED;											// set flag when Connect success
			
			/* GSM module on */
			printDebug_Semphr( INFO,"[GSM_TASK]: AT OK\r\n" );
			printDebug_Semphr( INFO, "<GSM_Start>: GSM Power On!!! \r\n" );
			break;
		}
		i++;
		delay_ms( 5000 );
		eGSM_receivePacket();
	}
	
#endif
	
#if GSM_USE_M10
	vGSM_SendATcommand( ( uint8_t* )"AT+QIREGAPP\r\n", 13 );
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 5 ) != GSM_ENOERR )
		printDebug_Semphr( DETAIL,"[GSM_TASK]: AT+QIREGAPP error\r\n" );									// for M10
	vGSM_SendATcommand( ( uint8_t* )"AT+QIPROMPT=2\r\n", 15 );
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 5 ) != GSM_ENOERR )
		printDebug_Semphr( DETAIL,"[GSM_TASK]: AT+QIPROMPT error\r\n" );									// for M10
#endif
	
	vGSM_SendATcommand( ( uint8_t* )"ATE0\r\n", 6 );
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 5 ) != GSM_ENOERR )
		printDebug_Semphr( DETAIL,"[GSM_TASK]: ATE0 error\r\n" );
	vGSM_SendATcommand( ( uint8_t* )"AT&W\r\n", 6 );
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 5 ) != GSM_ENOERR )
		printDebug_Semphr( DETAIL,"[GSM_TASK]: AT&W error\r\n" );
	
#if GSM_USE_UC20
	vGSM_SendATcommand( ( uint8_t* )"AT+QIACT=1\r\n", 12 );															// Activate context 1.
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 200 ) != GSM_ENOERR )
		printDebug_Semphr( INFO,"[GSM_TASK]: AT+QIACT=1 error\r\n" );
	
	vGSM_SendATcommand( ( uint8_t* )"AT+QHTTPCFG=\"requestheader\",1\r\n", 31 );															// Activate context 1.
	if ( ucGSM_waitForOk( (uint8_t *)"OK", 200 ) != GSM_ENOERR )
		printDebug_Semphr( INFO,"[GSM_TASK]: AT+QHTTPCFG=1 error\r\n" );
#endif
	
}
/*============================================================================================*/
/**
  * @brief  Start GSM module.
  * @param  None
  * @retval None
  */
void vGSM_Restart( void ) {
	printDebug_Semphr( INFO, "<GSM_Restart>: GSM HW Reset!!! \r\n" );
	
	/* Reset GSM module */
	GPIO_SetBits( GSM_PWR_PORT, GSM_PWR_PIN );
	delay_ms( 1000 );
	GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN );
	
	delay_ms( 5000 );
	GPIO_SetBits( GSM_PWR_PORT, GSM_PWR_PIN );
	delay_ms( 1000 );
	GPIO_ResetBits( GSM_PWR_PORT, GSM_PWR_PIN );
	
	delay_ms( 10000 );
	vGSM_Start( );
	
}
/*============================================================================================*/
/**
  * @brief  Connect to TCP/IP
  * @param  IP 		: IP Address
						port 	: number port
  * @retval Connection GSM status
							- 0 : connect fail
							- 1 : connect ok
  */
uint8_t ucGSM_ConnectTCP( char *IP,int port ) {
	/*-- Variable about  --*/
	eGSMState 					eState = GSM_STATE_IPCLOSE;
	eGSMConnectStatus		eConnectStatus;
	
	uint8_t 	CMD_openTCP[100];
	
 	while ( 1 ) {
 		switch ( eState ) {			
			case GSM_STATE_IPCLOSE :
#if GSM_USE_M10
				/* Connect to server with IP address and port. */
				printDebug_Semphr( DETAIL,"[GSM_TASK]: AT+QIOPEN\r\n" );
				sprintf( ( char* )CMD_openTCP,"AT+QIOPEN=\"TCP\",\"%s\",%d\r\n",IP,port );
				vGSM_SendATcommand( CMD_openTCP, strlen( ( char* )CMD_openTCP ) );
				
				/* delay wait response. */
				delay_ms( 1000 );
			
				/* Check connection. */
				eConnectStatus = ucGSM_waitForConnect( 6000 );
				
				if( eConnectStatus == GSM_CONNECT_OK ) {
					eState = GSM_STATE_CONNECTOK;
				}
				else {
					eState = GSM_STATE_CMDERROR;
				}
				break;
#endif
			
#if GSM_USE_UC20
				vGSM_SendATcommand( ( uint8_t* )"AT+QIOPEN=1,0,\"TCP\",\"smasmartdevice.appspot.com\",80,0,2", 55 );
			
				/* Check connection. */
				eConnectStatus = ucGSM_waitForConnect( 6000 );
				
				if( eConnectStatus == GSM_CONNECT_OK ) {
					eState = GSM_STATE_CONNECTOK;
				}
				else {
					eState = GSM_STATE_CMDERROR;
				}
				break;
#endif
				
			case GSM_STATE_CONNECTOK :
					return GSM_CONNECT_OK;
			
			case GSM_STATE_CMDERROR :
				return GSM_CONNECT_FAIL;
			
		}
 	}
}
/*============================================================================================*/
/**
  * @brief  Query current connection status.
  * @param  None
  * @retval GSM state
  */
eGSMState ucGSM_QueryState( void ) {
	/*-- Variable about check response --*/
	char			*ucBufferCheck;
	uint8_t 	ucBuffer[40];
	uint8_t 	ucLength;
	
	/*-- Variable about timer --*/
	uint8_t ucInterchar = 1, ucTimewait = 1;
	
	/* Send AT command to query connection state */
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QISTAT\r\n", 11 );
	
	while( 1 ) {
		/* wait response and clear buffer */
		delay_ms( 200 );
		strcpy( ( char* )ucBuffer, " " );
		
		/* Read response from GSM */
		ucLength = usiUSART_ReadGSM( ucBuffer, 40, ucInterchar );
		
		/* checking state */
		ucBufferCheck = strstr( ( char* )ucBuffer, "CONNECT OK" );
		if( ucBufferCheck[0] == 'C' ){ return GSM_STATE_CONNECTOK; }
		
		ucBufferCheck = strstr( ( char* )ucBuffer, "IP CLOSE" );
		if( ucBufferCheck[0] == 'I' ){ return GSM_STATE_IPCLOSE; }
		
		ucBufferCheck = strstr( ( char* )ucBuffer, "ERROR" );
		if( ucBufferCheck[0] == 'I' ){ return GSM_STATE_CMDERROR; }
				
		ucTimewait--;
		if( ucTimewait <= 0 ) {
			return GSM_STATE_IPCLOSE;
		}
	}
}
/*============================================================================================*/
/**
  * @brief  Send AT Command to GSM Module
  * @param  s : string command
						size : size string
  * @retval -
  */
void vGSM_SendATcommand( volatile uint8_t *s, uint32_t size ) {
	uint32_t cnt=0;
	
	while( cnt < size ) {
// 		printDebug_Semphr( INFO,"%c",s[cnt]);
		/* wait until data register is empty */
		while( !( EVAL_COM3->SR & 0x00000040 ) );
		USART_SendData( EVAL_COM3, s[cnt] );
		cnt++;
	}
//	printDebug_Semphr( INFO, "[GSM_TASK]: size pkt to send : %d\r\n", cnt );
}
/*============================================================================================*/
/**
  * @brief  Tranmit packet to server
  * @param  ucStrPacket : Data packet
						ulLength		: length packet
  * @retval None
  */
eGSMReturnCode eGSM_transmitPacket( uint8_t *ucStrPacket, uint32_t uiLength ) {
	uint8_t 	ucStrEscape[RX_BUFFER_SIZE3];
 	uint8_t 	ucSize = 0;
	
	/* Special Character AT command for send packet ( Ctrl+Z )*/
	uint8_t CTRL_Z[2]; CTRL_Z[0] = 0x1A; CTRL_Z[1] = '\0';
	
	/*------------------------------------------------------------------*/
	
 	/*	Show old packet	*/
	if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ) {
		printDebug( DETAIL, "[GSM_TASK]: Send : %d\r\n", uiLength );
		print_payload( DETAIL, ucStrPacket, uiLength );
		printDebug(DETAIL, "\r\n");
		xSemaphoreGive(xSemaphore);
	}

 	/*	escapes packet	*/
 	ucSize = ucGSM_escapes( ucStrPacket, ucStrEscape, uiLength );
	
 	/*=======================	Send Packet	========================*/
	printDebug_Semphr( DETAIL, "[GSM_TASK]: AT+QISEND\r\n" );
  vGSM_SendATcommand( (volatile uint8_t *)"AT+QISEND\r", 10 );
	delay_ms( 100 );

	vGSM_SendATcommand( ucStrEscape, ucSize );
	vGSM_SendATcommand( CTRL_Z, 1 );
	
	return GSM_RET_OK;
}
/*============================================================================================*/
/**
  * @brief  receive Packet from UART5 and send packet to Manager Task
  * @param  None
  * @retval Return protocol function code
  */
eGSMReturnCode eGSM_receivePacket( void ) {
 	PACKET_DATA sFormatrecv_GSM;
	
	uint8_t 		recvPacket[RX_BUFFER_SIZE3];           
	uint16_t 		size = 0;
	
	// -- for packet validation -- //
	uint16_t 	offset = 0;
	uint16_t 	ulLength = 0x0000;
	uint16_t 	total = 0;
	
	int 			cmp_res = 0;
	char			str_ok[] = {"\r\nOK\r\n"};
	char			str_err[] = {"\r\nERROR\r\n"};
	char			str_close[] = {"\r\nCLOSED\r\n"};
	
	char 			*Credit;
	/* --------------- Process GSM Data ---------------- */// USART 3
	strcpy( ( char* )recvPacket, " " );
	size = usiUSART_ReadGSM( recvPacket, RX_BUFFER_SIZE3, 5 );
	if( size <= 0 ) {
		/* Not have receive data packet */
		return GSM_RET_PKT_EMPTY;
	}
 	else {
		if( size < 20 ) {
			if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ){
				printDebug( INFO, "[GSM_TASK]: Receive : %d\r\n", size );
				print_payload( INFO, recvPacket, size );
				xSemaphoreGive( xSemaphore );
			}	
			cmp_res = memcmp( ( char* )recvPacket, str_ok, 6 );
			if(cmp_res == 0) {
				printDebug_Semphr( INFO, "[GSM_TASK]: OK \r\n" );
			}
			cmp_res = memcmp((char*)recvPacket, str_err, 9);
			if( cmp_res == 0 ) {
				printDebug_Semphr( MINOR_ERR, "[GSM_TASK]: ERROR \r\n" );
			}
			cmp_res = memcmp((char*)recvPacket, str_close, 10);
			if( cmp_res == 0 ) {
				printDebug_Semphr( MINOR_ERR, "[GSM_TASK]: CLOSED \r\n" );
				return GSM_RET_CLOSED;
			}
		}
		
 		/*		Show All Packet		*/
 		if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ){
 			printDebug( INFO, "[GSM_TASK]: Rcv : %d\r\n", size );
 			print_payload( INFO, recvPacket, size );
 			printDebug( INFO, "\r\n" );
 			xSemaphoreGive( xSemaphore );
 		}
 	}
	do {		
		/* seach for header */
		offset += strcspn( ( char* )&recvPacket[offset], "\x7E" );
		if( offset >= size ) {
			printDebug_Semphr( WARNING,"[GSM_TASK]: invalid coordinator packet\r\n" );   	// not found Start Delimiter 0x7E
			break;
		}
		ulLength = ( recvPacket[offset+1] & 0xffff )<< 8;             // check packet length ( MSB )
		ulLength |= recvPacket[offset+2];                            // check packet length ( LSB )
		printDebug_Semphr( DETAIL,"[GSM_TASK]: len=%d( %04x )offset: %d, size: %d\r\n", ulLength, ulLength, offset, size );						// Show packet ulLengthgth
		
		total += ulLength;                                          	// update total analyse 
		if ( total > size )	{																			// check length validation	
			offset++;
			total = offset;                                     	// roll back total analyse                          
			printDebug_Semphr( WARNING,"[GSM_TASK]: invalid length!!\r\n" );
			continue;
		}
		
    /* checksum error detection */
		printDebug_Semphr( DETAIL,"[GSM_TASK]: checksum = %02X\r\n", recvPacket[offset+3+ulLength] );
		if( ucGSM_checksum( &recvPacket[offset+3], ulLength )!= recvPacket[offset+3+ulLength] ) {
			printDebug_Semphr( WARNING,"[GSM_TASK]: checksum error\r\n" );
			offset++;
			total = offset;                                    		// roll back total analyse          
		}
    else {
			/* got a valid packet */
			printDebug_Semphr( DETAIL,"[GSM_TASK]: checksum correct\r\n" );
			
			/* analyse API-specific Structure */
			sFormatrecv_GSM.length = ulLength;
			memcpy( sFormatrecv_GSM.packet, &recvPacket[offset+3], sFormatrecv_GSM.length );
		
			/* send queue to Manager task */
			if( !xQueueSend( Queue_PacketFromGSM, &sFormatrecv_GSM , 0 ) ){
				printDebug_Semphr( WARNING,"[GSM_TASK]: can not send Queue_PacketFromGSM to Manager task\r\n" );
			}
		}
		offset += 4+ulLength;
		total = offset;
 	}while( total < size );
	
 	return GSM_RET_OK;
}
/*============================================================================================*/
/**
  * @brief  Get Internation Mobile Equipment Identity(IMEI).
  * @param  None
  * @retval IMEI : IMEI of the GSM module
  */
uint8_t ucGSM_GetIMEI( uint8_t* IMEI ) {
	char 		ucBuffer[RX_BUFFER_SIZE3];
	int 		i=0;
	uint8_t ucLength=0;
	
	/* send at command for read IMEI */
 	vGSM_SendATcommand( (volatile uint8_t *)"AT+GSN\r\n", 8 );
 	delay_ms( 50 );
 	
  ucLength = usiUSART_ReadGSM( ( uint8_t* )ucBuffer, RX_BUFFER_SIZE3, 5 );
	
	while(i < ucLength) {
		if(ucBuffer[i] == 'O' && ucBuffer[i+1] == 'K') {
			memcpy(IMEI, &ucBuffer[i-19], 15);
 			printDebug_Semphr( INFO,"<ucGSM_GetIMEI>: Get IMEI : %s\r\n", IMEI );
			return 1;
		}
		i++;
	}
	
	printDebug_Semphr( WARNING,"[GSM_TASK]: Get IMEI error\r\n" );
	return 0;
}
/*============================================================================================*/
/**
  * @brief  Query Credit balance
  * @param  Provider 	: Network provider
						code			: Prepaid code
  * @retval None
  */
void vGSM_QueryCreditBalance( uint8_t ucProvider, uint8_t *ucCode ) {
	char ucBuffer[50];
	
	/* Check Provider. */
	if( ucProvider == GSM_Provider_AIS ) {
		sprintf( ucBuffer, "AT+CUSD=1,\"*120*%s#\"\r\n", ( char* )ucCode );
		vGSM_SendATcommand( ( uint8_t* )ucBuffer, strlen( ( char* )ucBuffer ) );
	}
	else if( ucProvider == GSM_Provider_DTAC ) {
		sprintf( ucBuffer, "AT+CUSD=1,\"*100*%s#\"\r\n", ( char* )ucCode );
		vGSM_SendATcommand( ( uint8_t* )ucBuffer, strlen( ( char* )ucBuffer ) );
	}
	else {
		printDebug_Semphr( WARNING, "[GSM_TASK]: Add Credit Balance [Provider not found]\r\n" );
	}
}

/*============================================================================================*/
/**
  * @brief  Read Credit balance by send at command
  * @param  Provider : Network provider 
  * @retval None
  */
void vGSM_ReadCreditBalance( uint8_t ucProvider ) {
	/* Check Provider */
	if ( ucProvider == GSM_Provider_AIS ) {
		/* send at command fot ais provider */
		vGSM_SendATcommand( (volatile uint8_t *)"AT+CUSD=1,\"*121#\"\r\n",19 );
	}
	else if ( ucProvider == GSM_Provider_DTAC ) {
		/* send at command fot dtac provider */
		vGSM_SendATcommand( (volatile uint8_t *)"AT+CUSD=1,\"*101*9#\"\r\n",21 );
	}
	else {
		printDebug_Semphr( WARNING,"[GSM_TASK]: Read credit balance [Provider not found]\r\n" );
	}
}
/*============================================================================================*/
/**
  * @brief  Get Network provider.
  * @param  None
  * @retval Provider : Network provider
  */
uint8_t ucGSM_GetProvider( void ) {
	char 		*ucBuffer;
	uint8_t ucLength=0;
	
	/* send at command fot read provider */
	vGSM_SendATcommand( (volatile uint8_t *)"AT+COPS?\r\n", 10 );
	delay_ms( 50 );
	
	ucLength = usiUSART_ReadGSM( ( uint8_t* )ucBuffer, 20, 50 );
	
	if( ucLength > 0 ) {
		/* -------checking for AIS ------- */
		ucBuffer = strstr( ( char* )ucBuffer, "AIS" );
		if( ucBuffer[0] == 'A' ) { return GSM_Provider_AIS; }
		
		/* -------checking for DTAC ------- */
		ucBuffer = strstr( ( char* )ucBuffer, "DTAC" );
		if( ucBuffer[0] == 'D' ) { return GSM_Provider_DTAC; }
		
		/* -------checking for TRUE ------- */
		ucBuffer = strstr( ( char* )ucBuffer, "TRUE" );
		if( ucBuffer[0] == 'T' ) { return GSM_Provider_TRUE; }
		
	}
	else {
		printDebug_Semphr( WARNING,"[GSM_TASK]: Get Provider error\r\n" );
	}
}
/*============================================================================================*/
/**
  * @brief  Get Network provider.
  * @param  Provider : Network provider
  * @retval None
  */
void vGSM_SetAPN( uint8_t ucProvider ) {
	
	/* Check Provider */
	if ( ucProvider == GSM_Provider_AIS ) {
		/* send at command fot set apn of ais provider */
		vGSM_SendATcommand( (volatile uint8_t *)"AT+QICSGP=1,\"internet\"\r\n", 24 );
		if( ucGSM_waitForOk( (uint8_t *)"OK", 5 )!= GSM_ENOERR ) {
			printDebug_Semphr( DETAIL, "[GSM_TASK]: Set APN error\r\n" );
		}
	}
	else if ( ucProvider == GSM_Provider_DTAC ) {
		/* send at command fot set apn of DTAC */
		vGSM_SendATcommand( (volatile uint8_t *)"AT+QICSGP=1,\"www.dtac.co.th\"\r\n", 30 );
		if( ucGSM_waitForOk( (uint8_t *)"OK", 5 )!= GSM_ENOERR ) {
			printDebug_Semphr( DETAIL, "[GSM_TASK]: Set APN error\r\n" );
		}
	}
	else {
		printDebug_Semphr( WARNING,"[GSM_TASK]: Set APN [Provider not found]\r\n" );
	}
}
/*============================================================================================*/
/**
  * @brief  Read signal quality report.
  * @param  None
  * @retval Return signal percent(0-100%)
  */
uint8_t ucGSM_ReadSignalQuality( void ) {
	uint8_t ucStr_CSQ[5] = "+CSQ";
	uint8_t ucBuffer[20];
	uint8_t	ucSignalPrecent;
	uint8_t i=0,j=0,ucLength=0;
	char		ucGSM_RSSI, *ucStrRSSI;
	
	/* send at command for read signal quality report */
	vGSM_SendATcommand( (volatile uint8_t *)"AT+CSQ\r\n", 8 );
	delay_ms( 50 );
	ucLength = usiUSART_ReadGSM( ucBuffer, 20, 50 );

/*========== code retrun from module ===================	
		0			:	-113 dBm or less
		1			:	-111 dBm
		2..30	:	-109..-53 dBm
		31		:	-51 dBm or greater
		99		:	Not know or not detectable 
========================================================*/
	
	for( i=0, j=0; i < ucLength; i++ ) {
		 if( ( ucBuffer[i] == ucStr_CSQ[j] ) )j++;
		 else j = 0;
		 if( j == ( strlen( ( char* )ucStr_CSQ ) ) ) {
			 ucStrRSSI 	= strtok( ( char* )ucBuffer, " " );
			 ucStrRSSI 	= strtok( NULL, "," );
			 ucGSM_RSSI = atoi( ( char* )ucStrRSSI );
			 
			 if( ucGSM_RSSI == 99 ) { return 0; }
			 
			 /* RSSI convert to precent(%) */
			 ucSignalPrecent = ucGSM_RSSI * 3.33;
			 if( ucSignalPrecent > 100 ) {ucSignalPrecent = 100;}
 			 return ucSignalPrecent;
		 }
	}
	printDebug_Semphr( INFO, "[GSM_TASK]: Signal Quality not value\r\n" );	
	return 0;
}
/*============================================================================================*/
/**
  * @brief  Close TCP or UDP connection
  * @param  None
  * @retval None
  */
void vGSM_CloseConnection( void ) {
	
#if GSM_USE_UC20
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QICLOSE=0\r\n", 14 );								/* send at command close connection */
	if( ucGSM_waitForOk( (uint8_t *)"OK", 5 )== GSM_ENOERR ) {
		printDebug_Semphr( DETAIL, "[GSM_TASK]: CLOSE OK\r\n" );
	}
	else {
		printDebug_Semphr( WARNING, "[GSM_TASK]: CLOSE ERROR\r\n" );
	}
	
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIDEAC=0\r\n", 13 );
	if( ucGSM_waitForOk( (uint8_t *)"OK", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( DETAIL, "[GSM_TASK]: DEACT OK\r\n" );
	}
	else {
		printDebug_Semphr( WARNING, "[GSM_TASK]: DEACT ERROR\r\n" );
	}
#endif

#if GSM_USE_M10
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QICLOSE\r\n", 11 );								/* send at command close connection */
	if( ucGSM_waitForOk( (uint8_t *)"CLOSE", 5 )== GSM_ENOERR ) {
		printDebug_Semphr( DETAIL, "[GSM_TASK]: CLOSE OK\r\n" );
	}
	else {
		printDebug_Semphr( WARNING, "[GSM_TASK]: CLOSE ERROR\r\n" );
	}
	
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QIDEACT\r\n", 12 );
	if( ucGSM_waitForOk( (uint8_t *)"DEACT OK", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( DETAIL, "[GSM_TASK]: DEACT OK\r\n" );
	}
	else {
		printDebug_Semphr( WARNING, "[GSM_TASK]: DEACT ERROR\r\n" );
	}
#endif
}
/*============================================================================================*/
/**
  * @brief  
  * @param  None
  * @retval None
  */
void vGSM_SendHTTPGET( uint8_t* ucURL, uint16_t LenURL ) {
	uint8_t ESC[2] = {0x33,0x00};
	uint8_t* 	ucStrCommand[255];
	uint16_t	Timeout	= 100;

	sprintf( (char*)ucStrCommand, "AT+QHTTPURL=%d,%d\r\n", LenURL, Timeout );
	printDebug_Semphr( INFO, "[GSM_TASK]: %s(%d)\r\n", ucURL, LenURL );
	vGSM_SendATcommand( (volatile uint8_t *)ucStrCommand, 20 );
	if( ucGSM_waitForOk( (uint8_t *)"CONNECT", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( INFO, "[GSM_TASK]: Connect Http OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: Connect Http error\r\n" );
	}
 	
	vGSM_SendATcommand( ucURL, LenURL );
	if( ucGSM_waitForOk( (uint8_t *)"OK" ,200 )== GSM_ENOERR ) {
		printDebug_Semphr( INFO, "[GSM_TASK]: input URL OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: input URL error\r\n" );
	}
 	
	vGSM_SendATcommand( (volatile uint8_t *)"AT+QHTTPGET=60\r\n", 16 );
	if( ucGSM_waitForOk( (uint8_t *)"OK", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( INFO, "[GSM_TASK]: GET OK\r\n" );
	}
	else {
		vGSM_SendATcommand( ESC, 2 );
		printDebug_Semphr( INFO, "[GSM_TASK]: GET error\r\n" );
	}
// 	
//	vGSM_SendATcommand( (volatile uint8_t *)"AT+QHTTPREAD=30\r\n", 17 );
// 	if( ucGSM_waitForOk( (uint8_t *)"OK", 200 )== GSM_ENOERR ) {
// 		printDebug_Semphr( INFO, "[GSM_TASK]: READ OK\r\n" );
// 	}
// 	else {
// 		printDebug_Semphr( INFO, "[GSM_TASK]: READ error\r\n" );
// 	}
// 	
	delay_ms(1000);
}
/*============================================================================================*/
/**
  * @brief  Send HTTP POST Request
  * @param  None
  * @retval None
  */
eGSMErrorCode vGSM_SendHTTPOST( uint8_t* ucURL, uint16_t LenURL, uint8_t* ucBODY, uint16_t LenBODY ) {
	
	uint8_t* 			ucStrCommand[255];
	uint16_t			Timeout	= 30;
	uint8_t 			ucStrEscape[RX_BUFFER_SIZE3];
 	uint8_t 			ucSize = 0;
	uint8_t				printLV = DETAIL;
	/*------------------------------------------------------------------*/
	uint8_t buffer[512];
	int i=0,len=0;
	
	printDebug_Semphr( printLV, "[vGSM_SendHTTPOST]:IMEI(%s): %s(%d)\r\n", _serialnumber_GSM, ucURL, LenURL );
	sprintf((char*)buffer,"");
 	/*	Show old packet	*/
	if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ) {
		printDebug( printLV, "[vGSM_SendHTTPOST]: Send : %d\r\n", LenBODY );
		print_payload( printLV, ucBODY, LenBODY );
		printDebug(printLV, "\r\n");
		xSemaphoreGive(xSemaphore);
	}

	/* Create request headers */
	strcat((char*)buffer, "POST /smasmartdevice HTTP/1.1\r\n");
 	strcat((char*)buffer, "Host: smasmartdevice.appspot.com\r\n");
	sprintf((char*)buffer, "%sX-CLIENT-UUID: %s\r\n", buffer, _serialnumber_GSM);
	/* Create request body */
	sprintf((char*)buffer,"%sContent-Length: %u\r\n\n", buffer, LenBODY);//80
	len = strlen((char*)buffer);
	
 	memcpy(&buffer[len], ucBODY, LenBODY);
	len = len + LenBODY;
	
	if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ) {
		printDebug( printLV, "[vGSM_SendHTTPOST]: Send : %d\r\n", len );
		print_payload( printLV, buffer, len );
		printDebug(printLV, "\r\n");
		xSemaphoreGive(xSemaphore);
	}
 	/*	escapes packet	*/
//  	ucSize = ucGSM_escapes( ucBODY, ucStrEscape, LenBODY );
// 	
// 	if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ) {
// 		printDebug( INFO, "[vGSM_SendHTTPOST]: escapes : %d\r\n", ucSize );
// 		print_payload( INFO, ucStrEscape, ucSize );
// 		printDebug(INFO, "\r\n");
// 		xSemaphoreGive(xSemaphore);
// 	}
	
	/* Set HTTP Server URL */
	sprintf( (char*)ucStrCommand, "AT+QHTTPURL=%d,%d\r\n", LenURL, Timeout );
	vGSM_SendATcommand((volatile uint8_t *)ucStrCommand, strlen((char*)ucStrCommand));
	if( ucGSM_waitForOk( (uint8_t *)"CONNECT", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( printLV, "[GSM_TASK]: Connect Http OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: Connect Http error\r\n" );
		return GSM_ECMD;
	}
	vGSM_SendATcommand( ucURL, LenURL );
	if( ucGSM_waitForOk( (uint8_t *)"OK" ,200 )== GSM_ENOERR ) {
		printDebug_Semphr( printLV, "[GSM_TASK]: input URL OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: input URL error\r\n" );
		return GSM_ECMD;
	}
 	
	/* Send POST HTTP Request */
	sprintf( (char*)ucStrCommand, "AT+QHTTPPOST=%d,50,10\r\n", len );
	vGSM_SendATcommand( (volatile uint8_t *)ucStrCommand, strlen((char*)ucStrCommand) );
	if( ucGSM_waitForOk( (uint8_t *)"CONNECT", 200 )== GSM_ENOERR ) {
		printDebug_Semphr( printLV, "[GSM_TASK]: POST OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: POST error\r\n" );
		return GSM_ECMD;
	}
	
	vGSM_SendATcommand( buffer, len );
	if( ucGSM_waitForOk( (uint8_t *)"OK" ,200 )== GSM_ENOERR ) {
		printDebug_Semphr( printLV, "[GSM_TASK]: input BODY OK\r\n" );
	}
	else {
		printDebug_Semphr( INFO, "[GSM_TASK]: input BODY error\r\n" );
		return GSM_ECMD;
	}
	
	if( ucGSM_waitForOk( (uint8_t *)"+QHTTPPOST" ,10 )== GSM_ENOERR ) {
		vGSM_SendATcommand( (volatile uint8_t *)"AT+QHTTPREAD=30\r\n", 17 );
	}
	else {
		vGSM_SendATcommand( (volatile uint8_t *)"AT+QHTTPREAD=30\r\n", 17 );
		printDebug_Semphr( printLV, "[GSM_TASK]: QHTTPPOST error\r\n" );
		return GSM_ECMD;
	}
	
	return GSM_ENOERR;
}
/*============================================================================================
Support Function
============================================================================================*/
/**
  * @brief  Check packet OK or ERROR
  * @param  None
  * @retval GSM error code
  */
static eGSMErrorCode ucGSM_waitForOk( uint8_t* ucStr, uint16_t ulTimewait ) {
	eGSMErrorCode eStatus = GSM_ETIMEDOUT;
	
	/*-- Variable about check response --*/
	uint8_t 	ucBuffer[15];
	uint8_t 	ucLength;
	uint8_t		ucInterchar = 50;
	char			*ucBufferCheck;
	
	while( ulTimewait > 0 ) {
		/* wait response and clear buffer */
		delay_ms( 100 );
		strcpy( ( char* )ucBuffer, " " );
		
		ucLength = usiUSART_ReadGSM( ucBuffer, 12, ucInterchar );
		
		if( ucLength > 0 ) {
			
			if( xSemaphoreTake( xSemaphore, SEMPHR_WAIT ) ){
				printDebug( DETAIL, "<GSM_waitForOk>: " );
				print_payload( DETAIL, ucBuffer, 12 );
				printDebug( DETAIL, "\r\n" );
				xSemaphoreGive( xSemaphore );
			}	
			ucBufferCheck = strstr( ( char* )ucBuffer, ( char* )ucStr );
			if( ucBufferCheck[0] == ucStr[0] ){ return GSM_ENOERR; }
			
			ucBufferCheck = strstr( ( char* )ucBuffer, "ERROR" );
			if( ucBufferCheck[0] == 'E' ){ return GSM_ECMD; }
		}
		ulTimewait--;
	}
	return eStatus;
}
/*============================================================================================*/
/**
  * @brief  Check connection ok or fail
  * @param  None
  * @retval GSM ack messages
						- 0x00 : Connect OK
						- 0x01 : Connect ERROR
						- 0x02 : Timeout
						- 0x03 : Already connect
  */
static eGSMConnectStatus ucGSM_waitForConnect( uint16_t time_wait ) {
	/*-- Variable about check response --*/
	uint8_t 	ucBuffer[NBUF];
	uint8_t 	ucLength;
	char			*ucBufferCheck;
	
	/*-- Variable about time wait --*/
	uint8_t		ucInterchar 				= 1;
	uint8_t CTRL_Z[2]; CTRL_Z[0] = 0x1A; CTRL_Z[1] = '\0';
	
	/* wait response */
	delay_ms( 3000 );
	while( 1 ) {
		delay_ms( 10 );
		/* Clear buffer */
		strcpy( ( char* )ucBuffer, " " );
		
		/* Read response from GSM */
		ucLength = usiUSART_ReadGSM( ucBuffer, NBUF, ucInterchar );
//  		printf( "%s\r\n",ucBuffer );
		
		if( ucLength > 0 ) {
			/* -------checking for ALREADY------- */
			ucBufferCheck = strstr( ( char* )ucBuffer, "ALREADY" );
			if( ucBufferCheck[0] == 'A' ){ return GSM_CONNECT_OK; }
			
			/* -------checking for CONNECT OK------- */
			ucBufferCheck = strstr( ( char* )ucBuffer, "CONNECT" );
			if( ucBufferCheck[0] == 'C' ){ return GSM_CONNECT_OK; }
			
			/* -------checking for CONNECT FAIL------- */
			ucBufferCheck = strstr( ( char* )ucBuffer, "FAIL" );
			if( ucBufferCheck[0] == 'F' ){ return GSM_CONNECT_FAIL; }
			
			/* -------checking for CONNECT ERROR------- */
			ucBufferCheck = strstr( ( char* )ucBuffer, "ERROR" );
			if( ucBufferCheck[0] == 'E' ){ return GSM_CONNECT_FAIL; }
		}
		
		time_wait--;
		if( time_wait <= 0 ) {
			vGSM_SendATcommand( CTRL_Z, 1 );
			printDebug_Semphr( WARNING,"[GSM_TASK]: timeout!!\r\n" );
			return GSM_CONNECT_FAIL;
		}
	}
}
/*============================================================================================*/
/**
  * @brief  escapes packet for send to server
  * @param  src : packet
  * @retval size : size of packet after escapes
  */
static uint8_t ucGSM_escapes( const uint8_t* ucSrc, uint8_t* ucRetSrc, uint32_t ulLength )
{
	uint8_t ucCnt 				= 0;
	uint8_t ucIndex 			= 0;
	uint8_t ucCharacter;
	
	/************************************************************ 
		ex :: 0x01 0x1A 0x57 --after escapes--> 0x01 0xFE 0x01 0x57  
	**************************************************************/
	while( ucCnt != ulLength ) {
		ucCharacter = *( ucSrc++ );
		switch( ucCharacter ) {
			case 0x1A : 
				ucRetSrc[ucIndex++] = 0xFE;
				ucRetSrc[ucIndex++] = 0x01;
				break;
			case 0x08 : 
				ucRetSrc[ucIndex++] = 0xFE;
				ucRetSrc[ucIndex++] = 0x02;
				break;
			case 0x1B :
				ucRetSrc[ucIndex++] = 0xFE;
				ucRetSrc[ucIndex++] = 0x03;
			break;
			case 0xFE :
				ucRetSrc[ucIndex++] = 0xFE;
				ucRetSrc[ucIndex++] = 0xFE;
				break;
			default :
				ucRetSrc[ucIndex++] = ucCharacter;
		}
		ucCnt++;
	}
	ucRetSrc[ucIndex] = '\0';/* Ensure nul terminator */
	return ucIndex;
}
/*============================================================================================*/
/**
  * @brief  check length packet
  * @param  buf[] : buffer
						len   : length
  * @retval check sum value
  */
static uint8_t ucGSM_checksum( uint8_t buf[], uint16_t ulLength ) {
	uint16_t 	i;
  uint8_t 	sum = 0;
	
  for( i = 0; i < ulLength; i++ ) {
		sum += buf[i];
  }                 
  return ( 0xff - ( sum & 0xff ) );
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/

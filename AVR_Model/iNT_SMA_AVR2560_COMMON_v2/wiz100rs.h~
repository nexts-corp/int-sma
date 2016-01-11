#ifndef WIZ100RS_H
#define WIZ100RS_H
#include <mega2560.h>
#include "main.h"
#include "queue.h"
#include "debug.h"


#define WIZ100RS_CONF_MODE               (PORTH |= (1 << 7))                   //PORTH.7 set 1 = config, 0 = normal mode
#define WIZ100RS_NORM_MODE               (PORTH &= ~(1 << 7))
#define WIZ100RS_ON                      (PORTG |= (1 << 4))                   //PORTG.5 set 1 = normal mode, 0 = reset mode
#define WIZ100RS_OFF                     (PORTG &= ~(1 << 4))
#define WIZ100RS_DATA_MAX_SIZE           500
#define WIZ100SR_PRINT_DEBUG            0




typedef struct{
    char macAddress[12];       //MAC Address (xxxxxx is unique factory value)      //12=ASCII , 6 = byte
    char mode;                //Mode (MIXED mode: 01, SERVER mode: 02,Client mode: 00)
    iUChar_t ip[4];               //IP address
    iUChar_t subnet[4];           //Subnet mask
    iUChar_t gateWay[4];          //Gateway address
    char localPort[2];        //Local Port Number (Module’s Port Number)
    iUChar_t serverIP[4];         //Server IP address
    char serverPort[2];       //Server Port Number 
    char serialSpeed;         //Serial speed(bps) default is FE:57600
    char serialDatSize;       //Serial data size (08: 8 bit), (07: 7 bit)
    char parity;              //Parity (00: No), (01: Odd), (02: Even) 
    char stopBit;             //Stop bit
    char flowControl;         //Flow control (00: None), (01: XON/XOFF), (02: CTS/RTS)
    char delimiterChar;       //Delimiter char
    char delimiterSize[2];    //Delimiter size
    char delimiterTime[2];    //Delimiter time
    char delimIdleTime[2];    //Delimiter idle time
    char debugCode;           //Debug code (00: ON), (01: OFF)
    char swMajorVersion;      //Software major version
    char swMinorVersion;      //Software minor version
    char dhcpOption;          //DHCP Option (00: DHCP OFF, 01:DHCP ON)
    char updMode;             //UDP mode (00: TCP; 01: UDP)
    char connectStatus;       //Connection Status (00: not connected, 01: connected)
    char dnsFlag;             //DNS Flag (00:not use DNS, 01:use DNS)
    char dnsServerIP[4];      //DNS Server IP address
    char sdn[32];             //Server Domain Name
    char reservedByte;        //Reserved Byte 
}iWiz100srConfig_t;

#define WIZ_CONNECTED       0x01
#define WIZ_CLOSE           0x02
#define WIZ_UNKNOW          0x00
#define WIZ_CONN_FAIL       0x00

#define WIZ_DATA_EMPTY          0x00
#define WIZ_DATA_AVAILABLE      0x01
//#define WIZ_DATA_ERR_CODE      0x01

#define WIZ_STAT_UNKNOW         0x00
#define WIZ_STAT_UPDATE         0x01
#define WIZ_STAT_READ_SUCCESS   0x02


#define WIZ_RX_UNKNOW_MODE      0x00
#define WIZ_RX_STAT_MODE        0x01
#define WIZ_RX_DATA_MODE        0x02

#define WIZ_HTTP_RES_CODE_NOTFOUND      0x03             //Other
#define WIZ_HTTP_RES_CODE_OK            0x01             //200
#define WIZ_HTTP_RES_CODE_ERROR         0x00             //404

#define WIZ_HTTP_CONTEXT_LEN_NOTFOUND     0x00 //context-len not found
#define WIZ_HTTP_CONTEXT_LEN_LESS         0x00 //context-len < 0
#define WIZ_HTTP_CONTEXT_LEN_OVER         0x00 //context-len > 400
#define WIZ_HTTP_CONTEXT_LEN_OK           0x01 //context-len exist

#define WIZ_CONFIG_READ_MODE        0x04
#define WIZ_CONFIG_WRITE_MODE       0x05
#define WIZ_CONFIG_NORMAL_MODE      0x06

typedef struct{
  unsigned char status;
  unsigned char length;
  unsigned char *valueRX;  //mallog allocate memory
}iWiz100srConnection;


typedef struct{
    unsigned char responseCode;
    unsigned int contentLength;
    char connStatus; //close , keep
}iHTTP_t;

extern unsigned char iWizStatus;
extern unsigned char iWizRXMode;
//extern iWiz100srConnection iWizStatus;
//extern iWiz100srConnection iWizDataRX;


//typedef struct{
//    unsigned char mode; //0 = config, 1= data
//    unsigned int length;
//    char value[WIZ100RS_DATA_MAX_SIZE];
//}iWiz100srData_t;


typedef void * iWiz100rsHandle;
//extern void (*pfiWiz100rs)(void);
extern int (*piWizCallBack)(char *,unsigned int);
//extern int (*piWizCallBack)(char * const,unsigned int const );
extern iWiz100srConnection iWizData;
extern iHTTP_t iHTTPStatus;

void iWizGetConf();
void iWizSetConf();

void iWizRXdata();
void iWizTXdata();

void iWizRXTimeOut();
void iWizSend(const char * data_p,int len_p);
int iWizCBChekStat(char * const data_p,unsigned int const len_p);
int iWizCBCheckHTTPResCode(char *const data_p,unsigned int const len_p);
int iWizCBReadData(char *const data_p,unsigned int const len_p);
int iWizCBCheckHTTPContextLen(char *const data_p,unsigned int const len_p);
void iWizReceive();
int iWizConnected();
int iWizRead();  //int iWizRead(char * buffOut_p,unsigned int * lenOut_p);
int iWizStatusConnected(char * rx0Data, unsigned int rx0Index);
void iWizReadConfig();
iChar_t iWizConfigSend(iChar_t * pviCmd_arg,iChar_t *pviDataBuff_arg);
iChar_t iWizConfigIPParser(iChar_t *pviOutData_arg,iChar_t *pviInData_arg);
iChar_t iWizConfigMACParser(iChar_t *pviOutData_arg,iChar_t *pviInData_arg);
iChar_t iMapAsciiHexToChar(iChar_t *chIn_arg);
void iWizReActiveDataMode();

iUChar_t iWizCheckStatus();

/*
    Serial.println("connected");
    // Make a HTTP request:
    client.println("POST /smasmartdevice HTTP/1.1");
    client.println("Host: smasmartdevice.appspot.com");
    //client.println("User-Agent: Arduino/1.0");
    //client.println("Content-Type: application/x-www-form-urlencoded");
    //client.println("Connection: close");
    sprintf(outBuf, "Content-Length: %u\r\n", sizeof(dataBuff)); //80
    client.println(outBuf);
*/
//int connect(){
//    
//   return 1;
//}


#endif 
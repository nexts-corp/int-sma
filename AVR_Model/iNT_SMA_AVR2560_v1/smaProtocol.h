#ifndef SMA_PROTOCOL_H
#define SMA_PROTOCOL_H

#include "main.h"
#include "configDef.h"

#define PTC_INVALID         0x00
#define PTC_SUCCESS         0x01

#define PTC_DS_INVALID      0x00
#define PTC_DS_SUCCESS      0x01

#define PTC_LEN_INVALID     0x00  
#define PTC_LEN_SUCCESS     0x01

#define PTC_CS_INVALID      0x00  
#define PTC_CS_SUCCESS      0x01

#define PTC_HOST_NOT_REQ    0x00
#define PTC_HOST_REQ        0x01

///////////////////Tag define/////////////
#define TAG_2A      0x2a
#define TAG_3A      0x3a
#define TAG_4A      0x4a           //Config
#define TAG_5A      0x5a           //Data
#define TAG_6A      0x6a
#define TAG_7A      0x7a
#define TAG_8A      0x8a
#define TAG_9A      0x9a





//extern iChar_t * pviPTCData;


//-----------------Protocol-----------------------
typedef struct{
      iChar_t len[2]; //length
      iChar_t mti[2]; //message type indication
      iChar_t tid[8]; //terminal id
      iChar_t tim[4]; //transaction time 
      iChar_t fid[2]; //frame id 
}iPTCHeader_t;

typedef struct{
      iUInt_t len;   //length
      iChar_t * value; //data element
}iPTCDataElem_t;

     
typedef struct{
     iPTCHeader_t hd;
     iPTCDataElem_t dat;
}iPTC_t;
//----------------------------------------------

//-------------------Status & Data format-------
typedef struct{
     iChar_t tag;   //command 
     iChar_t statusType;   //status type
     iChar_t dataType;   //data type
     iChar_t * value;   //value
}iStatusFormat_t;

typedef struct{
     iChar_t tag;   //command 
     iChar_t channelID;   //channel id
     iChar_t sensorType;   //sensor type
     iChar_t dataType;   //data type
     iChar_t * value;   //value
}iStatusDataFormat_t;
//-----------------------------------------------

//-----Host & Terminal ack or response-----------
typedef struct{
    iChar_t tag;      //command
    iChar_t resCode;  //response code
}iPTCAck_t;
//-----------------------------------------------

////--------------------change interface mode--------------------------
//typedef struct{
//     unsigned char mode;   //input(RX) , output(TX) 
//     unsigned char interface;   //interface   lan , wifi , sd card 
//     unsigned char *fnBinding;   //interface   lan , wifi , sd card
//}ST_MODE_CONTROL;
////-------------------------------------------------------------------

void inBoundParser();
void OutBoundParser();

void protocolSendACK();     //send ack to server
void protocolGetACK();      //get ack from server

void getProtocolQueue();     //Protocol 's queue is for get from management task
void setProtocolQueue();     //Protocol 's queue is for set give other task

void tag2A();
void tag3A();
void tag4A();  
void tag5A();

void protocolManagement();


iChar_t iPTCParser(iData_t *pviOutData_arg);
iInt_t iCheckSum(iChar_t buf_ags[], iInt_t len_ags);

iUChar_t iPTCCheckHostReq(iData_t *pviData_arg);
iInt_t iGenFID();
void iPTCPack(iData_t *pviOutData_arg, iChar_t *pviData_arg, iUInt_t viLength_arg, iChar_t *pviMTI_arg, iChar_t *pviTID_arg);


#endif 
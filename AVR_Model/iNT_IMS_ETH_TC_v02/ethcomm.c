#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <delay.h>
#include "ethcomm.h"
#include "io.h"   
#include "log.h"
#include "logqueue.h"
#include "debug.h"
                                                
eeprom char     DEVICE_SERIAL[8]    = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};     

TIMER           commLostTimer       = 0;                  
unsigned char   duplicateListClear  = 0;                     
                                              
struct log_wait_st    logDataWait   = {0x00,0x00};
struct log_wait_st    logEventWait  = {0x00,0x00};
                 
/**************************************************************/
/***** Higher Level Communication Management (IMS_Protocol)****/
/**************************************************************/
/*============================================================*/
int ETH_sendNack(char frameID,char errCode)
{
    int     i=0,k=0;
    char    frameData[2+sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]      = PROTOERROR;    
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    frameData[i++]      = errCode;
                                    
    printDebug("<ETH_sendNack> sending..\r\n");   
    //print_payload(frameData,i);
    ETH_send(frameData,i,frameID,0);        
    
    return 0;
}
/*============================================================*/                    
int ETH_sendStatusRequestAck(char frameID,char accm,char status)
{
    int     i=0,k=0;
    char    frameData[7+sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]      = STATREQA;      
                  
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                 
                 
    frameData[i++]      = accm;                         
    frameData[i++]      = status;                       
    
    printDebug("<ETH_sendStatusRequestAck> sending..\r\n");   
    //print_payload(frameData,i);
    
    ETH_send(frameData,i,frameID,0);           
    
    return 0;
    
}
/*============================================================*/  
int ETH_sendStatusReportCmd(unsigned long int timestamp, unsigned char accm, unsigned char *paraResp, int paraLen)
{
    int     i=0,k=0;        
    int     frameLen    = 0;
    char    *frameData;    
                         
    frameLen            = 1 + sizeof(DEVICE_SERIAL) + sizeof(timestamp) + sizeof(accm) + paraLen;
    if(frameLen > 250){  
        printDebug("<ETH_sendStatusReportCmd> frameLen too long (%d bytes)\r\n",frameLen);       
        return -1;
    }      
    
    frameData           = (char *) malloc (frameLen);
    if(frameData == NULL){
        printDebug("<ETH_sendStatusReportCmd> malloc failed (%d bytes)\r\n",frameLen);      
        return -1;
    }      
    
    frameData[i++]      = STATREPC;  
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                                   
                 
    memcpy(&frameData[i],&timestamp,sizeof(timestamp));    
    i += sizeof(timestamp);                           
        
    frameData[i++]      = accm;
    
    memcpy(&frameData[i],paraResp,paraLen);    
    i += paraLen;                           
    
    ETH_send(frameData,i,0x00,CMD_TIMEOUT); 
    free( frameData );
}
/*============================================================*/  
int ETH_sendEventReportCmd(char eventType, unsigned long int timestamp, char eventID, char statusCode)
{
    int     i=0,k=0,res=0;       
    char    frameID;
    char    frameData[8 + sizeof(DEVICE_SERIAL)];
     
    if((eventType != TYP_RLT) && (eventType != TYP_LOG) && (eventType != TYP_SAV))
    {             
        printDebug("<ETH_sendEventReportCmd> Wrong eventType 0x%02X\r\n", eventType);
        return -1;
    }                        
    
    frameData[i++]      =   EVENREPC;
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    memcpy(&frameData[i],&timestamp,sizeof(timestamp));    
    i += sizeof(timestamp);       
    
    frameData[i++]      =   eventType;
    frameData[i++]      =   eventID;
    frameData[i++]      =   statusCode;
        
    frameID             = ETH_generateId();
                             
    if(eventType == TYP_RLT)
    {
        res = addEventLogQueue(frameID,timestamp,eventID,statusCode);
        if(res < 0)
        {
            printDebug("<ETH_sendEventReportCmd> addEventLogQueue(0x%02X,%ld,0x%02X,0x%02X) failed\r\n",
                        frameID,timestamp,eventID,statusCode);
        }
    }             
    else if(eventType == TYP_LOG)
    {
        logEventWait.frameID = frameID;        
    }
                         
    printDebug("<ETH_sendEventReportCmd> sending..\r\n");
    print_payload(frameData,i); 
                        
    res = ETH_send(frameData,i,frameID,CMD_TIMEOUT);   
    if(res < 0){            
        return -1;
    }             
    else{
        return 0;
    }
}
/*============================================================*/  
int ETH_sendConfigManageNack(char frameID,char accd,char cnfg,char status)
{
    int     i=0,k=0;                     
    char    frameData[4 + sizeof(DEVICE_SERIAL)];
                             
    frameData[i++]      = CONFMANA;    
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
               
    frameData[i++]      = accd;
    frameData[i++]      = cnfg;
    frameData[i++]      = status;    
                                
    printDebug("<ETH_sendConfigManageNack> sending..\r\n");
    //print_payload(frameData,i);                    
    
    ETH_send(frameData,i,frameID,0);    
    
    return 0;
}                                                                        
/*============================================================*/
int ETH_sendConfigManageAck(char frameID,char accd,char cnfg,char status,char *cnfgData, int cnfgLen)
{
    int     i=0,k=0;                  
    int     frameLen    = 0;
    char    *frameData;              
                                      
                            
    frameLen            = 1 + sizeof(DEVICE_SERIAL) + sizeof(accd) + sizeof(cnfg) + sizeof(status) + cnfgLen;
    if(frameLen > 250){  
        printDebug("<ETH_sendConfigManageAck> frameLen too long (%d bytes)\r\n",frameLen);       
        ETH_sendConfigManageNack(frameID,accd,cnfg,ERROR);
        return -1;
    }
    
    frameData           = (char *) malloc (frameLen);
    if(frameData == NULL){
        printDebug("<ETH_sendConfigManageAck> malloc failed (%d bytes)\r\n",frameLen);      
        ETH_sendConfigManageNack(frameID,accd,cnfg,ERROR);     
        return -1;
    }                                                 
                           
    frameData[i++]      = CONFMANA;
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
          
    frameData[i++]      = accd;
    frameData[i++]      = cnfg;
    frameData[i++]      = status;      
    if(cnfgLen > 0){                     
        memcpy(&frameData[i],cnfgData,cnfgLen);    
        i += cnfgLen;   
    }                              
    
    printDebug("<ETH_sendConfigManageAck> sending..\r\n");
    print_payload(frameData,i);
    
    ETH_send(frameData,i,frameID,0);                           
    
    free( frameData );             
    
    return 0;
}                                                                      
/*============================================================*/
int ETH_sendDataReportCmd(char dataType, unsigned long int timestamp, unsigned int sensorID, float value)
{
    int     i=0,k=0,res=0;      
    char    frameID;
    char    frameData[10 + sizeof(DEVICE_SERIAL)];
    char    did = 0x00;                         
       
    if((dataType != TYP_RLT) && (dataType != TYP_LOG) && (dataType != TYP_SAV))
    {             
        printDebug("<ETH_sendDataReportCmd> Wrong dataType 0x%02X\r\n", dataType);
        return -1;
    }                        
    
    did     = ((sensorID+1) & 0xff) << 4;                                                    
    did    |= dataType;                                                                  
    
    frameData[i++]      =   DATAREPC;
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    memcpy(&frameData[i],&timestamp,sizeof(timestamp));    
    i += sizeof(timestamp);                                    
    
    frameData[i++]      =   did;   
    
    memcpy(&frameData[i],&value,sizeof(value));    
    i += sizeof(value);               
     
    frameID             = ETH_generateId();
                             
    if(dataType == TYP_RLT)
    {
        res = addDataLogQueue(frameID,timestamp,sensorID,value);
        if(res < 0)
        {
            printDebug("<ETH_sendDataReportCmd> addDataLogQueue(0x%02X,%ld,%u,%.2f) failed\r\n",frameID,timestamp,sensorID,value);
        }
    }   
    else if(dataType == TYP_LOG)
    {
        logDataWait.frameID = frameID;        
    }                                      
    
    printDebug("<ETH_sendDataReportCmd> sending..\r\n");
    //print_payload(frameData,i); 
                        
    res = ETH_send(frameData,i,frameID,CMD_TIMEOUT); 
    if(res < 0){
        return -1;
    }             
    else{
        return 0;
    }
}
/*============================================================*/
int ETH_sendLogManipulationAck(char frameID,char accd,char status)
{
    int     i=0,k=0;
    char    frameData[3 + sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]      = LOGMANIA;
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
     
    frameData[i++]      = accd;      
    frameData[i++]      = status;
    
    printDebug("<ETH_sendLogManipulationAck> sending..\r\n");   
    //print_payload(frameData,i);
    ETH_send(frameData,i,frameID,0);           
    
    return 0;
}
/*============================================================*/   
int ETH_sendNetworkMaintenanceAck(char frameID,char cmd,char status)
{
    int     i=0,k=0;
    char    frameData[3 + sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]      = NETMAINA;    
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    frameData[i++]      = cmd;
    frameData[i++]      = status;
    
    printDebug("<ETH_sendNetworkMaintenanceAck> sending..\r\n");
    ETH_send(frameData,i,frameID,0);     
    
    return 0;
}
/*============================================================*/
int ETH_remoteDeviceControlAck(char frameID,char cmd,char status)
{
    int     i=0,k=0;
    char    frameData[3 + sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]      = RMDCTRLA;    
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    frameData[i++]      = cmd;
    frameData[i++]      = status;
    
    printDebug("<ETH_remoteDeviceControlAck> sending..\r\n");   
    ETH_send(frameData,i,frameID,0);          
    
    return 0;
}
/*============================================================*/
int ETH_clearDuplicateRequestCmd(void)
{
    int     i=0,k=0;
    char    frameData[2 + sizeof(DEVICE_SERIAL)];
                       
    frameData[i++]  = CLRLISTC;          
      
    for(k=0;k<sizeof(DEVICE_SERIAL);k++){
        frameData[i++]  = DEVICE_SERIAL[k];
    }                                     
    
    frameData[i++]  = CLR_DUPLIST;
                                    
    printDebug("<ETH_clearDuplicateRequestCmd> sending..\r\n");   
    ETH_send(frameData,i,0xFF,0);     
       
    return 0;
}
/*============================================================*/    
void ETH_checkPushLog(unsigned char id)
{
    FIL                 file;
    int                 res;            
    unsigned long int   timestamp;
    unsigned int        channel;
    float               value;
    unsigned char       eventID; 
    unsigned char       eventStatus;
                                                                    
    // -- check if id is in dataLogQueue -- //
    res = readDataLogQueue(id,&timestamp,&channel,&value);    
    if(res != -1)
    {
        // -- found in dataLogQueue -- //  
        if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) || (channel > (MAX_SENS_CHANNEL-1)) )
        {
            printDebug("\r\n<ETH_checkPushLog> === ERROR INFOMATION IN DATA LOG QUEUE ===\r\n");
        }
        else
        {                                                                                          
            printDebug("<ETH_checkPushLog> DataLog time[%ld],ch[%d],val[%.2f]\r\n",timestamp,channel,value);               
            res = LOG_openFile(&file,LOGFILE_DATA);  
            if(res < 0)
            {
                printDebug("<ETH_checkPushLog> LOG_openFile(%s) failed, couldn't save log\r\n",LOGFILE_DATA);
            }
            else
            {       
                res = LOG_insertData(&file,timestamp,channel,value);
                if(res < 0)
                {            
                    printDebug("<ETH_checkPushLog> LOG_insertData() failed\r\n");     
                }
                
                res = LOG_closeFile(&file);  
                if(res < 0)
                {
                    printDebug("<ETH_checkPushLog> LOG_closeFile() failed\r\n");
                }                   
                
                printDebug("<ETH_checkPushLog> Inserted Data Log\r\n");
            }
        }                      
                                            
        //isDataLogEmpty = 0;
        removeDataLogQueue(id);          
    } 
                                                                                          
    // -- check if id is in eventLogQueue -- //
    res = readEventLogQueue(id,&timestamp,&eventID,&eventStatus);    
    if(res != -1)
    {
        // -- found in eventLogQueue -- //
        if( (timestamp == 0) || (timestamp == 0xFFFFFFFF) )
        {
            printDebug("\r\n<ETH_checkPushLog> === ERROR INFOMATION IN EVENT LOG QUEUE ===\r\n");
        }  
        else{                                                                                          
                  
            res = LOG_openFile(&file,LOGFILE_EVENT);  
            if(res < 0)
            {
                printDebug("<ETH_checkPushLog> LOG_openFile(%s) failed, couldn't save log\r\n",LOGFILE_EVENT);
            }
            else
            {                                                                                        
                printDebug("<ETH_checkPushLog> EventLog time[%ld],id[0x%02X],stat[0x%02X]\r\n",timestamp,eventID,eventStatus);     
                res = LOG_insertEvent(&file,timestamp,eventID,eventStatus);
                if(res < 0)
                {            
                    printDebug("<ETH_checkPushLog> LOG_insertData() failed\r\n");     
                }
                
                res = LOG_closeFile(&file);  
                if(res < 0)
                {
                    printDebug("<ETH_checkPushLog> LOG_closeFile() failed\r\n");
                }                                    
                            
                printDebug("<ETH_checkPushLog> Inserted Event Log\r\n");
            }                                 
                               
        }                                
        
        //isEventLogEmpty    = 0;   
        removeEventLogQueue(id);          
    }                         
    
    return;
}
/*============================================================*/
void ETH_retransmit(void)
{
    int i;                                                   
    
    for(i=0;i<ETH_MAX_WAIT_QUEUE;i++)
    {
        if( TIMER_checkTimerExceed(eth_waitQueue[i].retryTime) && 
            (eth_waitQueue[i].packetID != 0))
        {  
            if(eth_waitQueue[i].retryCount >= ETH_MAX_RETRY)
            {    
                printDebug("<ETH_retransmit> packet [0x%02X] Expired\r\n",eth_waitQueue[i].packetID);       
                  
                if(eth_waitQueue[i].packetID == logDataWait.frameID){                
                    printDebug("<ETH_retransmit> update logData timeout\r\n");
                    logDataWait.frameID = 0x00;
                }                            
                else if(eth_waitQueue[i].packetID == logEventWait.frameID){          
                    printDebug("<ETH_retransmit> update logEvent timeout\r\n");
                    logEventWait.frameID = 0x00;
                }                                     
                else{                             
                    // ---- Push in log here ---- //              
                    ETH_checkPushLog(eth_waitQueue[i].packetID);
                    // -------------------------- //
                }              
                
                ETH_removeWaitQueue(eth_waitQueue[i].packetID);        
                                                                            
                printDebug("<ETH_retransmit> commLostTimer = %ld(%lX)\r\n",commLostTimer,commLostTimer);
                if((commLostTimer == 0xFFFFFFFF) || (commLostTimer == 0)){               
                    TIMER_setTimer(&commLostTimer,30);                                   
                    printDebug("<ETH_retransmit> LostTimer time left = %ld(%lX)\r\n",(commLostTimer-baseCounter),(commLostTimer-baseCounter));
                }           
            }
            else
            {                                                               
                TIMER_setTimer(&(eth_waitQueue[i].retryTime), eth_waitQueue[i].timeout);
                eth_waitQueue[i].retryCount++;      // increase retry counter      

                printDebug("\r\n<ETH_retransmit> --Retransmitting-- [0x%02X]\r\n",eth_waitQueue[i].packetID);
                                
                write_uart0(    eth_waitQueue[i].packetData,
                                eth_waitQueue[i].packetLen);  
            }  
        }
    }        
    return;      
}        
/*============================================================*/
void ETH_processPacket(unsigned char *ptr2frame,int len)                       
{
    FIL             file;
    unsigned char   frameType;   
    int             res=0;                                   
    unsigned char   frameID;    
    int             payload;            // -- pointer to payload data extracted from a frame -- //
    
    int             cnfgRd;             // -- for keeping read configuration -- //             
    char            *cnfgWr;
    
    int             i; 
                                                         
//    printDebug("\r\n\r\n****** processPacket ******\r\n");
//    print_payload(ptr2frame,len);            
//    printDebug("\r\n******************\r\n");        
    frameID         = ptr2frame[0];
    frameType       = ptr2frame[1];                 
    
    commLostTimer   = 0xFFFFFFFF;                     // -- cancel communication lost timer -- //
    //commLostTimer   = 0;                            // -- re-initial -- //   
    
    switch(frameType){    
        case STATREQC  :     // -- Status Request COMMAND              
            if( len < (2 + sizeof(struct stat_req_st) - (sizeof(unsigned int) + sizeof(unsigned char *)) ) ){
                printDebug("<ETH_processPacket> status request invalid length (%d)\r\n",len);                         
                ETH_sendNack(frameID,INVALID_LENGTH);    
                return;             
            }
            
            payload = (int) malloc (len);   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc() failed\r\n");          
                ETH_sendNack(frameID,NOT_READY);
                return;
            }    
                                                                                     
            res = statusRequestParser(ptr2frame+2,len-2,(struct stat_req_st *)payload);      
            if(res < 0){
                printDebug("<ETH_processPacket> statusRequestParser() failed\r\n");              
                free( (void *) payload );                                                                  
                ETH_sendNack(frameID,INVALID_PARAMETER);    
                return;
            }           
            
            printDebug("\r\n--statusRequestParser-- CMD [0x%02X]\r\n",frameID);                                
            printDebug("accessMethod    [0x%02X]\r\n",      ((struct stat_req_st *)payload)->accm);                     
            printDebug("interval        [0x%04X] = %d\r\n", ((struct stat_req_st *)payload)->intv,((struct stat_req_st *)payload)->intv);
            printDebug("statusList      |");    
            
            for( i=0 ; i<(((struct stat_req_st *)payload)->parLen) ; i++)
            {
                printDebug("%02X|",(((struct stat_req_st *)payload)->parlst)[i++]);
            }                          
            printDebug("\r\n-- ..Done.. --\r\n");                    
            
            ETH_sendStatusRequestAck(frameID,((struct stat_req_st *)payload)->accm,0x00);                    
                    
            if( ((struct stat_req_st *)payload)->accm < 0x30){
                free_statusRequest((struct stat_req_st *)payload);     
            } 
                    
            free( (void *) payload );                
            break;  
        case STATREPA  :     // -- Status Report ACKNOWLEDGE         
            if( len < (2 + sizeof(struct stat_rep_st)) ){
                printDebug("<ETH_processPacket> status report invalid length (%d)\r\n",len);     
                return;
            }                                    
            
            printDebug("\r\n--statusReportParser-- ACK [0x%02X]\r\n",frameID);                     
            printDebug("unixTime        [0x%02X][0x%02X][0x%02X][0x%02X]\r\n",
                                    ptr2frame[10],ptr2frame[11],ptr2frame[12],ptr2frame[13]);                                  
            res = setTimeSyncSchema( &ptr2frame[10] );
            if(res < 0){
                //printDebug("<ETH_processPacket> setTimeSyncSchema() failed in STATREPA\r\n");  
            }                        
            ETH_removeWaitQueue(frameID);    
            break;   
        case EVENREPA  :     // -- Event Report ACKNOWLEDGE  
            if( len < (2 + sizeof(struct event_rep_st)) ){
                printDebug("<ETH_processPacket> event report invalid length (%d)\r\n",len);      
                return;
            }                                                 
            
            printDebug("\r\n--eventReportParser-- ACK [0x%02X]\r\n", frameID);    
            if( (logEventWait.frameID != 0) && (frameID == logEventWait.frameID) )
            {
                printDebug("<ETH_processPacket> got event log ack\r\n");    
                
                res = LOG_openFile(&file,LOGFILE_EVENT);  
                if(res < 0){        printDebug("<logReport> LOG_openFile(%s) failed\r\n",LOGFILE_EVENT);            }                                                                                              
                else
                {                                                                  
                    res = LOG_setLastSend(&file,logEventWait.logID + 1);   
                    if(res < 0){    printDebug("<LOG_readData> LOG_setLastSend() failed\r\n");                      }   
                    else{           printDebug("<LOG_readData> LOG_setLastSend(%s,%ld)\r\n",LOGFILE_EVENT,logEventWait.logID + 1);   }
                    
                    res = LOG_closeFile(&file);  
                    if(res < 0){    printDebug("<logReport> LOG_closeFile(%s) failed\r\n",LOGFILE_EVENT);           }         
                }
                
                logEventWait.frameID    = 0; 
                logEventWait.logID      = 0; 
            }
            removeEventLogQueue(frameID);                  
            ETH_removeWaitQueue(frameID);                         
            break;  
        case CONFMANC  :     // -- Configuration Management COMMAND               
            printDebug("<ETH_processPacket> Config Manage\r\n");
            print_payload(ptr2frame, len);
            
            if( len < (2 + sizeof(struct conf_man_st) - (sizeof(unsigned int) + sizeof(unsigned char *)) ) ){
                printDebug("<ETH_processPacket> configuration management invalid length (%d)\r\n",len);                       
                ETH_sendNack(frameID,INVALID_LENGTH);    
                return;
            }                                                                                 
                                                                 
            payload = (int) malloc (sizeof(struct conf_man_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct conf_man_st));          
                ETH_sendNack(frameID,NOT_READY);
                return;
            }                                                                      
            
            res = configManageParser(ptr2frame+2,len-2,(struct conf_man_st *)payload);          
            if(res < 0){
                printDebug("<ETH_processPacket> configManageParser() failed\r\n");             
                free( (void *) payload );                                                                         
                ETH_sendNack(frameID,INVALID_PARAMETER);
                return;
            }                                      
            
            printDebug("\r\n--configManageParser-- CMD [0x%02X]\r\n",frameID);               
            printDebug("accd            [0x%02X]\r\n",       ((struct conf_man_st *)payload)->accd);   
            printDebug("cnfg            [0x%02X]\r\n",       ((struct conf_man_st *)payload)->cnfg); 
                                                               
            switch( ((struct conf_man_st *)payload)->accd ){
                case CONF_RD_CMD    :   // -- Configration Read -- //      
                                        res = getConfigLen(((struct conf_man_st *)payload)->cnfg); 
                                        if(res < 0){                
                                            printDebug("<ETH_processPacket> getConfigLen() failed\r\n");
                                            ETH_sendNack(frameID,INVALID_PARAMETER); 
                                            break;
                                        }                        
                                        
                                        cnfgRd = (int) malloc (res);
                                        if(cnfgRd == NULL){
                                            printDebug("<ETH_processPacket> cnfgRd malloc() failed\r\n");          
                                            ETH_sendNack(frameID,NOT_READY);    
                                            break;
                                        }
                                        
                                        res = configRead(((struct conf_man_st *)payload)->cnfg,(char *) cnfgRd);                      
                                        if(res <= 0){
                                            printDebug("<ETH_processPacket> configRead() failed\r\n");
                                            if(res == -1){                     
                                                ETH_sendNack(frameID,INVALID_PARAMETER);
                                            }   
                                            else if((res == -2) || (res == 0)){                 
                                                ETH_sendNack(frameID,NOT_READY);
                                            }                            
                                            else if(res == -3){                       
                                                ETH_sendConfigManageNack(   frameID,
                                                                            ((struct conf_man_st *)payload)->accd,
                                                                            ((struct conf_man_st *)payload)->cnfg,
                                                                            ERROR);
                                            }                                       
                                            else{
                                                ETH_sendNack(frameID,UNKNOWN_ERROR);     
                                            }     
                                            
                                            free((void *) cnfgRd);
                                            break;
                                        }         
                                        
                                        res = ETH_sendConfigManageAck(  frameID,
                                                                        ((struct conf_man_st *)payload)->accd,
                                                                        ((struct conf_man_st *)payload)->cnfg,
                                                                        SUCCESS,
                                                                        (char *)cnfgRd,
                                                                        res  );        
                                        if(res < 0){
                                            printDebug("<ETH_processPacket> RD_CMD : ETH_sendConfigManageAck() failed\r\n");
                                        }        
                                                    
                                        free( (void *) cnfgRd );
                                        break;
                case CONF_WR_CMD    :   // -- Configuration Write -- //                       
                                        cnfgWr = malloc(((struct conf_man_st *)payload)->confLen);   
                                        if(cnfgWr == NULL){
                                            printDebug("<ETH_processPacket> cnfgWr malloc() failed\r\n");          
                                            ETH_sendNack(frameID,NOT_READY);
                                            break;
                                        }                                   
                                                                                                                                                                 
                                        memcpy(cnfgWr,((struct conf_man_st *)payload)->confSpec,((struct conf_man_st *)payload)->confLen);                                        
                                        
                                        res = configWrite(  (unsigned char) (((struct conf_man_st *)payload)->cnfg),
                                                            cnfgWr,
                                                            (int) (((struct conf_man_st *)payload)->confLen) );    
                                                            
                                        free(cnfgWr);          
                                        if(res < 0){
                                            printDebug("<ETH_processPacket> configWrite() failed\r\n");     
                                            ETH_sendConfigManageNack(   frameID,
                                                                        ((struct conf_man_st *)payload)->accd,
                                                                        ((struct conf_man_st *)payload)->cnfg,
                                                                        ERROR);        
                                            break;
                                        }                            
                                        
                                             
                                        res = ETH_sendConfigManageAck(  frameID,
                                                                        ((struct conf_man_st *)payload)->accd,
                                                                        ((struct conf_man_st *)payload)->cnfg,
                                                                        SUCCESS,
                                                                        NULL,
                                                                        0  );                                 
                                        if(res < 0){
                                            printDebug("WARN : <ETH_processPacket> WR_CMD : ETH_sendConfigManageAck() failed\r\n");
                                        }           
                                        
                                        break;
                case CONF_CF_CMD    :   // -- Configuration Confirm -- //
                                        res = configConfirm();           
                                        if(res < 0){
                                            printDebug("<ETH_processPacket> configConfirm() failed\r\n");    
                                            ETH_sendConfigManageNack(   frameID,
                                                                        ((struct conf_man_st *)payload)->accd,
                                                                        ((struct conf_man_st *)payload)->cnfg,
                                                                        ERROR);        
                                            break;
                                        }           
                                        
                                        res = ETH_sendConfigManageAck(  frameID,
                                                                        ((struct conf_man_st *)payload)->accd,
                                                                        ((struct conf_man_st *)payload)->cnfg,
                                                                        SUCCESS,
                                                                        NULL,
                                                                        0  );                                 
                                        if(res < 0){
                                            printDebug("WARN : <ETH_processPacket> WR_CMD : ETH_sendConfigManageAck() failed\r\n");
                                        }                   
                                        
                                        break;  
                default             :   // -- Shouldn't reach here -- //
                                        printDebug("<ETH_processPacket> unknown type for config->accd [0x%02X]\r\n", 
                                                        ((struct conf_man_st *)payload)->accd );               
                                        ETH_sendNack(frameID,INVALID_PARAMETER);
                                        break;
            }                                        
                       
            free_configManage((struct conf_man_st *)payload);    
            free( (void *) payload );                
            
            break; 
        case DATAREPA  :     // -- Data Report ACKNOWLEDGE               
            if( len < (2 + sizeof(struct data_rep_st)) ){
                printDebug("<ETH_processPacket> Data report invalid length (%d)\r\n",len);  
                return;
            }               
            
            printDebug("\r\n--dataReportParser-- ACK [0x%02X]\r\n",frameID);                           
            if( (logDataWait.frameID != 0) && (frameID == logDataWait.frameID) )
            {
                printDebug("<ETH_processPacket> got data log ack\r\n");    
                
                res = LOG_openFile(&file,LOGFILE_DATA);  
                if(res < 0){        printDebug("<logReport> LOG_openFile(%s) failed\r\n",LOGFILE_DATA);             }                                                                                              
                else
                {                                                                  
                    res = LOG_setLastSend(&file,logDataWait.logID + 1);   
                    if(res < 0){    printDebug("<LOG_readData> LOG_setLastSend() failed\r\n");                      }      
                    else{           printDebug("<LOG_readData> LOG_setLastSend(%s,%ld)\r\n",LOGFILE_DATA,logDataWait.logID + 1);    }
                    
                    res = LOG_closeFile(&file);  
                    if(res < 0){    printDebug("<logReport> LOG_closeFile(%s) failed\r\n",LOGFILE_DATA);            }         
                }
                
                logDataWait.frameID    = 0; 
                logDataWait.logID      = 0; 
            }
            removeDataLogQueue(frameID);                  
            ETH_removeWaitQueue(frameID);   
            break;                         
        case LOGMANIC  :     // -- Log Manipulation COMMAND
            if( len < (2 + sizeof(struct log_manic_st)) ){
                printDebug("<ETH_processPacket> log manipulation (CMD) invalid length (%d)\r\n",len);              
                ETH_sendNack(frameID,INVALID_LENGTH);    
                return;
            }                                                                 
                                                    
            payload = (int) malloc (sizeof(struct log_manic_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct log_manic_st));          
                ETH_sendNack(frameID,NOT_READY);
                return;
            }                                                                   
            
            res = logManipulationCmdParser(ptr2frame+2,len-2,(struct log_manic_st *)payload);           
            if(res < 0){
                printDebug("<ETH_processPacket> logManipulationCmdParser() failed\r\n");             
                free( (void *) payload );                                                          
                ETH_sendNack(frameID,INVALID_PARAMETER);
                return;
            }                                         
            
            printDebug("\r\n--logManipulationCmdParser-- CMD [0x%02X]\r\n",frameID);       
            printDebug("accd            [0x%02X]\r\n",  ((struct log_manic_st *)payload)->accd);   
            printDebug("tStart          [%ld]\r\n",     ((struct log_manic_st *)payload)->tStart);
            printDebug("tStop           [%ld]\r\n",     ((struct log_manic_st *)payload)->tStop);     
                                                            
            res = setLogManipulateSchema(   ((struct log_manic_st *)payload)->accd,
                                            ((struct log_manic_st *)payload)->tStart,
                                            ((struct log_manic_st *)payload)->tStop);
            if(res < 0){                                                              
                printDebug("<ETH_processPacket> setLogManipulateSchema() failed\r\n");    
                ETH_sendLogManipulationAck(frameID,((struct log_manic_st *)payload)->accd,ERROR);
            }
            
            ETH_sendLogManipulationAck(frameID,((struct log_manic_st *)payload)->accd,SUCCESS);
            
            free( (void *) payload );                
            
            break;                              
        case LOGMANIA  :     // -- Log Manipulation ACKNOWLEDGE               
            if( len < (2 + sizeof(struct log_mania_st)) ){
                printDebug("<ETH_processPacket> log manipulation (ACK) invalid length (%d)\r\n",len); 
                return;
            }               
                                                    
            payload = (int) malloc (sizeof(struct log_mania_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct log_mania_st));   
                return;
            }                                                                   
            
            res = logManipulationAckParser(ptr2frame+2,(struct log_mania_st *)payload);           
            if(res < 0){
                printDebug("<ETH_processPacket> logManipulationCmdParser() failed\r\n");             
                free( (void *) payload );                                                    
                return;
            }                                         
            
            printDebug("\r\n--logManipulationAckParser-- ACK [0x%02X]\r\n",frameID);
            printDebug("accd            [0x%02X]\r\n",  ((struct log_mania_st *)payload)->accd);   
            printDebug("status          [%ld]\r\n",     ((struct log_mania_st *)payload)->status);
                                   
            free( (void *) payload );                               
            ETH_removeWaitQueue(frameID);    
            break;                         
        case NETMAINC  :     // -- Network Maintenance COMMAND
            if( len < (2 + sizeof(struct netw_man_st) - (sizeof(unsigned int) + sizeof(unsigned char *)) + 1 ) ){
                printDebug("<ETH_processPacket> network maintenance invalid length (%d)\r\n",len);          
                ETH_sendNack(frameID,INVALID_LENGTH);    
                return;
            }                                                                                                                      
                                                    
            payload = (int) malloc (sizeof(struct netw_man_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct netw_man_st));          
                ETH_sendNack(frameID,NOT_READY);
                return;
            }                                                            
            
            res = networkMaintenanceParser(ptr2frame+2,len-2,(struct netw_man_st *)payload);      
            if(res < 0){
                printDebug("<ETH_processPacket> networkMaintenanceParser() failed\r\n");             
                free( (void *) payload );                                                      
                ETH_sendNack(frameID,INVALID_PARAMETER);
                return;
            }                        
            
            printDebug("\r\n--networkMaintenanceParser-- CMD [0x%02X]\r\n",frameID);
            printDebug("cmd             [0x%02X]\r\n",  ((struct netw_man_st *)payload)->cmd);        
            printDebug("dataLen         [%d]\r\n",      ((struct netw_man_st *)payload)->dataLen);    
            if( ((struct netw_man_st *)payload)->dataLen > 0 ){
                printDebug("data ---        \r\n");    
                print_payload(((struct netw_man_st *)payload)->data,((struct netw_man_st *)payload)->dataLen);  
                printDebug("--------\r\n");                                 
            }
            
            switch(((struct netw_man_st *)payload)->cmd){
                case NETW_TIME_SYNC :   res = setTimeSyncSchema( ((struct netw_man_st *)payload)->data );
                                        if(res < 0){
                                            printDebug("<ETH_processPacket> setTimeSyncSchema() failed\r\n");         
                                            ETH_sendNack(frameID,INVALID_PARAMETER);       
                                            free_networkMaintenance((struct netw_man_st *)payload);  
                                            free( (void *) payload );             
                                            return;   
                                        }                  
                                        
                                        res = ETH_sendNetworkMaintenanceAck(frameID,((struct netw_man_st *)payload)->cmd,SUCCESS);
                                                                        
                                        if(res < 0){
                                            printDebug("WARN : <ETH_processPacket> NETW_TIME_SYNC : ETH_sendNetworkMaintenanceAck() failed\r\n");
                                        }                   
                                        
                                        break;
                default :               // -- Shouldn't reach here -- //
                                        printDebug("<ETH_processPacket> unknown type for config->accd [0x%02X]\r\n", 
                                                        ((struct netw_man_st *)payload)->cmd );               
                                        ETH_sendNack(frameID,INVALID_PARAMETER); 
                                        break;
            }
            
            if( ((struct netw_man_st *)payload)->dataLen > 0 ){
                free_networkMaintenance((struct netw_man_st *)payload);
            }
            
            free( (void *) payload );                
            
            break;                         
        case RMDCTRLC  :     // -- Remote Device Control COMMAND     
            if( len < (2 + sizeof(struct rem_dev_ctrl_st)) ){
                printDebug("<ETH_processPacket> remote device control invalid length (%d)\r\n",len);          
                ETH_sendNack(frameID,INVALID_LENGTH);    
                return;
            }                                                                                                                      
                                                           
            payload = (int) malloc (sizeof(struct rem_dev_ctrl_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct rem_dev_ctrl_st));          
                ETH_sendNack(frameID,NOT_READY);
                return;
            }                                                              
            
            res = remoteDeviceControlParser(ptr2frame+2,(struct rem_dev_ctrl_st *)payload);            
            if(res < 0){
                printDebug("<ETH_processPacket> remoteDeviceControlParser() failed\r\n");             
                free( (void *) payload );                                                      
                ETH_sendNack(frameID,INVALID_PARAMETER);
                return;
            }                                       
            
            printDebug("\r\n--remoteDeviceControlParser-- CMD [0x%02X]\r\n",frameID);
            printDebug("cmd             [0x%02X]\r\n",  ((struct rem_dev_ctrl_st *)payload)->cmd);        
            printDebug("value           [%ld]\r\n",      ((struct rem_dev_ctrl_st *)payload)->value);  
            
            switch(((struct rem_dev_ctrl_st *)payload)->cmd){
                case DEVCTRL_MUTE_ALARM :   res = setMuteAlarm( ((struct rem_dev_ctrl_st *)payload)->value );
                                            if(res < 0){
                                                printDebug("<ETH_processPacket> setTimeSyncSchema(%ld) failed\r\n",((struct rem_dev_ctrl_st *)payload)->value);         
                                                ETH_sendNack(frameID,INVALID_PARAMETER);       
                                                free( (void *) payload ); 
                                                return;   
                                            }                  
                                            
                                            res = ETH_remoteDeviceControlAck(frameID,((struct rem_dev_ctrl_st *)payload)->cmd,SUCCESS);
                                                                            
                                            if(res < 0){
                                                printDebug("WARN : <ETH_processPacket> DEVCTRL_MUTE_ALARM : ETH_remoteDeviceControlAck() failed\r\n");
                                            }        
                                            break;
                default :                   // -- Shouldn't reach here -- //
                                            printDebug("<ETH_processPacket> unknown type for config->accd [0x%02X]\r\n", 
                                                            ((struct netw_man_st *)payload)->cmd );               
                                            ETH_sendNack(frameID,INVALID_PARAMETER); 
                                            break;
            }                                      
            
            free( (void *) payload ); 
            break;                         
        case CLRLISTA  :     // -- Clear List ACKNOWLEDGE     
            if( len < (2 + sizeof(struct clr_lst_st)) ){
                printDebug("<ETH_processPacket> clear list ACK invalid length (%d)\r\n",len);          
                return;
            }                                                              
             
            payload = (int) malloc (sizeof(struct clr_lst_st));   
            if(payload == NULL){
                printDebug("<ETH_processPacket> payload malloc(%d) failed\r\n",sizeof(struct clr_lst_st));    
                return;
            }    
            
            res = clearListParser(ptr2frame+2,(struct clr_lst_st *)payload);            
            if(res < 0){
                printDebug("<ETH_processPacket> clearListParser() failed\r\n");                     
                free( (void *) payload );                                                      
                return;
            }                                       
            
            printDebug("\r\n--clearListParser-- ACK [0x%02X]\r\n",frameID);
            printDebug("cmd             [0x%02X]\r\n",  ((struct clr_lst_st *)payload)->cmd);        
            printDebug("status          [0x%02X]\r\n",  ((struct clr_lst_st *)payload)->status);  
            
            switch(((struct clr_lst_st *)payload)->cmd){
                case CLR_DUPLIST :  if(((struct clr_lst_st *)payload)->status == 0x00)
                                    {  
                                        printDebug("<ETH_processPacket> clear duplicate request (ACK) SUCCESS!!\r\n");
                                        duplicateListClear = 1; 
                                        free( (void *) payload ); 
                                        return;   
                                    }
                                    else
                                    {
                                        printDebug("<ETH_processPacket> clear duplicate request (ACK) FAILED[0x%02X]!!\r\n",
                                                    ((struct clr_lst_st *)payload)->status);
                                    }                         
                                    break;          
                default         :   // -- Shouldn't reach here -- //
                                    printDebug("<ETH_processPacket> unknown type for clr_lst_st->cmd [0x%02X]\r\n", 
                                                    ((struct clr_lst_st *)payload)->cmd );               
                                    break;
            }                                      
            
            free( (void *) payload ); 
            break;                                            
        case FMWUPLDC  :     // -- Firmware Upload COMMAND (Reserved)        
            printDebug("<ETH_processPacket> firmware upload hasn't supported yet\r\n");
            break;                        
        default:                      
            printDebug("\r\n<ETH_processPacket> invalid frame type ( %02X )!!\r\n",frameType);
            break;
    }                                         
    
    return;
}                      
/*============================================================*/

/**************************************************************/
/************* Low level Communication Management *************/
/**************************************************************/
/*============================================================*/
void ETH_receive()
{
    unsigned char   *ptr;           
    int             size=0;           
            
    // -- for packet validation -- //
    int     offset=0;
    int     len = 0x0000;
    int     total=0;
    
    // --------------- Process Xbee Data ---------------- // UART0
    do{
        ptr = (char *) malloc (RX_BUFFER_SIZE0);     
        if(ptr == NULL){                
            printDebug("<ETH_receive> malloc..FAILED..\r\n");
        }
    }while(ptr == NULL);      
    size = read_uart0(ptr,RX_BUFFER_SIZE0-1,10);
    
    if(size <= 0){
        free( ptr );
        return;
    }                     
        
//    printDebug("\r\n\r\n+++++++ Raw buffer +++++++\r\n");
//    print_payload(ptr,size);                             
    
    do{                            
        //printDebug("<ETH_receive> ptr");
        offset += strcspn(&ptr[offset],"\x7e");                                   // seach for header
        if(offset == size){                 
//            printDebug("<ETH_receive> invalid packet 0x7E not found\r\n");    // not found Start Delimiter 0x7E
            break;
        }
        len = (ptr[offset+1] & 0xffff) << 8;                                    // check packet length (MSB)
        len |= ptr[offset+2];                                                   // check packet length (LSB)
//        printDebug("len = [0x%04X](%d)\r\n",len,len);                     
        
        total += len;                                                           // update total analyse 
        if (total > size){                                                      //check length validation
            offset++;
            total = offset;                                                     // roll back total analyse                          
            printDebug("\r\ninvalid length!!\r\n");
            continue;
        }                             
                                             
//        printDebug("\r\n****** packet checksum ******\r\n");
//        print_payload(&ptr[offset+3],len);   
        if(ETH_checksum(&ptr[offset+3],len) != ptr[offset+3+len]){              // checksum error detection                  
            printDebug("<ETH_receive> checksum error\r\n");
            offset++;
            total = offset;                                                     // roll back total analyse          
        }
        else{                                                                   // got a valid packet 
//            printDebug("\r\n****** checksum correct ******\r\n");                    
//            print_payload(&ptr[offset+3],len);            
//            printDebug("\r\n******************\r\n");     
                                  
            ETH_processPacket(&ptr[offset+3],len);                              // analyse API-specific Structure
            offset += 3+len;           
        }            
	}while( total < size);						                                // loop if still unread byte
	                    
    
    
    free( ptr );   
}         
/*============================================================*/
int ETH_send(char *frameData, int frameLen, char frameID, int timeout)
{
    int     res;
    char    *sendPacket;    
    int     packetLen   = 0;      
        
    frameLen++;     // -- + frameID (1 Byte) -- //
                              
    packetLen   = frameLen + 2 + 1 + 1;       // length(2 bytes) + frameID(1 byte) + header(1 byte) + sum(1 byte)        
    
    if(packetLen > MAX_MTU){      
        printDebug("<ETH_send> WARNING : Packet is too long(%d bytes)\r\n",packetLen); 
        return -1;
    }                                                             
    
    sendPacket  = (char *) malloc ( packetLen );        
    if(sendPacket == NULL){                     
        printDebug("<ETH_send> WARNING : ETH_send malloc failed (%d)\r\n",packetLen); 
        return -1;
    }                 
    
    memset(sendPacket,'\0',packetLen);      
       
    sendPacket[0] = 0x7E;
    sendPacket[1] = (frameLen >> 8) & 0xFF;
    sendPacket[2] = frameLen & 0xFF;         
    
    if(frameID == 0x00){     
        frameID         = ETH_generateId();                                  
        //printDebug("<ETH_send> ETH_generateId() return 0x%02X\r\n",frameID);
    }                         
                                             
    sendPacket[3] = frameID;      
        
    memcpy(&sendPacket[4],frameData,frameLen);    
    sendPacket[packetLen-1] = ETH_checksum(&sendPacket[3],frameLen);          
    
    //printDebug("<ETH_send> checksum = 0x%02X\r\n",sendPacket[packetLen-1]);            
    
    if((timeout > 0) && (frameID != 0xFF)){
        res = ETH_addWaitQueue(frameID, &sendPacket[0], packetLen, timeout);    
        if(res < 0){    
            printDebug("<ETH_send> ETH_addWaitQueue() failed [id:0x%02X]\r\n",frameID);           
            free(sendPacket); 
            return -1;
        }               
    }                  
    
                               
//    if((commLostTimer == 0xFFFFFFFF) || (commLostTimer == 0)){
//        TIMER_setTimer(&commLostTimer,30);      
//    }  
                     
//    printDebug("\r\n--Transmitting [0x%02X]\r\n",frameID);
//    print_payload(sendPacket,packetLen); 
    write_uart0(sendPacket,packetLen);  
    free(sendPacket);
       
    //printDebug("-- Completed --\r\n");
    delay_ms(7);                       
    
    return 0;
}                                    
/*============================================================*/
int ETH_checksum(char buf[],int len)
{
    int i;
    char sum = 0;                                          
        
    //printDebug("<checksum> \r\n");
    for (i=0;i<len;i++){
        sum += buf[i];          
        //printDebug("<0x%02X> -> [0x%02X]\r\n",buf[i],sum);                            
    }                 
    return (0xff - (sum & 0xff));

}
/*============================================================*/

/**************************************************************/
/***************** Ethernet Hardare Management ****************/
/**************************************************************/
/*============================================================*/      
char ETH_current_mode;         
char ETH_switchMode(char mode)
{                                                            
    if(mode == MODE_CONF){
        printDebug("<ETH_switchMode> switch to Config mode\r\n");     
        ETH_CONF;        
    }
    else{
        printDebug("<ETH_switchMode> switch to Normal mode\r\n"); 
        ETH_NORM;
    }            
                                                      
    delay_ms(5);          
                                                           
    ETH_current_mode = mode;      
                                                        
    delay_ms(5);    
    ETH_OFF;     
    delay_ms(1500);                
    ETH_ON;              
    #asm("wdr")                    
    delay_ms(2000);
                 
    return ETH_MODE;
}                                
/*============================================================*/   
int ETH_getConfig(unsigned char *ptr, unsigned int *buflen)
{    
    TIMER   timeout;            
    char    getInfoCmd[] = ">R\r";
    int     i=0;
    
    ETH_switchMode((unsigned char) MODE_CONF);    
    /*
    if(mode != MODE_CONF){    
        printDebug("<ETH_getConfig> ETH_switchMode(MODE_CONF) failed\r\n");
        return -1;                                       
    } 
    */                                   
    
    for(i=0;i<CONF_READ_BUFFER;i++){
        configRd[i] = 0x00;
    }
    configLen       = 0;  
    config_start    = 0;    
    config_ready    = 0;    
    config_end      = 0;    
    config_response = 0;                        
                       
    printDebug("<ETH_getConfig> Write to uart 0 :");
    for(i=0;i<strlen(getInfoCmd);i++){     
        putchar3(getInfoCmd[i]);    
    }                                    
    printDebug("\r\n");
    
    for(i=0;i<strlen(getInfoCmd);i++){   
        putchar0(getInfoCmd[i]);     
    }                        
          
    TIMER_setTimer(&timeout, 7);      
    
    while( (!config_ready) && !TIMER_checkTimerExceed(timeout)){   
       delay_ms(50);
       #asm("wdr")        
    }         
    
    memcpy(ptr,&configRd[0],configLen);  
    
    *buflen    = configLen;      
                            
    ETH_switchMode(MODE_NORM);            
                
    /*
    if(mode != MODE_NORM){    
        printDebug("<ETH_getConfig> ETH_switchMode(MODE_NORM) failed\r\n");
        return -1;                                       
    }          
    */                                  
    
    if(config_response == 'S'){
        return 0;
    }
    else{
        return -1;
    }     
}         
/*============================================================*/
int ETH_setConfig(char *ip, char *mask, char *gw, char *server)
{                              
    TIMER   timeout;            
    char    getInfoCmd[] = ">R\r";   
    char    setInfoCmd[] = ">W";
    int     i=0;                
    char    setStr[30];                   
    int     readLen=0;
    char    *ptr;    
                                                      
    ptr = (unsigned char *) malloc( sizeof(char) * CONF_READ_BUFFER );
    if(ptr == NULL){       
        printDebug("<ETH_setConfig> malloc(CONF_READ_BUFFER) failed\r\n");
        return -1;   
    }                                             
    
    ETH_switchMode((unsigned char) MODE_CONF);          
                                                
    // ------------------------- Read Old Configuration --------------------------- //
    for(i=0;i<CONF_READ_BUFFER;i++){
        configRd[i] = 0x00;
    }
    configLen       = 0;  
    config_start    = 0;    
    config_ready    = 0;    
    config_end      = 0;    
    config_response = 0;                        
              
    printDebug("<ETH_setConfig> Write to uart 0 :");
    for(i=0;i<strlen(getInfoCmd);i++){     
        putchar3(getInfoCmd[i]);    
    }                                    
    printDebug("\r\n");
    
    for(i=0;i<strlen(getInfoCmd);i++){   
        putchar0(getInfoCmd[i]);     
    }                        
     
    TIMER_setTimer(&timeout, 7);      
    
    while( (!config_ready) && !TIMER_checkTimerExceed(timeout)){   
       delay_ms(50);
       #asm("wdr")        
    }         
    
    memcpy(ptr,&configRd[0],configLen);  
    readLen = configLen;
     
    if(config_response != 'S'){   
        printDebug("<ETH_setConfig> read config from ETH module failed\r\n");      
        free( ptr );
        return -1;
    }
                                                               
    // ------------------------- Update Configuration --------------------------- //
    sprintf(setStr,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
                (ip[0]    *100) + (ip[1]     *10) + ip[2],
                (ip[3]    *100) + (ip[4]     *10) + ip[5],
                (ip[6]    *100) + (ip[7]     *10) + ip[8],
                (ip[9]    *100) + (ip[10]    *10) + ip[11],  
                (mask[0]  *100) + (mask[1]   *10) + mask[2],
                (mask[3]  *100) + (mask[4]   *10) + mask[5],
                (mask[6]  *100) + (mask[7]   *10) + mask[8],
                (mask[9]  *100) + (mask[10]  *10) + mask[11],     
                (gw[0]    *100) + (gw[1]     *10) + gw[2],
                (gw[3]    *100) + (gw[4]     *10) + gw[5],
                (gw[6]    *100) + (gw[7]     *10) + gw[8],
                (gw[9]    *100) + (gw[10]    *10) + gw[11]);     
    
    printDebug("<ETH_setConfig> Set String [%s]\r\n",setStr);
                                    
    memcpy((ptr+14),&setStr[0],24);
    
    sprintf(setStr,"%02X%02X%02X%02X",   
                (server[0]*100) + (server[1] *10) + server[2],
                (server[3]*100) + (server[4] *10) + server[5],
                (server[6]*100) + (server[7] *10) + server[8],
                (server[9]*100) + (server[10]*10) + server[11]);       
                
    printDebug("<ETH_setConfig> Set String [%s]\r\n",setStr);       
    
    memcpy((ptr+42),&setStr[0],8);    
                               
//    printDebug("<ETH_setConfig> Modified setting msg ---\r\n");
//    for(i=0;i<configLen;i++){     
//        putchar3(ptr[i]);    
//    }                                    
//    printDebug("\r\n\---\r\n");
                                
    // ------------------------- Set New Configuration --------------------------- //
    for(i=0;i<CONF_READ_BUFFER;i++){
        configRd[i] = 0x00;
    }
    configLen       = 0;  
    config_start    = 0;    
    config_ready    = 0;    
    config_end      = 0;    
    config_response = 0;    
                            
    printDebug("<ETH_setConfig> Setting msg ---\r\n");
    for(i=0;i<strlen(setInfoCmd);i++){   
        putchar0(setInfoCmd[i]);         
        putchar3(setInfoCmd[i]);     
    }                         
    for(i=12;i<readLen;i++){     
        putchar0(ptr[i]);     
        putchar3(ptr[i]);     
    }                            
    putchar0('\r');           
    printDebug("\r\n\---\r\n");
                        
    TIMER_setTimer(&timeout, 10);      
    
    while( (!config_ready) && !TIMER_checkTimerExceed(timeout)){   
       delay_ms(50);
       #asm("wdr")        
    }         
    
    ETH_switchMode(MODE_NORM);    
                                         
    free( ptr );
    
    if(config_response == 'S'){
        return 0;
    }
    else{
        return -1;
    }     
}
/*============================================================*/
void ETH_reset(void)
{
    printDebug("<ETH_reset> Resetting Ethernet Module, please wait.");
    ETH_RESET   = 0;
    delay_ms(3000);
    ETH_RESET   = 1;                                                 
    delay_ms(1);                                                    
    printDebug("...Done\r\n");
}   
/*============================================================*/

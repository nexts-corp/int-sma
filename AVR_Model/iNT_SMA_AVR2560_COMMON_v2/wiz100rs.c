#include <delay.h>
#include "interruptTask.h"
#include "wiz100rs.h"
#include "uart.h"
#include "debug.h"
#include "ethcomm.h"
#include "io.h" 
#include "timer.h"

int flagConnect = 0;

iWiz100srConfig_t iWiz100srConfig;
//char httpPostHeader[] = "POST /smasmartdevice HTTP/1.1\r\nHost: smasmartdevice.appspot.com\r\nUser-Agent: AVR2560-LAN\r\nConnection: close\r\nContent-Length: ";
//char httpPostHeader[] = "POST /smasmartdevice HTTP/1.1\r\nHost: smasmartdevice.appspot.com\r\nX-CLIENT-UUID: 001100110011\r\nUser-Agent: AVR2560-LAN\r\nConnection: close\r\nContent-Length: ";


//iWiz100srData_t iWiz100srData;

void (*pfiWiz100rs)(void);
int (*piWizCallBack)(char *,unsigned int);

extern unsigned int rxBeginCount;
extern piInterrupt_t pviUart0TimeOut;
extern char rx0Buffer[];
extern unsigned int index0Buffer;
//char *pviValue;
//char pviValue[RX_BUFFER_SIZE0];
unsigned char iWizStatus = 0;
unsigned char iWizRXMode = 0;
//iWiz100srConnection iWizStatus;
iWiz100srConnection iWizData;
iHTTP_t iHTTPStatus;

//iWizStatus.status = 0;

void iWizGetConf(){
    int i;
    char    readCmd[] = ">R\r";
    WIZ100RS_CONF_MODE;        //change to configure mode     
    
    
    ETH_CONF;
    
    //module off then on
    WIZ100RS_OFF; 
    delay_ms(10);
    WIZ100RS_ON; 
    delay_ms(1000);
    
    for(i=0;i<strlen(readCmd);i++){   
        putchar0(readCmd[i]);     
    } 
}
void iWizSetConf(){}


void iWizTXdata(){}


void iWizRXTimeOut(){
     rxBeginCount = 0; 
     pviUart0TimeOut = -1;
     wiz100srSetTimer = -1;
     printDebug("[iWizRXTimeOut]\r\n");
}

int iWizCBCheckHTTPResCode(char *const data_p,unsigned int const len_p){
     char *iWizCheckHTTPResCode;
     int i;
     char * pch; 
     int iReturn = 0;
     iWizCheckHTTPResCode = (char *)malloc(len_p);
     if(iWizCheckHTTPResCode != NULL){ 
        memcpy(&iWizCheckHTTPResCode[0],&data_p[0],len_p);
        pch = strtok (iWizCheckHTTPResCode," ");
        while(pch != NULL){
            //printDebug("%s\r\n",pch); 
            if (strncmp (pch,"200",3) == 0)
            {   
                printDebug("[Res code: %s]\r\n",pch); 
                iReturn = 1;
                break;
            } 
            pch = strtok (NULL, " ");
        }
     }
     free(iWizCheckHTTPResCode); 
     return iReturn;
}
int iWizCBCheckHTTPContextLen(char *const data_p,unsigned int const len_p){
     char *iWizCheckHTTPContextLen;
     int i;
     char * pch; 
     int iReturn = 0;
     iWizCheckHTTPContextLen = (char *)malloc(len_p);
     if(iWizCheckHTTPContextLen != NULL){ 
        memcpy(&iWizCheckHTTPContextLen[0],&data_p[0],len_p);
        pch = strtok (iWizCheckHTTPContextLen," ");
        while(pch != NULL){
            if (strncmp (pch,"Content-Length",14) == 0)
            {    
                pch = strtok (NULL, " ");
                if(pch != NULL){
                    //if (strncmp (pch,"Content-Length",14) == 0){
                        printDebug("[Res len: %s]\r\n",pch); 
                        //iReturn = atoi(pch);  
                        iHTTPStatus.contentLength = atoi(pch);
                        iReturn = 1;
                        printDebug("[Res len: %d]\r\n",iReturn);
                    //    if(iReturn <= 0){
                    //         iReturn = WIZ_HTTP_CONTEXT_LEN_LESS;         //< 0
                    //    }else if(iReturn > 400){
                    //        iReturn = WIZ_HTTP_CONTEXT_LEN_OVER;          // > 400
                    //    }
                        break;
                    //}
                }
            }else{
                printDebug("[Notfound Res len]\r\n");
                break;
            } 
            //pch = strtok (NULL, " ");
        }
     }else{
        printDebug("[wizContentLen]can 't allocate mem.\r\n");
     }
     free(iWizCheckHTTPContextLen); 
     return iReturn;
}

int iWizCBChekStat(char *const data_p,unsigned int const len_p){
     char *iWizCheckStat;
     int i;
     iWizCheckStat = (char *)malloc(len_p); 
     if(iWizCheckStat != NULL){
         memcpy(&iWizCheckStat[0],&data_p[0],len_p);
         //printDebug("stat[%d](%s)\r\n",len_p,iWizCheckStat); 
//         printDebug("status[%d][",len_p); 
//         for(i=0;i<len_p;i++){
//            printDebug("%c",*(iWizCheckStat++)); 
//         }
//         printDebug("]\r\n");
         iWizData.status = WIZ_UNKNOW;
         if (strncmp (iWizCheckStat,"> Connected",11) == 0)
         {
            iWizData.status = WIZ_CONNECTED;
         }else if(strncmp (iWizCheckStat,"> Closed",8) == 0){
            iWizData.status = WIZ_CLOSE;
         }else{
            iWizData.status = WIZ_UNKNOW;
         }
     }else{
        printDebug("[wizStat]can 't allocate mem.\r\n");
     } 
     free(iWizCheckStat);   
     return 0;
}
int iWizCBReadData(char *const data_p,unsigned int const len_p){
     char *iWizReadData;
     int i;
     iWizReadData = (char *)malloc(len_p); 
     if(iWizReadData != NULL){
         memcpy(&iWizReadData[0],&data_p[0],len_p);
         //printDebug("stat[%d](%s)\r\n",len_p,iWizCheckStat); 
//         printDebug("status[%d][",len_p); 
//         for(i=0;i<len_p;i++){
//            printDebug("%c",*(iWizCheckStat++)); 
//         }
//         printDebug("]\r\n");
         //iWizData.status = WIZ_UNKNOW;
//         if (strncmp (iWizReadData,"> Connected",11) == 0)
//         {
//            iWizData.status = WIZ_CONNECTED;
//         }else if(strncmp (iWizReadData,"> Closed",8) == 0){
//            iWizData.status = WIZ_CLOSE;
//         }else{
//            iWizData.status = WIZ_UNKNOW;
//         }
     }else{
        printDebug("[wizStat]can 't allocate mem.\r\n");
     } 
     free(iWizReadData); 
     return 0;
}

void iWizSend(const char * data_p,int len_p){
     //char httpPostHeader[] = "POST /smasmartdevice HTTP/1.1\r\nHost: smasmartdevice.appspot.com\r\nUser-Agent: IMS_Test\r\nConnection: close\r\nContent-Length: "; 
     //char httpPostHeader[] = "POST /smasmartdevice HTTP/1.1\r\nHost: smasmartdevice.appspot.com\r\nUser-Agent: AVR2560-LAN\r\nConnection: close\r\nContent-Length: ";
     char *pviWizBuff;
     char lenData[10];
     int indexWrite = 0;  
     char ivHTTPHeaderMethod[] = "POST /smasmartdevice HTTP/1.1";               //29
     char ivHTTPHeaderHost[]  = "\r\nHost: smasmartdevice.appspot.com";    //34
     char ivHTTPHeaderMac[]   = "\r\nX-CLIENT-UUID: ";                     //18
     //char ivHTTPHeaderUser[]  = "\r\nUser-Agent: AVR2560-LAN";             //23
     char ivHTTPHeaderConn[]  = "\r\nConnection: close";                   //17
     char ivHTTPHeaderLen[]   = "\r\nContent-Length: ";                    //16
     
     //char 
     
//     printDebug("[wizSend]Print-1.[\r\n");
//     print_payload(data_p, len_p); 
//     printDebug("]\r\n");
     
     pviWizBuff = (char *)malloc(400);              //4(length unknow) = xx\r\n  
//     printDebug("[wizSend]Print-2.[\r\n");
//     print_payload(data_p, len_p); 
//     printDebug("]\r\n"); 
     
     if(pviWizBuff != NULL){
        sprintf(lenData,"%d\r\n\r\n",len_p);
         
//        memcpy(&pviWizBuff[indexWrite],&httpPostHeader[0],strlen(httpPostHeader));
//        indexWrite += strlen(httpPostHeader);
//        memcpy(&pviWizBuff[indexWrite],&lenData[0],strlen(lenData));
//        indexWrite += strlen(lenData);
//        memcpy(&pviWizBuff[indexWrite],&data_p[0],len_p); 
//        indexWrite += len_p; 
        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderMethod[0],strlen(ivHTTPHeaderMethod));
        indexWrite += strlen(ivHTTPHeaderMethod); 
         
        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderHost[0],strlen(ivHTTPHeaderHost));
        indexWrite += strlen(ivHTTPHeaderHost);
          
        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderMac[0],strlen(ivHTTPHeaderMac));
        indexWrite += strlen(ivHTTPHeaderMac);
        memcpy(&pviWizBuff[indexWrite],&iWiz100srConfig.macAddress[0],sizeof(iWiz100srConfig.macAddress));
        indexWrite += sizeof(iWiz100srConfig.macAddress);
        
//        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderUser[0],strlen(ivHTTPHeaderUser));
//        indexWrite += strlen(ivHTTPHeaderUser); 
         
        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderConn[0],strlen(ivHTTPHeaderConn));
        indexWrite += strlen(ivHTTPHeaderConn);
         
        memcpy(&pviWizBuff[indexWrite],&ivHTTPHeaderLen[0],strlen(ivHTTPHeaderLen));
        indexWrite += strlen(ivHTTPHeaderLen);
        memcpy(&pviWizBuff[indexWrite],&lenData[0],strlen(lenData));
        indexWrite += strlen(lenData);
        
        memcpy(&pviWizBuff[indexWrite],&data_p[0],len_p); 
        indexWrite += len_p; 
#if (WIZ100SR_PRINT_DEBUG == 1)        
        print_payload(pviWizBuff, indexWrite);
#endif         
        write_uart0(pviWizBuff, indexWrite);
        index0Buffer = 0;
        iWizRXMode = WIZ_RX_DATA_MODE;
        free(pviWizBuff); 
        
     }else{
        printDebug("[wizSend]can 't allocate mem.\r\n");
     }
     free(pviWizBuff);
     
}
void iWizReceive(){
     int i;
     //free();  
//     if(iHTTPStatus.connStatus == WIZ_HTTP_RES_CODE_OK && iHTTPStatus.contentLength >3){
//        print_payload(rx0Buffer, index0Buffer);
//     } 

     printDebug("index0Buffer(%d)\r\n",index0Buffer);
     //printDebug("iHTTPStatusLen(%d)\r\n",iHTTPStatus.contentLength); 
     if(index0Buffer > 0){
         for(i=0;i<index0Buffer;i++){
              printDebug("%c",rx0Buffer[i]);
         }
     }
     
     
     //print_payload(rx0Buffer, iHTTPStatus.contentLength);
}

int iWizConnected(){
    //char *pviCheckWizConn;
    int iReturn =  WIZ_CONN_FAIL; 
    char * pch; 
    TIMER   timeout;
    char  pviCheckWizConn[100];   
    //char viDataTest[] = "Pool\r\n";
    
    TIMER_setTimer(&timeout, 7); 
    memset(pviCheckWizConn,0,sizeof(pviCheckWizConn));
    index0Buffer = 0;
#if (WIZ100SR_PRINT_DEBUG == 1)    
    printDebug("[iWizConnected]Function.\r\n");
#endif     
    while(!TIMER_checkTimerExceed(timeout)){ 
        #asm("wdr") 
        delay_ms(50);
        //printDebug("[iWizConnected len(%d)]\r\n",index0Buffer);
        
        if((index0Buffer >= 11) && (index0Buffer <100) && (iWizRXMode == WIZ_RX_DATA_MODE)){        //11= >  Connected   //WIZ_RX_DATA_MODE  //WIZ_RX_STAT_MODE
            //pviCheckWizConn = (char *)malloc(index0Buffer);  
            //pviCheckWizConn = (char *)malloc(400); 
            //if(pviCheckWizConn != NULL){
               printDebug("[iWizConnected]len(%d)\r\n",index0Buffer);
               memcpy(&pviCheckWizConn[0],&rx0Buffer[0],index0Buffer); 
               pch = strtok (pviCheckWizConn,">");  
               //pch = strtok (pviCheckWizConn,"> ");
               while(pch != NULL){
                   #asm("wdr")
                   //printDebug("[Data split]%s\r\n",pch);
                   if(strncmp (pch," Connected",10)==0){
                        printDebug("[iWizConnected] %s\r\n",pch);
                        iReturn =  WIZ_CONNECTED;
                        break;
                   }else if(strncmp (pch," Listen : OK",12)==0){
                        printDebug("[iWizConnected] %s\r\n",pch);
                        iReturn =  WIZ_CONNECTED;
                        break;
                   }else if(strncmp (pch," Closed",7)==0){
                        printDebug("[iWizConnected] %s\r\n",pch);
                        iReturn =  WIZ_CLOSE;
                        //break;
                   }else{
                        printDebug("[iWizConnected] is unknow.\r\n");
                        iReturn =  WIZ_UNKNOW;
                   }
                   pch = strtok (NULL, ">");
               }
               
               //free(pviCheckWizConn);
               if(iReturn ==  WIZ_CONNECTED){   
                   printDebug("[iWizConnected] is connected.\r\n");
                   break;
               }else if((iReturn ==  WIZ_UNKNOW) || (iReturn ==  WIZ_CLOSE)){ 
                   //memset(rx0Buffer,0,400);
                   #asm("cli")
                   index0Buffer = 0; 
                   #asm("sei")
                   TIMER_setTimer(&timeout, 5);
                   printDebug("[iWizConnected] is close or unknow.\r\n");
                   //write_uart0(viDataTest, sizeof(viDataTest)-1);
                   delay_ms(100);
               }
//            }else{
//               printDebug("[iWizConnected]can 't allocate mem(%d).\r\n",index0Buffer); 
//               iReturn =  WIZ_CONN_FAIL;
//            } 
        }else if(index0Buffer >= 100){
           printDebug("[iWizConnected] buff overflow.\r\n");
        }else{
           iReturn =  WIZ_CONN_FAIL;
        }
    }      
    index0Buffer = 0;

    return iReturn; 
}

int iWizRead(){
    //char * pviWizRead;
    int iReturn = WIZ_DATA_EMPTY;
    char * pch;
    char * pviHttpResCode;
    char * pviHttpLen;
    char * pviHttpData;
    int i = 0;
    char *pviReadLine; 
    unsigned int viHtteDetaLen = 0;
    unsigned char viFlagHttpResCode;
    unsigned char viFlagHttpLen;  
    char pviWizRead[400];
    
    TIMER   timeout;
    TIMER_setTimer(&timeout, 3);
    while(!TIMER_checkTimerExceed(timeout)){
       #asm("wdr")
    } 
    //delay_ms(1000);
    
    memset(&pviWizRead,0,sizeof(pviWizRead));  
#if (WIZ100SR_PRINT_DEBUG == 1)
    printDebug("[iWizRead]Function\r\n");
#endif 
    if((index0Buffer > 0) && (iWizRXMode == WIZ_RX_DATA_MODE)){
        //pviWizRead = (char *)malloc(index0Buffer);
        //if(pviWizRead != NULL){
           memcpy(&pviWizRead[0],&rx0Buffer[0],index0Buffer);
//           for(i=0;i<index0Buffer;i++){
//                printDebug("%c",*(pviReadDisplay++));
//           }
           //printDebug("[HTTP]Raw Data\r\n"); 
#if (WIZ100SR_PRINT_DEBUG == 1)           
           print_payload(pviWizRead, index0Buffer);
#endif           
            
            pviReadLine = strtok(pviWizRead,"\r\n");
#if (WIZ100SR_PRINT_DEBUG == 1)           
            printDebug("[HTTP]Check ResCode\r\n");
#endif             
            while(pviReadLine != NULL){
                #asm("wdr")
                if(strncmp(pviReadLine,"HTTP/",5)==0){ 
                    pviHttpResCode = strtok (pviReadLine," ");
                    while(pviHttpResCode != NULL){
                         #asm("wdr")
                         if(strncmp(pviHttpResCode,"200",3)==0){
    #if (WIZ100SR_PRINT_DEBUG == 1)                     
                              printDebug("[HTTP]200 OK\r\n");
    #endif                            
                              viFlagHttpResCode = WIZ_HTTP_RES_CODE_OK;
                              //free(pviHttpResCode);
                              break;
                         }else{
                              viFlagHttpResCode = WIZ_HTTP_RES_CODE_ERROR;
                         }
                         pviHttpResCode = strtok (NULL," ");
                    }
                    if(viFlagHttpResCode == WIZ_HTTP_RES_CODE_OK){
                         break;
                    }
                    //pviReadLine = strtok(NULL,"\r\n");  
                }
                pviReadLine = strtok(NULL,"\r\n");
            }
            //free(pviReadLine);
           
            memset(&pviWizRead,0,sizeof(pviWizRead));
            memcpy(&pviWizRead[0],&rx0Buffer[0],index0Buffer);
#if (WIZ100SR_PRINT_DEBUG == 1)             
            printDebug("[HTTP]Check Length\r\n");
#endif            
            pviReadLine = strtok(pviWizRead,"\r\n");
            while(pviReadLine != NULL){ 
               #asm("wdr")
               if(strncmp(pviReadLine,"Content-Length",14)==0){ 
                  pviHttpLen = strtok(pviReadLine," ");
                  if(pviHttpLen != NULL){
                      pviHttpLen = strtok(NULL," "); 
#if (WIZ100SR_PRINT_DEBUG == 1)                      
                      printDebug("[DataLen][%s]\r\n",pviHttpLen);
#endif                      
                      viHtteDetaLen = atoi(pviHttpLen); 
                      viFlagHttpLen = WIZ_HTTP_CONTEXT_LEN_OK;
                      break;
                  }else{
                      viFlagHttpLen = WIZ_HTTP_CONTEXT_LEN_NOTFOUND;
                  }
               }
               if(viFlagHttpLen == WIZ_HTTP_CONTEXT_LEN_OK){
                  break;
               } 
               pviReadLine = strtok(NULL,"\r\n");
            }
            
            memset(&pviWizRead,0,sizeof(pviWizRead));
            memcpy(&pviWizRead[0],&rx0Buffer[0],index0Buffer);
#if (WIZ100SR_PRINT_DEBUG == 1)            
            printDebug("[HTTP]Parser Data\r\n"); 
#endif            
            pviHttpData = strstr (pviWizRead,"\r\n\r\n");
            if(pviHttpData){ 
#if (WIZ100SR_PRINT_DEBUG == 1)            
                printDebug("[Data][\r\n"); 
                print_payload(pviHttpData+4, viHtteDetaLen);
                printDebug("]\r\n");
#endif                 
                viRXData.length = viHtteDetaLen;
                memcpy(&viRXData.value[0],pviHttpData+4,viHtteDetaLen);  
                
//                memset(&pviWizRead,0,sizeof(pviWizRead));
//                memcpy(&pviWizRead[0],pviHttpData+4,viHtteDetaLen);
            } 
            
            if(viFlagHttpResCode==WIZ_HTTP_RES_CODE_OK && viFlagHttpLen == WIZ_HTTP_CONTEXT_LEN_OK){
                 
                 iReturn =  WIZ_DATA_AVAILABLE;
            }
    }else{
        iReturn =  WIZ_DATA_EMPTY;
    }
    index0Buffer = 0;
    return iReturn;
}

iUChar_t iWizCheckStatus(){
    iUChar_t viReturn = 0; 
    iChar_t viData[4];
    
    iWizSend(&viData[0],sizeof(viData));
    viReturn = iWizConnected();
    switch(viReturn){
        case WIZ_CONNECTED:{
           printDebug("[iWizCheckStatus]WIZ_CONNECTED.\r\n");
           break;
        }
        case WIZ_CLOSE:{
           printDebug("[iWizCheckStatus]WIZ_CLOSE.\r\n");
           break;
        }
        case WIZ_CONN_FAIL:{
           printDebug("[iWizCheckStatus]WIZ_CONN_FAIL.\r\n");
           break;
        }
    }
    return viReturn;
}

void iWizReActiveDataMode(){
    iChar_t viCmdModeChar[] = "+++";
    iChar_t viCmdRestart[] = "<WR>";       //Restart 
    
    iChar_t viWizReadBuffer[30]; 
    iChar_t viI = 0;
    TIMER   timeout;
    iChar_t viConfigMode;  
    iChar_t * pviCheckResponse;
    
    //Read configuration mode
    iWizRXMode = WIZ_CONFIG_READ_MODE;
    index0Buffer = 0;
    for(viI=0;viI<strlen(viCmdModeChar);viI++){
        putchar0(viCmdModeChar[viI]);
    }
    
    //wait wiz module response  
    TIMER_setTimer(&timeout, 1);
    while(!TIMER_checkTimerExceed(timeout)){
       #asm("wdr")  
       if(index0Buffer>0){ 
           delay_ms(100); 
           memset(viWizReadBuffer,0,sizeof(viWizReadBuffer));
           memcpy(&viWizReadBuffer[0],&rx0Buffer[0],index0Buffer);
           
           pviCheckResponse = strstr(viWizReadBuffer,"<E>");  
           if(pviCheckResponse){
#if (WIZ100SR_PRINT_DEBUG == 1)           
               printDebug("[iWizReadConfig]can config mode(Read).\r\n"); 
#endif               
           }else{
#if (WIZ100SR_PRINT_DEBUG == 1)            
               printDebug("[iWizReadConfig]can config mode(Read).\r\n"); 
#endif               
           }
           break;
       }
    } 
    
    if(pviCheckResponse){
        index0Buffer = 0;
        for(viI=0;viI<strlen(viCmdRestart);viI++){
            putchar0(viCmdRestart[viI]);
        }    
        delay_ms(20);
        //wait wiz module response  
        TIMER_setTimer(&timeout, 3);
        while(!TIMER_checkTimerExceed(timeout)){
           #asm("wdr")  
           if(index0Buffer>0){ 
               delay_ms(100);  
               memset(viWizReadBuffer,0,sizeof(viWizReadBuffer));
               memcpy(&viWizReadBuffer[0],&rx0Buffer[0],index0Buffer);
               
               pviCheckResponse = strstr(viWizReadBuffer,"<S>");   
               if(pviCheckResponse){ 
    #if (WIZ100SR_PRINT_DEBUG == 1)           
                   printDebug("[iWizReadConfig]Restart Wiz100SR success.\r\n");
    #endif 
               }else{
    #if (WIZ100SR_PRINT_DEBUG == 1)           
                   printDebug("[iWizReadConfig]Restart Wiz100SR unsuccess.\r\n");
    #endif 
               }
               break;
           }
        }
    }  
    
    //viConfigMode = WIZ_CONFIG_READ_MODE; 
    index0Buffer = 0;
    viConfigMode = WIZ_CONFIG_NORMAL_MODE;
}

void iWizReadConfig(){
    iChar_t viCmdModeChar[] = "+++";
    
    iChar_t viCmdMAC[] = "<RA>";                 //Get MAC Address                
    iChar_t viCmdIP[] = "<RI>";                  //Get Local IP  
    iChar_t viCmdSubnet[] = "<RS>";              //Get Subnet mask
    iChar_t viCmdGateWay[] = "<RG>";             //Get Gateway
    iChar_t viCmdLocalPort[] = "<RP>";           //Get Local IP’s port number
    iChar_t viCmdIPMethod[] = "<RD>";            //Get the IP configuration method
    iChar_t viCmdOperMode[] = "<RM>";            //Get the operation mode
    iChar_t viCmdProtocal[] = "<RK>";            //Get the Protocol 
    iChar_t viCmdBaudRate[] = "<RB>";            //Get the serial baud rate
    iChar_t viCmdCmdSerialMeth[] = "<RT>";       //Get the serial command method
    iChar_t viCmdCmdDomainName[] = "<RW>";       //Get the serial command method  
    
    iChar_t viCmdRestart[] = "<WR>";       //Restart 
    
    iChar_t viConfigMode;  
    iChar_t * pviCheckResponse;
    
    iChar_t viWizReadBuffer[30]; 
    iChar_t viI = 0;
    TIMER   timeout;
    
    //Read configuration mode
    //viConfigMode = WIZ_CONFIG_READ_MODE; 
    iWizRXMode = WIZ_CONFIG_READ_MODE; 
    //send +++  
    
    index0Buffer = 0;
    for(viI=0;viI<strlen(viCmdModeChar);viI++){
        putchar0(viCmdModeChar[viI]);
    }    

    //wait wiz module response  
    TIMER_setTimer(&timeout, 1);
    while(!TIMER_checkTimerExceed(timeout)){
       #asm("wdr")  
       if(index0Buffer>0){ 
           delay_ms(100); 
           memset(viWizReadBuffer,0,sizeof(viWizReadBuffer));
           memcpy(&viWizReadBuffer[0],&rx0Buffer[0],index0Buffer);
           
           pviCheckResponse = strstr(viWizReadBuffer,"<E>");  
           if(pviCheckResponse){
#if (WIZ100SR_PRINT_DEBUG == 1)           
               printDebug("[iWizReadConfig]can config mode(Read).\r\n"); 
#endif               
           }else{
#if (WIZ100SR_PRINT_DEBUG == 1)            
               printDebug("[iWizReadConfig]can not config mode(Read).\r\n"); 
#endif               
           }
           break;
       }
    }
    
    if(pviCheckResponse){
        delay_ms(50);
        if(iWizConfigSend(viCmdMAC,viWizReadBuffer)==1){
            iWizConfigMACParser(iWiz100srConfig.macAddress,viWizReadBuffer);  
            printDebug("[iWizReadConfig]MAC:");
            for(viI=0;viI<sizeof(iWiz100srConfig.macAddress);viI++){
                if(viI!=sizeof(iWiz100srConfig.macAddress)-1){
                    //printDebug("%02x:",iWiz100srConfig.macAddress[viI]);
                    printDebug("%c",iWiz100srConfig.macAddress[viI]);
                }else{
                    printDebug("%02x",iWiz100srConfig.macAddress[viI]);
                } 
            }
               
            printDebug("\r\n");
        }
        delay_ms(50);
        if(iWizConfigSend(viCmdIP,viWizReadBuffer)==1){
            iWizConfigIPParser(iWiz100srConfig.ip,viWizReadBuffer);  
            printDebug("[iWizReadConfig]IP:");
            for(viI=0;viI<sizeof(iWiz100srConfig.ip);viI++){
                if(viI!=sizeof(iWiz100srConfig.ip)-1){
                    //printDebug("%02x.",iWiz100srConfig.ip[viI]);
                    printDebug("%02d.",iWiz100srConfig.ip[viI]); 
                }else{
                    printDebug("%02x",iWiz100srConfig.ip[viI]);
                }
            } 
            printDebug("\r\n");
        }
        delay_ms(50); 
        if(iWizConfigSend(viCmdSubnet,viWizReadBuffer)==1){
            iWizConfigIPParser(iWiz100srConfig.subnet,viWizReadBuffer);  
            printDebug("[iWizReadConfig]Subnet mask:");
            for(viI=0;viI<sizeof(iWiz100srConfig.subnet);viI++){
                if(viI!=sizeof(iWiz100srConfig.subnet)-1){
                    //printDebug("%02x.",iWiz100srConfig.subnet[viI]);  
                    printDebug("%02d.",iWiz100srConfig.subnet[viI]);
                }else{
                    printDebug("%02x",iWiz100srConfig.subnet[viI]);
                }
            } 
            printDebug("\r\n");
        }
        delay_ms(50);  
        if(iWizConfigSend(viCmdGateWay,viWizReadBuffer)==1){
            iWizConfigIPParser(iWiz100srConfig.gateWay,viWizReadBuffer);  
            printDebug("[iWizReadConfig]Gateway:");
            for(viI=0;viI<sizeof(iWiz100srConfig.gateWay);viI++){
                if(viI!=sizeof(iWiz100srConfig.gateWay)-1){
                    //printDebug("%02x.",iWiz100srConfig.gateWay[viI]);
                    printDebug("%02d.",iWiz100srConfig.gateWay[viI]);
                }else{
                    printDebug("%02x",iWiz100srConfig.gateWay[viI]);
                }
            } 
            printDebug("\r\n");
        }
        delay_ms(50); 
        iWizConfigSend(viCmdLocalPort,viWizReadBuffer);
        iWizConfigSend(viCmdIPMethod,viWizReadBuffer);  
        iWizConfigSend(viCmdOperMode,viWizReadBuffer); 
        delay_ms(50); 
        if(iWizConfigSend(viCmdCmdDomainName,viWizReadBuffer)){
            memcpy(iWiz100srConfig.sdn,&viWizReadBuffer[2],sizeof(iWiz100srConfig.sdn));
            printDebug("[iWizReadConfig]SDN:%s\r\n",iWiz100srConfig.sdn);
        }
         
        
        
        index0Buffer = 0;
        for(viI=0;viI<strlen(viCmdRestart);viI++){
            putchar0(viCmdRestart[viI]);
        }    
        delay_ms(50);
        //wait wiz module response  
        TIMER_setTimer(&timeout, 3);
        while(!TIMER_checkTimerExceed(timeout)){
           #asm("wdr")  
           if(index0Buffer>0){ 
               delay_ms(100);  
               memset(viWizReadBuffer,0,sizeof(viWizReadBuffer));
               memcpy(&viWizReadBuffer[0],&rx0Buffer[0],index0Buffer);
               
               pviCheckResponse = strstr(viWizReadBuffer,"<S>");   
               if(pviCheckResponse){ 
    #if (WIZ100SR_PRINT_DEBUG == 1)           
                   printDebug("[iWizReadConfig]Restart Wiz100SR success.\r\n");
    #endif 
               }else{
    #if (WIZ100SR_PRINT_DEBUG == 1)           
                   printDebug("[iWizReadConfig]Restart Wiz100SR unsuccess.\r\n");
    #endif 
               }
               break;
           }
        } 
    } 
    
    
    index0Buffer = 0;
    viConfigMode = WIZ_CONFIG_NORMAL_MODE;
}

//iChar_t iWizWriteConfig(){
//    iChar_t viReturn = 0;//0==fail,1==success
//    iChar_t viCmdModeChar[] = "+++";
//                   
//    iChar_t viCmdIP[] = "<WI>";                  //Set Local IP  
//    iChar_t viCmdSubnet[] = "<WS>";              //Set Subnet mask
//    iChar_t viCmdGateWay[] = "<WG>";             //Set Gateway
//    iChar_t viCmdLocalPort[] = "<WP>";           //Set Local IP’s port number
//    iChar_t viCmdIPMethod[] = "<WD>";            //Set the IP configuration method
//    iChar_t viCmdOperMode[] = "<WM>";            //Set the operation mode
//    iChar_t viCmdProtocal[] = "<WK>";            //Set the Protocol 
//    iChar_t viCmdBaudRate[] = "<WB>";            //Set the serial baud rate
//    iChar_t viCmdCmdSerialMeth[] = "<WT>";       //Set the serial command method    
//    
//    iChar_t viCmdDnsUse[] = "WU";                //Set DNS Use or Not  
//    iChar_t viCmdServerIP[] = "WX";              //Set Server address 
//    iChar_t viCmdServerPort[] = "WN";            //Set Server port
//    iChar_t viCmdDnsIP[] = "WV";                 //Set DNS IP configuration
//    iChar_t viCmdDomainName[] = "<WW>";          //Set Domain configuration  
//    
//    iChar_t viCmdRestart[] = "<WR>";       //Restart 
//    
//    iChar_t viConfigMode;  
//    iChar_t * pviCheckResponse;
//    
//    iChar_t viWizReadBuffer[30]; 
//    iChar_t viI = 0;
//    TIMER   timeout;
//    
//    //Read configuration mode
//    //viConfigMode = WIZ_CONFIG_READ_MODE; 
//    iWizRXMode = WIZ_CONFIG_READ_MODE; 
//    //send +++  
//    
//    index0Buffer = 0;
//    for(viI=0;viI<strlen(viCmdModeChar);viI++){
//        putchar0(viCmdModeChar[viI]);
//    }  
//    
//    //wait wiz module response  
//    TIMER_setTimer(&timeout, 2);
//    while(!TIMER_checkTimerExceed(timeout)){
//       #asm("wdr")  
//       if(index0Buffer>0){ 
//           delay_ms(100); 
//           memset(viWizReadBuffer,0,sizeof(viWizReadBuffer));
//           memcpy(&viWizReadBuffer[0],&rx0Buffer[0],index0Buffer);
//           
//           pviCheckResponse = strstr(viWizReadBuffer,"<E>");  
//           if(pviCheckResponse){
//#if (WIZ100SR_PRINT_DEBUG == 1)           
//               printDebug("[iWizWriteConfig]can config mode.\r\n"); 
//#endif               
//           }else{
//#if (WIZ100SR_PRINT_DEBUG == 1)            
//               printDebug("[iWizReadConfig]can not config mode.\r\n"); 
//#endif         viReturn = 0; 
//               return viReturn;     
//           }
//           break;
//       }
//    }
//    
//    if(pviCheckResponse){
////        delay_ms(20);
////        if(iWizConfigSend(viCmdIP,viWizReadBuffer)==1){
////            iWizConfigIPParser(iWiz100srConfig.ip,viWizReadBuffer);  
////            printDebug("[iWizWriteConfig]IP:");
////            for(viI=0;viI<sizeof(iWiz100srConfig.ip);viI++){
////                if(viI!=sizeof(iWiz100srConfig.ip)-1){
////                    printDebug("%02x.",iWiz100srConfig.ip[viI]);
////                }else{
////                    printDebug("%02x",iWiz100srConfig.ip[viI]);
////                }
////            } 
////            printDebug("\r\n");
////        } 
//    } 
//    
//    
//    index0Buffer = 0;
//    viConfigMode = WIZ_CONFIG_NORMAL_MODE;   
//    
//    return viReturn;
//}



iChar_t iWizConfigSend(iChar_t * pviCmd_arg,iChar_t *pviDataBuff_arg){
    iChar_t viReturn = 0;
    TIMER   timeout;
    iChar_t viI = 0;
    iChar_t * pviCheckResponse;
    
    index0Buffer = 0;
    for(viI=0;viI<strlen(pviCmd_arg);viI++){
        putchar0(*(pviCmd_arg+viI));
    }    

    //wait wiz module response  
    TIMER_setTimer(&timeout, 1);
    while(!TIMER_checkTimerExceed(timeout)){
       #asm("wdr")  
       if(index0Buffer>0){ 
           delay_ms(100);   
           memset(pviDataBuff_arg,0,sizeof(pviDataBuff_arg));
           memcpy(pviDataBuff_arg,&rx0Buffer[0],index0Buffer);
               
           pviCheckResponse = strstr(pviDataBuff_arg,"<S");  
           if(pviCheckResponse){
#if (WIZ100SR_PRINT_DEBUG == 1)            
               printDebug("[iWizReadConfig]response data[.\r\n");
               print_payload(pviDataBuff_arg,strlen(pviDataBuff_arg)); 
               printDebug("].\r\n"); 
#endif               
           }else{
#if (WIZ100SR_PRINT_DEBUG == 1)            
               printDebug("[iWizReadConfig]wiz is not response.\r\n"); 
#endif               
           }
           break;
       }
    }
    viReturn = 1;
    return viReturn;
}

unsigned int* hex_decode(char *in, unsigned int len, unsigned int *out)
{
    unsigned int i, t, hn, ln;

    for (t = 0,i = 0; i < len; i+=2,++t) {

            hn = in[i] > '9' ? (in[i]|32) - 'a' + 10 : in[i] - '0';
            ln = in[i+1] > '9' ? (in[i+1]|32) - 'a' + 10 : in[i+1] - '0';

            out[t] = (hn << 4 ) | ln;
            printf("%s",out[t]);
    }
    return out;
}

iChar_t iWizConfigIPParser(iChar_t *pviOutData_arg,iChar_t *pviInData_arg){
    iChar_t *ptrCh;
    iChar_t viIP[4];   //ip,subnet,gateway
    iInt_t viNumber;
    iChar_t viReturn = 0;
    iChar_t viI=0; 
    
    ptrCh = strtok((pviInData_arg+2),".");   //2= <S
    
	viI=0;
	while(ptrCh != NULL) {
		viIP[viI] = (unsigned char)(atoi(ptrCh));
		ptrCh = strtok(NULL,".");
		viI++;
	}
    memcpy(pviOutData_arg,viIP,4);
//	printf("\r\n");
//	for(i=0; i<4; i++) {
//		printf("%02x ",ip[i]);
//	}
    
    viReturn = 1;
    return viReturn;
}

iChar_t iWizConfigMACParser(iUChar_t *pviOutData_arg,iChar_t *pviInData_arg){
    iChar_t *ptrCh;
    iChar_t viMAC[12];
    iInt_t viNumber;
    iChar_t viReturn = 0;
    iChar_t viI=0;
    
    ptrCh = strtok((pviInData_arg+2),".");    //2= <S
	viI=0;
	while(ptrCh != NULL){
		//viMAC[viI] = iMapAsciiHexToChar(ptrCh); 
        memcpy((pviOutData_arg+viI),ptrCh,2);
		ptrCh = strtok(NULL,".");
		viI+=2; 
	}
    viReturn = 1;
    return viReturn; 
}

iChar_t iMapAsciiHexToChar(iChar_t *chIn_arg){      //ascii hex 2 byte => char 1 byte
	iChar_t ivChar=0x00;
	iChar_t *ptrCh;
	iChar_t chReturn;
	iChar_t i,j,k,l;

	ptrCh = chIn_arg;

	for(i=0,j=0x41,k=0x61,l=10; i<6; i++,j++,k++,l++) { //41='A' //61='a'
		//printDebug("[hex1]%02x:%02x:%02x (%d) \r\n",i,j,k,l);
		if((*(ptrCh) == j) || (*(ptrCh) == k)) {
			ivChar = l;
			break;
		}
	}
	for(i=0,j=0x30; i<10; i++,j++) { //0-9 = 0x30-0x39
		if(*(ptrCh) == j) {
			ivChar = i;
			break;
		}
	}
	//printDebug("hex1:%02x \r\n",ivChar);
	ptrCh++;

	for(i=0,j=0x41,k=0x61,l=10; i<6; i++,j++,k++,l++) { //41='A' //61='a'
		//printDebug("[hex1]%02x:%02x:%02x (%d) \r\n",i,j,k,l);
		if((*(ptrCh) == j) || (*(ptrCh) == k)) {
			ivChar = (ivChar<<4)|l;
			break;
		}
	}
	for(i=0,j=0x30; i<10; i++,j++) { //0-9 = 0x30-0x39
		if(*(ptrCh) == j) {
			ivChar = (ivChar<<4)|i;
			break;
		}
	}
	//printDebug("hex2:%02x \r\n",ivChar);
	chReturn = ivChar;
	return chReturn;
}
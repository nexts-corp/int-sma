#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "oled.h"
#include "io.h"
#include "alarm.h"
#include "config.h"
#include "debug.h"
                                                     

eeprom char userPwd[4]              = {0,0,0,0};                    // -- default password for user permission -- //
eeprom char servicePwd[4]           = {2,4,7,5};                    // -- default password for sevice permission -- //
char currentPage                    = P_MONITOR;                    // -- track the currently displaying page -- //

/**************************************************************/
/************* Menu (Finite State Machine) Function ***********/
/**************************************************************/
/*============================================================*/
void MENU_processKey(char key)
{    
    int  i,j,res;                                     
    static signed char  cursor      = 0;                 
    static char         tmpNextPage = 0;                 
    static char         tmpPrevPage = 0;  
    static signed char  entPwd[4]   = {0,0,0,0};     
    static signed char  newPwd[4]   = {0,0,0,0};     
    static signed char  confPwd[4]  = {0,0,0,0};      
    
    static char setIp[12]           = {0,0,0,0,0,0,0,0,0,0,0,0};      
    static char setMask[12]         = {0,0,0,0,0,0,0,0,0,0,0,0};      
    static char setGw[12]           = {0,0,0,0,0,0,0,0,0,0,0,0};      
    static char setServer[12]       = {0,0,0,0,0,0,0,0,0,0,0,0};     
    
    unsigned char *msg;                            
    unsigned int  msg_len           = 0;
    
    static char devEn               = 0;
    static char almEn               = 0;           
    static char prbEn[5]            = {0,0,0,0,0};                  
    
    static char mute                = 0;          
    
    static char vRef[3]             = {0,0,0};    
    
    static char prb1                = 0;
    static char prb2                = 0;           
    static unsigned char calPnt[6]  = {0,0,0,0,0,0};
    
    char eeprom *uPwd = userPwd;
    char eeprom *sPwd = servicePwd;        
    
    char str[20];       
    char cmd[50];
    int  digit      = 0;                           
    
    if      ( currentPage == P_MONITOR )
    {
        if(key == KEY_MODE){
            currentPage     = P_MAIN_MENU;                    
            cursor          = 0;   
            printOLED("M1%1d",cursor);      
        }
    } 
    else if ( currentPage == P_MAIN_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MONITOR;        cursor  = 0;        printOLED("M0");            break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 3;    }   printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 3){               cursor  = 0;    }   printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- Device Config      
                                currentPage = P_DEV_MENU;  
                                cursor      = 0;     
                                printOLED("M2%1d",cursor);       
                            }
                            else if(cursor == 1){       // -- Probe Config        
                                currentPage = P_PRB_MENU;
                                cursor      = 0;     
                                printOLED("M3%1d",cursor);         
                            }
                            else if(cursor == 2){       // -- Management   
                                currentPage = P_MANAGE_MENU;
                                cursor      = 0;     
                                printOLED("M4%1d",cursor);
                            }
                            else if(cursor == 3){       // -- Maintenance    
                                currentPage = P_ENTER_M_PASS;    
                                tmpNextPage = P_MTNC_MENU;      
                                tmpPrevPage = P_MAIN_MENU;    
                                memset(entPwd,0xFF,sizeof(entPwd));  
                                cursor      = 0;             
                                printOLED("P1");            
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }        
    else if ( currentPage == P_DEV_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MAIN_MENU;      cursor  = 0;            printOLED("M1%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- View Device Config     
                                currentPage = P_DEV_V_MENU;
                                cursor      = 0;     
                                printOLED("M5%1d",cursor);       
                            }
                            else if(cursor == 1){       // -- Set Device Config     
                                currentPage = P_ENTER_U_PASS;   
                                tmpNextPage = P_DEV_S_MENU;    
                                tmpPrevPage = P_DEV_MENU;     
                                memset(entPwd,0xFF,sizeof(entPwd));  
                                cursor      = 0;     
                                printOLED("P0");                   
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }               
    else if ( currentPage == P_DEV_V_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_MENU;       cursor  = 0;            printOLED("M2%1d",cursor);  break;   
        /* // -- disable network view -- //
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;     
        */
        // -- enable network view -- //
        case KEY_UP     :   if(--cursor < 0){               cursor  = 2;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 2){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;  
        case KEY_OK     :   if(cursor == 0){            // -- View Properties             
                                currentPage = P_DEV_V_PROP;
                                cursor      = 0;                                                
                                printDebug("<MENU_processKey> VP%s,%s,%s,",  properties_config.deviceID,
                                                                            properties_config.hostID,
                                                                            properties_config.firmwareVersion); 
                                                        
                                printOLED("VP%s,%s,%s,", properties_config.deviceID,
                                                        properties_config.hostID,
                                                        properties_config.firmwareVersion);       
                            }
                            else if(cursor == 1){       // -- View Operate Config     
                                currentPage = P_DEV_V_OPER;
                                cursor      = 0;                                                    
                                                                                                                
                                memset(cmd,'\0',sizeof(cmd));   sprintf(cmd,"");    
                                memset(str,'\0',sizeof(str));
                                if(     operating_config.mode == MODE_REALTIME){    sprintf(str,"Realtime,");   }
                                else if(operating_config.mode == MODE_DATALOGGER){  sprintf(str,"Datalogger,"); }   
                                else{                                               sprintf(str,"Unknown,");    }
                                strcat(cmd,str);                                                    
                                                            
                                memset(str,'\0',sizeof(str));
                                if(     operating_config.deviceEnable == 0x01   ){  sprintf(str,"Enable,");     }    
                                else{                                               sprintf(str,"Disable,");    }   
                                strcat(cmd,str);                                                                    
                                                             
                                memset(str,'\0',sizeof(str));
                                if(     operating_config.alarmEnable  == 0x01   ){  sprintf(str,"Enable,");     }    
                                else{                                               sprintf(str,"Disable,");    }   
                                strcat(cmd,str);                                
                                                              
                                memset(str,'\0',sizeof(str));
                                for(i=0,j=0;i<MAX_SENS_CHANNEL;i++){     
                                    if(operating_config.sensorEnable[i]==0x01   ){  str[j++] = 'E';             }
                                    else{                                           str[j++] = 'D';             }      
                                    if( i < MAX_SENS_CHANNEL-1){
                                        str[j++] = '/';             
                                    }
                                    
                                }                                                                                 
                                str[j++]    = ',';                                                      
                                str[j++]    = '\0';                                                      
                                strcat(cmd,str);      
                                
                                
                                printDebug("<MENU_processKey> VO%s", cmd); 
                                printOLED("VO%s", cmd);              
                            }
                            else if(cursor == 2){       // -- View Network Config     
                                                                 
                                memset(cmd,'\0',sizeof(cmd));   sprintf(cmd,"");      
                                
                                msg = (unsigned char *) malloc( sizeof(char) * CONF_READ_BUFFER );
                                if(msg == NULL){       
                                    printDebug("<MENU_processKey> malloc(CONF_READ_BUFFER) failed\r\n");
                                    currentPage = P_DEV_MENU;       cursor  = 0;            printOLED("M2%1d",cursor);  break;     
                                }                                             
                                
                                res = ETH_getConfig(msg,&msg_len);               
                                if(res < 0){                             
                                    printDebug("<MENU_processKey> ETH_getConfig() failed\r\n");
                                }                                    
                                else{
                                    printDebug("<MENU_processKey> ETH_getConfig() success\r\n");             
                                    printDebug("\r\n\r\n<MENU_processKey> read[%d] -->\r\n",msg_len);
                                    for(i=0;i<msg_len;i++){
                                        putchar3(msg[i]);
                                    }                      
                                    printDebug("\r\n");
                                }                                 
                                      
                                res = getNetworkInfo(msg,&setIp[0],&setMask[0],&setGw[0],&setServer[0]); 
                                if(res<0){
                                    printDebug("<MENU_processKey> getNetworkInfo() failed\r\n");   
                                    return;
                                }              
                                
                                currentPage = P_DEV_V_NETW;
                                cursor      = 0;             
                                               
                                printDebug("<MENU_processKey> VN%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,\r\n",
                                            (setIp[0]    *100) + (setIp[1]     *10) + setIp[2],
                                            (setIp[3]    *100) + (setIp[4]     *10) + setIp[5],
                                            (setIp[6]    *100) + (setIp[7]     *10) + setIp[8],
                                            (setIp[9]    *100) + (setIp[10]    *10) + setIp[11],
                                            (setMask[0]  *100) + (setMask[1]   *10) + setMask[2],
                                            (setMask[3]  *100) + (setMask[4]   *10) + setMask[5],
                                            (setMask[6]  *100) + (setMask[7]   *10) + setMask[8],
                                            (setMask[9]  *100) + (setMask[10]  *10) + setMask[11], 
                                            (setGw[0]    *100) + (setGw[1]     *10) + setGw[2],
                                            (setGw[3]    *100) + (setGw[4]     *10) + setGw[5],
                                            (setGw[6]    *100) + (setGw[7]     *10) + setGw[8],
                                            (setGw[9]    *100) + (setGw[10]    *10) + setGw[11],     
                                            (setServer[0]*100) + (setServer[1] *10) + setServer[2],
                                            (setServer[3]*100) + (setServer[4] *10) + setServer[5],
                                            (setServer[6]*100) + (setServer[7] *10) + setServer[8],
                                            (setServer[9]*100) + (setServer[10]*10) + setServer[11]);
                                
                                printOLED("VN%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,",
                                            (setIp[0]    *100) + (setIp[1]     *10) + setIp[2],
                                            (setIp[3]    *100) + (setIp[4]     *10) + setIp[5],
                                            (setIp[6]    *100) + (setIp[7]     *10) + setIp[8],
                                            (setIp[9]    *100) + (setIp[10]    *10) + setIp[11],
                                            (setMask[0]  *100) + (setMask[1]   *10) + setMask[2],
                                            (setMask[3]  *100) + (setMask[4]   *10) + setMask[5],
                                            (setMask[6]  *100) + (setMask[7]   *10) + setMask[8],
                                            (setMask[9]  *100) + (setMask[10]  *10) + setMask[11], 
                                            (setGw[0]    *100) + (setGw[1]     *10) + setGw[2],
                                            (setGw[3]    *100) + (setGw[4]     *10) + setGw[5],
                                            (setGw[6]    *100) + (setGw[7]     *10) + setGw[8],
                                            (setGw[9]    *100) + (setGw[10]    *10) + setGw[11],     
                                            (setServer[0]*100) + (setServer[1] *10) + setServer[2],
                                            (setServer[3]*100) + (setServer[4] *10) + setServer[5],
                                            (setServer[6]*100) + (setServer[7] *10) + setServer[8],
                                            (setServer[9]*100) + (setServer[10]*10) + setServer[11]);  
                                free( msg );         
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }                
    else if ( currentPage == P_DEV_V_PROP )
    {  
        switch(key){
        case KEY_UP     :   /* no operation */                                                                  break;
        case KEY_DOWN   :   /* no operation */                                                                  break;
        case KEY_MODE   :   
        case KEY_OK     :   currentPage = P_DEV_V_MENU;     
                            cursor      = 0;    
                            printOLED("M5%1d",cursor);  
                        break;
        }
    }    
    else if ( currentPage == P_DEV_V_OPER )
    {  
        switch(key){
        case KEY_UP     :   /* no operation */                                                                  break;
        case KEY_DOWN   :   /* no operation */                                                                  break;
        case KEY_MODE   :   
        case KEY_OK     :   currentPage = P_DEV_V_MENU;     
                            cursor      = 1;    
                            printOLED("M5%1d",cursor);  
                        break;
        }
    } 
    else if ( currentPage == P_DEV_V_NETW )
    {  
        switch(key){
        case KEY_UP     :   /* no operation */                                                                  break;
        case KEY_DOWN   :   /* no operation */                                                                  break;
        case KEY_MODE   :   
        case KEY_OK     :   currentPage = P_DEV_V_MENU;     
                            cursor      = 2;    
                            printOLED("M5%1d",cursor);  
                        break;
        }
    }             
    else if ( currentPage == P_DEV_S_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_MENU;       cursor  = 1;            printOLED("M2%1d",cursor);  break;
        //case KEY_UP     :   break;
        //case KEY_DOWN   :   break;  
        // -- enable network configuration -- //
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;  
        
        case KEY_OK     :   if(cursor == 0){            // -- Set Operate Config
                                currentPage = P_DEV_S_OPER;
                                cursor      = 0;                 
                                devEn       = operating_config.deviceEnable;
                                almEn       = operating_config.alarmEnable;        
                                printDebug("<MENU_processKey> devEn[%d] almEn[%d]",devEn,almEn);
                                printOLED("SO%1d",cursor);     
                                printOLED("ED2%1d",devEn);     
                                printOLED("ED3%1d",almEn);
                            }
                            else if(cursor == 1){       // -- Set Network Config  
                                currentPage = P_DEV_S_IP;
                                cursor      = 0;     
                                printOLED("SI");     
                                for(i=0;i<12;i++){
                                    printOLED("ID%1X%1d",i,setIp[i]);
                                }
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;          
        }
    }             
    else if ( currentPage == P_DEV_S_OPER )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_MENU;     cursor  = 0;            printOLED("M6%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 2;    }       printOLED("SO%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 2){               cursor  = 0;    }       printOLED("SO%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- Set Device Enable     
                                devEn       = !devEn;                         
                                printDebug("<MENU_processKey> devEn -> ED%1d%1d",cursor+2,devEn);
                                printOLED("ED%1d%1d",cursor+2,devEn);       
                            }
                            else if(cursor == 1){       // -- Set Alarm Enable     
                                almEn       = !almEn;                               
                                printDebug("<MENU_processKey> almEn -> ED%1d%1d",cursor+2,almEn);
                                printOLED("ED%1d%1d",cursor+2,almEn);       
                            }
                            else if(cursor == 2){       // -- Display Finish page  
                                                    
                                operating_config.deviceEnable   = devEn;
                                operating_config.alarmEnable    = almEn;      
                                res = setOperatingConfig(&operating_config);
                                if(res < 0){
                                    printDebug("<MENU_processKey> setOperatingConfig() failed\r\n");    
                                    currentPage = P_FAILED;                                                     
                                    tmpNextPage = P_DEV_S_MENU;       
                                    tmpPrevPage = P_DEV_S_OPER;
                                    cursor      = 0;     
                                    printOLED("FL");       
                                }                 
                                else{                                      
                                    currentPage = P_FINISH;               
                                    tmpNextPage = P_DEV_S_MENU;       
                                    tmpPrevPage = P_DEV_S_OPER;     
                                    cursor      = 0;      
                                    printOLED("FI");
                                }     
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }    
    else if ( currentPage == P_DEV_S_IP )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_MENU;     cursor  = 1;            printOLED("M6%1d",cursor);              break;
        case KEY_UP     :   if(++cursor > 12){              cursor  = 0;    }       printOLED("IA%1X",cursor);              break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 12;   }       printOLED("IA%1X",cursor);              break;
        case KEY_OK     :   if(cursor == 12){    
                                cursor = 0;                                                 
                                currentPage = P_DEV_S_MASK;
                                printOLED("SM");   
                                for(i=0;i<12;i++){
                                    printOLED("ID%1X%1d",i,setMask[i]);
                                }
                            }                
                            else if(cursor < 12){            
                                if((cursor%3) == 0){    
                                    if(++setIp[cursor] >2)  setIp[cursor] = 0;  
                                }   
                                else{                   
                                    if(++setIp[cursor] >9)  setIp[cursor] = 0;  
                                }                    
                                //if(++setIp[cursor] >9)  setIp[cursor] = 0;
                                printOLED("ID%1X%1d",cursor, setIp[cursor]);  
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }   
            break;       
        }
    }          
    else if ( currentPage == P_DEV_S_MASK )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_IP;       cursor  = 0;            printOLED("SI");                        break;
        case KEY_UP     :   if(++cursor > 12){              cursor  = 0;    }       printOLED("IA%1X",cursor);              break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 12;   }       printOLED("IA%1X",cursor);              break;
        case KEY_OK     :   if(cursor == 12){    
                                cursor = 0;                                                 
                                currentPage = P_DEV_S_GATEWAY;
                                printOLED("SG");        
                                for(i=0;i<12;i++){
                                    printOLED("ID%1X%1d",i,setGw[i]);
                                }
                            }                
                            else if(cursor < 12){               
                                if((cursor%3) == 0){    
                                    if(++setMask[cursor] >2)  setMask[cursor] = 0;  
                                }   
                                else{                   
                                    if(++setMask[cursor] >9)  setMask[cursor] = 0;  
                                }                                  
                                //if(++setMask[cursor] >9)  setMask[cursor] = 0;
                                printOLED("ID%1X%1d",cursor, setMask[cursor]);  
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }  
            break;       
        }
    }              
    else if ( currentPage == P_DEV_S_GATEWAY )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_MASK;     cursor  = 0;            printOLED("SM");                        break;
        case KEY_UP     :   if(++cursor > 12){              cursor  = 0;    }       printOLED("IA%1X",cursor);              break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 12;   }       printOLED("IA%1X",cursor);              break;
        case KEY_OK     :   if(cursor == 12){    
                                cursor = 0;                                                 
                                currentPage = P_DEV_S_SERVER;
                                printOLED("SS");         
                                for(i=0;i<12;i++){
                                    printOLED("ID%1X%1d",i,setServer[i]);
                                }
                            }                
                            else if(cursor < 12){            
                                if((cursor%3) == 0){    
                                    if(++setGw[cursor] >2)  setGw[cursor] = 0;  
                                }   
                                else{                   
                                    if(++setGw[cursor] >9)  setGw[cursor] = 0;  
                                }                                             
                                //if(++setGw[cursor] >9)  setGw[cursor] = 0;
                                printOLED("ID%1X%1d",cursor, setGw[cursor]);  
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }  
            break;       
        }
    }                        
    else if ( currentPage == P_DEV_S_SERVER )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_GATEWAY;  cursor  = 0;            printOLED("SG");                        break;
        case KEY_UP     :   if(++cursor > 12){              cursor  = 0;    }       printOLED("IA%1X",cursor);              break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 12;   }       printOLED("IA%1X",cursor);              break;
        case KEY_OK     :   if(cursor == 12){    
                                currentPage = P_DEV_S_CONFIRM;                 
                                cursor      = 0;          
                                printOLED("SF%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,",
                                            (setIp[0]    *100) + (setIp[1]     *10) + setIp[2],
                                            (setIp[3]    *100) + (setIp[4]     *10) + setIp[5],
                                            (setIp[6]    *100) + (setIp[7]     *10) + setIp[8],
                                            (setIp[9]    *100) + (setIp[10]    *10) + setIp[11],
                                            (setMask[0]  *100) + (setMask[1]   *10) + setMask[2],
                                            (setMask[3]  *100) + (setMask[4]   *10) + setMask[5],
                                            (setMask[6]  *100) + (setMask[7]   *10) + setMask[8],
                                            (setMask[9]  *100) + (setMask[10]  *10) + setMask[11], 
                                            (setGw[0]    *100) + (setGw[1]     *10) + setGw[2],
                                            (setGw[3]    *100) + (setGw[4]     *10) + setGw[5],
                                            (setGw[6]    *100) + (setGw[7]     *10) + setGw[8],
                                            (setGw[9]    *100) + (setGw[10]    *10) + setGw[11],     
                                            (setServer[0]*100) + (setServer[1] *10) + setServer[2],
                                            (setServer[3]*100) + (setServer[4] *10) + setServer[5],
                                            (setServer[6]*100) + (setServer[7] *10) + setServer[8],
                                            (setServer[9]*100) + (setServer[10]*10) + setServer[11]);     
                            }                
                            else if(cursor < 12){     
                                if((cursor%3) == 0){    
                                    if(++setServer[cursor] >2)  setServer[cursor] = 0;  
                                }   
                                else{                   
                                    if(++setServer[cursor] >9)  setServer[cursor] = 0;  
                                }                                                                     
                                //if(++setServer[cursor] >9)  setServer[cursor] = 0;
                                printOLED("ID%1X%1d",cursor, setServer[cursor]);  
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }  
            break;       
        }
    }              
    else if ( currentPage == P_DEV_S_CONFIRM )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_DEV_S_MENU;     cursor  = 1;    printOLED("M6%1d",cursor);  break;
        case KEY_UP     :   /* no operation */                                                          break;
        case KEY_DOWN   :   /* no operation */                                                          break;
        case KEY_OK     :   currentPage = P_FINISH;                    
                            tmpNextPage = P_DEV_S_MENU;             
                            tmpPrevPage = P_DEV_S_CONFIRM;              
                            cursor      = 0;                         
                            // -- set network -- // 
                            res = ETH_setConfig(setIp,setMask,setGw,setServer);                                               
                            //res = setNetworkInfo(setIp,setMask,setGw,setServer);    
                            if(res<0){
                                printDebug("<MENU_processKey> ETH_setConfig() failed \r\n");
                            }                        
                            
                            // ----------------- //
                            printOLED("FI");  
                            break;       
        }
    }       
    else if ( currentPage == P_PRB_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MAIN_MENU;      cursor  = 1;            printOLED("M1%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- View Probe Config 
                                currentPage = P_PRB_V;
                                cursor      = 0;     
                                showProbeConfig(cursor);                         
                                                                   
                            }
                            else if(cursor == 1){       // -- Set Probe Config     
                                currentPage = P_ENTER_U_PASS;   
                                tmpNextPage = P_PRB_S;   
                                tmpPrevPage = P_PRB_MENU;     
                                memset(entPwd,0xFF,sizeof(entPwd));  
                                cursor      = 0;     
                                printOLED("P0");                   
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }  
    else if ( currentPage == P_PRB_V )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_PRB_MENU;       cursor  = 0;            printOLED("M3%1d",cursor);  break;
        case KEY_UP     :   if(++cursor > 4){               cursor  = 0;    }       showProbeConfig(cursor);    break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 4;    }       showProbeConfig(cursor);    break;
        case KEY_OK     :   currentPage = P_PRB_MENU;       cursor  = 0;            printOLED("M3%1d",cursor);  break;
        }
    }  
    else if ( currentPage == P_PRB_S )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_PRB_MENU;       cursor  = 1;            printOLED("M3%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 5;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 5){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor < 5){                       
                                prbEn[cursor] = !prbEn[cursor];
                                printOLED("ED%1d%1d",cursor,prbEn[cursor]);       
                            }
                            else if(cursor == 5){       // -- Display Finish page 
                                currentPage = P_FINISH;                    
                                tmpNextPage = P_PRB_MENU;
                                cursor      = 0;    
                                for(i=0;i<MAX_SENS_CHANNEL;i++){     
                                    operating_config.sensorEnable[i]    = prbEn[i];  
                                }                                        
                                
                                res = setOperatingConfig(&operating_config);
                                if(res < 0){
                                    printDebug("<MENU_processKey> setOperatingConfig() failed\r\n");    
                                    currentPage = P_FAILED;                                                     
                                    tmpNextPage = P_PRB_MENU;       
                                    tmpPrevPage = P_PRB_S;
                                    cursor      = 0;     
                                    printOLED("FL");       
                                }                 
                                else{                                      
                                    currentPage = P_FINISH;               
                                    tmpNextPage = P_PRB_MENU;       
                                    tmpPrevPage = P_PRB_S;     
                                    cursor      = 0;      
                                    printOLED("FI");
                                }                      
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }                          
    else if ( currentPage == P_MANAGE_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MAIN_MENU;      cursor  = 2;            printOLED("M1%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- Set Mute 
                                currentPage = P_MUTE_MENU;
                                cursor      = 0; 
                                mute        = muteMode;    
                                printOLED("MU%1d",cursor);
                                printOLED("ED%1d%1d",cursor+2,mute);       
                            }
                            else if(cursor == 1){       // -- Set New User Password        
                                currentPage = P_ENTER_U_PASS;   
                                tmpNextPage = P_PASS_NEW;
                                tmpPrevPage = P_MANAGE_MENU;  
                                memset(entPwd,0xFF,sizeof(entPwd));  
                                cursor      = 0;     
                                printOLED("P0");                   
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }     
    else if ( currentPage == P_MUTE_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MANAGE_MENU;    cursor  = 0;            printOLED("M4%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 1;    }       printOLED("MU%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 1){               cursor  = 0;    }       printOLED("MU%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- Set Device Enable     
                                mute       = !mute;
                                printOLED("ED%1d%1d",cursor+2,mute);       
                            }
                            else if(cursor == 1){       // -- Display Finish page     
                                currentPage = P_FINISH;                    
                                tmpNextPage = P_MANAGE_MENU;     
                                tmpPrevPage = P_MUTE_MENU;    
                                muteMode    = mute;
                                cursor      = 0;     
                                printOLED("FI");       
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }    
    else if ( currentPage == P_PASS_NEW )   
    {           
        switch(key){
        case KEY_MODE   :   currentPage = P_MANAGE_MENU;    cursor  = 1;        printOLED("M4%1d",cursor);      break;
        case KEY_UP     :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(++cursor > 4){   cursor  = 0;                }   printOLED("PA%1d",cursor);      break;
        case KEY_DOWN   :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(--cursor < 0){   cursor  = 4;                }   printOLED("PA%1d",cursor);      break;
        case KEY_OK     :   if(cursor == 4){ 
                                currentPage = P_PASS_CONFIRM;                       
                                memset(confPwd,0xFF,sizeof(confPwd)); 
                                cursor      = 0;    
                                printOLED("P5");
                            }                
                            else if(cursor < 4){
                                if(++newPwd[cursor] >9) newPwd[cursor] = 0;
                                printOLED("PD%1d%1d",cursor, newPwd[cursor]);  
                            }        
                            break; 
        }
    }   
    else if ( currentPage == P_PASS_CONFIRM )   
    {           
        switch(key){
        case KEY_MODE   :   currentPage = P_MANAGE_MENU;    cursor  = 1;        printOLED("M4%1d",cursor);      break;
        case KEY_UP     :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(++cursor > 4){   cursor  = 0;                }   printOLED("PA%1d",cursor);      break;
        case KEY_DOWN   :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(--cursor < 0){   cursor  = 4;                }   printOLED("PA%1d",cursor);      break;
        case KEY_OK     :   if(cursor == 4){ 
                                // -- checkValidation -- //
                                if(memcmp(newPwd,confPwd,sizeof(newPwd)) != 0){                   
                                    printDebug("<MENU_processKey> newPwd & confPwd not match\r\n");       
                                    memset(newPwd,0xFF,sizeof(newPwd));
                                    memset(confPwd,0xFF,sizeof(confPwd));
                                    currentPage = P_FAILED;                 
                                    tmpNextPage = P_MANAGE_MENU; 
                                    tmpPrevPage = P_PASS_CONFIRM;
                                    cursor      = 0;     
                                    printOLED("FL");      
                                }
                                else{                   
                                    r2ecpy(uPwd,(unsigned char *) confPwd,sizeof(userPwd));
                                    currentPage = P_FINISH;         
                                    tmpNextPage = P_MANAGE_MENU; 
                                    tmpPrevPage = P_PASS_CONFIRM;
                                    cursor      = 0;            
                                    printOLED("FI");       
                                }
                            }                
                            else if(cursor < 4){
                                if(++confPwd[cursor] >9) confPwd[cursor] = 0;
                                printOLED("PD%1d%1d",cursor, confPwd[cursor]);  
                            }   
                                 
                            break;
        }
    }
    else if ( currentPage == P_MTNC_MENU )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MAIN_MENU;      cursor  = 3;            printOLED("M1%1d",cursor);  break;
        case KEY_UP     :   if(--cursor < 0){               cursor  = 3;    }       printOLED("MA%1d",cursor);  break;
        case KEY_DOWN   :   if(++cursor > 3){               cursor  = 0;    }       printOLED("MA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){            // -- Set Voltage Reference          
                                currentPage = P_SET_VREF;
                                cursor      = 0;    
                                res = get_current_vref(&vRef[0]);    
                                if(res < 0){
                                    vRef[0] = 2;
                                    vRef[1] = 5;
                                    vRef[2] = 6;
                                }                             
                                printOLED("SV");              
                                for(i=0;i<3;i++){
                                    printOLED("VD%1d%1d",i,vRef[i]);  
                                }
                            }
                            else if(cursor == 1){       // -- Calibrate method 1         
                                currentPage = P_CAL1_SEL_REF_PRB;  
                                cursor      = 0;   
                                prb1        = 0;  
                                printOLED("S1%1d",prb1+1);                   
                            }
                            else if(cursor == 2){       // -- Calibrate method 2         
                                currentPage = P_CAL2_SEL_PRB;  
                                cursor      = 0;      
                                prb1        = 0;  
                                printOLED("S2%1d",prb1+1);                 
                            }
                            else if(cursor == 3){       // -- Reset User Password      
                                currentPage = P_RESET_CONFIRM;
                                cursor      = 0;     
                                printOLED("RC");                   
                            }
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }                   
    else if ( currentPage == P_SET_VREF )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MTNC_MENU;      cursor  = 0;            printOLED("M7%1d",cursor);              break;  
        case KEY_UP     :   if(++cursor > 3){   cursor  = 0;                }       printOLED("VA%1d",cursor);              break;
        case KEY_DOWN   :   if(--cursor < 0){   cursor  = 3;                }       printOLED("VA%1d",cursor);              break;
        case KEY_OK     :   if(cursor == 3){                                            
                                currentPage = P_FINISH;
                                tmpNextPage = P_MTNC_MENU;   
                                tmpPrevPage = P_SET_VREF;    
                                set_vref(&vRef[0]);
                                cursor = 0;                
                                printOLED("FI");  
                            }                
                            else if(cursor < 3){
                                if(++vRef[cursor] >9) vRef[cursor] = 0;
                                printOLED("VD%1d%1d",cursor, vRef[cursor]);  
                            }                                   
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }     
    else if ( currentPage == P_CAL1_SEL_REF_PRB )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MTNC_MENU;      cursor  = 1;    prb1 = 0;   printOLED("M7%1d",cursor);  break;     
        case KEY_UP     :   if(++cursor > 1){               cursor  = 0;    }           printOLED("SA%1d",cursor);  break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 1;    }           printOLED("SA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){  
                                if(++prb1 > 4){ prb1 = 0; }
                                printOLED("PN%1d",prb1+1); 
                            }   
                            else if(cursor == 1){                                           
                                currentPage = P_CAL1_SEL_CAL_PRB;
                                cursor      = 0;                    
                                prb2        = 0;
                                printOLED("S2%1d",prb2+1);     
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }       
    else if ( currentPage == P_CAL1_SEL_CAL_PRB )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_CAL1_SEL_REF_PRB;   cursor  = 0;    prb2 = 0;   printOLED("S1%1d",prb1+1);  break; 
        case KEY_UP     :   if(++cursor > 1){                   cursor  = 0;    }           printOLED("SA%1d",cursor);  break;
        case KEY_DOWN   :   if(--cursor < 0){                   cursor  = 1;    }           printOLED("SA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){                  
                                if(++prb2 > 4){ prb2 = 0; }
                                printOLED("PN%1d",prb2+1); 
                            }   
                            else if(cursor == 1){                                           
                                currentPage = P_CAL1_CONFIRM;
                                cursor      = 0;              
                                printOLED("CC%1d",prb2+1);        
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }         
//    else if ( currentPage == P_CAL1_CONFIRM )
//    {  
//        switch(key){
//        case KEY_MODE   :   currentPage = P_CAL1_SEL_CAL_PRB;   cursor  = 0;        printOLED("S2%1d",prb2+1);  break;
//        case KEY_UP     :   /* no operation */                                                                  break;
//        case KEY_DOWN   :   /* no operation */                                                                  break;
//        case KEY_OK     :   res = sensorCalibrate1(prb1,prb2);
//                            if(res < 0){
//                                currentPage = P_FAILED;                              
//                                tmpNextPage = P_CAL1_SEL_CAL_PRB;  
//                                tmpPrevPage = P_CAL1_CONFIRM;
//                                cursor      = 0;                
//                                printOLED("FL");  
//                            }
//                            else{
//                                currentPage = P_FINISH;                              
//                                tmpNextPage = P_CAL1_SEL_CAL_PRB;  
//                                tmpPrevPage = P_CAL1_CONFIRM;
//                                cursor      = 0;                
//                                printOLED("FI");  
//                            }      
//                            break;       
//        }
//    }     
    else if ( currentPage == P_CAL2_SEL_PRB )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MTNC_MENU;      cursor  = 2;            printOLED("M7%1d",cursor);  break;   
        case KEY_UP     :   if(++cursor > 1){               cursor  = 0;    }       printOLED("SA%1d",cursor);  break;
        case KEY_DOWN   :   if(--cursor < 0){               cursor  = 1;    }       printOLED("SA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 0){  
                                if(++prb1 > 4){ prb1 = 0; }
                                printOLED("PN%1d",prb1+1); 
                            }   
                            else if(cursor == 1){                                              
                                currentPage = P_CAL2_MENU;
                                cursor      = 0;                
                                printOLED("M8%1d",cursor);         
                            }                                      
                            else{                       // -- Exit -- should not occur      
                                currentPage = P_MONITOR;
                                cursor      = 0;     
                                printOLED("M0");       
                            }
            break;       
        }
    }      
//    else if ( currentPage == P_CAL2_MENU )
//    {  
//        switch(key){
//        case KEY_MODE   :   currentPage = P_CAL2_SEL_PRB;   cursor  = 0;        printOLED("S2%1d",prb1+1);  break;
//        case KEY_UP     :   if(--cursor < 0){               cursor  = 3;    }   printOLED("MA%1d",cursor);  break;
//        case KEY_DOWN   :   if(++cursor > 3){               cursor  = 0;    }   printOLED("MA%1d",cursor);  break;
//        case KEY_OK     :   if(cursor < 3){
//                                if(cursor == 0){        currentPage = P_CAL2_SPT1;  }
//                                else if(cursor == 1){   currentPage = P_CAL2_SPT2;  }  
//                                else if(cursor == 2){   currentPage = P_CAL2_SPT3;  }  
//                                    
//                                printOLED("CP%1d%1d",cursor+1,prb1+1);     
//                                    
//                                res = sensorGetCurrentSetPoint(prb1,CALPOINT(cursor+1),&calPnt[0]);   
//                                if(res<0){          memset(&calPnt[0],0x00,sizeof(calPnt));     }   
//                                for(i=0;i<6;i++){   printOLED("CD%1d%1d",i,calPnt[i]);          
//                                                    printDebug("calPnt[%1d] = %1d\r\n",i,calPnt[i]);         }  
//                                cursor      = 0;                  
//                            }
//                            else if(cursor == 3){       // -- Calculate                                       
//                                currentPage = P_CAL2_CONFIRM;
//                                cursor      = 0;                
//                                printOLED("CC%1d",prb1+1);                 
//                            }
//                            else{                       // -- Exit -- should not occur      
//                                currentPage = P_MONITOR;
//                                cursor      = 0;     
//                                printOLED("M0");       
//                            }
//            break;       
//        }
//    }                  
//    else if ((currentPage == P_CAL2_SPT1) || (currentPage == P_CAL2_SPT2) || (currentPage == P_CAL2_SPT3))
//    {  
//        switch(key){
//        case KEY_MODE   :   currentPage = P_CAL2_MENU;      
//                            if(currentPage == P_CAL2_SPT1){         cursor  = 0;    }            
//                            else if(currentPage == P_CAL2_SPT2){    cursor  = 1;    }           
//                            else if(currentPage == P_CAL2_SPT3){    cursor  = 2;    }
//                            printOLED("M8%1d",cursor);                                                                      break;  
//        case KEY_UP     :   if(++cursor > 6){                       cursor  = 0;    }       printOLED("CA%1d",cursor);      break;
//        case KEY_DOWN   :   if(--cursor < 0){                       cursor  = 6;    }       printOLED("CA%1d",cursor);      break;
//        case KEY_OK     :   if(cursor == 6){                                  
//                                   
//                                if(currentPage == P_CAL2_SPT1){  
//                                    res = sensorSetCurrentSetPoint(prb1,CALPOINT(1),&calPnt[0]);
//                                }
//                                else if(currentPage == P_CAL2_SPT2){
//                                    res = sensorSetCurrentSetPoint(prb1,CALPOINT(2),&calPnt[0]);
//                                }
//                                else if(currentPage == P_CAL2_SPT3){
//                                    res = sensorSetCurrentSetPoint(prb1,CALPOINT(3),&calPnt[0]);
//                                }
//                                else{           
//                                    // -- should not occur -- //
//                                    res = -1;
//                                }                                                  
//                                
//                                if(res < 0){
//                                    currentPage = P_FAILED;                              
//                                    tmpNextPage = P_CAL2_MENU;  
//                                    tmpPrevPage = currentPage;          
//                                    cursor      = 0;
//                                    printOLED("FL");  
//                                }                            
//                                else{                    
//                                    currentPage = P_FINISH;                        
//                                    tmpNextPage = P_CAL2_MENU;  
//                                    tmpPrevPage = currentPage;        
//                                    cursor      = 0; 
//                                    printOLED("FI");  
//                                } 
//                            }              
//                            else if(cursor == 0){   // toggle '+' & '-'
//                                if(++calPnt[cursor] > 1){ calPnt[cursor] = 0;   }                                
//                                printOLED("CD%1d%1d",cursor,calPnt[cursor]);
//                            }
//                            else if(cursor < 6){       
//                                if(++calPnt[cursor] > 9){ calPnt[cursor] = 0;   }                                
//                                printOLED("CD%1d%1d",cursor,calPnt[cursor]);         
//                            }                                      
//                            else{                       // -- Exit -- should not occur      
//                                currentPage = P_MONITOR;
//                                cursor      = 0;     
//                                printOLED("M0");       
//                            }
//            break;       
//        }
//    }    
//    else if ( currentPage == P_CAL2_CONFIRM )
//    {  
//        switch(key){
//        case KEY_MODE   :   currentPage = P_CAL2_MENU;      cursor  = 1;            printOLED("M8%1d",cursor);  break;
//        case KEY_UP     :   /* no operation */                                                                  break;
//        case KEY_DOWN   :   /* no operation */                                                                  break;
//        case KEY_OK     :   res = sensorCalibrate2(prb1);
//                            if(res < 0){
//                                currentPage = P_FAILED;                              
//                                tmpNextPage = P_CAL2_MENU;  
//                                tmpPrevPage = P_CAL2_CONFIRM;
//                                cursor      = 0;                
//                                printOLED("FL");  
//                            }
//                            else{
//                                currentPage = P_FINISH;                              
//                                tmpNextPage = P_CAL2_MENU;  
//                                tmpPrevPage = P_CAL2_CONFIRM;
//                                cursor      = 0;                
//                                printOLED("FI");  
//                            }      
//                            break;       
//        }
//    }    
    else if ( currentPage == P_RESET_CONFIRM )
    {  
        switch(key){
        case KEY_MODE   :   currentPage = P_MTNC_MENU;      cursor  = 3;            printOLED("M7%1d",cursor);  break;
        case KEY_UP     :   /* no operation */                                                                  break;
        case KEY_DOWN   :   /* no operation */                                                                  break;
        case KEY_OK     :   memset(newPwd,0x00,sizeof(newPwd));      
                            r2ecpy(uPwd,(unsigned char *) newPwd,sizeof(userPwd));
                            currentPage = P_FINISH;                              
                            tmpNextPage = P_MTNC_MENU;  
                            tmpPrevPage = P_RESET_CONFIRM;
                            cursor      = 0;                
                            printOLED("FI");        
                            break;       
        }
    }                
    else if ( currentPage == P_ENTER_U_PASS )   
    {           
        switch(key){
        case KEY_MODE   :   switch(tmpPrevPage){
                            case P_DEV_MENU     : currentPage = P_DEV_MENU;     cursor  = 1;    printOLED("M2%1d",cursor);  break;    
                            case P_PRB_MENU     : currentPage = P_PRB_MENU;     cursor  = 1;    printOLED("M3%1d",cursor);  break;    
                            case P_MANAGE_MENU  : currentPage = P_MANAGE_MENU;  cursor  = 1;    printOLED("M4%1d",cursor);  break;    
                            }                                                                                  
                            // --- clear entered password   
                            memset(entPwd,0xFF,sizeof(entPwd));                                                             break;
        case KEY_UP     :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(++cursor > 4){   cursor  = 0;                }   printOLED("PA%1d",cursor);                  break;
        case KEY_DOWN   :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(--cursor < 0){   cursor  = 4;                }   printOLED("PA%1d",cursor);                  break;
        case KEY_OK     :   if(cursor == 4){    
                    
                                // -- verifyPassword -- //
                                if(recmp((unsigned char *) entPwd,uPwd,sizeof(entPwd)) != 0){
                                    currentPage = P_WRONG_PASS;  cursor = 0;    printOLED("WP");     
                                    memset(entPwd,0xFF,sizeof(entPwd)); 
                                    break;
                                }       
                                            
                                // -- else, allow access -- //
                                switch(tmpNextPage){                                                                               
                                case P_DEV_S_MENU   : currentPage = P_DEV_S_MENU;   cursor  = 0;    printOLED("M6%1d",cursor);  break;
                                case P_PRB_S        : currentPage = P_PRB_S;        cursor  = 0;    printOLED("PS"); 
                                                      for(i=0;i<MAX_SENS_CHANNEL;i++){     
                                                        prbEn[i]  = operating_config.sensorEnable[i];
                                                        printOLED("ED%1d%1d",i,prbEn[i]); 
                                                      }            
                                                      break;
                                case P_PASS_NEW     : currentPage = P_PASS_NEW;     
                                                      memset(newPwd,0xFF,sizeof(newPwd));
                                                      cursor  = 0;    
                                                      printOLED("P4");            
                                                      break;
                                default             : // -- Exit -- should not occur      
                                                      currentPage = P_MONITOR;
                                                      cursor      = 0;     
                                                      printOLED("M0");               
                                                      break;
                                }
                            }                
                            else if(cursor < 4){
                                if(++entPwd[cursor] >9) entPwd[cursor] = 0;
                                printOLED("PD%1d%1d",cursor, entPwd[cursor]);  
                            }
            break;       
        }
    }
    else if ( currentPage == P_ENTER_M_PASS )   
    {           
        switch(key){
        case KEY_MODE   :   currentPage = P_MAIN_MENU;      cursor  = 3;            printOLED("M1%1d",cursor);  break;   
        case KEY_UP     :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(++cursor > 4){   cursor  = 0;                }       printOLED("PA%1d",cursor);  break;
        case KEY_DOWN   :   if(cursor < 4){     printOLED("PD%1d*",cursor); }
                            if(--cursor < 0){   cursor  = 4;                }       printOLED("PA%1d",cursor);  break;
        case KEY_OK     :   if(cursor == 4){    
                    
                                // -- verifyPassword -- //
                                if(recmp((unsigned char *) entPwd,sPwd,sizeof(entPwd)) != 0){
                                    tmpPrevPage = P_MAIN_MENU;
                                    currentPage = P_WRONG_PASS; cursor  = 0;    printOLED("WP");        
                                    memset(entPwd,0xFF,sizeof(entPwd));
                                    break;
                                }       
                                            
                                // -- else, allow access -- //
                                currentPage = P_MTNC_MENU;     cursor  = 0;     printOLED("M7%1d",cursor);               
                                memset(entPwd,0xFF,sizeof(entPwd)); 
                                break; 
                            }                
                            else if(cursor < 4){
                                if(++entPwd[cursor] >9) entPwd[cursor] = 0;
                                printOLED("PD%1d%1d",cursor, entPwd[cursor]);  
                            }
            break;       
        
         
                            
            break;       
        }
    }
    else if ( currentPage == P_WRONG_PASS )   
    {           
        switch(key){
        case KEY_MODE   :   /* no operation */                                                                              break;
        case KEY_UP     :   /* no operation */                                                                              break;
        case KEY_DOWN   :   /* no operation */                                                                              break;
        case KEY_OK     :   switch(tmpPrevPage){
                            case P_MAIN_MENU    : currentPage = P_MAIN_MENU;    cursor  = 3;    printOLED("M1%1d",cursor);  break;  
                            case P_DEV_MENU     : currentPage = P_DEV_MENU;     cursor  = 1;    printOLED("M2%1d",cursor);  break;    
                            case P_PRB_MENU     : currentPage = P_PRB_MENU;     cursor  = 1;    printOLED("M3%1d",cursor);  break;    
                            case P_MANAGE_MENU  : currentPage = P_MANAGE_MENU;  cursor  = 1;    printOLED("M4%1d",cursor);  break;    
                            } 
                            
                            
            break;       
        }
    }
    else if ((currentPage == P_FINISH) || (currentPage == P_FAILED) )   
    {           
        switch(key){
        case KEY_MODE   :   /* no operation */                                                                              break;
        case KEY_UP     :   /* no operation */                                                                              break;
        case KEY_DOWN   :   /* no operation */                                                                              break;
        case KEY_OK     :   switch(tmpNextPage){
                            case P_DEV_S_MENU       :   currentPage = P_DEV_S_MENU;   
                                                        if(       tmpPrevPage == P_DEV_S_OPER){       cursor  = 0;    }
                                                        else if(  tmpPrevPage == P_DEV_S_CONFIRM){    cursor  = 1;    }    
                                                        printOLED("M6%1d",cursor);                                                  break; 
                            case P_PRB_MENU         :   currentPage = P_PRB_MENU;     cursor = 1;     printOLED("M3%1d",cursor);    break; 
                            case P_MANAGE_MENU      :   currentPage = P_MANAGE_MENU;
                                                        if(       tmpPrevPage == P_MUTE_MENU){        cursor  = 0;    }
                                                        else if(  tmpPrevPage == P_PASS_CONFIRM){     cursor  = 1;    } 
                                                        printOLED("M4%1d",cursor);                                                  break; 
                            case P_MTNC_MENU        :   currentPage = P_MTNC_MENU;    
                                                        if(       tmpPrevPage == P_SET_VREF){         cursor  = 0;    }
                                                        else if(  tmpPrevPage == P_CAL2_MENU){        cursor  = 2;    } 
                                                        else if(  tmpPrevPage == P_RESET_CONFIRM){    cursor  = 3;    } 
                                                        printOLED("M7%1d",cursor);                                                  break;   
                            case P_CAL1_SEL_CAL_PRB :   currentPage = P_CAL1_SEL_CAL_PRB;
                                                        cursor      = 0;
                                                        printOLED("S2%1d",prb2+1);                                                  break;   
                            case P_CAL2_MENU        :   currentPage = P_CAL2_MENU;                                                  
                                                        if(       tmpPrevPage == P_CAL2_SPT1){        cursor  = 0;    } 
                                                        else if(  tmpPrevPage == P_CAL2_SPT2){        cursor  = 1;    } 
                                                        else if(  tmpPrevPage == P_CAL2_SPT3){        cursor  = 2;    } 
                                                        else if(  tmpPrevPage == P_CAL2_CONFIRM){     cursor  = 3;    }
                                                        printOLED("M8%1d",cursor);                                                  break;        
                            }
                            
                                               
            break;                                                                                
        }
    }
    
    return;
}
/*============================================================*/ 

/**************************************************************/
/*********************** Service Function *********************/
/**************************************************************/
/*============================================================*/
void showProbeConfig(int id)
{                                              
    int         res         = 0;              
    int         sampling    = 0; 
    signed char val[4]      = {0,0,0,0};          
    char        str1[8]     = {0,0,0,0,0,0,0,0}; 
    char        str2[8]     = {0,0,0,0,0,0,0,0};  
    
                                
    res = get_current_sampling(sensor_config[id].samplingRate,&val[0]);
                                
    if(res < 0){
        val[0] = 0;
        val[1] = 0;
        val[2] = 0;
        val[3] = 0;
    }
                                                                                
    sampling = 0;
    sampling += val[0] * 100;
    sampling += val[1] * 10;
    sampling += val[2];                
                                
    memset(str1,'\0',sizeof(str1));
    if( operating_config.sensorEnable[id]==0x01 ){  sprintf(str1,"[E]");        }    
    else{                                           sprintf(str1,"[D]");        } 
    
    memset(str2,'\0',sizeof(str2));
    if( sensor_config[id].alarmEnable == 0x01   ){  sprintf(str2,"Enable");     }    
    else{                                           sprintf(str2,"Disable");    }  
    
                                
    printDebug("<showProbeConfig> PC%1d%1d%d,%s,%s,%.1f,%d,%.1f,%.1f,%d,%.1f",
                id,val[3],sampling,         
                str1,str2,
                sensor_config[id].lowerLimit,
                sensor_config[id].lowerDelay,
                sensor_config[id].lowerExtreme,
                sensor_config[id].upperLimit,
                sensor_config[id].upperDelay,
                sensor_config[id].upperExtreme); 
                                
    printOLED("PC%1d%1d%d,%s,%s,%.1f,%d,%.1f,%.1f,%d,%.1f",
                id,val[3],sampling,         
                str1,str2,
                sensor_config[id].lowerLimit,
                sensor_config[id].lowerDelay,
                sensor_config[id].lowerExtreme,
                sensor_config[id].upperLimit,
                sensor_config[id].upperDelay,
                sensor_config[id].upperExtreme); 

}
/*============================================================*/
int get_current_sampling(unsigned int period, signed char *val)
{
    unsigned int min, min_c;
    unsigned int hr, hr_c;
                                    
    printDebug("\r\n<get_current_sampling> real value (%l sec.)\r\n");
    // -- display as hour unit -- //
    hr   = period / 3600;
    hr_c = period % 3600;
    if((hr > 0) && (hr < 1000) && (hr_c == 0)){
        printDebug("<get_current_sampling> %d hr.\r\n",hr);
        val[0] = hr / 100;
        val[1] = (hr % 100) / 10;
        val[2] = ((hr % 100) % 10);              
        val[3] = 2;
        return 0;
    }
                                    
    // -- display as minute unit -- //
    min   = period / 60;
    min_c = period % 60;
    if((min > 0) && (min < 1000)  && (min_c == 0)){
        printDebug("<get_current_sampling> %d min.\r\n",min);
        val[0] = min / 100;
        val[1] = (min % 100) / 10;
        val[2] = ((min % 100) % 10);              
        val[3] = 1;
        return 0;
    }            
                           
    // -- display as second unit -- //
    if(period < 1000){           // show only 0 - 999
        printDebug("<get_current_sampling> %d sec.\r\n",period);
        val[0] = period / 100;
        val[1] = (period % 100) / 10;
        val[2] = ((period % 100) % 10);              
        val[3] = 0;
        return 0;
    }                                                       
    
    if(min < 1000){                                      
        printDebug("<get_current_sampling> %d min. %d sec. ",min,min_c);
        if(min_c > 30){
            min++;
        }                                                           
        printDebug("--> %d min.\r\n",min);       
        val[0] = min / 100;
        val[1] = (min % 100) / 10;
        val[2] = ((min % 100) % 10);              
        val[3] = 1;
        return 0;
    }                
    
    if(hr < 1000){                                     
        min   = hr_c / 60;
        min_c = hr_c % 60;
        printDebug("<get_current_sampling> %d hr. %d min. %d sec. ",hr,min,min_c);
        if(min > 30){
            hr++;
        }                                                           
        printDebug("--> %d hr.\r\n",hr);
        val[0] = hr / 100;
        val[1] = (hr % 100) / 10;
        val[2] = ((hr % 100) % 10);              
        val[3] = 2;
        return 0;
    }            
    
    printDebug("<get_current_sampling> Can't Show.\r\n");
    return -1;
    
}
/*============================================================*/
int get_current_vref(char *buf)
{
    char    valTmp[10];
    float   vref;     
    int     i;
    int tmp;     
    int d2;
    int d1;
    int d0;
    
    if(eep_vref_val > 10.0){            
        return -1;
    }                 
                 
    vref = eep_vref_val;
    ftoa(vref,2,valTmp);
    
    if(strlen(valTmp) != 4){ return -1; }   
    
    /*     
    tmp = (int)(eep_vref_val * 100);  
    
    d2  = tmp / 100;   
    d1  = (tmp % 100) / 10;
    d0  = tmp % 10;       
    
    printDebug("<get_current_vref> vref(%f) tmp = %d -> [%d][%d][%d]\r\n",eep_vref_val,tmp,d2,d1,d0); 
    buf[0]  = d2;
    buf[1]  = d1;
    buf[2]  = d0;         
    */                                                                                         
    printDebug("<get_current_vref> vref(%f) valTmp = %s\r\n",eep_vref_val,valTmp);    
    buf[0]  = valTmp[0]-0x30;                                                             
    buf[1]  = valTmp[2]-0x30;
    buf[2]  = valTmp[3]-0x30;                                                         
                   
    printDebug("<get_current_vref> buf -> ");
    for(i=0;i<3;i++){
        printDebug("[%c]",buf[i]);
    }                                     
    printDebug("\r\n");
    
    return 0;
}
/*============================================================*/
int getNetworkInfo(char *msg, char *ip, char *mask, char *gw, char *server)
{
    int i,offset;          
    //char msg[] = "0008DC135E2401C0A80B03FFFFFF00C0A80B011388C0A80B011388FE08000100000000000000000003010000000000000000002020202020202020202020200202020202020202020202020202020202020012B2B2B0020202020202020202020202020202020202020202020202020202020202020002020202020202020020202020202020202020202020202020202020202020000000000000000000FF\r\n";
    char dIp[4];
    char dMask[4];
    char dGw[4];
    char dServer[4];
                                                   
    offset = 7;
    for(i=offset;i<(offset+4);i++){
        dIp[i-offset]                   = (h2i(msg[i*2]) * 16) + h2i(msg[(i*2) + 1]); 
        ip[(i-offset) * 3]              = (dIp[i-offset]/100);                                             
        ip[((i-offset) * 3) + 1]        = (dIp[i-offset]%100) / 10;                          
        ip[((i-offset) * 3) + 2]        = (dIp[i-offset]%10);                          
    }                        
                                                     
    offset = 11;
    for(i=offset;i<(offset+4);i++){
        dMask[i-offset]                 = (h2i(msg[i*2]) * 16) + h2i(msg[(i*2) + 1]);    
        mask[(i-offset) * 3]            = (dMask[i-offset]/100);                                             
        mask[((i-offset) * 3) + 1]      = (dMask[i-offset]%100) / 10;                          
        mask[((i-offset) * 3) + 2]      = (dMask[i-offset]%10);            
    }                            
                                                     
    offset = 15;
    for(i=offset;i<(offset+4);i++){
        dGw[i-offset]                   = (h2i(msg[i*2]) * 16) + h2i(msg[(i*2) + 1]);    
        gw[(i-offset) * 3]              = (dGw[i-offset]/100);                                             
        gw[((i-offset) * 3) + 1]        = (dGw[i-offset]%100) / 10;                          
        gw[((i-offset) * 3) + 2]        = (dGw[i-offset]%10);            
    }                               
                                                     
    offset = 21;
    for(i=offset;i<(offset+4);i++){
        dServer[i-offset]               = (h2i(msg[i*2]) * 16) + h2i(msg[(i*2) + 1]);    
        server[(i-offset) * 3]          = (dServer[i-offset]/100);                                             
        server[((i-offset) * 3) + 1]    = (dServer[i-offset]%100) / 10;                          
        server[((i-offset) * 3) + 2]    = (dServer[i-offset]%10);            
    }                                                         
    
    printDebug("<getNetworkInfo> LIP : %u.%u.%u.%u\r\n",dIp[0],dIp[1],dIp[2],dIp[3]);  
    printDebug("<getNetworkInfo> MSK : %u.%u.%u.%u\r\n",dMask[0],dMask[1],dMask[2],dMask[3]);
    printDebug("<getNetworkInfo> DGW : %u.%u.%u.%u\r\n",dGw[0],dGw[1],dGw[2],dGw[3]);
    printDebug("<getNetworkInfo> SIP : %u.%u.%u.%u\r\n",dServer[0],dServer[1],dServer[2],dServer[3]); 
    
    return 0;
}
/*============================================================*/
/*
int setNetworkInfo(char *ip, char *mask, char *gw, char *server)
{
    char    *msg;  
    int     msg_len;      
    int     i=0,res=0;
    
    // -- read -- <S0008DC18BDDC01C0A801FCFFFFFF00C0A80101232EC0A80101271AF4080001000000000005000001040100010000000000000020202020202020202020202020202020202020202020202020202020202020002B2B2B00202020202020202020202020202020202020202020202020202020202020200020202020202020202020202020202020202020202020202020202020202020000000000000000000FF -- //
    // -- write - >W01[IP(4)][MSK(4)][GW(4)]
    msg = (unsigned char *) malloc( sizeof(char) * CONF_READ_BUFFER );
    if(msg == NULL){       
        printDebug("<setNetworkInfo> malloc(CONF_READ_BUFFER) failed\r\n");
        return -1;     
    }                                             
                                
    res = ETH_getConfig(msg,&msg_len);               
    if(res < 0){                             
        printDebug("<setNetworkInfo> ETH_getConfig() failed\r\n");
    }                                    
    else{
        printDebug("<setNetworkInfo> ETH_getConfig() success\r\n");             
        printDebug("\r\n\r\n<setNetworkInfo> read[%d] -->\r\n",msg_len);
        for(i=0;i<msg_len;i++){
            putchar3(msg[i]);
        }                      
        printDebug("\r\n");
    }   
    
    printDebug("<setNetworkInfo> Network Configuring\r\n");
    printDebug("<setNetworkInfo> LIP       : ");                  
    printDebug("%u.%u.%u.%u  -- %02X,%02X,%02X,%02X\r\n",
                (ip[0]    *100) + (ip[1]     *10) + ip[2],
                (ip[3]    *100) + (ip[4]     *10) + ip[5],
                (ip[6]    *100) + (ip[7]     *10) + ip[8],
                (ip[9]    *100) + (ip[10]    *10) + ip[11],  
                (ip[0]    *100) + (ip[1]     *10) + ip[2],
                (ip[3]    *100) + (ip[4]     *10) + ip[5],
                (ip[6]    *100) + (ip[7]     *10) + ip[8],
                (ip[9]    *100) + (ip[10]    *10) + ip[11]);     
                
    printDebug("<setNetworkInfo> MASK      : ");      
    printDebug("%u.%u.%u.%u  -- %02X,%02X,%02X,%02X\r\n",
                (mask[0]  *100) + (mask[1]   *10) + mask[2],
                (mask[3]  *100) + (mask[4]   *10) + mask[5],
                (mask[6]  *100) + (mask[7]   *10) + mask[8],
                (mask[9]  *100) + (mask[10]  *10) + mask[11],  
                (mask[0]  *100) + (mask[1]   *10) + mask[2],
                (mask[3]  *100) + (mask[4]   *10) + mask[5],
                (mask[6]  *100) + (mask[7]   *10) + mask[8],
                (mask[9]  *100) + (mask[10]  *10) + mask[11]);  
    printDebug("<setNetworkInfo> GATEWAY   : ");     
    printDebug("%u.%u.%u.%u  -- %02X,%02X,%02X,%02X\r\n",
                (gw[0]    *100) + (gw[1]     *10) + gw[2],
                (gw[3]    *100) + (gw[4]     *10) + gw[5],
                (gw[6]    *100) + (gw[7]     *10) + gw[8],
                (gw[9]    *100) + (gw[10]    *10) + gw[11],     
                (gw[0]    *100) + (gw[1]     *10) + gw[2],
                (gw[3]    *100) + (gw[4]     *10) + gw[5],
                (gw[6]    *100) + (gw[7]     *10) + gw[8],
                (gw[9]    *100) + (gw[10]    *10) + gw[11]); 
    printDebug("<setNetworkInfo> SIP       : ");       
    printDebug("%u.%u.%u.%u  -- %02X,%02X,%02X,%02X\r\n",
                (server[0]*100) + (server[1] *10) + server[2],
                (server[3]*100) + (server[4] *10) + server[5],
                (server[6]*100) + (server[7] *10) + server[8],
                (server[9]*100) + (server[10]*10) + server[11],   
                (server[0]*100) + (server[1] *10) + server[2],
                (server[3]*100) + (server[4] *10) + server[5],
                (server[6]*100) + (server[7] *10) + server[8],
                (server[9]*100) + (server[10]*10) + server[11]);    
                                                                     
    
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
                
    return 0;
}
*/
/*============================================================*/
int set_vref(char *buf)
{
    int     tmp;                            
    float   tmpf;
                 
    tmp = (buf[0] * 100) + (buf[1] * 10) + buf[2]; 
    tmpf = (float) (tmp / 100.0);
    
    printDebug("<set_vref> [%d][%d][%d] -> %d -> %f\r\n",buf[0],buf[1],buf[2],tmp,tmpf);               
           
    eep_vref_val = tmpf;
    
    return 0;
}
/*============================================================*/ 

/**************************************************************/
/*********************** Support Function *********************/
/**************************************************************/
/*============================================================*/
int h2i(char hex)
{
    if      ((hex >= 'A') && (hex <= 'F')){ return hex - 0x37;  }
    else if ((hex >= 'a') && (hex <= 'f')){ return hex - 0x57;  }
    else if ((hex >= '0') && (hex <= '9')){ return hex - 0x30;  }   
    
    return 0;
}
/*============================================================*/
int e2rcpy(char *dest, char eeprom *src, int len)
{
    int i;                         
    
    for(i=0;i<len;i++){
        dest[i] = src[i];
    }
    return 0;
}
/*============================================================*/    
int r2ecpy(char eeprom *dest, char *src, int len)
{
    int i;
    for(i=0;i<len;i++){
        dest[i] = src[i];
    }
    return 0;
}
/*============================================================*/ 
int recmp(char *src1, char eeprom *src2, int len)
{
    int i;        
    
    for(i=0;i<len;i++){                 
        printDebug("<recmp> src1[%02X] src2[%02X] user[%02X] service[%02X]\r\n",src1[i],src2[i],userPwd[i],servicePwd[i]);
    }                             
    
    for(i=0;i<len;i++){
        if(src1[i] != src2[i]){
            return -1;
        }
    }
    return 0;
}
/*============================================================*/

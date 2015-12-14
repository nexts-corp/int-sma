#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <delay.h>
#include <ctype.h>
#include "debug.h"
#include "adc.h"
#include "ds1672.h"
#include "alarm.h"
#include "log.h"
#include "io.h"
#include "ethcomm.h"
#include "calibration.h"
#include "sensor.h"
//sensorGetValue(u int)
extern ST_CAL_CONFIG sensorCalConfig;
extern eeprom  ST_CAL sensorCalulate;          //improtance    extern from  calibration.h
extern void (*pfCalibrate) (ST_CAL_CONFIG calConfig);   //pointer function of calibation set  extern from  calibration.h

flash unsigned char cmdSetRTCDateTime[]     = "$CMDSDT";        // -- Set Date/Time -- //
flash unsigned char cmdResetAlarmStatus[]   = "$CMDRAS";        // -- Re-initiate all alarm flag -- //  
flash unsigned char cmdClearLogFile[]       = "$CMDCLF";        // -- Re-initiate log file -- //                              
flash unsigned char cmdSetVref[]            = "$CMDSVR";        // -- Set Vref measure from multimeter -- //                  
flash unsigned char cmdSensorSelect[]       = "$CMDSNS";        // -- Select Sensor for Hardware Calibration [Disable mode require] -- //
flash unsigned char cmdDeviceEnable[]       = "$CMDDEN";        // -- Enable Device -- //
flash unsigned char cmdDeviceDisable[]      = "$CMDDDS";        // -- Disable Device -- //
flash unsigned char cmdSetSerialNumber[]    = "$CMDSSN";        // -- Set Serial Number -- //
flash unsigned char cmdReadSerialNumber[]   = "$CMDRSN";        // -- Read Serial Number -- //

flash unsigned char cmdGetCal[]             = "$CMDGCL";         // -- Get calibrate value -- //      $CMDRCL,1#        ,chID(1)
flash unsigned char cmdSetCal[]             = "$CMDSCL";         // -- Set calibrate value -- //      $CMDSCL,1,1,29.20     ,chID(1),pointID(1),std1Value(29.20)#
flash unsigned char cmdConfirmCal[]         = "$CMDCCL";         // -- Confirm calibrate type -- //   ,chID(1),calType(1) 0:defualt-1:offset-2:linear-3:polynomail
flash unsigned char cmdGetCurSensor[]       = "$CMDGCS";         // -- Get current sensor value -- //   ,chID(1)
/*============================================================*/

unsigned int hwCalId    = 9;
                                                                
/**************************************************************/
/************* Service Debug Command Function *****************/
/**************************************************************/
/*============================================================*/
int debugGetCurSensor(){
    unsigned char i=0;
    unsigned char buffer[2];  
    unsigned int channelID;
    
    printDebug("\r\n<Debug>GetCurSensor: \r\n");
    memset(buffer,0,sizeof(buffer));  
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+8];
    }
    channelID = atoi(buffer);
    printDebug("Channel ID:%d \r\n",channelID);
    printDebug("\r\n"); 
    
    if( channelID>0 && channelID<6 ){           //max 5 ch
       printDebug("current sensor ch[%d] , value:%0.2f\r\n",channelID,sensorGetValue(channelID));
    }else{
        printDebug("Channel ID is in range.\r\n");
    }
    
    return 0;
}
int debugGetCal(){
    unsigned char i=0;
    unsigned char buffer[4];  
    unsigned int channelID;
    
    printDebug("\r\n<Debug>GetCal: \r\n");
    memset(buffer,0,sizeof(buffer));  
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+8];
    }
    channelID = atoi(buffer);
    printDebug("Channel ID:%d \r\n",channelID);
    printDebug("\r\n"); 
    
    if( channelID>0 && channelID<6 ){           //max 5 ch
       for(i=0;i<3;i++){
           printDebug("stdRef[%d] : %0.2f , ",i+1,sensorCalulate.sChennel[channelID-1].stdRef[i]); 
           printDebug("sensorRef[%d] : %0.2f\r\n",i+1,sensorCalulate.sChennel[channelID-1].sensorRef[i]);
       } 
       switch(sensorCalulate.sChennel[channelID-1].calType){
            case CAL_DEFAULT:{               //0
                printDebug("CalType : CAL_DEFAULT");
                break;
            } 
            case CAL_EXTOFFSET:{             //1
                printDebug("CalType : CAL_EXTOFFSET");
                break;
            }
            case CAL_EXTLINEAR:{             //2
                printDebug("CalType : CAL_EXTLINEAR");
                break;
            }
            case CAL_EXTPOLYNOMIAL:{         //3
                printDebug("CalType : CAL_EXTPOLYNOMIAL");
                break;
            }
       }
       printDebug("\r\n");
    }else{
        printDebug("Channel ID is in range.\r\n");
    }
    
    return 0;
}
int debugSetCal(){
    unsigned char i=0;
    unsigned char buffer[6];  
    unsigned int channelID;
    unsigned int pointID;
    float pointValue;         //std value
    
    //void (*pfnCalibrate) (ST_CAL_CONFIG calConfig);   
    
    pfCalibrate = &calSet;
    
    printDebug("\r\n<Debug>SetCal: \r\n");
    memset(buffer,0,sizeof(buffer));  
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+8];
    }
    channelID = atoi(buffer);
    sensorCalConfig.channelID =  channelID;
    printDebug("Channel ID:%d \r\n",channelID);  
    
    memset(buffer,0,sizeof(buffer)); 
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+10];
    }
    pointID = atoi(buffer);
    sensorCalConfig.pointID =  pointID;
    printDebug("Point ID:%d \r\n",pointID); 
    
    memset(buffer,0,sizeof(buffer)); 
    for(i=0;i<5;i++){
       buffer[i] = rxPackage3[i+12];
    }
    pointValue = atof(buffer);
    sensorCalConfig.pointValue =  pointValue;     //std value for meter
    printDebug("pointValue(meterRef) :%0.2f \r\n",pointValue);
    printDebug("\r\n"); 
    
    pfCalibrate(sensorCalConfig);
    
    return 0;
}
int debugConfirmCal(){
    unsigned char i=0;
    unsigned char buffer[6];
    unsigned int channelID;
    unsigned int calType; 
    
    pfCalibrate = &calSetConfirm;
    
    printDebug("\r\n<Debug>ConfirmCal: \r\n");
    memset(buffer,0,sizeof(buffer));  
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+8];
    }
    channelID = atoi(buffer);
    sensorCalConfig.channelID =  channelID;
    printDebug("Channel ID:%d \r\n",channelID);  
    
    memset(buffer,0,sizeof(buffer)); 
    for(i=0;i<1;i++){
       buffer[i] = rxPackage3[i+10];
    }
    calType = atoi(buffer);
    sensorCalConfig.calType =  calType;
    //printDebug("calType :%d \r\n",calType);
    switch(sensorCalConfig.calType){
        case CAL_DEFAULT:{               //0
            printDebug("CalType : CAL_DEFAULT");
            break;
        } 
        case CAL_EXTOFFSET:{             //2
            printDebug("CalType : CAL_EXTOFFSET");
            break;
        }
        case CAL_EXTLINEAR:{             //3
            printDebug("CalType : CAL_EXTLINEAR");
            break;
        }
        case CAL_EXTPOLYNOMIAL:{         //4
            printDebug("CalType : CAL_EXTPOLYNOMIAL");
            break;
        }
    }
    printDebug("\r\n");
    
    pfCalibrate(sensorCalConfig);
    
    return 0;
}

int DEBUG_setDateTime(void)
{
    unsigned char   i; 
    unsigned char   buffer[32];                  
    int             tz;      
    
    struct rtc_structure rtcSet;        
        
    for(i=0;i<4;i++)
    {
        buffer[i] = rxPackage3[i+8];
    }
    buffer[4] = 0;
    
    rtcSet.year = atol(buffer);

    printDebug("get year = %d\r\n",rtcSet.year);

    for(i=0;i<2;i++)
    {
        buffer[i] = rxPackage3[i+13];
    }
    buffer[2] = 0;

    rtcSet.month = atol(buffer);

    printDebug("get month = %d\r\n",rtcSet.month);

    for(i=0;i<2;i++)
    {
        buffer[i] = rxPackage3[i+16];
    }
    buffer[2] = 0;

    rtcSet.date = atol(buffer);

    printDebug("get date = %d\r\n",rtcSet.date);

    for(i=0;i<2;i++)
    {
        buffer[i] = rxPackage3[i+19];
    }
    buffer[2] = 0;

    rtcSet.hour = atol(buffer);

    printDebug("get hour = %d\r\n",rtcSet.hour);

    for(i=0;i<2;i++)
    {
        buffer[i] = rxPackage3[i+22];
    }
    buffer[2] = 0;

    rtcSet.min = atol(buffer);

    printDebug("get min = %d\r\n",rtcSet.min);

    for(i=0;i<2;i++)
    {
        buffer[i] = rxPackage3[i+25];
    }
    buffer[2] = 0;

    rtcSet.sec = atol(buffer);

    printDebug("get sec = %d\r\n",rtcSet.sec);    
    
    for(i=0;i<3;i++)
    {
        buffer[i] = rxPackage3[i+28];
    }
    buffer[4] = 0;

    tz = atol(buffer);

    printDebug("get timezone = %+d\r\n",tz);  
                                                     
    RTC_setDateTime(rtcSet,tz);
    
    return 0;
}
/*============================================================*/
void DEBUG_resetAlarm(void)
{
//    resetAlarm();                                                            
    
    printDebug("\r\n<DEBUG> Alarm Status Reset..!!\r\n");
}
/*============================================================*/
void DEBUG_ClearLogFile(void)
{   
    int         res;  
    
    printDebug("<DEBUG_ClearLogFile> Re-initiate log file\r\n");    
                                                       
    res = LOG_createFile(LOGFILE_DATA,SD_FORCE,LOG_INIT_STR,SIZE_RECORD);       
    if(res < 0)
    {
        printDebug("<DEBUG_ClearLogFile> LOG_createFile(%s) failed\r\n",LOGFILE_DATA);   
    }
                                                    
    res = LOG_createFile(LOGFILE_EVENT,SD_FORCE,LOG_INIT_STR,SIZE_RECORD);       
    if(res < 0)
    {
        printDebug("<DEBUG_ClearLogFile> LOG_createFile(%s) failed\r\n",LOGFILE_EVENT);  
    }
    
    res = LOG_createFile(LOGFILE_ERROR,1,NULL,0);       
    if(res < 0)
    {
        printDebug("<DEBUG_ClearLogFile> LOG_createFile(%s) failed\r\n",LOGFILE_ERROR);  
    }         
            
    return;
}
/*============================================================*/
void DEBUG_setRealVref(void)
{
    unsigned char i;
    unsigned char rxStrSetVREF[6];
                    
    for(i=0;i<=5;i++)
    {
        rxStrSetVREF[i] = rxPackage3[i+8];  
    }
    rxStrSetVREF[5] = '\0';
    printDebug("<DEBUG_setRealVref> get set Vref<string> %s\r\n",rxStrSetVREF);
    eep_vref_val = (atof(rxStrSetVREF));
    printDebug("<DEBUG_setRealVref> After convert = %f\r\n",eep_vref_val); 
}
/*============================================================*/
void DEBUG_sensorSelect(void)
{
    unsigned char ch[2];          
    
    if(TEMP_SEL != TEMP_TYPE_K){
        printDebug("<DEBUG_sensorSelect> This TEMP_SEL not support\r\n\r\n");
        return;
    }
                      
    if(rxPackage3[8] == 'N'){
        hwCalId     = 9;            
        printDebug("<DEBUG_sensorSelect> Deselect sensor\r\n\r\n");
    }                 
    else if(isdigit(rxPackage3[8])){        
        ch[0]       = rxPackage3[8];  
        ch[1]       = '\0';           
        hwCalId     = atoi(ch);                                             

        SENSOR_SELECT(hwCalId);
        delay_ms(50);           // -- for multiplex stable -- //   
        
        printDebug("<DEBUG_sensorSelect> Selected sensor %d\r\n\r\n",hwCalId);
    }
    else{
        hwCalId     = 9;                                                    
        printDebug("<DEBUG_sensorSelect> ERROR : Selecting sensor [%c][0x%02X]\r\n\r\n",rxPackage3[8],rxPackage3[8]);
    }
                                                    
    return;
}
/*============================================================*/     
void DEBUG_setDeviceEnable(char flag)
{
//    if(flag){
//        config_oper.deviceEnable        =   0x01;
//    }
//    else{
//        config_oper.deviceEnable        =   0x00;
//    }                                         
//                                               
//    config_oper_flag                    =   0x01;
//    is_operating_config_update          =   1;
//                                                    
//    return;
}
/*============================================================*/
void DEBUG_setSerialNumber(void)
{   
    // -- assume input is "$CMDSSN,01,02,03,04,05,06,07,08#" -- // 
    unsigned char i;                                                                         
    unsigned char set_str[24];
                            
    for(i=0;i<24;i++){              
        if( ( rxPackage3[i+7] != ',') && !isxdigit(rxPackage3[i+7])){ 
            printDebug("<DEBUG_setSerialNumber> invalid setting string (%c)[0x%02]\r\n",rxPackage3[i+7],rxPackage3[i+7]);
            return;
        }
        set_str[i] = tolower(rxPackage3[i+7]);           // -- copy only ",01,02,03,04,05,06,07,08" -- //
    }                                                                                             
                                                                   
    printDebug("<DEBUG_setSerialNumber> Replacing DEVICE_SERIAL..\r\n");    
    
    printDebug("<DEBUG_setSerialNumber> Set String --> ");
    for(i=0;i<24;i++){
        if((i%3) == 0)      {   continue;                       }
        else if((i%3) == 1) {                          

            printDebug("[%c",set_str[i]);      
            
            DEVICE_SERIAL[i/3]      = 0x00;
             
            if(isdigit(set_str[i])){
                DEVICE_SERIAL[i/3]  = (set_str[i] - 48) << 4;
            }
            else{
                DEVICE_SERIAL[i/3]  = (set_str[i] - 87) << 4;
            }
        }                          
        else if((i%3) == 2) {             

            printDebug("%c]",set_str[i]);    
            
            if(isdigit(set_str[i])){
                DEVICE_SERIAL[i/3]  |= (set_str[i] - 48);
            }
            else{
                DEVICE_SERIAL[i/3]  |= (set_str[i] - 87);
            }
        }        
    }
    
    printDebug("<DEBUG_setSerialNumber> Success!! DEV_SERIAL ==> ");
    for(i=0;i<8;i++){
        printDebug("[%02X]",DEVICE_SERIAL[i]);   
    }                                        
    printDebug("\r\n");
                                         
    return;
}
/*============================================================*/
void DEBUG_readSerialNumber(void)
{                                               
    unsigned char i;                                                                         

    printDebug("<DEBUG_readSerialNumber> DEV_SERIAL ==> ");
    for(i=0;i<8;i++){
        printDebug("[%02X]",DEVICE_SERIAL[i]);   
    }                                        
    printDebug("\r\n");
    
    return;
}
/*============================================================*/
                                                                
/**************************************************************/
/****************** Debug Support Function ********************/
/**************************************************************/
/*============================================================*/
void debug_req(void)
{
    flag_PackageTerminate3 = 0;

    printDebug("Get Debug Req.:");    
                                  
    
    if(!strncmpf(rxPackage3,cmdSetRTCDateTime,7)){
        DEBUG_setDateTime();
    } 
    else if(!strncmpf(rxPackage3,cmdResetAlarmStatus,7)){
        DEBUG_resetAlarm();
    }
    else if(!strncmpf(rxPackage3,cmdClearLogFile,7)){
        DEBUG_ClearLogFile();
    }
    else if(!strncmpf(rxPackage3,cmdSetVref,7)){
        DEBUG_setRealVref();
    }
    else if(!strncmpf(rxPackage3,cmdSensorSelect,7)){
        DEBUG_sensorSelect();
    }
    else if(!strncmpf(rxPackage3,cmdDeviceEnable,7)){
        DEBUG_setDeviceEnable(1);
    }
    else if(!strncmpf(rxPackage3,cmdDeviceDisable,7)){
        DEBUG_setDeviceEnable(0);
    }
    else if(!strncmpf(rxPackage3,cmdSetSerialNumber,7)){
        DEBUG_setSerialNumber();
    }
    else if(!strncmpf(rxPackage3,cmdReadSerialNumber,7)){
        DEBUG_readSerialNumber();
    }else if(!strncmpf(rxPackage3,cmdGetCal,7)){
        debugGetCal();
    }else if(!strncmpf(rxPackage3,cmdSetCal,7)){
        debugSetCal();
    }else if(!strncmpf(rxPackage3,cmdConfirmCal,7)){
        debugConfirmCal();
    }else if(!strncmpf(rxPackage3,cmdGetCurSensor,7)){
        debugGetCurSensor();
    }
    return;         
}
/*============================================================*/
void printDebug(flash char *fmtstr, ...)
{
    char textBuffer[256];
    va_list argptr; 
    
    va_start(argptr, fmtstr);
    vsprintf(textBuffer,fmtstr,argptr);     
    send_uart(3,textBuffer);   
    va_end(argptr);        
    
    return;        
}
/*============================================================*/
void print_hex_ascii_line(const unsigned char *payload, int len, int offset)
{

    int i;
    int gap;
    const unsigned char *ch;

    // offset //                      
    printDebug("%05d   ", offset);               
    
    
    // hex //                                                                                                      
    ch = payload;
    for(i = 0; i < len; i++) {                            
        printDebug("%02x ", *ch);                    
        
        ch++;
        // print extra space after 8th byte for visual aid //
        if (i == 7){                            
            printDebug(" ");                                     
            
        }
    }
    // print space to handle line less than 8 bytes //
    if (len < 8){                            
        printDebug(" ");                                              
        
    }
    
    // fill hex gap with spaces if not full line //
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printDebug("   ");                                           
            
        }
    }
    printDebug("   ");                                                      
    
    
    // ascii (if printable) //
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch)){
            printDebug("%c", *ch);                                           
            
        }
        else{
            printDebug(".");                                                 
            
        }
        ch++;
    }

    printDebug("\r\n");                                                        
    

return;
}
/*============================================================*/
void print_payload(const unsigned char *payload, int len)
{

    int len_rem = len;
    int line_width = 16;            // number of bytes per line //
    int line_len;
    int offset = 0;                    // zero-based offset counter //
    const unsigned char *ch = payload;

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

    return;
}            
/*============================================================*/
void debugConfWiz100sr(){

}                
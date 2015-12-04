#include <stdlib.h>
#include <string.h>
#include "ethservice.h"
#include "ds1672.h"
#include "alarm.h"
#include "debug.h"

/*============================================================*/
int getConfigLen(unsigned char cnfg)
{
    unsigned char   type = cnfg & 0xF0;;       
         
    switch( type ){
        case CONF_PROPERTIES    :   return sizeof(struct prop_cnfg_st);    
        case CONF_OPERATE       :   return sizeof(struct oper_cnfg_st);
        case CONF_SENSOR        :   return sizeof(struct sens_cnfg_st);
        case CONF_AUXDI         :   return sizeof(struct din_cnfg_st);
        case CONF_AUXDO         :   return sizeof(struct dout_cnfg_st);
        case CONF_AUXAI         :   return sizeof(struct ain_cnfg_st);
        case CONF_AUXAO         :   return sizeof(struct aout_cnfg_st);   
        case CONF_BUZZER        :   return sizeof(struct buzz_cnfg_st);
        case CONF_ENVALARM      :   return sizeof(struct alm_cnfg_st);
        default                 :   printDebug("<getConfigLen> unknown configuration type [0x%02X]\r\n",type);
                                    return -1;        
    }                    
}
/*============================================================*/
int configRead(unsigned char cnfg, unsigned char *ptr)
{   
    int i = 0;
    int res = 0;
    unsigned char   type            = 0x00;
    int             channel         = 0;               
    void            *cnfgStrc       = NULL;
    
    type    =   cnfg & 0xF0;            
      
    
    switch( type ){
        case CONF_PROPERTIES    :   cnfgStrc = malloc (sizeof(struct prop_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_PROPERTIES malloc1 failed (%d bytes)\r\n",sizeof(struct prop_cnfg_st));      
                                        return -2;
                                    }   
                                    
                                    res = getPropertiesConfig((struct prop_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getPropertiesConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                             
                                                                                
                                    printDebug("\r\n--- READ CONF_PROPERTIES ---\r\n");
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->deviceType);
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->firmwareVersion);
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->deviceName);
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->deviceID);
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->hostName);
                                    printDebug("%s\r\n",((struct prop_cnfg_st *)cnfgStrc)->hostID);
                                    printDebug("%d\r\n",(signed char) (((struct prop_cnfg_st *)cnfgStrc)->timeZone));
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct prop_cnfg_st));   
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct prop_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct prop_cnfg_st));                     
                                               
                                    free ( cnfgStrc );                        
                                    
                                    return sizeof(struct prop_cnfg_st);   
                                     
        case CONF_OPERATE       :   cnfgStrc = malloc (sizeof(struct oper_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_OPERATE malloc1 failed (%d bytes)\r\n",sizeof(struct oper_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getOperatingConfig((struct oper_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getOperatingConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                                     
                                                                                
                                    printDebug("\r\n--- READ CONF_OPERATE ---\r\n");
                                    printDebug("mode            [0x%02X]\r\n",((struct oper_cnfg_st *)cnfgStrc)->mode);
                                    printDebug("deviceEnable    [0x%02X]\r\n",((struct oper_cnfg_st *)cnfgStrc)->deviceEnable);
                                    printDebug("alarmEnable     [0x%02X]\r\n",((struct oper_cnfg_st *)cnfgStrc)->alarmEnable);    
                                    printDebug("sensorEnable    ");
                                    for(i=0;i<MAX_SENS_CHANNEL;i++){
                                        printDebug("[0x%02X]",((struct oper_cnfg_st *)cnfgStrc)->sensorEnable[i]);  
                                        if(i == (MAX_SENS_CHANNEL-1)){
                                            printDebug("\r\n");
                                        } 
                                    }                                                           
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct oper_cnfg_st));    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct oper_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct oper_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct oper_cnfg_st);   
                                    
        case CONF_SENSOR        :   channel =   (int)cnfg & 0x000F;       
        
                                    cnfgStrc = malloc (sizeof(struct sens_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_SENSOR malloc1 failed (%d bytes)\r\n",sizeof(struct sens_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getSensorConfig(channel, (struct sens_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getSensorConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_SENSOR [%d]---\r\n",channel);    
                                    printDebug("samplingRate    [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->samplingRate);
                                    printDebug("reOccurrence    [0x%04X]\r\n",  ((struct sens_cnfg_st *)cnfgStrc)->reOccurrence);
                                    printDebug("timeStartHour   [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->timeStartHour); 
                                    printDebug("timeStartMin    [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->timeStartMin); 
                                    printDebug("timeStopHour    [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->timeStopHour); 
                                    printDebug("timeStopMin     [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->timeStopMin); 
                                    printDebug("alarmEnable     [0x%02X]\r\n",  ((struct sens_cnfg_st *)cnfgStrc)->alarmEnable); 
                                    printDebug("lowerLimit      [%f]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->lowerLimit); 
                                    printDebug("lowerDelay      [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->lowerDelay); 
                                    printDebug("lowerExtreme    [%f]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->lowerExtreme); 
                                    printDebug("upperLimit      [%f]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->upperLimit); 
                                    printDebug("upperDelay      [%d]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->upperDelay); 
                                    printDebug("upperExtreme    [%f]\r\n",      ((struct sens_cnfg_st *)cnfgStrc)->upperExtreme); 
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct sens_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct sens_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct sens_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct sens_cnfg_st);   
                                    
        case CONF_AUXDI         :   channel =   (int)cnfg & 0x000F;       
        
                                    cnfgStrc = malloc (sizeof(struct din_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_AUXDI malloc1 failed (%d bytes)\r\n",sizeof(struct din_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getAuxDinConfig(channel, (struct din_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getAuxDinConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_AUXDI [%d]---\r\n",channel);    
                                    printDebug("channelEnable    [0x%02X]\r\n", ((struct din_cnfg_st *)cnfgStrc)->channelEnable);
                                    printDebug("alarmDelay       [%d]\r\n",     ((struct din_cnfg_st *)cnfgStrc)->alarmDelay); 
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct din_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct din_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct din_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct din_cnfg_st); 
                                    
        case CONF_AUXDO         :   channel =   (int)cnfg & 0x000F;       
        
                                    cnfgStrc = malloc (sizeof(struct dout_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_AUXDO malloc1 failed (%d bytes)\r\n",sizeof(struct dout_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getAuxDoutConfig(channel, (struct dout_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getAuxDoutConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_AUXDO [%d]---\r\n",channel);    
                                    printDebug("channelEnable     [0x%02X]\r\n",    ((struct dout_cnfg_st *)cnfgStrc)->channelEnable);
                                    printDebug("stateOnEvent      [%d]\r\n",        ((struct dout_cnfg_st *)cnfgStrc)->stateOnEvent);
                                    printDebug("eventSourceNumber [%d]\r\n",        ((struct dout_cnfg_st *)cnfgStrc)->eventSourceNumber); 
                                                 
                                    printDebug("eventSource       ");
                                    for(i=0;i<MAX_EVENT_SOURCE;i++){
                                        printDebug("[0x%02X]",((struct dout_cnfg_st *)cnfgStrc)->eventSource[i]);  
                                        if(i == (MAX_EVENT_SOURCE-1)){
                                            printDebug("\r\n");
                                        } 
                                    }
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct dout_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct dout_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct dout_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct dout_cnfg_st);  
                                    
        case CONF_AUXAI         :   channel =   (int)cnfg & 0x000F;       
        
                                    cnfgStrc = malloc (sizeof(struct ain_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_AUXAI malloc1 failed (%d bytes)\r\n",sizeof(struct ain_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getAuxAinConfig(channel, (struct ain_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getAuxAinConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_AUXAI [%d]---\r\n",channel);    
                                    printDebug("channelEnable    [0x%02X]\r\n", ((struct ain_cnfg_st *)cnfgStrc)->channelEnable);
                                    printDebug("alarmDelay       [%f]\r\n",     ((struct ain_cnfg_st *)cnfgStrc)->alarmLevel); 
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct ain_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct ain_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct ain_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct ain_cnfg_st);
                                    
        case CONF_AUXAO         :   channel =   (int)cnfg & 0x000F;       
        
                                    cnfgStrc = malloc (sizeof(struct aout_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_AUXAO malloc1 failed (%d bytes)\r\n",sizeof(struct aout_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getAuxAoutConfig(channel, (struct aout_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getAuxAoutConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_AUXAO [%d]---\r\n",channel);    
                                    printDebug("channelEnable     [0x%02X]\r\n",    ((struct aout_cnfg_st *)cnfgStrc)->channelEnable);
                                    printDebug("levelOnEvent      [%f]\r\n",        ((struct aout_cnfg_st *)cnfgStrc)->levelOnEvent);
                                    printDebug("eventSourceNumber [%d]\r\n",        ((struct aout_cnfg_st *)cnfgStrc)->eventSourceNumber); 
                                                 
                                    printDebug("eventSource       ");
                                    for(i=0;i<MAX_EVENT_SOURCE;i++){
                                        printDebug("[0x%02X]",((struct aout_cnfg_st *)cnfgStrc)->eventSource[i]);  
                                        if(i == (MAX_EVENT_SOURCE-1)){
                                            printDebug("\r\n");
                                        } 
                                    }
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct aout_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct aout_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct aout_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct aout_cnfg_st);  
                                    
        case CONF_BUZZER        :   cnfgStrc = malloc (sizeof(struct buzz_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_BUZZER malloc1 failed (%d bytes)\r\n",sizeof(struct buzz_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getBuzzerConfig((struct buzz_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getBuzzerConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                              
                                                                                            
                                    printDebug("\r\n--- READ CONF_BUZZER ---\r\n");    
                                    printDebug("enable            [0x%02X]\r\n",    ((struct buzz_cnfg_st *)cnfgStrc)->enable);
                                    printDebug("eventSourceNumber [%d]\r\n",        ((struct buzz_cnfg_st *)cnfgStrc)->eventSourceNumber); 
                                                 
                                    printDebug("eventSource       ");
                                    for(i=0;i<MAX_EVENT_SOURCE;i++){
                                        printDebug("[0x%02X]",((struct buzz_cnfg_st *)cnfgStrc)->eventSource[i]);  
                                        if(i == (MAX_EVENT_SOURCE - 1)){
                                            printDebug("\r\n");
                                        } 
                                    }
                                                                                
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct buzz_cnfg_st));                    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct buzz_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct buzz_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct buzz_cnfg_st);  
                                    
        case CONF_ENVALARM      :   cnfgStrc = malloc (sizeof(struct alm_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_ENVALARM malloc1 failed (%d bytes)\r\n",sizeof(struct alm_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getEnvAlarmConfig((struct alm_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getEnvAlarmConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                                     
                                                                                
                                    printDebug("\r\n--- READ CONF_ENVALARM ---\r\n");
                                    printDebug("gsmRSSILevel      [%f]\r\n",((struct alm_cnfg_st *)cnfgStrc)->gsmRSSILevel);
                                    printDebug("gsmCreditBalance  [%d]\r\n",((struct alm_cnfg_st *)cnfgStrc)->gsmCreditBalance);
                                    printDebug("batteryLevel      [%f]\r\n",((struct alm_cnfg_st *)cnfgStrc)->batteryLevel); 
                                    printDebug("commSignalLevel   [%f]\r\n",((struct alm_cnfg_st *)cnfgStrc)->commSignalLevel); 
                                    printDebug("commLostDelay     [%d]\r\n",((struct alm_cnfg_st *)cnfgStrc)->commLostDelay); 
                                    printDebug("memoryMin         [%d]\r\n",((struct alm_cnfg_st *)cnfgStrc)->memoryMin);    
                                    
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct alm_cnfg_st));    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct alm_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct alm_cnfg_st));                      
                                               
                                    free ( cnfgStrc );                       
                                    
                                    return sizeof(struct alm_cnfg_st);   
        case CONF_STATREPORT    :   cnfgStrc = malloc (sizeof(struct stat_cnfg_st));
                                    if(cnfgStrc == NULL){
                                        printDebug("<configRead> CONF_STATREPORT malloc1 failed (%d bytes)\r\n",sizeof(struct stat_cnfg_st));      
                                        return -2;
                                    }        
                                    
                                    res = getStatusReportConfig((struct stat_cnfg_st *) cnfgStrc);
                                    if(res < 0){
                                        printDebug("<configRead> getStatusReportConfig() failed\r\n"); 
                                        free ( cnfgStrc );
                                        return -3;
                                    }                                     
                                    
                                    printDebug("\r\n--- READ CONF_STATREPORT ---\r\n");
                                    printDebug("accm      [0x%02X]\r\n",((struct stat_cnfg_st *)cnfgStrc)->accm);
                                    printDebug("intv      [%d]\r\n",    ((struct stat_cnfg_st *)cnfgStrc)->intv);
                                    
//                                    printDebug("\r\n--- cnfgStrc ---\r\n");
//                                    print_payload((char *)cnfgStrc, sizeof(struct stat_cnfg_st));    
                                    
                                    memcpy(ptr,(char *) cnfgStrc,sizeof(struct stat_cnfg_st));      
//                                    printDebug("\r\n--- ptr ---\r\n");
//                                    print_payload((char *)ptr, sizeof(struct stat_cnfg_st));   
                                    
                                    free ( cnfgStrc );                       
        default                 :   printDebug("<configRead> unknown configuration type [0x%02X]\r\n",type);
                                    return -1;        
    }                                                
             
    return 0;
                    
}                                        
/*============================================================*/
int configWrite(unsigned char cnfg, unsigned char *conf, int len)                                               
{       
    int res = 0;
    unsigned char   type            = 0x00;
    int             channel         = 0;   
    
    type    =   cnfg & 0xF0;         
    
    printDebug("\r\n --- WRITE CONFIG packet (%d)--- \r\n",len);
    print_payload(conf,len);
                         
    switch( type ){   
        case CONF_PROPERTIES    :   printDebug("\r\n--- Write CONF_PROPERTIES ---");
                                  
                                    res = setPropertiesConfig((struct prop_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setPropertiesConfig() failed\r\n");    
                                        return -3;
                                    }                           
                                    break;
        case CONF_OPERATE       :   printDebug("\r\n--- Write CONF_OPERATE ---");
                                  
                                    res = setOperatingConfig((struct oper_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setOperatingConfig() failed\r\n");    
                                        return -3;
                                    }                           
                                    break;
        case CONF_SENSOR        :   channel =   (int)cnfg & 0x000F;             
                                    printDebug("\r\n--- Write CONF_SENSOR (%d) ---",channel);  
                                  
                                    res = setSensorConfig(channel,(struct sens_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setSensorConfig() failed\r\n");    
                                        return -3;
                                    }                           
                                    break;
        case CONF_AUXDI         :   channel =   (int)cnfg & 0x000F;           
                                    printDebug("\r\n--- Write CONF_AUXDI (%d) ---",channel);
                                  
                                    res = setAuxDinConfig(channel,(struct din_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setAuxDinConfig() failed\r\n");    
                                        return -3;
                                    }                           
                                    break;
        case CONF_AUXDO         :   channel =   (int)cnfg & 0x000F;          
                                    printDebug("\r\n--- Write CONF_AUXDO (%d) ---",channel);
                                  
                                    res = setAuxDoutConfig(channel,(struct dout_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setAuxDoutConfig() failed\r\n");    
                                        return -3;
                                    }                 
                                    break; 
        case CONF_AUXAI         :   channel =   (int)cnfg & 0x000F;   
                                    printDebug("\r\n--- Write CONF_AUXAI (%d) ---",channel);
                                  
                                    res = setAuxAinConfig(channel,(struct ain_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setAuxAinConfig() failed\r\n");    
                                        return -3;
                                    }                 
                                    break; 
        case CONF_AUXAO         :   channel =   (int)cnfg & 0x000F;   
                                    printDebug("\r\n--- Write CONF_AUXAO (%d) ---",channel);
                                  
                                    res = setAuxAoutConfig(channel,(struct aout_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setAuxAoutConfig() failed\r\n");    
                                        return -3;
                                    }                 
                                    break; 
        case CONF_BUZZER        :   printDebug("\r\n--- Write CONF_BUZZER ---");
                                  
                                    res = setBuzzerConfig((struct buzz_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setBuzzerConfig() failed\r\n");    
                                        return -3;
                                    }                 
                                    break;
        case CONF_ENVALARM      :   printDebug("\r\n--- Write CONF_ENVALARM ---");
                                  
                                    res = setEnvAlarmConfig((struct alm_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setEnvAlarmConfig() failed\r\n");    
                                        return -3;
                                    }                 
                                    break;                                            
        case CONF_STATREPORT    :   printDebug("\r\n--- Write CONF_STATREPORT ---");
                                    
                                    res = setStatusReportConfig((struct stat_cnfg_st *)conf);           
                                    if(res < 0){
                                        printDebug("<configWrite> setStatusReportConfig() failed\r\n");    
                                        return -3;
                                    } 
                                    
        default                 :   printDebug("<configRead> unknown configuration type [0x%02X]\r\n",type);
                                    return -1;        
    }              
    return 0;
}                                        
/*============================================================*/
int setLogManipulateSchema(unsigned char accd, unsigned long int tStart, unsigned long int tStop)
{
    if(tStop < tStart){
        printDebug("<setLogManipulateSchema> time period error start[%ld] - stop[%ld]\r\n",tStart,tStop);    
        return -1;
    }             
    
    return 0;
}                                              
/*============================================================*/
int setTimeSyncSchema(unsigned char *timeData)
{
    int res;
    unsigned long int unixTime = 0x00000000;
    
    memcpy(&unixTime,timeData,sizeof(unixTime)); 
                    
    if(unixTime == 0){
        printDebug("<setTimeSyncSchema> Got 0 Unix time stamp\r\n");
        return -1;
    }
                                                     
    printDebug("<setTimeSyncSchema> Received time sync. packet %ld\r\n",unixTime);
    
    res = RTC_setBinary( unixTime );  
    if( res < 0 ){
        printDebug("<setTimeSyncSchema> RTC_setBinary(%ld) failed\r\n", unixTime);    
        return -1; 
    }            
    
    return 0;
}
/*============================================================*/
int setMuteAlarm(unsigned long int period)
{
    int res; 
         
    if(period == 0){
        printDebug("<setMuteAlarm> permanent mute alarm\r\n");  
    }
    else if(((period < 300) || (period > 86400))){               
        printDebug("<setMuteAlarm> invalid period (%ld)\r\n", period);
        return -1;
    }                                                        
    else{            
        printDebug("<setMuteAlarm> mute alarm for %ld second\r\n",period);
    }                                                                  
    
    res = ALARM_mute( period );  
    if( res < 0 ){
        printDebug("<setMuteAlarm> ALARM_mute(%ld) failed\r\n", period);    
        return -1; 
    }
    
    return 0;
}
/*============================================================*/
int configConfirm(void)                                             
{
    return 0;
}                                        
/*============================================================*/  
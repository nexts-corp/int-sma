#include "config.h"

//eeprom iDVConfig_t viDVConfigDefualt;
eeprom iDVConfig_t viDVConfigUpdate;
//iDVConfig_t viDVConfigBuffer;


void iConfigDefaultInt(){ 
    int i = 0; 
    iChar_t viIP_Suit[4]={0,0,0,0};  
    
//         iChar_t viNetworkPort;
//     iChar_t viMethod;
//     iChar_t viLocolIP[4];
//     iChar_t viLocalPort[4];
//     iChar_t viSubNetMask[4];
//     iChar_t viGateWay[4];
//     iChar_t viServerIP[4];
//     iChar_t viServerPort[4];
//     iChar_t viDNS_IP[4];
//     iChar_t viSDN[32];
//     iChar_t viSSID[32];
//     iChar_t viPassword[20];
//     iChar_t viSecurity;
//     iChar_t viAPN[20];
//     
//    
    viDVConfigUpdate.viNetworkConfig.viNetworkPort  = 0x01;  
    viDVConfigUpdate.viNetworkConfig.viMethod       = 0x01;
    iMemcpye((char eeprom *)&viDVConfigUpdate.viNetworkConfig.viNetworkPort[0],viIP_Suit,4); 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01; 
//    viDVConfigUpdate.viNetworkConfig.viNetworkPort = 0x01;   
    
    viDVConfigUpdate.viOperationConfig.viDeviceEnable = 0x01; 

     
    for(i=0;i<MAX_SENSOR_CHANNEL_def;i++){
        viDVConfigUpdate.viSensorChannelConfig[i].viAlarmEnable = 0x00; 
        viDVConfigUpdate.viSensorChannelConfig[i].viSensorEnable = 0x00;
        viDVConfigUpdate.viSensorChannelConfig[i].viSamplingRate = 0x00; 
        viDVConfigUpdate.viSensorChannelConfig[i].viSensorOutput = 0x00;
        viDVConfigUpdate.viSensorChannelConfig[i].viSensorTypeID = 0x00; 
        viDVConfigUpdate.viSensorChannelConfig[i].viDataUnit = 0x00;
        viDVConfigUpdate.viSensorChannelConfig[i].viMinimum = 0.0; 
        viDVConfigUpdate.viSensorChannelConfig[i].viMaximum = 0.0;
        viDVConfigUpdate.viSensorChannelConfig[i].viLower = 0.0; 
        viDVConfigUpdate.viSensorChannelConfig[i].viLowerDelay = 0.0;   
        viDVConfigUpdate.viSensorChannelConfig[i].viLowerExtreme = 0.0; 
        viDVConfigUpdate.viSensorChannelConfig[i].viUpper = 0.0;
        viDVConfigUpdate.viSensorChannelConfig[i].viUpperDelay = 0.00; 
        viDVConfigUpdate.viSensorChannelConfig[i].viUpperExtreme = 0.0;
    } 
     
   
    for(i=0;i<MAX_DI_CHANNEL_def;i++){
        viDVConfigUpdate.viDI_ChannelConfig[i].viAlarmEnable = 0x00; 
        viDVConfigUpdate.viDI_ChannelConfig[i].viDI_Enable = 0x00; 
        viDVConfigUpdate.viDI_ChannelConfig[i].viMode = 0x00;
        viDVConfigUpdate.viDI_ChannelConfig[i].viDetectEdge = 0x00;
    } 
    
    
    for(i=0;i<MAX_DO_CHANNEL_def;i++){
        viDVConfigUpdate.viDO_ChannelConfig[i].viD_O_Enable = 0x00; 
        viDVConfigUpdate.viDO_ChannelConfig[i].viState = 0x00;  
        viDVConfigUpdate.viDO_ChannelConfig[i].viDelay = 0x00;
    } 
    
    
    viDVConfigUpdate.viPWM_ChannelConfig.viPWM_Enable = 0x00; 
    viDVConfigUpdate.viPWM_ChannelConfig.viLevel = 0.0;  
   
}

void iMemcpye(char eeprom *eptr, char *dptr, int len)
{
   int i=0; 
   for(i=0;i<len;i++) {
      *eptr++ = *dptr;
      dptr++;
   }
   return;
}

void iMemcpyd(char *dptr, char eeprom *eptr,  int len)
{
   int i=0; 
   for(i=0;i<len;i++) {
      *dptr++ = *eptr;
      eptr++;
   }
   return;
}

void iPrintConfig(char eeprom *eptr,unsigned int len)
{
   int i=0,j=0;
   printDebug("[printConfig]Data:[\r\n"); 
   printDebug("%04d\t\t",0);
   for(i=0,j=1;i<len;i++,j++) {
      printDebug("%02x",*(eptr++));
      if(j==8){
         printDebug("\t\t");
      }else if(j==16){
         printDebug("\r\n");
         printDebug("%04d\t\t",i+1);
         j=0;
      }
   } 
   printDebug("\r\n]\r\n");
   return;
}

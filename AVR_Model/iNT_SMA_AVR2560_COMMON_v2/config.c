#include "config.h"

//eeprom iDVConfig_t viDVConfigDefualt;
eeprom iDVConfig_t viDVConfigUpdate;

//iChar_t viGroupCode[MAX_GROUP_TAG]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e};


//void * viPtrGroupCode[MAX_GROUP_TAG] = NULL;
//iDVConfig_t viDVConfigBuffer;

void* iMapConfigPointer(char iCode_arg){
      void *voidPtr = NULL;   
      //network
      if(iCode_arg == 0x00){voidPtr = (void*)&viDVConfigUpdate.viNetworkConfig;}    
      else if(iCode_arg == 0x01){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viNetworkPort);}
      else if(iCode_arg == 0x02){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viMethod);}     
      else if(iCode_arg == 0x03){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viLocolIP[0]);}   
      else if(iCode_arg == 0x04){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viLocalPort[0]);}
      else if(iCode_arg == 0x05){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viSubNetMask[0]);}
      else if(iCode_arg == 0x06){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viGateWay[0]);}
      else if(iCode_arg == 0x07){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viServerIP[0]);}
      else if(iCode_arg == 0x08){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viServerPort[0]);}
      else if(iCode_arg == 0x09){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viDNS_IP[0]);}
      else if(iCode_arg == 0x0a){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viSDN[0]);}
      else if(iCode_arg == 0x0b){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viSSID[0]);}
      else if(iCode_arg == 0x0c){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viPassword[0]);}
      else if(iCode_arg == 0x0d){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viSecurity);}
      else if(iCode_arg == 0x0e){voidPtr = (void*)(&viDVConfigUpdate.viNetworkConfig.viAPN[0]);}
      //properties  
      else if(iCode_arg == 0x0f){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viDeviceType[0]);}   
      else if(iCode_arg == 0x10){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viFirmWareVersion[0]);}
      else if(iCode_arg == 0x11){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viDeviceName[0]);}
      else if(iCode_arg == 0x12){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viDeviceID[0]);}
      else if(iCode_arg == 0x13){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viHostName[0]);}
      else if(iCode_arg == 0x14){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viHostID[0]);}
      else if(iCode_arg == 0x15){voidPtr = (void*)(&viDVConfigUpdate.viPropertieseConfig.viTimeZone);}       
      //operate
      else if(iCode_arg == 0x16){voidPtr = (void*)(&viDVConfigUpdate.viOperationConfig.viDeviceEnable);} 
      //Sensor  
      //Sensor Channel[0]
      else if(iCode_arg == 0x17){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viAlarmEnable);}  
      else if(iCode_arg == 0x18){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viSensorEnable);}
      else if(iCode_arg == 0x19){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viSamplingRate);}
      else if(iCode_arg == 0x1a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viSensorOutput);}
      else if(iCode_arg == 0x1b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viSensorTypeID);}
      else if(iCode_arg == 0x1c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viDataUnit);}
      else if(iCode_arg == 0x1d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viMinimum);}
      else if(iCode_arg == 0x1e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viMaximum);}
      else if(iCode_arg == 0x1f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viLower);}
      else if(iCode_arg == 0x20){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viLowerDelay);}
      else if(iCode_arg == 0x21){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viLowerExtreme);}
      else if(iCode_arg == 0x22){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viUpper);}
      else if(iCode_arg == 0x23){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viUpperDelay);}
      else if(iCode_arg == 0x24){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[0].viUpperExtreme);} 
      //Sensor Channel[1]
      else if(iCode_arg == 0x25){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viAlarmEnable);}  
      else if(iCode_arg == 0x26){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viSensorEnable);}
      else if(iCode_arg == 0x27){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viSamplingRate);}
      else if(iCode_arg == 0x28){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viSensorOutput);}
      else if(iCode_arg == 0x29){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viSensorTypeID);}
      else if(iCode_arg == 0x2a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viDataUnit);}
      else if(iCode_arg == 0x2b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viMinimum);}
      else if(iCode_arg == 0x2c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viMaximum);}
      else if(iCode_arg == 0x2d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viLower);}
      else if(iCode_arg == 0x2e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viLowerDelay);}
      else if(iCode_arg == 0x2f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viLowerExtreme);}
      else if(iCode_arg == 0x30){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viUpper);}
      else if(iCode_arg == 0x31){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viUpperDelay);}
      else if(iCode_arg == 0x32){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[1].viUpperExtreme);}     
      //Sensor Channel[2]
      else if(iCode_arg == 0x33){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viAlarmEnable);}  
      else if(iCode_arg == 0x34){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viSensorEnable);}
      else if(iCode_arg == 0x35){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viSamplingRate);}
      else if(iCode_arg == 0x36){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viSensorOutput);}
      else if(iCode_arg == 0x37){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viSensorTypeID);}
      else if(iCode_arg == 0x38){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viDataUnit);}
      else if(iCode_arg == 0x39){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viMinimum);}
      else if(iCode_arg == 0x3a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viMaximum);}
      else if(iCode_arg == 0x3b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viLower);}
      else if(iCode_arg == 0x3c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viLowerDelay);}
      else if(iCode_arg == 0x3d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viLowerExtreme);}
      else if(iCode_arg == 0x3e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viUpper);}
      else if(iCode_arg == 0x3f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viUpperDelay);}
      else if(iCode_arg == 0x40){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[2].viUpperExtreme);}  
      //Sensor Channel[3]
      else if(iCode_arg == 0x41){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viAlarmEnable);}  
      else if(iCode_arg == 0x42){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viSensorEnable);}
      else if(iCode_arg == 0x43){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viSamplingRate);}
      else if(iCode_arg == 0x44){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viSensorOutput);}
      else if(iCode_arg == 0x45){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viSensorTypeID);}
      else if(iCode_arg == 0x46){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viDataUnit);}
      else if(iCode_arg == 0x47){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viMinimum);}
      else if(iCode_arg == 0x48){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viMaximum);}
      else if(iCode_arg == 0x49){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viLower);}
      else if(iCode_arg == 0x4a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viLowerDelay);}
      else if(iCode_arg == 0x4b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viLowerExtreme);}
      else if(iCode_arg == 0x4c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viUpper);}
      else if(iCode_arg == 0x4d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viUpperDelay);}
      else if(iCode_arg == 0x4e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[3].viUpperExtreme);}   
      //Sensor Channel[4]
      else if(iCode_arg == 0x4f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viAlarmEnable);}  
      else if(iCode_arg == 0x50){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viSensorEnable);}
      else if(iCode_arg == 0x51){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viSamplingRate);}
      else if(iCode_arg == 0x52){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viSensorOutput);}
      else if(iCode_arg == 0x53){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viSensorTypeID);}
      else if(iCode_arg == 0x54){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viDataUnit);}
      else if(iCode_arg == 0x55){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viMinimum);}
      else if(iCode_arg == 0x56){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viMaximum);}
      else if(iCode_arg == 0x57){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viLower);}
      else if(iCode_arg == 0x58){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viLowerDelay);}
      else if(iCode_arg == 0x59){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viLowerExtreme);}
      else if(iCode_arg == 0x5a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viUpper);}
      else if(iCode_arg == 0x5b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viUpperDelay);}
      else if(iCode_arg == 0x5c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[4].viUpperExtreme);}      
      //Sensor Channel[5]
      else if(iCode_arg == 0x5d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viAlarmEnable);}  
      else if(iCode_arg == 0x5e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viSensorEnable);}
      else if(iCode_arg == 0x5f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viSamplingRate);}
      else if(iCode_arg == 0x60){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viSensorOutput);}
      else if(iCode_arg == 0x61){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viSensorTypeID);}
      else if(iCode_arg == 0x62){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viDataUnit);}
      else if(iCode_arg == 0x63){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viMinimum);}
      else if(iCode_arg == 0x64){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viMaximum);}
      else if(iCode_arg == 0x65){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viLower);}
      else if(iCode_arg == 0x66){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viLowerDelay);}
      else if(iCode_arg == 0x67){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viLowerExtreme);}
      else if(iCode_arg == 0x68){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viUpper);}
      else if(iCode_arg == 0x69){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viUpperDelay);}
      else if(iCode_arg == 0x6a){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[5].viUpperExtreme);} 
      //Sensor Channel[6]
      else if(iCode_arg == 0x6b){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viAlarmEnable);}  
      else if(iCode_arg == 0x6c){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viSensorEnable);}
      else if(iCode_arg == 0x6d){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viSamplingRate);}
      else if(iCode_arg == 0x6e){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viSensorOutput);}
      else if(iCode_arg == 0x6f){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viSensorTypeID);}
      else if(iCode_arg == 0x70){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viDataUnit);}
      else if(iCode_arg == 0x71){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viMinimum);}
      else if(iCode_arg == 0x72){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viMaximum);}
      else if(iCode_arg == 0x73){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viLower);}
      else if(iCode_arg == 0x74){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viLowerDelay);}
      else if(iCode_arg == 0x75){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viLowerExtreme);}
      else if(iCode_arg == 0x76){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viUpper);}
      else if(iCode_arg == 0x77){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viUpperDelay);}
      else if(iCode_arg == 0x78){voidPtr = (void*)(&viDVConfigUpdate.viSensorChannelConfig[6].viUpperExtreme);} 
      //Digital Input 
      //Digital Channel[0]    
      else if(iCode_arg == 0x79){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[0].viAlarmEnable);}
      else if(iCode_arg == 0x7a){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[0].viDI_Enable);}
      else if(iCode_arg == 0x7b){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[0].viMode);}
      else if(iCode_arg == 0x7c){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[0].viDetectEdge);} 
      //Digital Channel[1]    
      else if(iCode_arg == 0x7d){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[1].viAlarmEnable);}
      else if(iCode_arg == 0x7e){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[1].viDI_Enable);}
      else if(iCode_arg == 0x7f){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[1].viMode);}
      else if(iCode_arg == 0x80){voidPtr = (void*)(&viDVConfigUpdate.viDI_ChannelConfig[1].viDetectEdge);}
      //Digital Output  
      //Digital Channel[0]    
      else if(iCode_arg == 0x81){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[0].viD_O_Enable);}
      else if(iCode_arg == 0x82){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[0].viState);}
      else if(iCode_arg == 0x83){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[0].viDelay);}
      //Digital Channel[1]    
      else if(iCode_arg == 0x84){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[1].viD_O_Enable);}
      else if(iCode_arg == 0x85){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[1].viState);}
      else if(iCode_arg == 0x86){voidPtr = (void*)(&viDVConfigUpdate.viDO_ChannelConfig[1].viDelay);}  
      //PWM
      else if(iCode_arg == 0x84){voidPtr = (void*)(&viDVConfigUpdate.viPWM_ChannelConfig.viPWM_Enable);}
      else if(iCode_arg == 0x84){voidPtr = (void*)(&viDVConfigUpdate.viPWM_ChannelConfig.viLevel);}
      
      return   voidPtr;
}


void iConfigDefaultInt(){ 
    int i = 0; 
    iChar_t viIP_Suit[4]={0,0,0,0};  
  
    viDVConfigUpdate.viNetworkConfig.viNetworkPort  = 0x01;  
    viDVConfigUpdate.viNetworkConfig.viMethod       = 0x01;
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viLocolIP,viIP_Suit,4);  
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viLocalPort,viIP_Suit,4);   
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viLocalPort,viIP_Suit,4);
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viSubNetMask,viIP_Suit,4);
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viGateWay,viIP_Suit,4);
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viServerIP,viIP_Suit,4);
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viServerPort,viIP_Suit,4);
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viDNS_IP,viIP_Suit,4); 
    
  //  memset((char eeprom *)viDVConfigUpdate.viNetworkConfig.viSDN,0,32);
    
//    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viSDN,viIP_Suit,32);
//    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viSSID,viIP_Suit,32);
//    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viPassword,viIP_Suit,20); 
    viDVConfigUpdate.viNetworkConfig.viSecurity       = 0x00;
    iMemcpye((char eeprom *)viDVConfigUpdate.viNetworkConfig.viAPN,viIP_Suit,20);  
    
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

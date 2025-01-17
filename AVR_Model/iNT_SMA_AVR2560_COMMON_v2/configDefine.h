#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

#include "main.h"


//#define MAX_CONFIG_ALL_GROUP_def    0x0E
//#define MAX_SENSOR_CHANNEL_def      0x07
//#define MAX_DI_CHANNEL_def          0x02
//#define MAX_DO_CHANNEL_def          0x02
//#define MAX_PWM_CHANNEL_def         0x01
//
//#define NETWORK_GROUP_def           0x00
//#define PROPERTIESE_GROUP_def       0x01
//#define OPERATION_GROUP_def         0x02
//#define ADC_CH1_GROUP_def           0x03
//#define ADC_CH2_GROUP_def           0x04
//#define ADC_CH3_GROUP_def           0x05
//#define ADC_CH4_GROUP_def           0x06
//#define ADC_CH5_GROUP_def           0x07
//#define ONE_WIRE_CH1_GROUP_def      0x08
//#define ONE_WIRE_CH2_GROUP_def      0x09
//#define DI_CH1_GROUP_def            0x0A
//#define DI_CH2_GROUP_def            0x0B
//#define DO_CH1_GROUP_def            0x0C
//#define DO_CH2_GROUP_def            0x0D
//#define PWM_CH1_GROUP_def           0x0E
//
//
////member NETWORK_GROUP_def
//#define nNETWORK_PORT_def           0x00
//#define nMETHOD_def                 0x01
//#define nLOCAL_IP_def               0x02
//#define nLOCAL_PORT_def             0x03
//#define nSUBNET_MASK_def            0x04
//#define nGATE_WAY_def               0x05
//#define nSERVER_IP_def              0x06
//#define nSERVER_PORT_def            0x07
//#define nDNS_IP_def                 0x08
//#define nSDN_def                    0x09
//#define nSSID_def                   0x0A
//#define nPASSWORD_def               0x0B
//#define nSECURITY_def               0x0C
//#define nAPN_def                    0x0D
//
//
////member PROPERTIESE_GROUP_def
//#define pDEVICE_TYPE_def            0x00
//#define pFIRMWARE_VERS_def          0x01
//#define pDEVICE_NAME_def            0x02
//#define pDEVICE_ID_def              0x03
//#define pHOST_NAME_def              0x04
//#define pHOST_ID_def                0x05
//#define pTIME_ZONE_def              0x06
//
////member OPERATION_GROUP_def
//#define oDEVICE_ENABLE_def          0x00
//
//
////sensor member ADC_1WIRE_GROUP_def
//#define sALARM_ENABLE_def           0x00
//#define sSENSOR_ENABLE_def          0x01
//#define sSAMPLING_RATE_def          0x02
//#define sSENSOR_OUTPUT_def          0x03
//#define sSENSOR_TYPE_ID_def         0x04
//#define sDATA_UNIT_def              0x05
//#define sMINIMUM_def                0x06
//#define sMAXIMUM_def                0x07
//#define sLOWER_def                  0x08
//#define sLOWER_DELAY_def            0x09
//#define sLOWER_EXTREME_def          0x0A
//#define sUPPER_def                  0x0B
//#define sUPPER_DELAY_def            0x0C
//#define sUPPER_EXTREME_def          0x0D
//
////sensor member DI_CH_GROUP_def
//#define diALARM_ENABLE_def          0x00
//#define diDI_ENABLE_def             0x01
//#define diMODE_def                  0x02
//#define diDETECT_EDGE_def           0x03
//
////sensor member DO_CH_GROUP_def
//#define doDO_ENABLE_def             0x00
//#define doState_def                 0x01
//#define doMODE_def                  0x02
//#define doDelay_def                 0x03
//
////sensor member PWM_CH_GROUP_def
//#define pwmPWM_ENABLE_def           0x00
//#define pwmLevel_def                0x01
//
//
//
//#define MAX_GROUP_TAG           0x0E
//
//#define MAX_PROP_SUB_TAG        0x0E
//
//#define NETWORK_CONF_def 0x00

//typedef struct{
//     iChar_t iTag;
//     iUChar_t iLength; 
//     void *iValue;
//}iTLV_t;
//
//typedef struct{
//     iChar_t iTag;
//     void *iValue;
//}iTV_t;



//typedef struct{
//     iChar_t iNetworkPort;
//     iChar_t iMethod;
//     iChar_t iLocolIP[4];
//     iChar_t iLocalPort[4];
//     iChar_t iSubNetMask[4];
//     iChar_t iGateWay[4];
//     iChar_t iServerIP[4];
//     iChar_t iServerPort[4];
//     iChar_t iDNS_IP[4];
//     iChar_t iSDN[32];
//     iChar_t iSSID[32];
//     iChar_t iPassword[20];
//     iChar_t iSecurity;
//     iChar_t iAPN[20];
//}iNetworkConfig_t;

//typedef struct{
//     iChar_t iDeviceType[20];
//     iChar_t iFirmWareVersion[20];
//     iChar_t iDeviceName[40];
//     iChar_t iDeviceID[20];
//     iChar_t iHostName[40];
//     iChar_t iHostID[20];
//     iChar_t iTimeZone;
//}iPropertieseConfig_t;
//
//typedef struct{
//     iChar_t iDeviceEnable;
//}iOperationConfig_t;
//
//typedef struct{
//     iChar_t iAlarmEnable;
//     iChar_t iSensorEnable; 
//     float iSamplingRate;
//     iChar_t iSensorOutput;
//     iChar_t iSensorTypeID;
//     iChar_t iDataUnit;
//     float iMinimum;
//     float iMaximum;
//     float iLower;
//     float iLowerDelay;
//     float iLowerExtreme;
//     float iUpper;
//     float iUpperDelay;
//     float iUpperExtreme;
//}iSensorChannelConfig_t;
//
//typedef struct{
//     iChar_t iAlarmEnable;
//     iChar_t iDI_Enable; 
//     iChar_t iMode;
//     iChar_t iDetectEdge;
//}iDI_ChannelConfig_t;
//
//typedef struct{
//     iChar_t iD_O_Enable;
//     iChar_t iState; 
//     float iDelay;
//}iDO_ChannelConfig_t;
//
//typedef struct{
//     iChar_t iPWM_Enable;
//     float iLevel;
//}iPWM_ChannelConfig_t;


//typedef struct{
//     //iNetworkConfig_t viNetworkConfig;
//     iPropertieseConfig_t viPropertieseConfig;
//     iOperationConfig_t viOperationConfig;
//     iSensorChannelConfig_t viSensorChannelConfig[MAX_SENSOR_CHANNEL_def];
//     iDI_ChannelConfig_t viDI_ChannelConfig[MAX_DI_CHANNEL_def]; 
//     iDO_ChannelConfig_t viDO_ChannelConfig[MAX_DO_CHANNEL_def];
//     iPWM_ChannelConfig_t viPWM_ChannelConfig;
//}iDeviceConfig_t;

#endif 
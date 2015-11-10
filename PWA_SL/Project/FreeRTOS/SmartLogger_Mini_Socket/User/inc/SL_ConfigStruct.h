/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	SL_ConfigStruct.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	24-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _SL_CONFIGSTRUCT_H
#define _SL_CONFIGSTRUCT_H

#include <stdint.h>

#define MAX_CONF_SIZE					161

#define DEV_CONF							0x01
#define PROPERTIES_CONF				0x02
#define OPERATING_CONF				0x03
#define BUZZER_CONF						0x04
#define WORKING_CONF					0x05
#define NETWORK_CONF					0x06
#define SEN420_CONF						0x07
#define SENADC_CONF						0x08
#define SENMODBUS_CONF				0x09
#define SENPULSE_CONF					0x0A
#define DIN_CONF							0x0B
#define SEN420ALARM_CONF			0x0C
#define SENADCALARM_CONF			0x0D
#define SENMODBUSALARM_CONF		0x0E
#define SENPULSEALARM_CONF		0x0F
#define VALVE_CONF						0x10
#define SEN420CAL_CONF				0x11
#define VALVE420INCAL_CONF		0x12
#define VALVE420OUTCAL_CONF		0x13
#define SENPULSEADJ_CONF			0x14

#define CHANNEL_1							0x00
#define CHANNEL_2							0x01
#define CHANNEL_3							0x02
#define CHANNEL_4							0x03
#define CHANNEL_5							0x04
#define CHANNEL_6							0x05
#define CHANNEL_7							0x06
#define CHANNEL_8							0x07
#define CHANNEL_9							0x08
#define CHANNEL_10						0x09
#define CHANNEL_11						0x0A
#define CHANNEL_12						0x0B
#define CH_NONE								0xFF

/* Information_Structure */
__packed typedef struct {
	uint8_t			user[40];							/* User Name */										 	
	uint8_t 		password[40];					/* Password */
	uint8_t			serialNumber[8];			/* Serial Number */
	uint8_t			destSerial[8];				/* Destination Serial Number */ 
	uint8_t			flagRegistered;				/* Flag Registered */
}ST_INFO;

/* Information_Structure */
__packed typedef struct {
	uint8_t			dout_1;								/* Digital Output Status 1 */
	uint8_t			dout_2;								/* Digital Output Status 2 */
	uint8_t			dout_3;								/* Digital Output Status 3 */
	uint8_t			dout_4;								/* Digital Output Status 4 */
	uint8_t			valve_open;						/* Butterfly Valve Open % */
	float				net_flow;							/* Water Accumulate */
}ST_STATUS;

/* Developer Configuration_Structure */
__packed typedef struct {
	uint8_t			debug_level;					/* Print debug level */										 	
	uint8_t 		transmitRetry;				/* Packet transmission retry(times) */
	uint16_t		retransmitInterval;		/* Retrasmission interval(second) */ 	
	uint16_t		heartbeatInterval;		/* Send Heartbeat interval time */
	uint8_t			heartbeatTimeout; 		/* Number of Heartbeat to timeout */
	float				adc_Vref;							/* ADC Voltage referent */               		
}STCONFIG_DEV;

/* Properties Configuration_Structure */
__packed typedef struct {
	uint8_t 		deviceType[20];				/* Device Type */
	uint8_t 		firmwareVer[20];			/* Firmware Version */
	uint8_t 		deviceName[40];				/* Device Name */
	uint8_t 		deviceID[20];					/* Device ID */
	uint8_t 		hostName[40];					/* Host Name */
	uint8_t 		hostID[20];						/* Host ID */
	int8_t 			timezone;							/* Timezone */
}STCONFIG_PROPERTIES;

/* Operating Configuration_Structure */
__packed typedef struct {
	uint8_t			deviceEnable;					/* Device Enable */
	uint8_t			alarmEnable;					/* Alarm Enable */
}STCONFIG_OPERATING;

/* Buzzer Configuration_Structure */
__packed typedef struct {
	uint8_t			buzzerEnable;					/* Buzzer Enable */
	uint8_t			numberOfSrc;					/* Number of Source */
	uint8_t 		eventSrc[25];					/* Bine to event source */
}STCONFIG_BUZZER;

/* Working Management Configuration_Structure */
__packed typedef struct {
	float 			minBattery;						/* Minimum battery voltage before enter Save_Mode(voltage) */
	uint8_t			recordMode;						/* Recording mode */
	uint32_t		transmitInterval;			/* Transmission interval and Recording interval(second) */
	uint32_t		samplingInterval;			/* Sampling interval(second) */	
	uint32_t		commDataLimit;				/* Communication data limit */
}STCONFIG_WORKING;

/* Network Configuration_Structure */
__packed typedef struct {
	uint16_t		serverPort;						/* Server port address */
	uint8_t			serverAddrType;				/* Server address type (IP Address or Host Name) */
	uint8_t			serverAddrLen;				/* Server Address legth */	
	uint8_t 		serverAddr[100];			/* Server Address */
}STCONFIG_NETWORK;

/*  4-20mA Sensor Configuration_Structure */
__packed typedef struct {
	uint8_t 	sensorOutput;						/* Sensor output type */
	uint8_t 	sensorType;							/* Sensor type */
	uint8_t 	dataUnit;								/* Data unit */
	uint8_t 	sensorEnable;						/* Sensor enable */
	uint8_t		numCalibrate;						/* Number of calibrate interval */
	float 		x1;											/* Point 1 */
	float 		y1;											/* Point 1 */
	float 		x2;											/* Point 2 */
	float 		y2;											/* Point 2 */
	float 		x3;											/* Point 3 */
	float 		y3;											/* Point 3 */
	float 		x4;											/* Point 4 */
	float 		y4;											/* Point 4 */
	float 		x5;											/* Point 5 */
	float 		y5;											/* Point 5 */
	float 		x6;											/* Point 6 */
	float 		y6;											/* Point 6 */
	float 		x7;											/* Point 7 */
	float 		y7;											/* Point 7 */
	float 		x8;											/* Point 8 */
	float 		y8;											/* Point 8 */
	
}STCONFIG_ANALOGSENSOR;

/* MODBUS Flow Sensor Configuration_Structure  */
__packed typedef struct {
	uint8_t 	slave_ID;								/* MODBUS Slave ID */
	uint32_t 	scan_rate;							/* Scan Rate */
	uint8_t 	baudrate;								/* Baudrate */
	uint8_t 	sensor_enable;					/* Sensor enable */
} STCONFIG_MODBUSSENSOR;

/* PULSE Flow Sensor Configuration_Structure */
__packed typedef struct {
	uint8_t 	vol_per_pulse;					/* Volume per Pulse */
	uint32_t 	net_total;							/* Old total net flow */
	uint8_t 	sensor_enable;					/* Sensor enable */
} STCONFIG_PULSESENSOR;

typedef struct {
	float			multiplier_value;
}STCONFIG_PULSEADJ;

/* Digital Input Configuration_Structure */
__packed typedef struct {
	uint8_t			trigAlarmEn;					/* Trigger Alarm Enable */
	uint8_t			trigAlarmType;				/* Trigger Alarm Type */
	uint16_t		trigDelay;						/* Trigger Delay Time */
}STCONFIG_DIGITALsIN;

/* Sensor Alarm Configuration_Structure */
__packed typedef struct {
	uint8_t			alarmEnable;					/* Sensor alarm enable */
	float				lower;								/* Sensor alarm level (lower) */	
	uint16_t 		lowerTime;						/* Lower time (sec.) */
	float				lowerExtreme;					/* Sensor alarm level (lower extreme) */
	float				upper;								/* Sensor alarm level (upper) */	
	uint16_t 		upperTime;						/* Upper time (sec.) */
	float				upperExtreme;					/* Sensor alarm level (upper extreme) */
}STCONFIG_SENSORALARM;

/* Butterfly Valve Configuration_Structure */
__packed typedef struct {
	uint8_t 	interface_type;					/* Interface Type 4-20mA or MODBUS */
	uint8_t 	slave_ID;								/* MODBUS Slave ID */
	uint32_t	scan_rate;							/* Scan Rate */
	uint8_t 	enable;									/* Valve enable */
} STCONFIG_VALVE;

/* 4-20mA Calibration Configuration_Structure */
__packed typedef struct {
	uint8_t 	calculation;						/* Calculation use Default or Calibrate Value */
	float 		cal_val;								/* Calibrate Value */
	float			real_val;								/* Real Value */
	float 		constant;								/* Constant(c)  **Calculate from cal_val */
} STCONFIG_420CAL;

extern ST_INFO								struct_info;
extern ST_STATUS 							struct_Status;
extern STCONFIG_DEV 					struct_DevConfig;
extern STCONFIG_PROPERTIES		struct_ConfigProperties;
extern STCONFIG_OPERATING			struct_ConfigOperating;
extern STCONFIG_BUZZER				struct_ConfigBuzzer;
extern STCONFIG_WORKING				struct_ConfigWorking;
extern STCONFIG_NETWORK 			struct_ConfigNetwork;
extern STCONFIG_ANALOGSENSOR 	struct_Config420Sensor[];
extern STCONFIG_ANALOGSENSOR 	struct_ConfigADCSensor[];
extern STCONFIG_MODBUSSENSOR	struct_ConfigMODBUSSensor;
extern STCONFIG_PULSESENSOR		struct_ConfigPulseSensor;
extern STCONFIG_PULSEADJ			struct_ConfigPulseADJ;
extern STCONFIG_DIGITALsIN 		struct_ConfigDIN[];
extern STCONFIG_SENSORALARM 	struct_Config420SensorAlarm[];
extern STCONFIG_SENSORALARM 	struct_ConfigADCSensorAlarm[];
extern STCONFIG_SENSORALARM 	struct_ConfigMODBUSSensorAlarm;
extern STCONFIG_SENSORALARM 	struct_ConfigPulseSensorAlarm;
extern STCONFIG_VALVE					struct_ConfigValve;
extern STCONFIG_420CAL				struct_Config420Cal[];
extern STCONFIG_420CAL 				struct_Config420ValveInCal;
extern STCONFIG_420CAL 				struct_Config420ValveOutCal;

#endif /*_SL_CONFIGSTRUCT_H*/
	





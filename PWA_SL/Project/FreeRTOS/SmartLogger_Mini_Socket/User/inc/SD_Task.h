/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	SDnMB_Task.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	9-September-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _SDNMB_TASK_H
#define _SDNMB_TASK_H

#include <stdint.h>

#include "INT_STM32F4_ProtocolComm.h"

#define LOG_CMD_READ				0x10
#define LOG_CMD_WRITE				0x20
#define LOG_CMD_SETiNDEX		0x30
#define LOG_ACK_OK					0x50
#define LOG_ACK_FAIL				0x60
#define LOG_ACK_EMPTY				0x70

/*-- For update status --*/
#define STAT_DOUT1					0x01
#define STAT_DOUT2					0x02
#define STAT_DOUT3					0x03
#define STAT_DOUT4					0x04
#define STAT_VALVE					0x10
extern uint8_t	STAT_DOUTn[];

#define SD_RETRY						3
#define	SD_STABLE_DELAY			100

typedef struct {
	uint8_t 		cmd;
	uint8_t			fid;  // Frame id for send ack
	uint32_t 		timestamp;
	uint8_t			num_status;
	STATUS_SET	status[MAX_STATUS];
	uint8_t			num_data;
	DATA_SET		data[MAX_DATA];
}ST_LOG_DATA;

typedef struct {
	uint8_t 		cmd;
	uint32_t 		timestamp;
	uint8_t			eid;
	uint32_t		edata;
}ST_LOG_EVENT;

typedef struct {
	uint8_t 		cmd;
	uint32_t 		timestamp;
	uint8_t			accd;
	uint8_t			cnfg;
	uint8_t			spec_len;
	uint8_t			spec[180];
}ST_LOG_CONFIG;

typedef struct {
	uint8_t 		stat_id;
	uint8_t			value;  // Frame id for send ack
}ST_UPDATE_STAT;

typedef struct {
	uint32_t		base_addr;
	uint32_t		config_length;
	uint8_t			config_data[161];    // Size for max config_data (Properties)
}ST_CONFIG_Q;
	
void vSDnMB_Task(void *pvParameters);
	
#endif /* _SDNMB_TASK_H */



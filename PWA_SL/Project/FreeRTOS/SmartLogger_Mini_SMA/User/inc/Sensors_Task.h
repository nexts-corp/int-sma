/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Sensors_Task.h
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#ifndef _SENSORS_TASK_H
#define _SENSORS_TASK_H

#include "main.h"

#define ERR_LOWER_BOUND		3.0           /* 4-20mA Sensor Error lower bound */
#define ERR_UPPER_BOUND		22.0          /* 4-20mA Sensor Error upper bound */

#define MAX_SENSOR				10
#define MAX_420SENSOR			6
#define MAX_ADCSENSOR			2
#define MAX_DIGItOUT			4
#define MAX_DIGItIN 			12

/*-- Pulse FlowSensor --*/
#define VOL_0001CU			(float)0.001
#define VOL_001CU				(float)0.01
#define VOL_01CU				(float)0.1
#define VOL_1CU					(float)1.0
#define VOL_10CU				(float)10.0

/*-- Valve --*/
#define VALVE_SLOPE			(float)6.25
#define VALVE_CONST			(float)-25.0

#define HIGH							1
#define LOW								0

typedef struct {
	float data420mA[MAX_420SENSOR];
	float dataADC[MAX_ADCSENSOR];
	float dataMODBUSFlow;
	float dataMODBUSPosNet;
	float dataMODBUSNegNet;
	float dataPULSEFlow;
	float dataPULSENet;
}SENSOR_DATA;

extern __IO uint8_t		_DINStatus[];
extern __IO uint8_t		_SensorStatus_420mA[];
extern __IO uint8_t 	_DataStatus_420mA[];
extern __IO uint8_t		_DataStatus_ADC[];
extern __IO uint8_t		_DataStatus_MBFlow;
extern __IO uint8_t		_DataStatus_PULSEFlow;
extern uint16_t				adcData_Ch1;
extern uint32_t				_ValveStatus;
extern float					_Valve_mA;
extern uint16_t				_adc420mA_sensor[];
extern uint16_t				_adc420mA_inputVal;
extern float					_val420mA_sensor[];

void vSensors_Task(void *pvParameters);
  
  
	
#endif /*_SENSORS_TASK_H*/




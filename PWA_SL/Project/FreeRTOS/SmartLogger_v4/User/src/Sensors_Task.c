/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	Sensors_Task.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"
#include "Sensors_Task.h"
#include "SL_ConfigStruct.h"
#include "SL_PinDefine.h"

#include "INT_STM32F4_debug.h"
#include "INT_STM32F4_Timer.h"
#include "INT_STM32F4_420RClick.h"
#include "INT_STM32F4_ProtocolComm.h"
#include "INT_STM32F4_MCP4725.h"

#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_bkpsram.h"

#define PULSE_VALUE 	2

float fSENSOR_ReadADCSensor(uint8_t ch);
float fSENSOR_420mACalibration(uint8_t index, float val420mA);

__IO bool 		flag_1sec_sensTask = false;

__IO uint8_t	_DINStatus[MAX_DIN] = {EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,
																			EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING,EV_DIN_RISING};
__IO uint8_t	_SensorStatus_420mA[MAX_420SENSOR] 	= {0,0,0,0,0,0};
__IO uint8_t 	_DataStatus_420mA[MAX_420SENSOR] 		= {0,0,0,0,0,0};
__IO uint8_t	_DataStatus_ADC[MAX_ADCSENSOR]			= {0,0};
__IO uint8_t	_DataStatus_MBFlow									= 0;
__IO uint8_t	_DataStatus_PULSEFlow								= 0;

uint16_t			adcData_Ch1 	= 0;
uint32_t			_PulseCounter = 0;
float					_PulsePeriod	= 0;
uint32_t			_ValveStatus	=	0;
float					_Valve_mA	=	0.0;
uint16_t			_adc420mA_sensor[MAX_420SENSOR] = {0,0,0,0,0,0};     // for sensor calibrate
uint16_t			_adc420mA_inputVal = 0;
float					_val420mA_sensor[MAX_420SENSOR] = {0.0,0.0,0.0,0.0,0.0,0.0};     // for sensor calibrate (only show)

extern SENSOR_DATA 				_Sensor_Data;
extern __IO uint8_t 		_index_pulseData;
extern __IO uint8_t			_PulseData_Counter;
extern float fMNG_ReadBatt(void);

extern __IO uint32_t 	_ms_counter_pulse;

void vSensors_Task(void *pvParameters) {
	
	uint8_t		i,j;
	uint32_t 	samplingPulseTimer		= 0;
	uint32_t	samplingDACTimer			= 0;
	uint32_t 	testTimer							= 0;
	
	static uint32_t 	timer420Err[MAX_420SENSOR]       		= {0,0,0,0,0,0}; 
	static bool				flag_Sensor420Err[MAX_420SENSOR]		= {false,false,false,false,false,false};
	
	static uint32_t 	timer420[MAX_420SENSOR]       			= {0,0,0,0,0,0}; 
	static uint32_t 	timer420ex[MAX_420SENSOR]       		= {0,0,0,0,0,0}; 
	static bool				flag_420Lower[MAX_420SENSOR]				= {false,false,false,false,false,false};
	static bool				flag_420LowerEx[MAX_420SENSOR]			= {false,false,false,false,false,false};
	static bool				flag_420Upper[MAX_420SENSOR]				= {false,false,false,false,false,false};
	static bool				flag_420UpperEx[MAX_420SENSOR]			= {false,false,false,false,false,false};

	static uint32_t 	timerADC[MAX_ADCSENSOR]       			= {0,0}; 
	static uint32_t 	timerADCex[MAX_ADCSENSOR]       		= {0,0}; 
	static bool				flag_ADCLower[MAX_ADCSENSOR]				= {false,false};
	static bool				flag_ADCLowerEx[MAX_ADCSENSOR]			= {false,false};
	static bool				flag_ADCUpper[MAX_ADCSENSOR]				= {false,false};
	static bool				flag_ADCUpperEx[MAX_ADCSENSOR]			= {false,false};

	static uint32_t 	timerMBFlow       			= 0; 
	static uint32_t 	timerMBFlowEx       		= 0; 
	static bool				flag_MBFlowLower				= false;
	static bool				flag_MBFlowLowerEx			= false;
	static bool				flag_MBFlowUpper				= false;
	static bool				flag_MBFlowUpperEx			= false;

	static uint32_t 	timerPULSEFlow      		= 0; 
	static uint32_t 	timerPULSEFlowEx       	= 0; 
	static bool				flag_PULSEFlowLower			= false;
	static bool				flag_PULSEFlowLowerEx		= false;
	static bool				flag_PULSEFlowUpper			= false;
	static bool				flag_PULSEFlowUpperEx		= false;
	
	uint8_t						dinState 								= HIGH;
	static uint8_t		tmp_dinState[MAX_DIN] 	= {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
	float 						pulse_buf[PULSE_VALUE] = {0.0, 0.0};
	float 						pulse_buf_sort[PULSE_VALUE] = {0.0, 0.0};
//	float 						val_420mA				= 0.0;
	float 						sum_flow 				= 0.0;
	float 						tmp_flow 				= 0.0;
	float 						valve_percent		= 0.0;
	uint32_t					valve_open	= 0;
	
	float 						Flowrate_Period		=	0.0;
	uint8_t						Flow_CalculateMethod = 0;
	float							swap;
	
	printDebug_Semphr(DESCRIBE, "[SENSOR_TASK]: Running... \r\n");
	
	_PulseCounter = 0;															/* Reset pulse counter */
	ucTIMER_SetTimer(&samplingPulseTimer, 2);				/* Set sampling pulse timer */
	ucTIMER_SetTimer(&samplingDACTimer, 1);				/* Set sampling pulse timer */
	
	TM_BKPSRAM_Init();
	
	while(1) {
		
		/* Control Valve Interface 4-20mA ---------------------------------------*/
		if(struct_ConfigValve.enable && (struct_ConfigValve.interface_type == VALVE_420MA)) {
			if(xQueueReceive(Queue_CtrlValve, &valve_open, 0)) {
				v420TClick_ControlValve(valve_open);
			}
		}
		
		/* Update data PULSE FlowSensor -----------------------------------------*/
		if(struct_ConfigPulseSensor.sensor_enable == SENSOR_ENABLE) {
			if(cTIMER_CheckTimerExceed(samplingPulseTimer)) {
				ucTIMER_SetTimer(&samplingPulseTimer, 60);
				/*-- Calculate water volume for 1 sec --*/
				if(Flow_CalculateMethod == 0) {
					if(struct_ConfigPulseSensor.vol_per_pulse == VOL_10CU_ID) {					/* 10 m³/Pulse */
						tmp_flow = (float)_PulseCounter*VOL_10CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_1CU_ID) {		/* 1 m³/Pulse */
						tmp_flow = (float)_PulseCounter*VOL_1CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_01CU_ID) {		/* 0.1 m³/Pulse */
						tmp_flow = (float)_PulseCounter*VOL_01CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_001CU_ID){		/* 0.01 m³/Pulse **default */
						tmp_flow = (float)_PulseCounter*VOL_001CU;
					}else {																															/* 0.001 m³/Pulse */
						tmp_flow = (float)_PulseCounter*VOL_0001CU;
					}
					_PulseCounter = 0;
	 				_Sensor_Data.dataPULSENet += tmp_flow;															/* Increase flow accumulate */
					
					/*-- Convert to water flow from m³/min to m³/hr --*/
					if(_PulseData_Counter == 0) {																				/* For first times */
						/* Read from backup SRAM location PulseFlow value	*/
						pulse_buf[_index_pulseData] = TM_BKPSRAM_Read32(0x00);
					}else {																															/* Other times */
						/* m³/min to m³/hr */
						pulse_buf[_index_pulseData] = tmp_flow*60.0;
					}
					
					/*-- Update amount of data_pulse --*/
					if(_PulseData_Counter < PULSE_VALUE) {
						_PulseData_Counter++;
					}
					
					/*-- Calculate average --*/
					sum_flow = 0.0;
					for(i = 0; i < _PulseData_Counter; i++) {
						sum_flow += pulse_buf[i];
					}
					_Sensor_Data.dataPULSEFlow = sum_flow/(float)(_PulseData_Counter);
 					_Sensor_Data.dataPULSEFlow = (_Sensor_Data.dataPULSEFlow * struct_ConfigPulseADJ.multiplier_value);
					
				}
				else if(Flow_CalculateMethod == 1) {
					if(_PulsePeriod<1){
						Flowrate_Period = 0;
					}else {
						Flowrate_Period = (3600/(_PulsePeriod / 1000000));											/*  pulse/min*/
					}
				
					if(struct_ConfigPulseSensor.vol_per_pulse == VOL_10CU_ID) {					/* 10 m³/Period */
						tmp_flow = Flowrate_Period*VOL_10CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_1CU_ID) {		/* 1 m³/Period */
						tmp_flow = Flowrate_Period*VOL_1CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_01CU_ID) {		/* 0.1 m³/Period */
						tmp_flow = Flowrate_Period*VOL_01CU;
					}else if(struct_ConfigPulseSensor.vol_per_pulse == VOL_001CU_ID) {	/* 0.01 m³/Period **default */
						tmp_flow = Flowrate_Period*VOL_001CU;
					}else {																															/* 0.001 m³/Period */
						tmp_flow = Flowrate_Period*VOL_0001CU;
					}
					
					_Sensor_Data.dataPULSENet += tmp_flow;															/* Increase flow accumulate */
					pulse_buf[_index_pulseData] = tmp_flow;
					
					_PulseCounter = 0;
					/*-- Update amount of data_pulse --*/
					if(_PulseData_Counter < PULSE_VALUE) {
						_PulseData_Counter++;
						
						/*-- Calculate average --*/
						printDebug_Semphr(INFO, "[SENSOR_TASK]: pulse buf : ");
						sum_flow = 0.0;
						for(i = 0; i < _PulseData_Counter; i++) {
							sum_flow += pulse_buf[i];
							printDebug_Semphr(INFO, "%0.2f ", pulse_buf[i]);
						}
						printDebug_Semphr(INFO, "\r\n");
						
						_Sensor_Data.dataPULSEFlow = sum_flow/(float)(_PulseData_Counter);
					}else {
						memcpy(pulse_buf_sort, pulse_buf, PULSE_VALUE*sizeof(float));
						
						for(i = 0; i < _PulseData_Counter; i++) {
							for(j = 0 ; j < _PulseData_Counter - i - 1; j++)
							{
								if(pulse_buf_sort[j] > pulse_buf_sort[j+1]) /* For decreasing order use < */
								{
									swap       			= pulse_buf_sort[j];
									pulse_buf_sort[j]   	= pulse_buf_sort[j+1];
									pulse_buf_sort[j+1] 	= swap;
								}
							}
						}
						printDebug_Semphr(INFO, "[SENSOR_TASK]: pulse buf : ");
						for(i = 0; i < _PulseData_Counter; i++) {
							printDebug_Semphr(INFO, "%0.2f ", pulse_buf[i]);
						}
						
						printDebug_Semphr(INFO, "\r\n");
						printDebug_Semphr(INFO, "[SENSOR_TASK]: pulse buf sort : ");
						sum_flow = 0.0;
						for(i = 1; i < _PulseData_Counter-1; i++) {
							sum_flow += pulse_buf_sort[i];
							printDebug_Semphr(INFO, "%0.2f ", pulse_buf_sort[i]);
						}
						printDebug_Semphr(INFO, "\r\n");
						
						_Sensor_Data.dataPULSEFlow = sum_flow/(float)(_PulseData_Counter-2);
					}
					
					/* Calculate use Period */
// 					_Sensor_Data.dataPULSEFlow = sum_flow/(float)(_PulseData_Counter);
					_Sensor_Data.dataPULSEFlow = (_Sensor_Data.dataPULSEFlow * struct_ConfigPulseADJ.multiplier_value);
					
				}
				
				/* Write PulseFlow value at backup SRAM location 0x00 */
        TM_BKPSRAM_Write32(0x00, _Sensor_Data.dataPULSEFlow);
				
				printDebug_Semphr(INFO, "[SENSOR_TASK]: volume: %0.4f,  Flow_Rate: %0.4f,  Net: (%0.4f + %d),  (%d,%d), pulse/min: %0.4f ,period: (%0.2f) \r\n", 
																											tmp_flow,
																											_Sensor_Data.dataPULSEFlow,				
																											_Sensor_Data.dataPULSENet,
																											struct_ConfigPulseSensor.net_total, 
																											_index_pulseData, _PulseData_Counter,
																											Flowrate_Period, _PulsePeriod);
				
				/* Update array index */
				_index_pulseData++;
				if(_index_pulseData >= PULSE_VALUE) {
					_index_pulseData = 0;
				}
			}
		}/*Pulse sensor enable*/
		
		
		
		if(flag_1sec_sensTask) {
			/* Un-Check Flag 1 sec */
			flag_1sec_sensTask = false;
			
			/* Update Digital Input Status ------------------------------------------*/
			for(i = 0; i < MAX_DIN; i++) {
				dinState = GPIO_ReadInputDataBit((GPIO_TypeDef*)DINn_PORT[i], DINn_PIN[i]);
				if(dinState != tmp_dinState[i]) {
					if(dinState == HIGH) {
						_DINStatus[i] = EV_DIN_RISING;
					}else {
						_DINStatus[i] = EV_DIN_FALLING;
					}
					tmp_dinState[i] = dinState;
				}
			}
			
			/* Update data 4-20mA Sensor --------------------------------------------*/
			for(i = 0; i < MAX_420SENSOR; i++) {
				if(struct_Config420Sensor[i].sensorEnable == SENSOR_ENABLE) {
					_adc420mA_sensor[i] = us420RClick_GetADCValue(IN420MA_CH[i]);
					if(struct_Config420Cal[i].calculation == CAL_USE_CALIBRATE) {
						_val420mA_sensor[i] = (((float)_adc420mA_sensor[i]*102.4)/(4095.0*4.99)) + struct_Config420Cal[i].constant;
					}else {
						/* Use default */
						_val420mA_sensor[i] = ((float)_adc420mA_sensor[i]*102.4)/(4095.0*4.99);
					}
					
					if((struct_Config420Sensor[i].dataUnit != UNIT_MA) && (struct_Config420Sensor[i].numCalibrate >= 2)) {
						_Sensor_Data.data420mA[i] = fSENSOR_420mACalibration(i, _val420mA_sensor[i]);
					}else {
						_Sensor_Data.data420mA[i] = _val420mA_sensor[i];
					}
					
					/*-- Check Sensor Error --*/
					if(_val420mA_sensor[i] < ERR_LOWER_BOUND) {        					/* if mA_Value less than about 3 mA -> Sensor Error */
						if(!flag_Sensor420Err[i]) {
							flag_Sensor420Err[i] = true;
							ucTIMER_SetTimer(&timer420Err[i], 5);
						}else {
							if(cTIMER_CheckTimerExceed(timer420Err[i])) {
								_SensorStatus_420mA[i] |= EV_SENSOR_ERR;
							}
						}
					}else {
						flag_Sensor420Err[i] = false;
						_SensorStatus_420mA[i] &= (~EV_SENSOR_ERR);
					}
				}
			}
			
			/* Update data ADC Sensor --------------------------------------------*/
			for(i = 0; i < MAX_ADCSENSOR; i++) {
				if(struct_ConfigADCSensor[i].sensorEnable == SENSOR_ENABLE) {
					/* Read data from sensor */
					_Sensor_Data.dataADC[i] = fSENSOR_ReadADCSensor(i);
				}
			}
			
			/* Update valve status --------------------------------------------------*/
			if(struct_ConfigValve.enable == SENSOR_ENABLE) {
				if(struct_ConfigValve.interface_type == VALVE_420MA) {
					_adc420mA_inputVal = us420RClick_GetADCValue(IN420MA_VALVE);
					/* Get mA Value */
					if(struct_Config420ValveInCal.calculation == CAL_USE_CALIBRATE) {
						_Valve_mA = (((float)_adc420mA_inputVal*102.4)/(4095.0*4.99)) + struct_Config420ValveInCal.constant;
					}else {
						/* Use default */
						_Valve_mA = ((float)_adc420mA_inputVal*102.4)/(4095.0*4.99);
					}
					/* Convert mA value to % valve open */
					if(_Valve_mA <= 4.0) {
						_ValveStatus = 0; 					// 0%
					}else if(_Valve_mA >= 20.0) {
						_ValveStatus = 100;     		// 100%
					}else {
						valve_percent = (VALVE_SLOPE*_Valve_mA) + VALVE_CONST;
						_ValveStatus = (uint32_t)valve_percent;
					}
				}else if(struct_ConfigValve.interface_type == VALVE_MODBUS) {
					/* Read data from valve */
				}
			}
			
			/* Update data status 4-20mA Sensor -------------------------------------*/
			for(i = 0; i < MAX_420SENSOR; i++) {
				if(struct_Config420Sensor[i].sensorEnable == SENSOR_ENABLE) {
					/*-- Lower --*/
					if(_Sensor_Data.data420mA[i] < struct_Config420SensorAlarm[i].lower) {
						if(!flag_420Lower[i]) {
							flag_420Lower[i] = true;
							ucTIMER_SetTimer(&timer420[i], struct_Config420SensorAlarm[i].lowerTime);
						}else {
							if(cTIMER_CheckTimerExceed(timer420[i])) {
								_DataStatus_420mA[i] |= EV_LOWER;
							}
						}
					}else {
						flag_420Lower[i] = false;
						_DataStatus_420mA[i] &= (~EV_LOWER);
					}
					/*-- Lower Extreme --*/ 
					if(_Sensor_Data.data420mA[i] < struct_Config420SensorAlarm[i].lowerExtreme) {
						if(!flag_420LowerEx[i]) {
							flag_420LowerEx[i] = true;
							ucTIMER_SetTimer(&timer420ex[i], 2);
						}else {
							if(cTIMER_CheckTimerExceed(timer420ex[i])) {
								_DataStatus_420mA[i] |= EV_LOWER_EXTREME;
							}
						}
					}else {
						flag_420LowerEx[i] = false;
						_DataStatus_420mA[i] &= (~EV_LOWER_EXTREME);
					}
					/*-- Upper --*/
					if(_Sensor_Data.data420mA[i] > struct_Config420SensorAlarm[i].upper) {
						if(!flag_420Upper[i]) {
							flag_420Upper[i] = true;
							ucTIMER_SetTimer(&timer420[i], struct_Config420SensorAlarm[i].upperTime);
						}else {
							if(cTIMER_CheckTimerExceed(timer420[i])) {
								_DataStatus_420mA[i] |= EV_UPPER;
							}
						}
					}else {
						flag_420Upper[i] = false;
						_DataStatus_420mA[i] &= (~EV_UPPER);
					}
					/*-- Upper Extreme --*/ 
					if(_Sensor_Data.data420mA[i] > struct_Config420SensorAlarm[i].upperExtreme) {
						if(!flag_420UpperEx[i]) {
							flag_420UpperEx[i] = true;
							ucTIMER_SetTimer(&timer420ex[i], 2);
						}else {
							if(cTIMER_CheckTimerExceed(timer420ex[i])) {
								_DataStatus_420mA[i] |= EV_UPPER_EXTREME;
							}
						}
					}else {
						flag_420UpperEx[i] = false;
						_DataStatus_420mA[i] &= (~EV_UPPER_EXTREME);
					}
				}
			}
			
			/* Update data status ADCSensor -------------------------------------*/
			for(i = 0; i < MAX_ADCSENSOR; i++) {
				if(struct_ConfigADCSensor[i].sensorEnable == SENSOR_ENABLE) {
					/*-- Lower --*/
					if(_Sensor_Data.dataADC[i] < struct_ConfigADCSensorAlarm[i].lower) {
						if(!flag_ADCLower[i]) {
							flag_ADCLower[i] = true;
							ucTIMER_SetTimer(&timerADC[i], struct_ConfigADCSensorAlarm[i].lowerTime);
						}else {
							if(cTIMER_CheckTimerExceed(timerADC[i])) {
								_DataStatus_ADC[i] |= EV_LOWER;
							}
						}
					}else {
						flag_ADCLower[i] = false;
						_DataStatus_ADC[i] &= (~EV_LOWER);
					}
					/*-- Lower Extreme --*/ 
					if(_Sensor_Data.dataADC[i] < struct_ConfigADCSensorAlarm[i].lowerExtreme) {
						if(!flag_ADCLowerEx[i]) {
							flag_ADCLowerEx[i] = true;
							ucTIMER_SetTimer(&timerADCex[i], 2);
						}else {
							if(cTIMER_CheckTimerExceed(timerADCex[i])) {
								_DataStatus_ADC[i] |= EV_LOWER_EXTREME;
							}
						}
					}else {
						flag_ADCLowerEx[i] = false;
						_DataStatus_ADC[i] &= (~EV_LOWER_EXTREME);
					}
					/*-- Upper --*/
					if(_Sensor_Data.dataADC[i] > struct_ConfigADCSensorAlarm[i].upper) {
						if(!flag_ADCUpper[i]) {
							flag_ADCUpper[i] = true;
							ucTIMER_SetTimer(&timerADC[i], struct_ConfigADCSensorAlarm[i].upperTime);
						}else {
							if(cTIMER_CheckTimerExceed(timerADC[i])) {
								_DataStatus_ADC[i] |= EV_UPPER;
							}
						}
					}else {
						flag_ADCUpper[i] = false;
						_DataStatus_ADC[i] &= (~EV_UPPER);
					}
					/*-- Upper Extreme --*/ 
					if(_Sensor_Data.dataADC[i] > struct_ConfigADCSensorAlarm[i].upperExtreme) {
						if(!flag_ADCUpperEx[i]) {
							flag_ADCUpperEx[i] = true;
							ucTIMER_SetTimer(&timerADCex[i], 2);
						}else {
							if(cTIMER_CheckTimerExceed(timerADCex[i])) {
								_DataStatus_ADC[i] |= EV_UPPER_EXTREME;
							}
						}
					}else {
						flag_ADCUpperEx[i] = false;
						_DataStatus_ADC[i] &= (~EV_UPPER_EXTREME);
					}
				}
			}
			
			/* Update data status MODBUS FlowSensor ---------------------------------*/
			if(struct_ConfigMODBUSSensor.sensor_enable == SENSOR_ENABLE) {
				/*-- Lower --*/
				if(_Sensor_Data.dataMODBUSFlow < struct_ConfigMODBUSSensorAlarm.lower) {
					if(!flag_MBFlowLower) {
						flag_MBFlowLower = true;
						ucTIMER_SetTimer(&timerMBFlow, struct_ConfigMODBUSSensorAlarm.lowerTime);
					}else {
						if(cTIMER_CheckTimerExceed(timerMBFlow)) {
							_DataStatus_MBFlow |= EV_LOWER;
						}
					}
				}else {
					flag_MBFlowLower = false;
					_DataStatus_MBFlow &= (~EV_LOWER);
				}
				/*-- Lower Extreme --*/ 
				if(_Sensor_Data.dataMODBUSFlow < struct_ConfigMODBUSSensorAlarm.lowerExtreme) {
					if(!flag_MBFlowLowerEx) {
						flag_MBFlowLowerEx = true;
						ucTIMER_SetTimer(&timerMBFlowEx, 2);
					}else {
						if(cTIMER_CheckTimerExceed(timerMBFlowEx)) {
							_DataStatus_MBFlow |= EV_LOWER_EXTREME;
						}
					}
				}else {
					flag_MBFlowLowerEx = false;
					_DataStatus_MBFlow &= (~EV_LOWER_EXTREME);
				}
				/*-- Upper --*/
				if(_Sensor_Data.dataMODBUSFlow > struct_ConfigMODBUSSensorAlarm.upper) {
					if(!flag_MBFlowUpper) {
						flag_MBFlowUpper = true;
						ucTIMER_SetTimer(&timerMBFlow, struct_ConfigMODBUSSensorAlarm.upperTime);
					}else {
						if(cTIMER_CheckTimerExceed(timerMBFlow)) {
							_DataStatus_MBFlow |= EV_UPPER;
						}
					}
				}else {
					flag_MBFlowUpper = false;
					_DataStatus_MBFlow &= (~EV_UPPER);
				}
				/*-- Upper Extreme --*/ 
				if(_Sensor_Data.dataMODBUSFlow > struct_ConfigMODBUSSensorAlarm.upperExtreme) {
					if(!flag_MBFlowUpperEx) {
						flag_MBFlowUpperEx = true;
						ucTIMER_SetTimer(&timerMBFlowEx, 2);
					}else {
						if(cTIMER_CheckTimerExceed(timerMBFlowEx)) {
							_DataStatus_MBFlow |= EV_UPPER_EXTREME;
						}
					}
				}else {
					flag_MBFlowUpperEx = false;
					_DataStatus_MBFlow &= (~EV_UPPER_EXTREME);
				}
			}

			/* Update data status PULSE FlowSensor ----------------------------------*/
			if(struct_ConfigPulseSensor.sensor_enable == SENSOR_ENABLE) {
				/*-- Lower --*/
				if(_Sensor_Data.dataPULSEFlow < struct_ConfigPulseSensorAlarm.lower) {
					if(!flag_PULSEFlowLower) {
						flag_PULSEFlowLower = true;
						ucTIMER_SetTimer(&timerPULSEFlow, struct_ConfigPulseSensorAlarm.lowerTime);
					}else {
						if(cTIMER_CheckTimerExceed(timerPULSEFlow)) {
							_DataStatus_PULSEFlow |= EV_LOWER;
						}
					}
				}else {
					flag_PULSEFlowLower = false;
					_DataStatus_PULSEFlow &= (~EV_LOWER);
				}
				/*-- Lower Extreme --*/ 
				if(_Sensor_Data.dataPULSEFlow < struct_ConfigPulseSensorAlarm.lowerExtreme) {
					if(!flag_PULSEFlowLowerEx) {
						flag_PULSEFlowLowerEx = true;
						ucTIMER_SetTimer(&timerPULSEFlowEx, 2);
					}else {
						if(cTIMER_CheckTimerExceed(timerPULSEFlowEx)) {
							_DataStatus_PULSEFlow |= EV_LOWER_EXTREME;
						}
					}
				}else {
					flag_PULSEFlowLowerEx = false;
					_DataStatus_PULSEFlow &= (~EV_LOWER_EXTREME);
				}
				/*-- Upper --*/
				if(_Sensor_Data.dataPULSEFlow > struct_ConfigPulseSensorAlarm.upper) {
					if(!flag_PULSEFlowUpper) {
						flag_PULSEFlowUpper = true;
						ucTIMER_SetTimer(&timerPULSEFlow, struct_ConfigPulseSensorAlarm.upperTime);
					}else {
						if(cTIMER_CheckTimerExceed(timerPULSEFlow)) {
							_DataStatus_PULSEFlow |= EV_UPPER;
						}
					}
				}else {
					flag_PULSEFlowUpper = false;
					_DataStatus_PULSEFlow &= (~EV_UPPER);
				}
				/*-- Upper Extreme --*/ 
				if(_Sensor_Data.dataPULSEFlow > struct_ConfigPulseSensorAlarm.upperExtreme) {
					if(!flag_PULSEFlowUpperEx) {
						flag_PULSEFlowUpperEx = true;
						ucTIMER_SetTimer(&timerPULSEFlowEx, 2);
					}else {
						if(cTIMER_CheckTimerExceed(timerPULSEFlowEx)) {
							_DataStatus_PULSEFlow |= EV_UPPER_EXTREME;
						}
					}
				}else {
					flag_PULSEFlowUpperEx = false;
					_DataStatus_PULSEFlow &= (~EV_UPPER_EXTREME);
				}
			}

		}/*Flag 1 sec*/
		
	}/*Main Loop*/
}/*Sensor_Task*/


/******************************************************************************/
/*            STM32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles External line 3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void) {
	
  if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
		
    if(struct_ConfigPulseSensor.sensor_enable == SENSOR_ENABLE) {
			_PulseCounter += 1;
			
 			_PulsePeriod = _ms_counter_pulse;
			_ms_counter_pulse = 0;
			
		}
    
    /* Clear the EXTI line 3 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}
/*==========================================================================================*/
/**
  * @brief  Read data ADC Sensor
  * @param  
  * @retval ADC data
  */
float fSENSOR_ReadADCSensor(uint8_t ch) {
	
	uint8_t 	i = 0;
	uint32_t 	adc_sum = 0;
	uint32_t 	adc_value = 0;
	uint32_t 	adc_buf[18];
	uint32_t  adc_max = 0, adc_min = 0x0FFF;
	float			volt_1 = 0.0;
	float			volt_2 = 0.0;
	float 		adc_vref = struct_DevConfig.adc_Vref;
	
	for(i = 0; i < 18; i++) {
		switch(ch) {
			case 0:
				adc_buf[i] = TM_ADC_Read(ADC1, AIN3_ADC1_CH); 				/* Add ADC value to checkup array */
				break;
			case 1:
				adc_buf[i] = TM_ADC_Read(ADC1, AIN4_ADC2_CH); 				/* Add ADC value to checkup array */
				break;
			default:
				break;
		}
		adc_sum += adc_buf[i];																		/* Sum ADC values */
		if(adc_buf[i] < adc_min) {																/* Search the array for MIN value */
			adc_min = adc_buf[i];
		}
		if(adc_buf[i] > adc_max) {																/* Search the array for MAX value */
			adc_max = adc_buf[i];
		}
		delay_ms(5);
	}
	adc_sum -= adc_min;																					/* Remove the MIN value from sum */
	adc_sum -= adc_max;																					/* Remove the MAX value from sum */
	adc_sum >>= 4;																							/* Shift bits 4 places - divide the sum by 16 (2 to the power of 4) */
	adc_value = adc_sum;
//	adc_value = (adc_sum + (adc_sum/100));											/* Final value correction ~1% (Shunt resistor tolerance) */
	
	volt_1 = ((float)adc_value/4095.0)*adc_vref;
	volt_2 = (10.0/adc_vref)*volt_1;
	
//	printDebug_Semphr(INFO, "[%d] ADC: %d, Volt_1: %0.4f, Volt_2: %0.4f \r\n", ch+1, adc_value, volt_1, volt_2);
	
	return volt_2;
}
/*==========================================================================================*/
/**
  * @brief  Calculate linear equation
  * @param  
  * @retval Caculated value
  */
float fSENSOR_420mACalibration(uint8_t index, float val420mA) {
	
	float result 		= 0.0;
	float slope 		= 0.0;
	float constant 	= 0.0;
	
	switch(struct_Config420Sensor[index].numCalibrate) {
		case 2:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x2) {
				result = struct_Config420Sensor[index].y2;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
			}
			break;
		case 3:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x3) {
				result = struct_Config420Sensor[index].y3;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
		case 4:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x3) && (val420mA <= struct_Config420Sensor[index].x4)) {
				slope 		= (struct_Config420Sensor[index].y4 - struct_Config420Sensor[index].y3) / ((struct_Config420Sensor[index].x4 - struct_Config420Sensor[index].x3));
				constant 	= struct_Config420Sensor[index].y4 - (slope * struct_Config420Sensor[index].x4);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x4) {
				result = struct_Config420Sensor[index].y4;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
		case 5:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x3) && (val420mA <= struct_Config420Sensor[index].x4)) {
				slope 		= (struct_Config420Sensor[index].y4 - struct_Config420Sensor[index].y3) / ((struct_Config420Sensor[index].x4 - struct_Config420Sensor[index].x3));
				constant 	= struct_Config420Sensor[index].y4 - (slope * struct_Config420Sensor[index].x4);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x4) && (val420mA <= struct_Config420Sensor[index].x5)) {
				slope 		= (struct_Config420Sensor[index].y5 - struct_Config420Sensor[index].y4) / ((struct_Config420Sensor[index].x5 - struct_Config420Sensor[index].x4));
				constant 	= struct_Config420Sensor[index].y5 - (slope * struct_Config420Sensor[index].x5);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x5) {
				result = struct_Config420Sensor[index].y5;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
		case 6:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x3) && (val420mA <= struct_Config420Sensor[index].x4)) {
				slope 		= (struct_Config420Sensor[index].y4 - struct_Config420Sensor[index].y3) / ((struct_Config420Sensor[index].x4 - struct_Config420Sensor[index].x3));
				constant 	= struct_Config420Sensor[index].y4 - (slope * struct_Config420Sensor[index].x4);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x4) && (val420mA <= struct_Config420Sensor[index].x5)) {
				slope 		= (struct_Config420Sensor[index].y5 - struct_Config420Sensor[index].y4) / ((struct_Config420Sensor[index].x5 - struct_Config420Sensor[index].x4));
				constant 	= struct_Config420Sensor[index].y5 - (slope * struct_Config420Sensor[index].x5);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x5) && (val420mA <= struct_Config420Sensor[index].x6)) {
				slope 		= (struct_Config420Sensor[index].y6 - struct_Config420Sensor[index].y5) / ((struct_Config420Sensor[index].x6 - struct_Config420Sensor[index].x5));
				constant 	= struct_Config420Sensor[index].y6 - (slope * struct_Config420Sensor[index].x6);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x6) {
				result = struct_Config420Sensor[index].y6;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
		case 7:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x3) && (val420mA <= struct_Config420Sensor[index].x4)) {
				slope 		= (struct_Config420Sensor[index].y4 - struct_Config420Sensor[index].y3) / ((struct_Config420Sensor[index].x4 - struct_Config420Sensor[index].x3));
				constant 	= struct_Config420Sensor[index].y4 - (slope * struct_Config420Sensor[index].x4);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x4) && (val420mA <= struct_Config420Sensor[index].x5)) {
				slope 		= (struct_Config420Sensor[index].y5 - struct_Config420Sensor[index].y4) / ((struct_Config420Sensor[index].x5 - struct_Config420Sensor[index].x4));
				constant 	= struct_Config420Sensor[index].y5 - (slope * struct_Config420Sensor[index].x5);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x5) && (val420mA <= struct_Config420Sensor[index].x6)) {
				slope 		= (struct_Config420Sensor[index].y6 - struct_Config420Sensor[index].y5) / ((struct_Config420Sensor[index].x6 - struct_Config420Sensor[index].x5));
				constant 	= struct_Config420Sensor[index].y6 - (slope * struct_Config420Sensor[index].x6);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x6) && (val420mA <= struct_Config420Sensor[index].x7)) {
				slope 		= (struct_Config420Sensor[index].y7 - struct_Config420Sensor[index].y6) / ((struct_Config420Sensor[index].x7 - struct_Config420Sensor[index].x6));
				constant 	= struct_Config420Sensor[index].y7 - (slope * struct_Config420Sensor[index].x7);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x7) {
				result = struct_Config420Sensor[index].y7;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
		case 8:
			if(val420mA < struct_Config420Sensor[index].x1) {
				result = struct_Config420Sensor[index].y1;
			}else if((val420mA >= struct_Config420Sensor[index].x1) && (val420mA <= struct_Config420Sensor[index].x2)) {
				slope 		= (struct_Config420Sensor[index].y2 - struct_Config420Sensor[index].y1) / ((struct_Config420Sensor[index].x2 - struct_Config420Sensor[index].x1));
				constant 	= struct_Config420Sensor[index].y2 - (slope * struct_Config420Sensor[index].x2);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x2) && (val420mA <= struct_Config420Sensor[index].x3)) {
				slope 		= (struct_Config420Sensor[index].y3 - struct_Config420Sensor[index].y2) / ((struct_Config420Sensor[index].x3 - struct_Config420Sensor[index].x2));
				constant 	= struct_Config420Sensor[index].y3 - (slope * struct_Config420Sensor[index].x3);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x3) && (val420mA <= struct_Config420Sensor[index].x4)) {
				slope 		= (struct_Config420Sensor[index].y4 - struct_Config420Sensor[index].y3) / ((struct_Config420Sensor[index].x4 - struct_Config420Sensor[index].x3));
				constant 	= struct_Config420Sensor[index].y4 - (slope * struct_Config420Sensor[index].x4);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x4) && (val420mA <= struct_Config420Sensor[index].x5)) {
				slope 		= (struct_Config420Sensor[index].y5 - struct_Config420Sensor[index].y4) / ((struct_Config420Sensor[index].x5 - struct_Config420Sensor[index].x4));
				constant 	= struct_Config420Sensor[index].y5 - (slope * struct_Config420Sensor[index].x5);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x5) && (val420mA <= struct_Config420Sensor[index].x6)) {
				slope 		= (struct_Config420Sensor[index].y6 - struct_Config420Sensor[index].y5) / ((struct_Config420Sensor[index].x6 - struct_Config420Sensor[index].x5));
				constant 	= struct_Config420Sensor[index].y6 - (slope * struct_Config420Sensor[index].x6);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x6) && (val420mA <= struct_Config420Sensor[index].x7)) {
				slope 		= (struct_Config420Sensor[index].y7 - struct_Config420Sensor[index].y6) / ((struct_Config420Sensor[index].x7 - struct_Config420Sensor[index].x6));
				constant 	= struct_Config420Sensor[index].y7 - (slope * struct_Config420Sensor[index].x7);
				result 		= (slope * val420mA) + constant;
			}else if((val420mA > struct_Config420Sensor[index].x7) && (val420mA <= struct_Config420Sensor[index].x8)) {
				slope 		= (struct_Config420Sensor[index].y8 - struct_Config420Sensor[index].y7) / ((struct_Config420Sensor[index].x8 - struct_Config420Sensor[index].x7));
				constant 	= struct_Config420Sensor[index].y8 - (slope * struct_Config420Sensor[index].x8);
				result 		= (slope * val420mA) + constant;
			}else if(val420mA > struct_Config420Sensor[index].x8) {
				result = struct_Config420Sensor[index].y8;
			}else {
				printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: %0.4f mA out of calibrate range \r\n", val420mA);
				result = val420mA;
			}
			break;
			
		default:
			printDebug_Semphr(MINOR_ERR, "<SENSOR_420mACalibration>: Number of Calibrate(%d) out of range \r\n", struct_Config420Sensor[index].numCalibrate);
			break;
	}
	return result;
}
/*==========================================================================================*/
/**
  * @brief  Calculate linear equation
  * @param  
  * @retval Caculated value
  */
float fSENSOR_ADCCalibration(uint8_t index, float val420mA) {
	
	float result;
	float adc_vref = struct_DevConfig.adc_Vref;
	
}
/*==========================================================================================*/

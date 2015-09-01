/**
  ******************************************************************************
  * @file    INT_STM32F4_adc.c
  * @author  Intelenics Co.,Ltd
  * @version V1.0
  * @date    2-January-2014
  * @brief   This file contain fucntion to config ADC with DMA
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "INT_STM32F4_adc.h"
#include "SL_PinDefine.h"

/* Private define ------------------------------------------------------------*/
#define NUM_ADC_CH		5

/* Global variables ----------------------------------------------------------*/
__IO uint32_t ADCValue[NUM_ADC_CH];

/**
  * @brief  ADC1 scan channel 1,8,9,11,12,14 with DMA configuration
  * @param  None
  * @retval None
  */
void ADC1_SCAN_CH_DMA_Config(void) {
	
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC1, DMA2 and GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// TEST
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* DMA2 Stream0 channel 0 configuration *************************************/
  DMA_InitStructure.DMA_Channel 								= DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr 			= (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr	 				= (uint32_t)&ADCValue[0];
  DMA_InitStructure.DMA_DIR 										= DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize 							= NUM_ADC_CH;
  DMA_InitStructure.DMA_PeripheralInc 					= DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc 							= DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize 			= DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize 					= DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode 										= DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority 								= DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode 								= DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold 					= DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst 						= DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst 				= DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC1 Channel pin as analog input *****************************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	
	/* Battery Voltage */
	GPIO_InitStructure.GPIO_Pin = AIN1_BATT_PIN;
  GPIO_Init(AIN1_BATT_PORT, &GPIO_InitStructure);
	/* Solar Cell Voltage */
	GPIO_InitStructure.GPIO_Pin = AIN2_SOLARCELL_PIN;
  GPIO_Init(AIN2_SOLARCELL_PORT, &GPIO_InitStructure);
	/* 0-5 VDC */
	GPIO_InitStructure.GPIO_Pin = AIN3_ADC1_PIN;
  GPIO_Init(AIN3_ADC1_PORT, &GPIO_InitStructure);
	/* 0-10 VDC */
	GPIO_InitStructure.GPIO_Pin = AIN4_ADC2_PIN;
  GPIO_Init(AIN4_ADC2_PORT, &GPIO_InitStructure);
	// TEST
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode 							= ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler 				= ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode 		= ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay 	= ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution 							= ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode 						= ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode 			= ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge 		= ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign 							= ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion 				= NUM_ADC_CH;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel 1,4,5,6,7,8 configuration ***************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_112Cycles);
	ADC_RegularChannelConfig(ADC1, AIN1_BATT_CH, 2, ADC_SampleTime_112Cycles);
	ADC_RegularChannelConfig(ADC1, AIN2_SOLARCELL_CH, 3, ADC_SampleTime_112Cycles);
	ADC_RegularChannelConfig(ADC1, AIN3_ADC1_CH, 4, ADC_SampleTime_112Cycles);
	ADC_RegularChannelConfig(ADC1, AIN4_ADC2_CH, 5, ADC_SampleTime_112Cycles);
	// TEST
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 6, ADC_SampleTime_112Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 7, ADC_SampleTime_112Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 8, ADC_SampleTime_112Cycles);
	
	ADC_EOCOnEachRegularChannelCmd(ADC1, DISABLE);

	/* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
	
	ADC_TempSensorVrefintCmd(ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
	
}


/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_watchdog.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	26-June-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "stm32f4xx.h"
#include "INT_STM32F4_watchdog.h"
#include "INT_STM32F4_debug.h"

__IO uint32_t LsiFreq = 0;
__IO uint32_t _CaptureNumber = 0, _PeriodValue = 0;
	
/*==========================================================================================*/
/**
  * @brief  Initial Independent Watchdog.
  * @param  None
  * @retval None
  */
void vWDG_IntialIWDG(void) {
	
	/* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
  LsiFreq = uiWDG_GetLSIFrequency();
	
//	printDebug(INFO, "LSI Freq: %d \r\n", LsiFreq);

  /* IWDG timeout equal to 2 sec. (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_128);

  /* Set counter reload value to obtain 2sec IWDG TimeOut.
     Counter Reload Value = 2sec/IWDG counter clock period
                          = 2sec / (LSI/32)
                          = 2sec / (LsiFreq/32)
                          = LsiFreq/(32 * 0.5)
                          = LsiFreq/16
   */
  IWDG_SetReload(LsiFreq/12);

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	
}
/*==========================================================================================*/
/**
  * @brief  Initial Independent Watchdog.
  * @param  None
  * @retval None
  */
uint32_t uiWDG_GetLSIFrequency(void) {
	
	NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef  RCC_ClockFreq;

  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* TIM5 configuration *******************************************************/ 
  /* Enable TIM5 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  
  /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
  TIM_RemapConfig(TIM5, TIM5_LSI);

  /* Configure TIM5 presclaer */
  TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);
  
  /* TIM5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM5 CH4
     The Rising edge is used as active edge,
     The TIM5 CCR4 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM5, &TIM_ICInitStructure);
  
  /* Enable TIM5 Interrupt channel */
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM5 counter */
  TIM_Cmd(TIM5, ENABLE);

  /* Reset the flags */
  TIM5->SR = 0;
    
  /* Enable the CC4 Interrupt Request */  
  TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);


  /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in 
    stm32f40x_it.c file) ******************************************************/
  while(_CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM5 peripheral registers to their default reset values */
  TIM_DeInit(TIM5);

  /* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);

  /* Get PCLK1 prescaler */
  if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return ((RCC_ClockFreq.PCLK1_Frequency / _PeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * RCC_ClockFreq.PCLK1_Frequency) / _PeriodValue) * 8) ;
  }
}
/*==========================================================================================*/
















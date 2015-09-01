/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	timer.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	17-February-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include <stdbool.h>

#include "stm32f4xx.h"
#include "main.h"
#include "SL_PinDefine.h"
#include "INT_STM32F4_Timer.h"

#define TIMER_CLOCK_TIM12				1000000
#define TIMER_CLOCK_TIM3				50000

uint16_t capture = 0;
__IO uint16_t tim12_CCR1_Val 			= 1;    		// Couter for Timer 1 sec.
__IO uint16_t tim3_CCR1_Val 			= 50000;    // Couter for Timer 1 sec.
__IO uint16_t tim3_CCR2_Val 			= 50;       // Couter for Timer 1 ms.
// __IO uint16_t tim3_CCR3_Val 			= 15000;    // Couter for Timer 0.3 sec.
__IO uint16_t tim3_CCR3_Val 			= 40000;    // Couter for Timer 0.8 sec.
__IO uint32_t baseCounter 				= 0;
__IO bool 		_buzzer_2hz 				= false;

extern __IO bool 			flag_1sec_sensTask;
extern __IO bool 			flag_1sec_DataTask;
extern __IO uint32_t 	_ms_counter;
extern __IO uint32_t 	_us_counter;
extern __IO uint32_t 	_SDWDGCounter;
extern __IO uint32_t	_GSMCounter;
__IO uint32_t 	_ms_counter_pulse = 0;

/*==========================================================================================*/
/**
  * @brief  Initial timer
  * @param  pvParameters not used
  * @retval None
  */
void vTIMER_initialTimer3(void) {
	
	uint16_t PrescalerValue = 0;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel 										= TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period 					= 65535;
  TIM_TimeBaseStructure.TIM_Prescaler 			= 0;
  TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
  TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TIMER_CLOCK_TIM3) - 1;
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel 1 */
  TIM_OCInitStructure.TIM_OCMode 				= TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 				= tim3_CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity 		= TIM_OCPolarity_High;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
	/* Output Compare Timing Mode configuration: Channel 2 */
  TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 				= tim3_CCR2_Val;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
	/* Output Compare Timing Mode configuration: Channel 3 */
  TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 				= tim3_CCR3_Val;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
  /* TIM3 Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	
}
/*==========================================================================================*/
/**
  * @brief  Initial timer
  * @param  pvParameters not used
  * @retval None
  */
void vTIMER_initialTimer12(void) {
	
	uint16_t PrescalerValue = 0;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM12 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	
	/* Enable the TIM12 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel 										= TIM8_BRK_TIM12_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period 					= 65535;
  TIM_TimeBaseStructure.TIM_Prescaler 			= 0;
  TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
  TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	/* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TIMER_CLOCK_TIM12) - 1;
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM12, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel 1 */
  TIM_OCInitStructure.TIM_OCMode 				= TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse 				= tim12_CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity 		= TIM_OCPolarity_High;
  TIM_OC1Init(TIM12, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Disable);
   
	/* TIM12 Interrupts enable */
  TIM_ITConfig(TIM12, TIM_IT_CC1, ENABLE);

	/* TIM12 enable counter */
  TIM_Cmd(TIM12, ENABLE);
	
}
/*==========================================================================================*/
/**
  * @brief  Set timer
  * @param  
  * @retval status
  */
uint8_t ucTIMER_SetTimer(uint32_t *keeper, uint32_t period) {
	
	uint32_t temp;                 

	if(period == 0) { 
		*keeper = 0xFFFFFFFF;
		return 1;
	}

	temp = baseCounter + period;
	 
	*keeper = temp;

	if(temp <= baseCounter) {
		*keeper = 0;
		return 1;
	}else {return 0;}
}
/*==========================================================================================*/
/**
  * @brief  Check timer exceed
  * @param  
  * @retval status
  */
int8_t cTIMER_CheckTimerExceed(uint32_t keeper) {
	
    if(keeper == 0) {
        return -1;
    }

    if((unsigned long int)baseCounter < (unsigned long int)keeper) {         
        return 0;
    }else {return 1;} /* time exceed */
}
/*==========================================================================================*/


/******************************************************************************/
/*            STM32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM8_BRK_TIM12_IRQHandler(void) {
	
	static uint16_t	tim12_capture = 0;
	
  if (TIM_GetITStatus(TIM12, TIM_IT_CC1) != RESET) {			/* 1 micro sec */
    TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);
		
		/* Increase counter of delay_us() */
		_us_counter++;
		
		/* Increase counter of Pulse Period */
		_ms_counter_pulse++;
		
		tim12_capture = TIM_GetCapture1(TIM12);
    TIM_SetCompare1(TIM12, tim12_capture + tim12_CCR1_Val);
		
		//printf("  %d %d %d\r\n", capture, CCR1_Val, capture+CCR1_Val);
  }
}
/*==========================================================================================*/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)  /* 1 sec */
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		
		baseCounter++;
//		printf("%d ", baseCounter);
		
		/* Check Flag 1 sec */
		flag_1sec_sensTask = true;
		
		/* Check Flag 1 sec */
// 		flag_1sec_DataTask = true;
		
		/* Increase SDTask WDG Timer */
		_SDWDGCounter++;
		
		/* Increase Reset GSM Timer */
		_GSMCounter++;
		
		capture = TIM_GetCapture1(TIM3);
    TIM_SetCompare1(TIM3, capture + tim3_CCR1_Val);
		
		//printf("  %d %d %d\r\n", capture, CCR1_Val, capture+CCR1_Val);
  }
	else if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)  /* 1 msec */
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		
		/* Increase counter of delay_ms() */
		_ms_counter++;
		
// 		/* Increase counter of Pulse Period */
// 		_ms_counter_pulse++;
		
    capture = TIM_GetCapture2(TIM3);
    TIM_SetCompare2(TIM3, capture + tim3_CCR2_Val);
  }
	else if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)  /* 0.3 sec */
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		
//		GPIO_ToggleBits((GPIO_TypeDef*)DOUT1_PORT, DOUT1_PIN);
		
		/* Check Flag 1 sec */
		flag_1sec_DataTask = true;
		
		if(_buzzer_2hz) {
			/* Buzzer toggling with frequency = 2 Hz */
			GPIO_ToggleBits((GPIO_TypeDef*)BUZZER_PORT, BUZZER_PIN);
		}else {
			/* Buzzer Off */
			GPIO_ResetBits((GPIO_TypeDef*)BUZZER_PORT, BUZZER_PIN);
		}
		
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare3(TIM3, capture + tim3_CCR3_Val);
  }
}
/*==========================================================================================*/

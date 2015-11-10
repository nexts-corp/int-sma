/**
  ******************************************************************************
  * <h2><center>&copy; Intelenics Co., Ltd.</center></h2>
  * @file    	INT_STM32F4_rtc.c
  * @author  	BIG M
  * @version 	V1.0.0
  * @date    	13-March-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "INT_STM32F4_rtc.h"

#define	RTC_CLOCK_SOURCE_LSE		0

/*==========================================================================================*/
/**
  * @brief  Initial real time clock
  * @param  None
  * @retval Initial error status
	*					 - 0x00	Initial success
	*					 - 0x01	RTC_Init error
	*					 - 0x02 RTC_SetTime error
	*					 - 0x03 RTC_SetDate error
  */
uint8_t ucRTC_Initial(void) {
	
	ErrorStatus 			err;
	RTC_InitTypeDef 	RTC_InitStructure;
	
	/* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
	
#if RTC_CLOCK_SOURCE_LSE
	/* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */ 
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}
 
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
#else
	/* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
 
  /* Wait till LSI is ready */ 
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){}
	
	/* Select the RTC clock source */ 
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);	
#endif
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour format */
  RTC_InitStructure.RTC_AsynchPrediv 	= 0x7F;
  RTC_InitStructure.RTC_SynchPrediv 	= 0xFF;
  RTC_InitStructure.RTC_HourFormat 		= RTC_HourFormat_24;
  err = RTC_Init(&RTC_InitStructure);
	if(err == ERROR) {
		/* RTC registers are not initialized */
		return 0x01;
	}
	
	/* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
	
	return 0x00;	
}
/*==========================================================================================*/
/**
  * @brief  Set date&time in RTC
  * @param  
  * @retval Error Status
	*					 - 0x00	Success
	* 				 - 0x01 Set date error
	* 				 - 0x02 Set time error
  */
uint8_t ucRTC_SetDateTime(RTC_DateTypeDef RTC_DateStruct, RTC_TimeTypeDef RTC_TimeStruct, uint8_t time_zone) {
	
	ErrorStatus		err;
	uint32_t 			epoch_time;
	
	epoch_time = uiRTC_DateTimeToEpoch(RTC_DateStruct, RTC_TimeStruct);
	epoch_time = epoch_time + ((uint32_t)time_zone * 60 * 60 * (-1));
	vRTC_EpochToDateTime(&RTC_DateStruct, &RTC_TimeStruct, epoch_time);
	
	err = RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	if(err != SUCCESS) {return 0x01;}

	err = RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	if(err != SUCCESS) {return 0x02;}
	
	return 0x00;
}
/*==========================================================================================*/
/**
  * @brief  Set Date&time in RTC from Unix_Epoch_Time format
  * @param  Unix_Epoch_Time
  * @retval Error Status
	*					 - 0x00	Success
	* 				 - 0x01 Set date error
	* 				 - 0x02 Set time error
  */
uint8_t ucRTC_SetDateTimeFromEpoch(uint32_t epoch_time) {
	
	ErrorStatus		err;
	RTC_DateTypeDef 	RTC_DateStructure;
	RTC_TimeTypeDef 	RTC_TimeStructure;
	
	vRTC_EpochToDateTime(&RTC_DateStructure, &RTC_TimeStructure, epoch_time);
	
	err = RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
	if(err != SUCCESS) {return 0x01;}

	err = RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
	if(err != SUCCESS) {return 0x02;}
	
	return 0x00;
}
/*==========================================================================================*/
/**
  * @brief  Get current date and time
  * @param  
  * @retval None
  */
void vRTC_GetDateTime(RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct, int8_t time_zone) {
	
	uint32_t		epoch_time;
	
	RTC_GetDate(RTC_Format_BIN, RTC_DateStruct);
	RTC_GetTime(RTC_Format_BIN, RTC_TimeStruct);
	
	epoch_time = uiRTC_DateTimeToEpoch(*RTC_DateStruct, *RTC_TimeStruct);
	epoch_time = epoch_time + (time_zone * 60 * 60);
	vRTC_EpochToDateTime(RTC_DateStruct, RTC_TimeStruct, epoch_time);
}
/*==========================================================================================*/
/**
  * @brief  Get current date and time in Unix_Epoch_Time Format (Timestamp)
  * @param  None
  * @retval Current Time (Unix_Epoch_Time Format)
  */
uint32_t uiRTC_GetEpochTime(void) {
	
	uint32_t					epoch_time = 0;
	RTC_TimeTypeDef 	RTC_TimeStructure;
	RTC_DateTypeDef 	RTC_DateStructure;
	
	vRTC_GetDateTime(&RTC_DateStructure, &RTC_TimeStructure, 0);
	epoch_time = uiRTC_DateTimeToEpoch(RTC_DateStructure, RTC_TimeStructure);
	
	return epoch_time;
}
/*==========================================================================================*/
/**
  * @brief  Convert Date&Time to Unix_Epoch_Time Format
  * @param  
  * @retval Epoch Time.
	* @credit Mr.Tawan Srisang
  */
uint32_t uiRTC_DateTimeToEpoch(RTC_DateTypeDef RTC_DateStruct, RTC_TimeTypeDef RTC_TimeStruct) {
  
	/* ---- convert date to elapsed days in binary ---- */                                                        
	uint32_t 		epoch_time;                                       /* total second */
	uint32_t		year;
	
	year = (uint32_t)RTC_DateStruct.RTC_Year + 2000;
    
	/* the following is broken down for clarity */
	epoch_time = 365 * ((uint32_t)year - 1970);          					/* calculate number of days for previous years */
	epoch_time += (((uint32_t)year - 1969) >> 2);                 /* add a day for each leap year */
	if((RTC_DateStruct.RTC_Month > 2) && (year % 4 == 0)) {      	/* add a day if current year is leap and past Feb 29th */
		epoch_time++;
	}
	
	switch(RTC_DateStruct.RTC_Month) {

			case 1:     epoch_time += 0;
									break;
			case 2:     epoch_time += 31;
									break;
			case 3:     epoch_time += 59;
									break;
			case 4:     epoch_time += 90;
									break;
			case 5:     epoch_time += 120;
									break;
			case 6:     epoch_time += 151;
									break;
			case 7:     epoch_time += 181;
									break;
			case 8:     epoch_time += 212;
									break;
			case 9:     epoch_time += 243;
									break;
			case 10:    epoch_time += 273;
									break;
			case 11:    epoch_time += 304;
									break;
			case 12:    epoch_time += 334;
									break;
	}
	epoch_time += (uint32_t)RTC_DateStruct.RTC_Date - 1;    			/* finally, add the days into the current month */
	epoch_time = 	epoch_time * 86400;                           	/* and calculate the number of seconds in all those days (1 day = 86400 second)*/
	epoch_time += (uint32_t)RTC_TimeStruct.RTC_Hours * 1800;     	/* add the number of seconds in the hours */
	epoch_time += (uint32_t)RTC_TimeStruct.RTC_Hours * 1800;     	/* add the number of seconds in the hours */
	epoch_time += (uint32_t)RTC_TimeStruct.RTC_Minutes * 60;      /* ditto the minutes */
	epoch_time +=	(uint32_t)RTC_TimeStruct.RTC_Seconds;         	/* finally, the seconds */   

	return epoch_time;
}
/*==========================================================================================*/
/**
  * @brief  Convert Unix_Epoch_Time Format to Date&Time
  * @param  
  * @retval None
	* @credit Mr.Tawan Srisang
  */
void vRTC_EpochToDateTime(RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef *RTC_TimeStruct, uint32_t epoch_time) {
  
	uint32_t yrs = 99, mon = 99, day = 99, tmp, jday, hrs, min, sec;
	uint32_t j, n;                                                 
	uint32_t dow;     // day of week
	
	j = epoch_time / 60;                             /* whole minutes since 1/1/70 */
	sec = epoch_time - (60 * j);                     /* leftover seconds */
	n = j / 60;
	min = j - (60 * n);
	j = n / 24;
	hrs = n - (24 * j);
	j = j + (365 + 366);                    /* whole days since 1/1/68 */   
	
	dow = (j % 7)+1;                        /* since 1/1/68 is Monday */
																					/* 1-7 => Sun - Sat */ 
	day = j / ((4 * 365) + 1);
	tmp = j % ((4 * 365) + 1);
	if(tmp >= (31 + 29))                    /* if past 2/29 */
	day++;                                  /* add a leap day */
	yrs = (j - day) / 365;                  /* whole years since 1968 */
	jday = j - (yrs * 365) - day;           /* days since 1/1 of current year */
	if(tmp <= 365 && tmp >= 60)             /* if past 2/29 and a leap year then */
	jday++;                                 /* add a leap day */
	yrs += 1968;                            /* calculate year */
	for(mon = 12; mon > 0; mon--) {
		switch(mon) {
			case 1:     tmp = 0;
									break;
			case 2:     tmp = 31;
									break;
			case 3:     tmp = 59;
									break;
			case 4:     tmp = 90;
									break;
			case 5:     tmp = 120; 
									break;
			case 6:     tmp = 151;
									break;
			case 7:     tmp = 181;
									break;
			case 8:     tmp = 212;
									break;
			case 9:     tmp = 243;
									break;
			case 10:    tmp = 273;
									break;
			case 11:    tmp = 304;
									break;
			case 12:    tmp = 334;
									break;
		}
		if((mon > 2) && !(yrs % 4))         /* adjust for leap year */
		tmp++;
		if(jday >= tmp) break;
	}
	day = jday - tmp + 1;                   /* calculate day in month */

	RTC_DateStruct->RTC_Year    = (yrs-2000);
	RTC_DateStruct->RTC_Month   = mon;
	RTC_DateStruct->RTC_Date    = day;
	RTC_DateStruct->RTC_WeekDay = dow;
	RTC_TimeStruct->RTC_Hours   = hrs;
	RTC_TimeStruct->RTC_Minutes = min;
	RTC_TimeStruct->RTC_Seconds = sec;
}
/*==========================================================================================*/


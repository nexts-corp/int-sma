/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_ds1307.h"
#include "INT_STM32F4_debug.h"

uint8_t TM_DS1307_Months[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

TM_DS1307_Result_t TM_DS1307_Init(void) {
	/* Initialize clock */
	TM_I2C_Init(DS1307_I2C, DS1307_I2C_PINSPACK, DS1307_I2C_CLOCK);
	
	/* Check if device is connected */
	if (!TM_I2C_IsDeviceConnected(DS1307_I2C, DS1307_I2C_ADDR)) {
		return TM_DS1307_Result_DeviceNotConnected;
	}
	
	/* Return OK */
	return TM_DS1307_Result_Ok;
}

void TM_DS1307_GetDateTime(TM_DS1307_Time_t* time) {
	uint8_t data[7];
	
	/* Read multi bytes */
	TM_I2C_ReadMulti(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, data, 7);
	
	/* Fill data */
	time->seconds = TM_DS1307_Bcd2Bin(data[DS1307_SECONDS]);
	time->minutes = TM_DS1307_Bcd2Bin(data[DS1307_MINUTES]);
	time->hours = TM_DS1307_Bcd2Bin(data[DS1307_HOURS]);
	time->day = TM_DS1307_Bcd2Bin(data[DS1307_DAY]);
	time->date = TM_DS1307_Bcd2Bin(data[DS1307_DATE]);
	time->month = TM_DS1307_Bcd2Bin(data[DS1307_MONTH]);
	time->year = TM_DS1307_Bcd2Bin(data[DS1307_YEAR]);
}

void TM_DS1307_SetDateTime(TM_DS1307_Time_t* time) {
	uint8_t data[7];
	
	/* Format data */
	data[DS1307_SECONDS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->seconds, 0, 59));
	data[DS1307_MINUTES] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->minutes, 0, 59));
	data[DS1307_HOURS] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->hours, 0, 23));
	data[DS1307_DAY] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->day, 1, 7));
	data[DS1307_DATE] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->date, 1, 31));
	data[DS1307_MONTH] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->month, 1, 12));
	data[DS1307_YEAR] = TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(time->year, 0, 99));
	
	/* Write to device */
	TM_I2C_WriteMulti(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, data, 7);
}

uint8_t TM_DS1307_GetSeconds(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS));
}

uint8_t TM_DS1307_GetMinutes(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES));
}

uint8_t TM_DS1307_GetHours(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS));
}

uint8_t TM_DS1307_GetDay(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY));
}

uint8_t TM_DS1307_GetDate(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE));
}

uint8_t TM_DS1307_GetMonth(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH));
}

uint8_t TM_DS1307_GetYear(void) {
	return TM_DS1307_Bcd2Bin(TM_I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR));
}


void TM_DS1307_SetSeconds(uint8_t seconds) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(seconds, 0, 59)));
}

void TM_DS1307_SetMinutes(uint8_t minutes) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(minutes, 0, 59)));
}
void TM_DS1307_SetHours(uint8_t hours) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(hours, 0, 23)));
}

void TM_DS1307_SetDay(uint8_t day) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(day, 1, 7)));
}

void TM_DS1307_SetDate(uint8_t date) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(date, 1, 31)));
}

void TM_DS1307_SetMonth(uint8_t month) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(month, 1, 12)));
}

void TM_DS1307_SetYear(uint8_t year) {
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR, TM_DS1307_Bin2Bcd(TM_DS1307_CheckMinMax(year, 0, 99)));
}

void TM_DS1307_EnableOutputPin(TM_DS1307_OutputFrequency_t frequency) {
	uint8_t temp;
	if (frequency == TM_DS1307_OutputFrequency_1Hz) {
		temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE;
	} else if (frequency == TM_DS1307_OutputFrequency_4096Hz) {
		temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS0;	
	} else if (frequency == TM_DS1307_OutputFrequency_8192Hz) {
		temp =	1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1;
	} else if (frequency == TM_DS1307_OutputFrequency_32768Hz) {
		temp = 1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1 | 1 << DS1307_CONTROL_RS0;	
	} else if (frequency == TM_DS1307_OutputFrequency_High) {
		temp = 1 << DS1307_CONTROL_OUT;	
	} else if (frequency == TM_DS1307_OutputFrequency_Low) {
		temp = 0;
	}
	
	/* Write to register */
	TM_I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_CONTROL, temp);
}

void TM_DS1307_DisableOutputPin(void) {
	/* Set output pin to high */
	TM_DS1307_EnableOutputPin(TM_DS1307_OutputFrequency_High);
}

uint8_t TM_DS1307_Bcd2Bin(uint8_t bcd) {
	uint8_t dec = 10 * (bcd >> 4);
	dec += bcd & 0x0F;
	return dec;
}

uint8_t TM_DS1307_Bin2Bcd(uint8_t bin) {
	uint8_t low = 0;
	uint8_t high = 0;
	
	/* High nibble */
	high = bin / 10;
	/* Low nibble */
	low = bin - (high * 10);
	
	/* Return */
	return high << 4 | low;
}

uint8_t TM_DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max) {
	if (val < min) {
		return min;
	} else if (val > max) {
		return max;
	}
	return val;
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
uint8_t TM_DS1307_SetDateTimeFromEpoch(uint32_t epoch_time) {
	
	ErrorStatus		err;
	TM_DS1307_Time_t 	time;
	
	TM_DS1307_EpochToDateTime(&time, epoch_time);
	
	TM_DS1307_SetDateTime(&time);
	
	return 0x00;
}

/*==========================================================================================*/
/**
  * @brief  Convert Unix_Epoch_Time Format to Date&Time
  * @param  
  * @retval None
	* @credit Mr.Tawan Srisang
  */
void TM_DS1307_EpochToDateTime(TM_DS1307_Time_t* time, uint32_t epoch_time) {
  
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

	time->year    = (yrs-2000);
	time->month   = mon;
	time->date    = day;
	time->day = dow;
	time->hours   = hrs;
	time->minutes = min;
	time->seconds = sec;
}
/*==========================================================================================*/
/**
  * @brief  Get current date and time in Unix_Epoch_Time Format (Timestamp)
  * @param  None
  * @retval Current Time (Unix_Epoch_Time Format)
  */
uint32_t TM_DS1307_GetEpochTime(void) {
	
	uint32_t					epoch_time = 0;
	TM_DS1307_Time_t 	time;
	
	TM_DS1307_GetDateTime(&time);
	epoch_time = TM_DS1307_DateTimeToEpoch(time);
	
	return epoch_time;
}
/*==========================================================================================*/
/**
  * @brief  Convert Date&Time to Unix_Epoch_Time Format
  * @param  
  * @retval Epoch Time.
	* @credit Mr.Tawan Srisang
  */
uint32_t TM_DS1307_DateTimeToEpoch(TM_DS1307_Time_t time) {
  
	/* ---- convert date to elapsed days in binary ---- */                                                        
	uint32_t 		epoch_time;                                       /* total second */
	uint32_t		year;
	
	year = (uint32_t)time.year + 2000;
    
	/* the following is broken down for clarity */
	epoch_time = 365 * ((uint32_t)year - 1970);          					/* calculate number of days for previous years */
	epoch_time += (((uint32_t)year - 1969) >> 2);                 /* add a day for each leap year */
	if((time.month > 2) && (year % 4 == 0)) {      	/* add a day if current year is leap and past Feb 29th */
		epoch_time++;
	}
	
	switch(time.month) {

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
	epoch_time += (uint32_t)time.date - 1;    			/* finally, add the days into the current month */
	epoch_time = 	epoch_time * 86400;              	/* and calculate the number of seconds in all those days (1 day = 86400 second)*/
	epoch_time += (uint32_t)time.hours * 1800;     	/* add the number of seconds in the hours */
	epoch_time += (uint32_t)time.hours * 1800;     	/* add the number of seconds in the hours */
	epoch_time += (uint32_t)time.minutes * 60;      /* ditto the minutes */
	epoch_time +=	(uint32_t)time.seconds;         	/* finally, the seconds */   

	return epoch_time;
}
/*==========================================================================================*/

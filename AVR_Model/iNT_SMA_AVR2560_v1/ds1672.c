#include <i2c.h>
#include <delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ds1672.h"

/*============================================================*/
unsigned long raw_rtc;                                           
/*============================================================*/
int DS1672_trickerChargeEnable(void) /* ----- enable the trickle-charger ------ */
{                                             
    #asm("cli")
    i2c_start();
    i2c_write(ADDRTC);
    i2c_write(5);
    i2c_write(0xA6);            /* enable the trickle-charger : No diode, 2k ohm resistor */
    i2c_stop();                           
    #asm("sei")
    
    /* 10ms delay to complete the write operation */
    delay_ms(10);
    
    return 0;
}
/*============================================================*/
unsigned long int DS1672_read(void)
{
    unsigned long int z;                                
    unsigned char rtc_reg1, rtc_reg2, rtc_reg3, rtc_reg4;
                                                
    #asm("cli")     
    i2c_start();
    i2c_write(ADDRTC);                      /* write slave address, write 1672 */
    i2c_write(0x00);                        /* write register address, 1st clock register */
    i2c_start();
    i2c_write(ADDRTC | 1);                  /* write slave address, read 1672 */
    rtc_reg1 = i2c_read(ACK);                 /* starts w/last address stored in register pointer */
    rtc_reg2 = i2c_read(ACK);
    rtc_reg3 = i2c_read(ACK);
    rtc_reg4 = i2c_read(NACK);
    i2c_stop();                           
    #asm("sei")
 
    z = rtc_reg4;
    z <<= 8;
    z += rtc_reg3;
    z <<= 8;
    z += rtc_reg2;
    z <<= 8;
    z += rtc_reg1;
                         
    //printDebug("\r\nDS1672_read -> 0x%02X%02X%02X%02X\r\n",rtc_reg4,rtc_reg3,rtc_reg2,rtc_reg1);
    //printDebug("rawtime [%ld]\r\n", z);
         
    return z;
}
/*============================================================*/
int binary_to_date(unsigned long x, struct rtc_structure *rtcCov) /* ------ convert binary time to date format ------ */
{
    unsigned long yrs = 99, mon = 99, day = 99, tmp, jday, hrs, min, sec;
    unsigned long j, n;                                                 
    unsigned int dow;     // day of week
    
    j = x / 60;                             /* whole minutes since 1/1/70 */
    sec = x - (60 * j);                     /* leftover seconds */
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
    for(mon = 12; mon > 0; mon--)
    {
        switch(mon)
        {
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

    rtcCov->year    = yrs;
    rtcCov->month   = mon;
    rtcCov->date    = day;
    rtcCov->hour    = hrs;
    rtcCov->min     = min;
    rtcCov->sec     = sec;
    rtcCov->day     = dow;

    return 0;
}
/*============================================================*/
unsigned long int date_to_binary(struct rtc_structure rtcSrc)
{  
    /* ---- convert date to elapsed days in binary ---- */                                                        
    unsigned long x;                                        // total second
    
    /* the following is broken down for clarity */
    x = (unsigned long)365 * (rtcSrc.year - 1970);          /* calculate number of days for previous years */
    x += ((rtcSrc.year - 1969) >> 2);                       /* add a day for each leap year */
    if((rtcSrc.month > 2) && (rtcSrc.year % 4 == 0))        /* add a day if current year is leap and past Feb 29th */
        x++;

    switch(rtcSrc.month)
    {
        case 1:     x += 0;
                    break;
        case 2:     x += 31;
                    break;
        case 3:     x += 59;
                    break;
        case 4:     x += 90;
                    break;
        case 5:     x += 120;
                    break;
        case 6:     x += 151;
                    break;
        case 7:     x += 181;
                    break;
        case 8:     x += 212;
                    break;
        case 9:     x += 243;
                    break;
        case 10:    x += 273;
                    break;
        case 11:    x += 304;
                    break;
        case 12:    x += 334;
                    break;
    }
    
    x += rtcSrc.date - 1;                       /* finally, add the days into the current month */
    x = x * 86400;                              /* and calculate the number of seconds in all those days (1 day = 86400 second)*/
    x += (unsigned long)rtcSrc.hour * 1800;     /* add the number of seconds in the hours */
    x += (unsigned long)rtcSrc.hour * 1800;     /* add the number of seconds in the hours */
    x += (unsigned long)rtcSrc.min * 60;        /* ditto the minutes */
    x += rtcSrc.sec;                            /* finally, the seconds */   
    
    return(x);
}
/*============================================================*/
int RTC_setBinary(unsigned long int rtc_bin)
{                                         
    unsigned long int rtc_current;
    unsigned long int rtc_diff;
    unsigned char a, b, c, d;             
    
    rtc_current = DS1672_read();                               
    
    if(rtc_current > rtc_bin){
        rtc_diff = (unsigned long int) (rtc_current - rtc_bin);    
    }
    else if(rtc_bin > rtc_current){
        rtc_diff = (unsigned long int) (rtc_bin - rtc_current);   
    }
    else{        
        // set value and current value are equal           
        //printDebug("\r\nset time and current time are equal\r\n");
        rtc_diff = 0;
    }                                    
    
    if((rtc_diff >= 0) && (rtc_diff < 10)){
        //printDebug("\r\nset time is too close to current time, not set!!!\r\n");                                  
        //printDebug("\r\nset time is too close to current time, not set!!! c[%ld] s[%ld]\r\n", rtc_current, rtc_bin);
        return -1;
    }
                                                   
    printDebug("c_time[%ld] & s_time[%ld] => diff[%ld]\r\n",rtc_current,rtc_bin,rtc_diff);
    
    a = (rtc_bin & 0xff);
    b = ((rtc_bin >> 8) & 0xff);
    c = ((rtc_bin >> 16) & 0xff);
    d = ((rtc_bin >> 24) & 0xff);
                                                
    #asm("cli")
    i2c_start();
    i2c_write(ADDRTC);          /* write slave address, write 1672 */
    i2c_write(0x00);            /* write register address, 1st clock register */
    i2c_write(a);
    i2c_write(b);
    i2c_write(c);
    i2c_write(d);
    i2c_stop();                          
    #asm("sei")
                                   
    printDebug(">> RTC_setBinary RTC Sync.\r\n");
    
    /* 10ms delay to complete the write operation */
    delay_ms(10);  

    return 0;
}
/*============================================================*/
int RTC_setDateTime(struct rtc_structure rtcSrc,int timezone)
{
    unsigned long int rtc_bin;
    unsigned long int rtc_current;
    unsigned long int rtc_diff;
    unsigned char a, b, c, d;                        
    
    if((timezone > 14) || (timezone < -12)){
        printDebug("<RTC_setDateTime> Timezone out of range (%+d)\r\n",timezone);
        return -2;
    }          
    
    rtc_bin = date_to_binary(rtcSrc);  
    
    rtc_bin = rtc_bin + (int)(timezone * 60 * 60 * -1);        

    raw_rtc = rtc_bin;        
    
    rtc_current = DS1672_read();
    
    if(rtc_current > rtc_bin){
        rtc_diff = (unsigned long int) (rtc_current - rtc_bin);    
    }
    else if(rtc_bin > rtc_current){
        rtc_diff = (unsigned long int) (rtc_bin - rtc_current);   
    }
    else{        
        // set value and current value are equal
        rtc_diff = 0;
    }                                    
    
    if((rtc_diff >= 0) && (rtc_diff < 10)){
        //printDebug("\r\nset time is too close to current time, not set!!! c[%ld] s[%ld]\r\n", rtc_current, rtc_bin);
        return -1;
    }
    
    printDebug("c_time[%ld] & s_time[%ld] => diff[%ld]\r\n",rtc_current,rtc_bin,rtc_diff);
    
    a = (rtc_bin & 0xff);
    b = ((rtc_bin >> 8) & 0xff);
    c = ((rtc_bin >> 16) & 0xff);
    d = ((rtc_bin >> 24) & 0xff);
                                                
    #asm("cli")
    i2c_start();
    i2c_write(ADDRTC);          /* write slave address, write 1672 */
    i2c_write(0x00);            /* write register address, 1st clock register */
    i2c_write(a);
    i2c_write(b);
    i2c_write(c);
    i2c_write(d);
    i2c_stop();                          
    #asm("sei")
    
    /* 10ms delay to complete the write operation */
    delay_ms(10);  

    return 0;
}
/*============================================================*/
int RTC_getDateTime(struct rtc_structure *rtcPtr, int timezone)
{
    unsigned long r_rtc;
    int flag;
    
    r_rtc = DS1672_read();             
    if(r_rtc == 0xFFFFFFFF){          
        flag    = RTC_BACKUP;            
        r_rtc   = raw_rtc;
    }
    else{
        flag    = RTC_REAL;       
        raw_rtc = r_rtc;
    }                  
    
    r_rtc = r_rtc + (int)(timezone * 60 * 60);
    
    binary_to_date(r_rtc,rtcPtr);       
    
    return flag;
}
/*============================================================*/
void printRTC(struct rtc_structure time)
{
    switch(time.day){
        case DAY_MON: printDebug("Mon. "); break;
        case DAY_TUE: printDebug("Tue. "); break;
        case DAY_WED: printDebug("Wed. "); break;
        case DAY_THU: printDebug("Thu. "); break;
        case DAY_FRI: printDebug("Fri. "); break;
        case DAY_SAT: printDebug("Sat. "); break;
        case DAY_SUN: printDebug("Sun. "); break;
    }                                              
    
    printDebug(
        "[%d]%04d-%02d-%02d %02d:%02d:%02d\r\n",
        time.day,
        time.year,
        time.month,
        time.date,
        time.hour,
        time.min,
        time.sec
    );
    return;
}
/*============================================================*/
void init_RTC(void)
{
    struct rtc_structure rtcTest;                
                   
    raw_rtc = 0;            /* initial default time_value in RAM */
                                          
    #asm("cli")
    i2c_start();
    i2c_write(ADDRTC);
    i2c_write(0x04);
    i2c_write(0x00);         /* enable the oscillator */
    i2c_stop();                    
    #asm("sei")

    DS1672_trickerChargeEnable();
    
    RTC_getDateTime(&rtcTest,7);        
                                  
    printDebug("Running with System Clock @ ");  
     
    printRTC(rtcTest);
}

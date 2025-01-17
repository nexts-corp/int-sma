#include <delay.h>
#include <stdlib.h>
#include <i2c.h>
#include "initial_sys.h"               
#include "uart.h"
#include "timer.h"
#include "io.h"
#include "ds1672.h"
#include "config.h"
//#include "oled.h"
#include "adc.h"
#include "debug.h"
#include "log.h"
#include "sdcard.h"

/*============================================================*/
int initial_system(void)
{                                   
    int res=0;
    char viTest[9] = "AT+CSQ\r\n";
    // Crystal Oscillator division factor: 1
    #pragma optsize-
    CLKPR=0x80;
    CLKPR=0x00;
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif
    
    init_uart();
    iUartSetBaudRate(2,BAUD_RATE_38K4_def);
           
    if(MCUSR & (1<<PORF)){                      
        printDebug("\r\nPower-on Reset\r\n");    
        MCUSR &= ~(1<<PORF);
    }               
    if(MCUSR & (1<<EXTRF)){                        
        printDebug("\r\nExternal Reset\r\n");    
        MCUSR &= ~(1<<EXTRF);
    }               
    if(MCUSR & (1<<BORF)){
        printDebug("\r\nBrown-out Reset\r\n");    
        MCUSR &= ~(1<<BORF);
    }               
    if(MCUSR & (1<<WDRF)){                        
        printDebug("\r\nWatchdog Reset\r\n");    
        MCUSR &= ~(1<<WDRF);
    }               
    if(MCUSR & (1<<JTRF)){                        
        printDebug("\r\nJTAG Reset\r\n");    
        MCUSR &= ~(1<<JTRF);
    }
    
    // -------- Peripheral Initialize ----------- //      
    init_io();                                                               
    init_i2c();
    init_spi();
    init_adc();               
    init_timer();
    init_common();

    enable_interrupt(2);   
    
    delay_ms(500);                         
                                                    
    // -------- Module Initialize ----------- //               
    BUZZER = 0;                 // BUZZER Off.
    init_RTC();         delay_ms(100);                           
    //init_oled();        delay_ms(100);
    adc_init_read();            //getting first ADC to initial
    
    enable_interrupt(1);        // -- adc_sampling
    enable_interrupt(3);        // -- 1 sec timer
    enable_interrupt(4);        // -- 0.5 sec timer                            
                                      
    //getAllConfig(); 
    
    
    // -- Initialize Complete -- //  
    /*bongkot*/                                                        
    //playBeep();
    showLedInitComplete();
    showDebugInitComplete();  
    
    delay_ms(2000);                                        
    send_uart(2,viTest); 
    delay_ms(2000); 
    return;
    
    if(SD_init() == 0)
    {      
//        if(LOG_init() < 0)
//        {
//            // -- log status = error -- //
//            printDebug("<initial_system> LOG_init() failed\r\n");
//        }        
    
        // -- log status = ok -- //
    }              
    else
    {                                          
        // -- sd status = error -- //                      
        printDebug("<initial_system> SD_init() failed\r\n");
    }
    
    srand((unsigned int)raw_rtc + (unsigned int)(read_adc(8) * 13));        // set random seed  
    
    // -------- Configuration Initialize ----------- //
//    readConfig();                                                             
//    initAlarm();        //delay_ms(100);
//     
//    if(init_logfile() < 0){
//        printDebug("init_logdata() failed\r\n");
//        return -1;
//    }                                  
            
    
    return res;  
}
/*============================================================*/
void init_i2c(void)
{                                
    // I2C Bus initialization
    i2c_init();
}
/*============================================================*/
void init_spi(void)
{              
    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 2000.000 kHz
    // SPI Clock Phase: Cycle Start
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR=0x50;
    SPSR=0x00;
}
/*============================================================*/
void init_common(void)
{
    // External Interrupt(s) initialization
    // INT0: Off, INT1: Off, INT2: Off, INT3: Off, INT4: Off, INT5: Off, INT6: Off, INT7: Off
    EICRA=0x00;
    EICRB=0x00;
    EIMSK=0x00;          
    
    PCMSK0=0x00;
    PCMSK1=0x00;
    PCMSK2=0x00;
    PCICR=0x00;

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR=0x80;
    ADCSRB=0x00;
    DIDR1=0x00;

    // TWI initialization
    // TWI disabled
    TWCR=0x00;
}
/*============================================================*/
void playBeep(void)
{
    BUZZER = 1;                                     // Buzzer ON Starting
    delay_ms(200);
    BUZZER = 0;
}
/*============================================================*/
void showDebugInitComplete(void)
{                        
//    struct rtc_structure time;       
//    struct prop_cnfg_st properties_config;
//    
//    printDebug("Initial Device Completed at - ");
//    RTC_getDateTime(&time,properties_config.timeZone);       
//    printDebug("%04d-%02d-%02d %02d:%02d:%02d\r\n",
//            time.year,time.month,time.date,time.hour,time.min,time.sec);
}
/*============================================================*/
void showLedInitComplete(void)
{
    LED_ON(BIT_LED_ALARM);
    delay_ms(200);

    LED_ON(BIT_LED_LINK);
    delay_ms(200);

    LED_ON(BIT_LED_TEMP);
    delay_ms(200);

    LED_ON(BIT_LED_PWR);
    delay_ms(200);

    LED_OFF(BIT_LED_PWR|BIT_LED_TEMP|BIT_LED_LINK|BIT_LED_ALARM);
    delay_ms(200);

    LED_ON(BIT_LED_PWR);   
    delay_ms(50);
}
/*============================================================*/


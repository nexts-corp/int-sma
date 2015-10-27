#include "timer.h"
#include "ds1672.h"
#include "adc.h"  
#include "io.h"
#include "debug.h"
#include "memcard.h"
                      
TIMER       baseCounter         = 0;    // -- increase continually by 1 second timer. use for timer comparison -- //
bit         timer_1sec_flag     = 0;    // -- 1 second trigger flag for main loop -- //

bit         buzzer_on           = 0;    // -- buzzer control [0:1] => [on:off] -- //

unsigned int countTimerIntpTask  = 0;//set count of interrupt task 
                                                              
/**************************************************************/
/******************* Software Timer Support *******************/
/**************************************************************/ 
/*============================================================*/
TIMER TIMER_checkTimeLeft(TIMER time)
{
    if(baseCounter < time){
        return ( time - baseCounter );
    }            
    else{
        return 0;
    }  
}
/*============================================================*/
int TIMER_checkTimerExceed(TIMER keeper)
{
    if(keeper == 0){
        return -1;
    }

    if((unsigned long int)baseCounter < (unsigned long int)keeper){         
        return 0;
    }            
    else{                             // -- time exceed -- //
        return 1;
    }
}
/*============================================================*/
int TIMER_setTimer(TIMER *keeper, unsigned long int period)
{
    unsigned long int t;                 
    
    if(period == 0){
        printDebug("<TIMER_setTimer> ERR: period = 0\r\n");  
        *keeper = 0xFFFFFFFF;
        return -1;
    }
    
    t = baseCounter + period;
     
    *keeper = t;
    
    if(t <= baseCounter){
        printDebug("<TIMER_setTimer> baseCounter[%ld] + period[%ld] = %ld OVERFLOW!!\r\n",baseCounter,period,t); 
        *keeper = 0;
        return -1;
    }               
    else{                             
        return 0;
    }
}
/*============================================================*/

/**************************************************************/
/********************** Hardware Support **********************/
/**************************************************************/  
/*============================================================*/
// Watchdog timeout interrupt service routine
interrupt [WDT] void wdt_timeout_isr(void)
{
    // Place your code here   
    printDebug("<WDT> WatchDog Timer Reset!!\r\n");
    return;
}
/*============================================================*/
// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    // Reinitialize Timer 0 value
    TCNT0=0x64;
    // Place your code here

}
/*============================================================*/
// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    // Reinitialize Timer1 value
//    TCNT1H=0xFF64 >> 8;
//    TCNT1L=0xFF64 & 0xff;      
    TCNT1H=0xFE79 >> 8;
    TCNT1L=0xFE79 & 0xff;
    
    //adc_sampling();  
}
/*============================================================*/
// Timer2 overflow interrupt service routine    (100Hz)
interrupt [TIM2_OVF] void timer2_ovf_isr(void)
{
    // Reinitialize Timer2 value
    TCNT2=0xb2;          
                
    /* card access low level timing function */
    disk_timerproc();    
    
}
/*============================================================*/
// Timer1 overflow interrupt service routine  (1 second)
interrupt [TIM3_OVF] void timer3_ovf_isr(void)
{
    // Reinitialize Timer1 value
    TCNT3H=0x85ED >> 8;
    TCNT3L=0x85ED & 0xff;  
          
    // Place your code here     
    timer_1sec_flag = 1;   
    
    countTimerIntpTask++;
    if(countTimerIntpTask==100){
       countTimerIntpTask = 0;
    }
    
    baseCounter++; 
                    
    raw_rtc++;                          // -- update backup RTC     
    
    //printDebug("<%d>\r\n",baseCounter);
}
/*============================================================*/
// Timer4 overflow interrupt service routine (0.5 sec)
interrupt [TIM4_OVF] void timer4_ovf_isr(void)
{
    static bit toggle = 0;
    // Reinitialize Timer4 value
    TCNT4H=0x85ED >> 8;
    TCNT4L=0x85ED & 0xff;

    // Place your code here                        
    toggle = !toggle; 
                       
    if(buzzer_on){
        if(toggle)  {   BUZZER = 1; }
        else        {   BUZZER = 0; }              
    }                             
    else{
        BUZZER = 0;  
    }
    
}

//// Timer5 overflow interrupt service routine
//interrupt [TIM5_OVF] void timer5_ovf_isr(void)
//{
//// Place your code here
//
//}
/*============================================================*/
void init_timer(void)
{

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 7.813 kHz
    // Mode: Normal top=0xFF
    // OC0A output: Disconnected
    // OC0B output: Disconnected
    TCCR0A=0x00;
    TCCR0B=0x05;
    TCNT0=0x64;
    OCR0A=0x00;
    OCR0B=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 7.813 kHz
    // Mode: Normal top=0xFFFF
    // OC1A output: Discon.
    // OC1B output: Discon.
    // OC1C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR1A=0x00;
    TCCR1B=0x05;
    //TCNT1H=0xFF;
    //TCNT1L=0x64;
    TCNT1H=0xFE;
    TCNT1L=0x79;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;
    OCR1CH=0x00;
    OCR1CL=0x00;
    
    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: 7.813 kHz
    // Mode: Normal top=0xFF
    // OC2A output: Disconnected
    // OC2B output: Disconnected
    ASSR=0x00;
    TCCR2A=0x00;
    TCCR2B=0x07;
    TCNT2=0xb2;
    OCR2A=0x00;
    OCR2B=0x00;

    // Timer/Counter 3 initialization
    // Clock source: System Clock
    // Clock value: 31.250 kHz
    // Mode: Normal top=0xFFFF
    // OC3A output: Discon.
    // OC3B output: Discon.
    // OC3C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer3 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR3A=0x00;
    TCCR3B=0x04;
    TCNT3H=0x85;
    TCNT3L=0xED;
    ICR3H=0x00;
    ICR3L=0x00;
    OCR3AH=0x00;
    OCR3AL=0x00;
    OCR3BH=0x00;
    OCR3BL=0x00;
    OCR3CH=0x00;
    OCR3CL=0x00;
    
    // Timer/Counter 4 initialization
    // Clock source: System Clock
    // Clock value: 125.000 kHz
    // Mode: Normal top=0xFFFF
    // OC4A output: Discon.
    // OC4B output: Discon.
    // OC4C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer4 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR4A=0x00;
    TCCR4B=0x03;
    TCNT4H=0x85;
    TCNT4L=0xED;
    ICR4H=0x00;
    ICR4L=0x00;
    OCR4AH=0x00;
    OCR4AL=0x00;
    OCR4BH=0x00;
    OCR4BL=0x00;
    OCR4CH=0x00;
    OCR4CL=0x00;

    // Timer/Counter 5 initialization
    // Clock source: System Clock
    // Clock value: Timer5 Stopped
    // Mode: Normal top=0xFFFF
    // OC5A output: Discon.
    // OC5B output: Discon.
    // OC5C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer5 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR5A=0x00;
    TCCR5B=0x00;
    TCNT5H=0x00;
    TCNT5L=0x00;
    ICR5H=0x00;
    ICR5L=0x00;
    OCR5AH=0x00;
    OCR5AL=0x00;
    OCR5BH=0x00;
    OCR5BL=0x00;
    OCR5CH=0x00;
    OCR5CL=0x00;          
    
    // Watchdog Timer initialization
    // Watchdog Timer Prescaler: OSC/1024k
    // Watchdog Timer interrupt: On
    #pragma optsize-
    #asm("wdr")
    WDTCSR=0x79;   
    WDTCSR=0x69;
    
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif


    // Timer/Counter 0 Interrupt(s) initialization
    TIMSK0=0x00;

    // Timer/Counter 1 Interrupt(s) initialization
    TIMSK1=0x00;

    // Timer/Counter 2 Interrupt(s) initialization
    TIMSK2=0x00;

    // Timer/Counter 3 Interrupt(s) initialization 
    TIMSK3=0x00;

    // Timer/Counter 4 Interrupt(s) initialization
    TIMSK4=0x00;

    // Timer/Counter 5 Interrupt(s) initialization
    TIMSK5=0x00;
}
/*============================================================*/
void enable_interrupt(int ch)
{
    
    // -------------- enable timer ---------------  //
    
    switch(ch){
        case 0: // Timer/Counter 0 Interrupt(s) initialization
                //TIMSK0=0x01;
                break;
        case 1: // Timer/Counter 1 Interrupt(s) initialization
                TIMSK1=0x01;
                break;
        case 2: // Timer/Counter 2 Interrupt(s) initialization
                TIMSK2=0x01;
                break;
        case 3: // Timer/Counter 3 Interrupt(s) initialization 
                TIMSK3=0x01;
                break;
        case 4: // Timer/Counter 4 Interrupt(s) initialization
                TIMSK4=0x01;
                break;
    }
    // Global enable interrupts
    #asm("sei") 
}
/*============================================================*/
void disable_interrupt(int ch)
{
    
    // -------------- enable timer ---------------  //
    
    switch(ch){
        case 0: // Timer/Counter 0 Interrupt(s) initialization
                //TIMSK0=0x00;
                break;
        case 1: // Timer/Counter 1 Interrupt(s) initialization
                TIMSK1=0x00;
                break;
        case 2: // Timer/Counter 2 Interrupt(s) initialization
                TIMSK2=0x00;
                break;
        case 3: // Timer/Counter 3 Interrupt(s) initialization 
                TIMSK3=0x00;
                break;
        case 4: // Timer/Counter 4 Interrupt(s) initialization
                TIMSK4=0x00;
                break;
    }
    // Global enable interrupts
    #asm("sei") 
}
/*============================================================*/

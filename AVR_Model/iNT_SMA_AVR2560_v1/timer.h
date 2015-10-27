#ifndef TIMER_H
#define TIMER_H

#include <mega2560.h>

typedef unsigned long int TIMER;

extern TIMER    baseCounter;
extern bit      timer_1sec_flag;
extern bit      buzzer_on;
extern unsigned int countTimerIntpTask;//set count of interrupt task 
                                       
TIMER TIMER_checkTimeLeft(TIMER time);
int   TIMER_checkTimerExceed(TIMER keeper);
int   TIMER_setTimer(TIMER *keeper, unsigned long int period);
void  init_timer(void);                                 
void  enable_interrupt(int ch);
void  disable_interrupt(int ch);


#endif

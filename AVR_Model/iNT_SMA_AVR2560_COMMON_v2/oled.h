#ifndef OLED_H
#define OLED_H

#include <mega2560.h>
#include "uart.h"

#define OLED_RESET PORTG.5

#define OLED_BUZZER_ENABLE          0
#define OLED_BUZZER_DISABLE         1
#define OLED_BUZZER_MUTE            2
                                     
#define OLED_SERVER_UNKNOWN         0
#define OLED_SERVER_DISCONNECT      1
#define OLED_SERVER_CONNECT         2

#define OLED_LAN_DISCONNECT         0
#define OLED_LAN_CONNECT            1

#define OLED_MEMORY_NORMAL          0
#define OLED_MEMORY_LOW             1
#define OLED_MEMORY_ERROR           2
                                       
void OLED_showDateTime(int timezone); 
void OLED_showSensorValue(void);       
void OLED_showPowerStatus(char pwrSrc,float batLevel);
void OLED_showInputStatus(int ch,char state);  
void OLED_showOutputStatus(int ch,char state);
void OLED_showServerCommStatus(char state);
void OLED_showLanStatus(char state);
void OLED_showBuzzerStatus(char state);
void OLED_showMemoryStatus(char state);           
void OLED_showAlarmStatus(void);           
void printOLED(flash char *fmtstr, ...);   
void OLED_reset(void);
void init_oled(void);

#endif               

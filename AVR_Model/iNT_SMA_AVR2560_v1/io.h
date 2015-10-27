#ifndef IO_H
#define IO_H

#include <mega2560.h>

#include "config.h"

#define DHT11_TO_MCU_PIN           PINL.0
#define MCU_TO_DHT11_PIN           PORTL.0

//#define DIN1                        PINJ.2
//#define DIN2                        PINJ.3
#define DIN3                        PINJ.4
#define DIN4                        PINJ.5
#define DIN5                        PINJ.6
                                        

#define ETH_CONF                    (PORTH |= (1 << 7))
#define ETH_NORM                    (PORTH &= ~(1 << 7))
#define ETH_MODE                    (PORTH & 0x80)
                                    
#define ETH_ON                      (PORTG |= (1 << 4))     
#define ETH_OFF                     (PORTG &= ~(1 << 4))

#define ETH_RESET                   PORTE.2
#define DOUT1                       PORTE.3
#define DOUT2                       PORTE.4
#define DOUT3                       PORTE.5
#define DOUT4                       PORTE.6    
#define BUZZER                      PORTE.7

#define DOOR_OPEN                   1
#define DOOR_CLOSE                  0

#define MAINTENANCE                 1
#define MONITOR                     0

#define LED_PWR                     1
#define LED_TEMP                    2
#define LED_LINK                    3
#define LED_ALARM                   4
#define LED_ALL                     5

/*** usage LED_ON( BIT_LED_PWR | BIT_LED_TEMP ); ***/
#define BIT_LED_ALARM               (1<<6)
#define BIT_LED_LINK                (1<<5)
#define BIT_LED_TEMP                (1<<4)
#define BIT_LED_PWR                 (1<<3)

#define LED_ON(x)                   (PORTH &= ~(x))
#define LED_OFF(x)                  (PORTH |= (x))

#define IO_LAN_CONNECT              (!((PINH & 0x04) >> 2))

// -- define input -- //
// 1 = PC7, 2 = PC6
#define TEMP_SEL                    (~((PINC & 0xC0) >> 6) & 0x03)
#define TEMP_TYPE_K                 0x00
#define TEMP_TMEC                   0x01
#define TEMP_PT100                  0x02
#define TEMP_RESERVED               0x03

#define SENSOR_SELECT(x)            (PORTK &= 0xF1); (PORTK |= (x << 1))
#define SENSOR_CONNECT              ((PINK & 0x10) >> 4)         

#define MAIN_POWER_CONNECT          (!((PINJ & 0x80) >> 7))             /*  PINJ.7 = 0 -> main power 
                                                                    PINJ.7 = 1 -> battery   */      
#define DIN1                        (!((PINJ & 0x04) >> 2))      
#define DIN2                        (!((PINJ & 0x08) >> 3))
       
#define ON                          1
#define OFF                         0
                  
#define HIGH                        1
#define LOW                         0

// -- menu driven I/O -- //
#define KEY_DELAY                   5              // debounce key(milli seconds)        
#define KEY_RELEASE_CHK_MODE        ((PINB & 0x10) >> 4)  
#define KEY_RELEASE_CHK_UP          ((PINB & 0x20) >> 5)  
#define KEY_RELEASE_CHK_DOWN        ((PINB & 0x40) >> 6)  
#define KEY_RELEASE_CHK_OK          ((PINB & 0x80) >> 7)    
//#define KEY_PRESS_CHK_MODE          (!((PINB & 0x10) >> 4))  
//#define KEY_PRESS_CHK_UP            (!((PINB & 0x20) >> 5))  
//#define KEY_PRESS_CHK_DOWN          (!((PINB & 0x40) >> 6))  
//#define KEY_PRESS_CHK_OK            (!((PINB & 0x80) >> 7))
#define KEY_PRESS_CHK_MODE          (!KEY_RELEASE_CHK_MODE)  
#define KEY_PRESS_CHK_UP            (!KEY_RELEASE_CHK_UP)  
#define KEY_PRESS_CHK_DOWN          (!KEY_RELEASE_CHK_DOWN)  
#define KEY_PRESS_CHK_OK            (!KEY_RELEASE_CHK_OK)

#define KEY_MODE                    1
#define KEY_UP                      2
#define KEY_DOWN                    3
#define KEY_OK                      4

struct ioval{                   // For dealing with doorAlarm & MIPAlarm
    char    state;              // high or low (1 or 0)      
    char    laststate;          // previous read value high or low (1 or 0)
    char    timecount;          // timer counter how long staying on this state
    int     alarm_flag;         // set to 1 if alarm 
};

//extern unsigned char   d_out[];

void init_io(void);                                    
void IO_dout(int ch, char state);                           
char IO_keyCheck(void);
  
#endif

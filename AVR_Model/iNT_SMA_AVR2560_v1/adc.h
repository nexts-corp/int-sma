#ifndef ATD_H
#define ATD_H

#include <mega2560.h>            
#include "io.h"                   

//#define ADC_VREF_TYPE   0xC0          // 2.56 Internal VREF 
//#define ADC_VREF_TYPE   0x80          // 1.1 Internal VREF
#define ADC_VREF_TYPE   0x40            // AVCC with external capacitor at AREF pin
//#define ADC_VREF_TYPE   0x00            // AREF, Internal VREF turned off 
//#define ADC_CONV_FAC    2.56/0x3FF                 
#define ADC_CONV_FAC    (eep_vref_val/0x3FF)
#define SENSOR_TYPE     (PINC.7 == 1) //true tmec ,false pt100

#define RAWTEMP         0
#define APPTEMP         1

extern float            adcData[];        
extern eeprom float     eep_vref_val;
                                                  
unsigned int read_adc(unsigned char adc_input);
unsigned char adc_sampling(void);
float readBattery(void);    
                         
void init_adc(void);
void adc_init_read(void);
#endif

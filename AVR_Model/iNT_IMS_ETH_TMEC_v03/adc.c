#include <mega2560.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc.h"
#include "debug.h"

float           adcData[5];  
float           adcBatt;

eeprom float    eep_vref_val = 2.56;
                                     
/*============================================================*/
unsigned int read_adc(unsigned char adc_input)
{
    unsigned int adc_data = 0x0000;
    
    ADMUX  &= ADC_VREF_TYPE;                                       
    ADCSRB &= 0xf7;          
    
    ADMUX  |= (adc_input & 0x07) | (ADC_VREF_TYPE & 0xff);  
        
    if(adc_input > 7)
    {
        ADCSRB |= 0x08;
    }
    else
    {                                                         
        ADCSRB &= 0xf7;
    }
    
    // Delay needed for the stabilization of the ADC input voltage
    //delay_us(20);
    delay_us(4);     //bongkot edit
    
    // Start the AD conversion
    ADCSRA  |= 0x80; //ENABLE ADC 
    ADCSRA  |= 0x40;
    
    // Wait for the AD conversion to complete     
    //while ( ((ADCSRA & 0x10)==0) || ((ADCSRA & 0x40) == 0x40));
    while ( (ADCSRA & 0x10)==0);        
    adc_data = ADCL;                
    adc_data |= (ADCH & 0x00ff) << 8;           
    
    ADCSRA  |=0x10;                           

    //delay_us(50);
    //delay_us(4);     //bongkot edit
    
    return adc_data;
}
/*============================================================*/
unsigned char adc_sampling(void)
{
    static int      i = 0;                      
    static char     batt_read_flag = 0;
                                         
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){         
        /*
        printDebug("<%d>",i);
        SENSOR_SELECT(i);                     
        delay_ms(20);
        adcData[i] = (float)((adcData[i] * 9.0) + (float)(read_adc(8) * 1.0))/10;           // Temp Sensor Low-Pass Filter      
        if(++i >= 5){   i = 0;  }                             
        */
    }                
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){
//        for( i=0 ; i<5 ; i++ ){
//            adcData[i] = (float)((adcData[i] * 9.9) + (float)(read_adc(8+i) * 0.1))/10;     // Temp Sensor Low-Pass Filter     
//        }
    }    
    
//    if(++batt_read_flag > 25){
//        adcBatt = (float)((adcBatt * 9.9) + (float)(read_adc(14) * 0.1))/10;                // Battery Read                
//        batt_read_flag = 0;
//    }
    
    return 0;
}                             
/*============================================================*/
float readBattery(void)
{
    float   vBATT;                         
                                                  
    adcBatt = (float)((adcBatt * 9.9) + (float)(read_adc(14) * 0.1))/10;                // Battery Read   
    vBATT   = adcBatt*ADC_CONV_FAC*1.8;                                // from voltage divider 120k & 150k                                  
    //printDebug("                                                     -- volt[%f]\r\n", vBATT);    // padding line  
    
    return vBATT;
}
/*============================================================*/
void adc_init_read(void)
{
    int i;
    printDebug("Initializing ADC.. ");
                                                     
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){  
        for(i=0;i<5;i++){ \
            SENSOR_SELECT(i);                     
            delay_ms(20);
            adcData[i] = read_adc(8);                                      
        }  
    }                
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){
        adcData[0] = read_adc(8);     
        adcData[1] = read_adc(9);         
        adcData[2] = read_adc(10);
        adcData[3] = read_adc(11);
        adcData[4] = read_adc(12);   
    }    
    
    adcBatt = read_adc(14);

    printDebug("OK\r\n");    
}
/*============================================================*/
void init_adc(void)
{
    // ADC initialization
    // ADC Clock frequency: 1000.000 kHz
    // ADC Voltage Reference: 2.56V, cap. on AREF
    // ADC Auto Trigger Source: Free Running
    // Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
    // ADC4: On, ADC5: On, ADC6: On, ADC7: On
    DIDR0=0x00;
    
                              
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){       
        // Digital input buffers on ADC8: Off, ADC9: On, ADC10: On, ADC11: On
        // ADC12: On, ADC13: Off, ADC14: Off, ADC15: On
        DIDR2=0x61;             
    }                
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){
        // Digital input buffers on ADC8: Off, ADC9: Off, ADC10: Off, ADC11: Off
        // ADC12: Off, ADC13: Off, ADC14: Off, ADC15: On
        DIDR2=0x7F;         
    }
    
    ADMUX   = ADC_VREF_TYPE;
    ADCSRA  = (1<<7) |          // ADEN:    ADC enable
              (0<<6) |          // ADSC:    ADC Start Conversion
              (0<<5) |          // ADATE:   ADC Auto Trigger Enable
              (0<<4) |          // ADIF:    ADC Interrupt Flag
              (0<<3) |          // ADIE:    ADC Interrupt
              (1<<2) |          //-----
              (1<<1) |          //- ADPS2-0: ADC Prescaler Select Bits      (011 = clk/8) (110 = clk/64)
              (0<<0);           //-----
                            
    ADCSRB  = 0;          
                                      
    //adc_init_read();
}
/*============================================================*/

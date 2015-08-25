#include <mega2560.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dht11.h"
#include "timer.h"
#include "debug.h"

/**
*
//Brif use timer5
**/
void MCUSignalOut(int status){
//    DDRL.0  = 1;   //set output
    char tempRegister = 0;
    DDRJ |= 0x04;    //oboooo o10o
    if(status==1){
       PORTJ |= 0x04; //0b0000 0100
    }else{
        PORTJ &= 0xFB;//0b1111 1011
    }
}
int DHT11SignalIn(){
//    DDRL.0   = 0;   //set input 
//    return DHT11_TO_MCU_PIN;
    DDRJ &= 0xFB;  
    if((PINJ & 0x04)==0x04){
       return 0x01;
    }else{
       return 0x00;
    }
    
}
int DHT11Read(ST_DHT11 *dhtDevice){
    int i, j, temp;
    int buffer = 0; 
    int count = 0;
	int data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    
    /*start condition*/
    dhtDevice->status = DHT11_ERROR_TIMEOUT; 
    //MCUSignalOut(MCU_SIGNAL_LOW);
    //delay_ms(4);  
    MCUSignalOut(MCU_SIGNAL_HIGH); 
    delay_ms(4);
    
//    TCCR5A=0x00;
//    TCCR5B=0x05;     //count = 128 microsec , 128 * 7812 = about 1sec
//               
//    TCNT5H=0x00;     //timer count clearing 
//    TCNT5L=0x00; 
    
    MCUSignalOut(MCU_SIGNAL_LOW);
    //while((TCNT5H <<8 | TCNT5L)<160);           //about 20 ms  
    delay_ms(20);  
    
    /*set clk/8
    *clk@8Mhz/prescaling 8
    *get count every 1 us
    */     
//    TCCR5A=0x00;
//    TCCR5B=0x02;     //count = 1 microsec 
//               
//    TCNT5H=0x00;     //timer count clearing 
//    TCNT5L=0x00; 
    
    MCUSignalOut(MCU_SIGNAL_HIGH);
    delay_us(40);    
    /*end start condition*/   
   // printDebug("DHT11: end Condition \r\n");
    
    TCCR5A=0x00;
    TCCR5B=0x02;     //count = 1 microsec
    TCNT5H=0x00;     //timer count clearing 
    TCNT5L=0x00;
    while(DHT11SignalIn()==0){
        if((TCNT5H <<8 | TCNT5L)>100){       //about 80 us      //if((TCNT5H <<8 | TCNT5L)>100){       //about 80 us 
            dhtDevice->status = DHT11_ERROR_TIMEOUT;
           return DHT11_ERROR_TIMEOUT;
        }
    }
    
    TCNT5H=0x00;     //timer count clearing 
    TCNT5L=0x00;
    while(DHT11SignalIn()==1){
        if((TCNT5H <<8 | TCNT5L)>100){       //about 80 us        //if((TCNT5H <<8 | TCNT5L)>100){       //about 80 us
            dhtDevice->status = DHT11_ERROR_TIMEOUT;
           return DHT11_ERROR_TIMEOUT;
        }
    }  
    
    //Read 40 bits (8*5)
	for(j = 0; j < 5; ++j) {
		for(i = 0; i < 8; ++i) {
			
			//LOW for 50us
			TCNT5H=0x00;     //timer count clearing 
            TCNT5L=0x00;
            while(DHT11SignalIn()==0){
                if((TCNT5H <<8 | TCNT5L)>70){       //about 50 us            //if((TCNT5H <<8 | TCNT5L)>70){
                    dhtDevice->status = DHT11_ERROR_TIMEOUT;
                   return DHT11_ERROR_TIMEOUT;
                }
            }  
            
			//Start counter
			//buffer = 0;
			//HIGH for 26-28us = 0 / 70us = 1 
            
			TCNT5H=0x00;     //timer count clearing 
            TCNT5L=0x00;
            while(DHT11SignalIn()==1){  
                count = (TCNT5H <<8 | TCNT5L);
                if(count>90){       //about 70 us                //if(count>90){
                    dhtDevice->status = DHT11_ERROR_TIMEOUT;
                   return DHT11_ERROR_TIMEOUT;
                }
                
            }
			 
//            if(count > 40){
//               buffer = 1;
//            }
			//Calc amount of time passed
			temp = count;
			
			//shift 0
			data[j] = data[j] << 1;
			
			//if > 30us it's 1
			if(temp > 40)
				data[j] = data[j]+1;
		}
	}
      
    //MCUSignalOut(MCU_SIGNAL_HIGH);
    
    //verify the Checksum
	if(data[4] != (data[0] + data[2])){
        dhtDevice->status = DHT11_ERROR_CHECKSUM;
        return DHT11_ERROR_CHECKSUM;
    }
	
    
    
    dhtDevice->temp = (float)data[2]; 
    dhtDevice->humi = (float)data[0];
    dhtDevice->status = DHT11_SUCCESS;
    return DHT11_SUCCESS;	
}

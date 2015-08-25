#ifndef DHT11_H
#define DHT11_H

#include <mega2560.h>            
#include "io.h" 

#define DHT11_SUCCESS         1
#define DHT11_ERROR_CHECKSUM  2
#define DHT11_ERROR_TIMEOUT   3

#define MCU_SIGNAL_LOW        0
#define MCU_SIGNAL_HIGH       1

typedef struct{
    float temp;  
    float humi;
    int status;
}ST_DHT11;

void MCUSignalOut(int status);
int DHT11SignalIn();
int DHT11Read(ST_DHT11 *dhtDevice);

#endif 
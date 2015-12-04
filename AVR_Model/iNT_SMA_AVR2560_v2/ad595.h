#ifndef AD595_H
#define AD595_H
#include "main.h"
#include "debug.h"

typedef struct{
    float tempCelsius;
    float tempFahrenheit;
}iAD595_t;

iAD595_t   iAD595;

iAD595_t ad595Get(unsigned int adcValue_p);

#endif 
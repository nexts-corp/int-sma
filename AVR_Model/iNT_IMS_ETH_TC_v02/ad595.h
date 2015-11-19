#ifndef AD595_H
#define AD595_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ad595.h"


float iAD595ADCToTemp(long viADC_p,int viADCLoop_p);
float iAD595VoltToTemp(float viVolt_arg);
float iAD595ADCToVolt(unsigned long viADC_p,float viADCLoop_p);

#endif 
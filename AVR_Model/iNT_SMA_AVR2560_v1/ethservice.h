#ifndef ETHSERVICE_H
#define ETHSERVICE_H

//#include "config.h"

int getConfigLen(unsigned char cnfg);       
int configRead(unsigned char cnfg, unsigned char *ptr);
int configWrite(unsigned char cnfg, unsigned char *conf, int len);
int configConfirm(void);                                        
int setLogManipulateSchema(unsigned char accd, unsigned long int tStart, unsigned long int tStop);
int setTimeSyncSchema(unsigned char *timeData);      
int setMuteAlarm(unsigned long int period);

#endif

#ifndef INITIAL_SYS_H
#define INITIAL_SYS_H

int  initial_system(void);
void init_i2c(void);
void init_spi(void);
void init_common(void);
void playBeep(void);
void showDebugInitComplete(void);
void showLedInitComplete(void); 


#endif

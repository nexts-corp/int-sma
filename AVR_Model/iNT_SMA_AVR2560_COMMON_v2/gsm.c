#include "gsm.h"

void iGsmSetUart(iChar_t viChannel_arg,iChar_t viBaudRate_arg){
     iUartSetBaudRate(viChannel_arg,viBaudRate_arg);
}

void iSend_AtCmd(iChar_t *viBuff_arg){
    send_uart(2,viBuff_arg);
}

void iGsmReadSignal(iChar_t *viSignalOut_arg){
    char viAtCmd[9] = "AT+CSQ\r\n";
    
    delay_ms(1000);                                        
    send_uart(2,viAtCmd); 
    delay_ms(2000); 
}
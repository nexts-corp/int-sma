#include "gsm.h"

void iGsmSetUart(iChar_t viChannel_arg,iChar_t viBaudRate_arg){
     iUartSetBaudRate(viChannel_arg,viBaudRate_arg);
}

void iSend_AtCmd(iChar_t *viBuff_arg){
    send_uart(2,viBuff_arg);
}


/**
  * @brief  Read signal quality report.
  * @param  None
  * @retval Return signal percent(0-100%)
**/
void iGsmReadSignal(iChar_t *viSignalOut_arg){
    char viAtCmd[9] = "AT+CSQ\r\n";
    
    
    
    delay_ms(1000);                                        
    send_uart(2,viAtCmd); 
    
    
    delay_ms(2000); 
}

void iGsmCheckCB(iChar_t *viSignalOut_arg){    //check credit balance
    
}

iChar_t iGsmInit(){
     return 1;
}

iChar_t iGsmConn(){
     return 1;
}

iChar_t iGsmClose(){
     return 1;
}
#include "interruptTask.h"
#include "timer.h"
#include "wiz100rs.h"
int wiz100srSetTimer = -1;
//int viUart0TimeOutTemp = -1;
piInterrupt_t pviUart0TimeOut = -1;
extern void (*pfiWiz100rs)(void);

void interruptTask(void *pviParameter){
    
     printDebug("<interruptTask>Task Running...\r\n");  
     
//     if(pviUart0TimeOut == 1){    //start interrupt start timeout
//        printf("<interruptTask>start count pviUart0TimeOut\r\n");
//        
//        if(wiz100srSetTimer == -1){
//           wiz100srSetTimer = countTimerIntpTask;
//        }else if((countTimerIntpTask - wiz100srSetTimer) > 2){                  //set timer = 1 sec
//             printDebug("<interruptTask>wiz100srSetTimer(timeout %d)\r\n",(countTimerIntpTask - wiz100srSetTimer)); 
//             wiz100srSetTimer = -1;
//             pviUart0TimeOut = 0;
//        } 
//     }     

     printDebug("<interruptTask>pviUart0TimeOut(%d),wiz100srSetTimer(%d)\r\n",pviUart0TimeOut,wiz100srSetTimer);
     if(pviUart0TimeOut >= 0){    //start interrupt start timeout
         printDebug("<interruptTask>start count pviUart0TimeOut\r\n");
         if(wiz100srSetTimer == -1){
             wiz100srSetTimer = countTimerIntpTask;
             //viUart0TimeOutTemp = pviUart0TimeOut;
         }else if((countTimerIntpTask - wiz100srSetTimer) > 2){                  //set timer = 1 sec
             printDebug("<interruptTask>wiz100srSetTimer(timeout %d)\r\n",(countTimerIntpTask - wiz100srSetTimer));  
             pfiWiz100rs = &iWizRXTimeOut;
             pfiWiz100rs();
             wiz100srSetTimer = -1;
             pviUart0TimeOut = -1;
         } 
     }else if(pviUart0TimeOut == -1){
         wiz100srSetTimer = -1;
     }  
    
      
     
}
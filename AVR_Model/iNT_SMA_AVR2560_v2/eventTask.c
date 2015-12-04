#include "eventTask.h"
#include "eventManager.h"

iEventReport_t viEventReportBuff;
extern iDataMini_t viEventReport;

void eventTask(void *pviParameter){
    #asm("wdr")
    printDebug("[dataAndStatusTask]Task Running...\r\n"); 
    iEventRead(&viEventReport);
}
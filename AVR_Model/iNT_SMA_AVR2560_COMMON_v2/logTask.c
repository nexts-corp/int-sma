#include "logTask.h"
#include "debug.h"
#include "logManager.h"

void logTask(void *pviParameter){
     printDebug("[logTask]Running...\r\n");
     
     iGenFileDaily();
}
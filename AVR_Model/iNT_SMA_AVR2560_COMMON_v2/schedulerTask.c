#include "schedulerTask.h"

int task_count_g = 0; 

iTask_t *pviTaskList[MAX_TASK];

itaskHandel_t iTaskCreate(piTaskFunction piTaskCode_p,const char * const name_p,uint_t priority_p){
    itaskHandel_t pvTaskHandleReturn;
    iTask_t *pviTaskNew;
    void * vp;
    int i = 0;
    vp = &i;
    
    if(task_count_g < MAX_TASK){
        pviTaskNew = (iTask_t *)malloc(sizeof(iTask_t));
        if(pviTaskNew != NULL){
            printDebug("<iTaskCreate>Task can create.\r\n");
            pviTaskNew->id = 0;
            pviTaskNew->hookFunction = piTaskCode_p;
            memcpy(pviTaskNew->name,name_p,MAX_TASK_NAME);
            pviTaskNew->priority = priority_p;
            
            pviTaskList[task_count_g] = pviTaskNew;
            task_count_g++;
            
        }else{
            printDebug("<iTaskCreate>Task can not create.\r\n");
            pvTaskHandleReturn = (int *)(-1);
        }
    }else if(task_count_g == MAX_TASK){
        pvTaskHandleReturn = (void *)(-1);
    }
    
    
//    if(task_count_g == 0){
//        piTaskList_p->taskValue = pviTaskNew;
//        piTaskList_p->next = NULL;
//        task_count_g ++;
//        piTaskList_p->taskValue->hookFunction(vp);
//    }else if(task_count_g > 0){
////        piTaskList_p->taskValue = pviTaskNew;
////        piTaskList_p->next = NULL;
////        task_count_g ++;
//    }
    //pviTaskNew->hookFunction(vp);
    //piTaskCode_p(vp);
    //free(pviTaskNew);
    return pvTaskHandleReturn;
}

void startSchedulerTask(){
    itaskHandel_t pvTaskRunHandle;
    void * vp;       //parameter
    int i=0;
    int j=0;
    //pvTaskRunHandle = iTaskCreate(task_a_print,"TaskAP",5);
    for(;;){   
        #asm("wdr")
        for(i=0;i<task_count_g;i++){
            for(j=0;j<task_count_g;j++){
                if(pviTaskList[j]->priority == 10){
                    pviTaskList[j]->hookFunction(vp);
                }
            }
            pviTaskList[i]->hookFunction(vp);
        }
    }
//    for(i=0;i<task_count_g;i++){
//        pviTaskList[i]->hookFunction(vp);
//    }
}
#include <stdlib.h>
#include <string.h>
#include "debug.h"
                                                              
/*============================================================*/
char * addByteList(char *list, int *len, char element, int eSize)
{
    int size=0;
                                  
    size = (*len) * sizeof(char);
        
    if(size > 0)
    {                                  
        printDebug("<addByteList> reallocate step 1: list[%d] & list ptr[%d]\r\n", *list, *list);
        
        list = (char *) realloc(list,size+1);
        if(list == NULL){
            printDebug("<addByteList> list realloc(%d) failed\r\n",size);
            return NULL;
        }                         
        
        printDebug("<addByteList> reallocate step 2: list[%d] & list ptr[%d]\r\n", *list, *list);
                                                
    }    
    else
    {                                                
        list = (char *) malloc(size + 1);
        if(list == NULL){
            printDebug("<addByteList> list malloc(%d) failed\r\n", size+1);
            return NULL;
        }                                               
        printDebug("<addByteList> list ptr : %d\r\n", list);
    }                            
    
    list[size] = element;  
    *len = size + 1;           
                             
    printDebug("<addByteList> len = %d\r\n", *len);
    print_payload(list,*len);
    
    return list;
}                                                               
/*============================================================*/
void finalizeList(char *list, int *len)
{
    free (list);                       
    *len = 0;
}
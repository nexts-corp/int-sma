#include <stdlib.h>
#include "ethparser.h"
#include "debug.h"
                                                                  
/*============================================================*/
int statusRequestParser(unsigned char *pkt, int len, struct stat_req_st *st)
{ 
    int     i,k=0;
                        
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                        
    st->accm        =   pkt[k++];                   
    
    st->intv        =   ((pkt[k++] & 0x00ff) << 8) | 
                        ((pkt[k++] & 0x00ff));                                 
                                    
    if(st->accm >= 0x30){
        return 0;    
    }               
    
    st->parLen      =   len - k;                                               
   
    st->parlst      =   malloc((st->parLen) * sizeof(char));
    if(st->parlst == NULL)
    {             
        printDebug("<statusRequestParser> malloc failed\r\n");
        return -1;
    }                                                                            
               
    for( i=0 ; i<(st->parLen) ; i++ )
    {
        (st->parlst)[i]      = pkt[i+k];
    }
                                
    return 0;
}         
/*============================================================*/
int statusReportParser(unsigned char *pkt, unsigned long int *time)
{   
    int i,k=0;                       
                                  
    k               +=  8;
    
    *time           =   (((long)pkt[k++] & 0x000000FF) << 24) | 
                        (((long)pkt[k++] & 0x000000FF) << 16) | 
                        (((long)pkt[k++] & 0x000000FF) << 8)  | 
                        (((long)pkt[k++] & 0x000000FF));
  
    return 0;
}                                                                                                                                 
/*============================================================*/
int configManageParser(unsigned char *pkt, int len, struct conf_man_st *st)                      
{
    int i,k=0;                                  
    
    printDebug("<configManageParser>\r\n");
    print_payload(pkt,len);
                     
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                        
    st->accd        =   pkt[k++];                          
    
    st->cnfg        =   pkt[k++];     
    
    if(st->accd == CONF_RD_CMD){             
        printDebug("<configManageParser> READING\r\n");           
        st->confLen     =   0;
        st->confSpec    =   NULL;
        return 0;
    }                                                  
    printDebug("<configManageParser> WRITING\r\n");
    
    st->confLen     =   len - k;                  
    
    printDebug("<configManageParser> confLen = %d\r\n",st->confLen);                            
          
    
    st->confSpec    =   malloc((st->confLen) * sizeof(char)); 
    if(st->confSpec == NULL)
    {             
        printDebug("<configManageParser> malloc failed\r\n");
        return -1;
    }                                                       
           
    for( i=0 ; i<(st->confLen) ; i++ )
    {
        (st->confSpec)[i]      = pkt[i+k];
    }
                                
    return 0;       
    
}                                                                 
/*============================================================*/
/*
int dataReportParser(unsigned char *pkt, int len, unsigned char *frameID)                        
{
    *frameID        =   pkt[0];   
    return 0;
} 
*/                                                                
/*============================================================*/
int logManipulationCmdParser(unsigned char *pkt, int len, struct log_manic_st *st)                   
{
    int i,k=0;
                     
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                        
    st->accd        =   pkt[k++];
    
    if(len > 6){
        st->tStart  =   (((long)pkt[k++] & 0x000000FF) << 24) | 
                        (((long)pkt[k++] & 0x000000FF) << 16) | 
                        (((long)pkt[k++] & 0x000000FF) << 8)  | 
                        (((long)pkt[k++] & 0x000000FF));  
                        
        st->tStop   =   (((long)pkt[k++] & 0x000000FF) << 24) | 
                        (((long)pkt[k++] & 0x000000FF) << 16) | 
                        (((long)pkt[k++] & 0x000000FF) << 8)  | 
                        (((long)pkt[k++] & 0x000000FF));
    }        
                                                       
    return 0;
}  
/*============================================================*/
int logManipulationAckParser(unsigned char *pkt, struct log_mania_st *st)                   
{
    int i,k=0;         
                     
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                        
    st->accd        =   pkt[k++];
    st->status      =   pkt[k++];
         
    return 0;
}                                                               
/*============================================================*/
int networkMaintenanceParser(unsigned char *pkt, int len, struct netw_man_st *st)                
{
    int i,k=0;
    
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
      
    st->cmd         =   pkt[k++];
                                         
    st->dataLen     =   len - 2;                                              
                               
    if( st->dataLen > 0 ){
        
        st->data    =   malloc((st->dataLen) * sizeof(char)); 
        if(st->data == NULL)
        {             
            printDebug("<networkMaintenanceParser> malloc failed (%d)\r\n",((st->dataLen) * sizeof(char)));
            return -1;
        }                                                       

        for( i=0 ; i<(st->dataLen) ; i++ )
        {
            (st->data)[i]      = pkt[i+k];
        }
                              
    }
    
    return 0;
}                                                                 
/*============================================================*/
int remoteDeviceControlParser(unsigned char *pkt, struct rem_dev_ctrl_st *st)              
{
    int i,k=0;
                     
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                        
    st->cmd         =   pkt[k++];                           
                                                 
    st->value       =   (((long)pkt[k++] & 0x000000FF) << 24) | 
                        (((long)pkt[k++] & 0x000000FF) << 16) | 
                        (((long)pkt[k++] & 0x000000FF) << 8)  | 
                        (((long)pkt[k++] & 0x000000FF));         
                        
    return 0;
}              
/*============================================================*/
int clearListParser(unsigned char *pkt, struct clr_lst_st *st)              
{
    int i,k=0;       
                     
    for(i=0;i<sizeof(st->nid);i++){
        st->nid[0]  =   pkt[k++];
    }
                
    st->cmd         =   pkt[k++];                
    st->status      =   pkt[k++];       
                        
    return 0;
}                                     
/*============================================================*/
void free_statusRequest(struct stat_req_st *st)
{                                                         
    free ( st->parlst );
}                                                                 
/*============================================================*/       
void free_configManage(struct conf_man_st *st)
{                                                                   
    free ( st->confSpec );
}                                                                 
/*============================================================*/                             
void free_networkMaintenance(struct netw_man_st *st)
{
    free ( st->data );
}                                                                 
/*============================================================*/


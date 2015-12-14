#ifndef ETHPARSER_H
#define ETHPARSER_H


struct stat_req_st
{
    unsigned char       nid[8];                 // -- node address
    unsigned char       accm;                   // -- access method
	unsigned int        intv;                   // -- interval           
    unsigned int        parLen;                 // -- length of parameter list
    unsigned char       *parlst;                // -- parameter list     
};
     
struct stat_rep_st
{
    unsigned char       nid[8];                 // -- node address
    unsigned long int   tsync;                  // -- time sync.
};       
     
struct event_rep_st
{
    unsigned char       nid[8];                 // -- node address
};
                                                                 
#define CONF_RD_CMD     0x10                    // -- configuration read command
#define CONF_WR_CMD     0x20                    // -- configuration write command
#define CONF_CF_CMD     0x30                    // -- configuration confirm command
struct conf_man_st
{
    unsigned char       nid[8];                 // -- node address
    unsigned char       accd;                   // -- access direction
    unsigned char       cnfg;                   // -- configuration group                    
    unsigned int        confLen;                // -- length of configuration specific structure
    unsigned char       *confSpec;              // -- configuration specific structure       
};            
     
struct data_rep_st
{
    unsigned char       nid[8];                 // -- node address
};

struct log_manic_st
{
    unsigned char       nid[8];                 // -- node address
    unsigned char       accd;                   // -- access direction
    unsigned long int   tStart;                 // -- start timestamp
    unsigned long int   tStop;                  // -- stop timestamp
};       

struct log_mania_st
{
    unsigned char       nid[8];                 // -- node address
    unsigned char       accd;                   // -- access direction
    unsigned char       status;                 // -- status
};

#define NETW_TIME_SYNC      0x10
struct netw_man_st
{                                                                      
    unsigned char       nid[8];                 // -- node address
    unsigned char       cmd;                    // -- command               
    unsigned int        dataLen;                // -- length of data
    unsigned char       *data;                  // -- data                  
};                                                        

#define DEVCTRL_MUTE_ALARM  0x10
struct rem_dev_ctrl_st
{                                                                          
    unsigned char       nid[8];                 // -- node address
    unsigned char       cmd;                    // -- command            
    unsigned long int   value;                  // -- value                  
};

#define CLR_DUPLIST         0x01
struct clr_lst_st
{                                                          
    unsigned char       nid[8];                 // -- node address                       
    unsigned char       cmd;                    // -- command       
    unsigned char       status;                 // -- status         
};

int statusRequestParser(unsigned char *pkt, int len, struct stat_req_st *st);
int statusReportParser(unsigned char *pkt, unsigned long int *time);
int configManageParser(unsigned char *pkt, int len, struct conf_man_st *st);
int logManipulationCmdParser(unsigned char *pkt, int len, struct log_manic_st *st);
int logManipulationAckParser(unsigned char *pkt, struct log_mania_st *st);
int networkMaintenanceParser(unsigned char *pkt, int len, struct netw_man_st *st);
int remoteDeviceControlParser(unsigned char *pkt, struct rem_dev_ctrl_st *st);
int clearListParser(unsigned char *pkt, struct clr_lst_st *st);
                    
void free_statusRequest(struct stat_req_st *st);       
void free_configManage(struct conf_man_st *st);                             
void free_networkMaintenance(struct netw_man_st *st);

#endif

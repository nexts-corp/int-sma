#ifndef GSM_H
#define GSM_H

#include "main.h"

#define GSM_READ_SIGNAL_def "+CSQ"

typedef enum {
    GSM_ENOERR,                                                    /*! < no error. >*/
    GSM_ECMD,                                                        /*! < AT command error >*/
    GSM_ETIMEDOUT,                                            /*! < timeout error occurred. >*/
	GSM_ENVMODE													/*! < Mode not vaild. >*/
} eGSMErrorCode;

void iGsmSetUart(iChar_t viChannel_arg,iChar_t viBaudRate_arg);

#endif 


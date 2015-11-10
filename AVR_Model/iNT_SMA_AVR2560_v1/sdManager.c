#include "sdManager.h"

// global variable
//iChar_t viFolderDataName[] = "Data";
iChar_t viFolderDataName[] = "Record";
//iChar_t viFolderDataLogName[] = "Data_Log";
iChar_t viFolderDataLogName[] = "Log";
iChar_t viFolderEventName[] = "Event";
iChar_t viFolderStatusName[] = "Status";
iChar_t viFolderErrorName[] = "Error";     
iChar_t viFolderConfigName[] = "Config"; 

iChar_t viDirRootPath[] = "0:/";
//iChar_t viDirDataPath[] = "0:/DATA";
//iChar_t viDirDataPath[] = "0:/RECORD";
iChar_t viDirDataPath[] = "0:/RECORD";
//iChar_t viDirDataLogPath[] = "0:/DATA_LOG";
iChar_t viDirDataLogPath[] = "0:/LOG";
iChar_t viDirEventPath[] = "0:/EVENT";
iChar_t viDirStatusPath[] = "0:/STATUS";
iChar_t viDirErrorPath[] = "0:/ERROR"; 
iChar_t viDirConfigPath[] = "0:/CONFIG";

//iChar_t viDataFName[] = "0:./data1.nc";
iChar_t viDataFName[] = "0:./00000000.nc";
iChar_t viDataLogFName[] = "0:./dataLog1.nc";
iChar_t viEventFName[] = "0:./event1.nc";
iChar_t viStatusFName[] = "0:./status1.nc";
iChar_t viErrorFName[] = "0:./error1.nc";
iChar_t viConfigFName[] = "0:./config1.nc";

/* will hold file/directory information returned by f_readdir*/
FILINFO file_info;


///* recursively scan directory entries and display them */
//FRESULT directory_scan(char *path){
//    /* will hold the directory information */
//    DIR directory;
//    /* FAT function result */
//    FRESULT res;
//    int i;
//
//
//    if ((res=f_opendir(&directory,path))==FR_OK){
//       while (((res=f_readdir(&directory,&file_info))==FR_OK) &&
//             file_info.fname[0])
//             {
//             /* display file/directory name and associated information */
//             printDebug("%c%c%c%c%c %02u/%02u/%u %02u:%02u:%02u %9lu"                 "  %s/%s\r\n",
//                    (file_info.fattrib & AM_DIR) ? 'D' : '-',
//                    (file_info.fattrib & AM_RDO) ? 'R' : '-',
//                    (file_info.fattrib & AM_HID) ? 'H' : '-',
//                    (file_info.fattrib & AM_SYS) ? 'S' : '-',
//                    (file_info.fattrib & AM_ARC) ? 'A' : '-',
//                    file_info.fdate & 0x1F,(file_info.fdate >> 5) & 0xF,
//                    (file_info.fdate >> 9)+1980,
//                    file_info.ftime >> 11,(file_info.ftime >> 5) & 0x3F,
//                    (file_info.ftime & 0xF) << 1,
//                    file_info.fsize,path,file_info.fname);
//             if (file_info.fattrib & AM_DIR)
//                {
//                /* its a subdirectory */
//                /* make sure to skip past "." and ".." when recursing */
//                if (file_info.fname[0]!='.')
//                   {
//                   i=strlen(path);
//                   /* append the subdirectory name to the path */
//                   if (path[i-1]!='/') strcatf(path,"/");
//                   strcat(path,file_info.fname);
//                   /* scan subdirectory */
//                   res=directory_scan(path);
//                   /* restore the old path name */
//                   path[i]=0;
//                   /* remove any eventual '/' from the end of the path */
//                   --i;
//                   if (path[i]=='/') path[i]=0;
//                   /* stop if an error occured */
//                   if (res!=FR_OK) break;
//                   }
//                }
//              }
//    }
//   return res;
//}



iInt_t iCreateFolder(iUChar_t viFolderName_arg){
     
}


iInt_t iInitSDCard(){
    //Check SD folder 
//    char viFolderDataName[] = "Data";
//    char viFolderEventName[] = "Event";
//    char viFolderStatusName[] = "Status";
//    char viFolderErrorName[] = "Error";     
//    char viFolderConfigName[] = "Config"; 
//    char viDirRootPath[] = "0:/";
//    char viDirDataPath[] = "0:/DATA";
//    char viDirEventPath[] = "0:/EVENT";
//    char viDirStatusPath[] = "0:/STATUS";
//    char viDirErrorPath[] = "0:/ERROR"; 
//    char viDirConfigPath[] = "0:/CONFIG";
    FRESULT viFReturn;
    DIR viOutDir;
    

    viFReturn = f_opendir(&viOutDir, viDirDataPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]Record dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderDataName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir Record success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir Record error.\r\n");
            } 
        }
    } 
    
    viFReturn = f_opendir(&viOutDir, viDirDataLogPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]DataLog dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderDataLogName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir DataLog success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir DataLog error.\r\n");
            } 
        }
    }
    
    viFReturn = f_opendir(&viOutDir, viDirEventPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]Event dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderEventName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir Event success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir Event error.\r\n");
            } 
        }
    }
    
    viFReturn = f_opendir(&viOutDir, viDirStatusPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]Status dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderStatusName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir Status success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir Status error.\r\n");
            } 
        }
    }
    
    viFReturn = f_opendir(&viOutDir, viDirErrorPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]Error dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderErrorName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir Error success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir Error error.\r\n");
            } 
        }
    } 
        
    viFReturn = f_opendir(&viOutDir, viDirConfigPath);
    if(viFReturn == FR_OK){
        printDebug("[iInitSDCard]Config dir is exist.\r\n");
    }else{
        iFDisplayReturn(viFReturn);
        if(viFReturn == FR_NO_PATH){
            viFReturn = f_mkdir(viFolderConfigName);
            if(viFReturn == FR_OK){
               printDebug("[iInitSDCard]mkdir Config success.\r\n");
            }else{
               printDebug("[iInitSDCard]mkdir Config error.\r\n");
            } 
        }
    }  
}


void iFDisplayReturn(FRESULT viFResulyt_arg){
    switch(viFResulyt_arg){
        case FR_OK:{  
            printDebug("success.\r\n");
            break;
        } 
        case FR_NO_PATH:{  
            printDebug("couldn't find the path.\r\n");
            break;
        }
        case FR_INVALID_NAME:{  
            printDebug("the directory name is invalid.\r\n");
            break;
        }
        case FR_INVALID_DRIVE:{  
            printDebug("the drive number is invalid.\r\n");
            break;
        }
        case FR_NOT_READY:{  
            printDebug("no disk access was possible due to missing media or other reason.\r\n");
            break;
        }
        case FR_DISK_ERR:{  
            printDebug("the function failed because of a physical disk access function failure.\r\n");
            break;
        }
        case FR_INT_ERR:{  
            printDebug("the function failed due to a wrong FAT structure or an internal error.\r\n");
            break;
        } 
        case FR_NOT_ENABLED:{  
            printDebug("the logical drive was not mounted with f_mount.\r\n");
            break;
        }
        case FR_NO_FILESYSTEM:{  
            printDebug("there is no valid FAT partition on the disk.\r\n");
            break;
        }
        case FR_DENIED:{  
            printDebug("file access was denied because it was opened in read-only mode.\r\n");
            break;
        } 
        case FR_INVALID_OBJECT:{  
            printDebug("the file was not opened with f_open.\r\n");
            break;
        }
        case FR_NO_FILE:{  
            printDebug("couldn't find the file or directory.\r\n");
            break;
        }
        case FR_WRITE_PROTECTED:{  
            printDebug("the media in the drive is write protected.\r\n");
            break;
        }
    }
}


void iFCreate(FIL *pviOutFilePtr_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg){
    FRESULT viFReturn;
    //iUChar_t viReturn = 0;
    DIR viOutDir;
    FIL pviFilePtr; 
    
    if((viFReturn=f_chdir(pviDirPath_arg))==FR_OK){
       printDebug("[iFCreate]Current directory : %s.\r\n",pviDirPath_arg);  
       if((viFReturn=f_open(pviOutFilePtr_arg,pviFilename_arg,FA_CREATE_NEW))==FR_OK){
           printDebug("[iFCreate]File %s is created.\r\n",pviFilename_arg);
           if((viFReturn=f_close(pviOutFilePtr_arg))==FR_OK){ 
           }else{
              iFDisplayReturn(viFReturn);
           }
       }else{
           printDebug("[iFCreate]File %s : %s.[\r\n",pviFilename_arg); 
           iFDisplayReturn(viFReturn);
           printDebug("]\r\n");
       }
    }else{
       printDebug("[iFCreate]directory : %s.[\r\n",pviDirPath_arg); 
       iFDisplayReturn(viFReturn);
       printDebug("]\r\n"); 
    }
}

void iFRwite(const iChar_t * const pviDataBuff,iUInt_t ivLength_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long const * const pviWritePtr_arg){
    FIL *viFilePtr;
    FRESULT viFReturn;
    unsigned int nbytes;
    const iChar_t * pviDataBuffRef = (iChar_t * const )pviDataBuff;

    //print_payload((const iChar_t *)pviDataBuff,ivLength_arg);  
    print_payload(pviDataBuffRef,ivLength_arg);
    viFilePtr = (FIL * const)malloc(sizeof(FIL));
    if(viFilePtr!=NULL){
        if((viFReturn=f_chdir(pviDirPath_arg))==FR_OK){
           printDebug("[iFRwite]Current directory : %s.\r\n",pviDirPath_arg);  
           if((viFReturn=f_open(viFilePtr,pviFilename_arg,FA_WRITE|FA_OPEN_ALWAYS))==FR_OK){
               printDebug("[iFRwite]File %s is openned.\r\n",pviFilename_arg);
               
               /* Move to end of the file to append data */ 
               printDebug("[iFRwite]File size(%ld).\r\n",viFilePtr->fsize);
               if((viFReturn=f_lseek(viFilePtr, viFilePtr->fsize))==FR_OK){        //seek end of file in order append 
                   printDebug("[iFRwite]Pointer(seek) of File moved(%ld).\r\n",viFilePtr->fsize); 
                   //print_payload((const iChar_t *)pviDataBuff,ivLength_arg);
                   print_payload(pviDataBuffRef,ivLength_arg);
                   if((viFReturn=f_write(viFilePtr,(const iChar_t *)pviDataBuff,ivLength_arg,&nbytes))==FR_OK){    //viFReturn=f_write(viFilePtr,&pviDataBuff[0],strlen(pviDataBuff),&nbytes)
                       printDebug("[iFRwite]%d bytes written of %d\r\n",nbytes,ivLength_arg);
                   }else{
                       printDebug("[iFRwite]Write error(%d)\r\n",viFReturn);
                       iFDisplayReturn(viFReturn);
                   }  
               }else{
                   iFDisplayReturn(viFReturn);
               }
               
               if((viFReturn=f_close(viFilePtr))==FR_OK){ 
               }else{
                  iFDisplayReturn(viFReturn);
               }
           }else{
               printDebug("[iFRwite]File %s : %s.[\r\n",pviFilename_arg); 
               iFDisplayReturn(viFReturn);
               printDebug("]\r\n");
           }
        }else{
           printDebug("[iFRwite]directory : %s.[\r\n",pviDirPath_arg); 
           iFDisplayReturn(viFReturn);
           printDebug("]\r\n"); 
        }
        free(viFilePtr);
    }else{
        printDebug("[iFRwite]can't allocate mem.\r\n");
    }
    
}

void iFDelete(iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg){
    FRESULT viFReturn;
    if((viFReturn=f_chdir(pviDirPath_arg))==FR_OK){
       printDebug("[iFDelete]Current directory : %s.\r\n",pviDirPath_arg);  
       if((viFReturn=f_unlink(pviFilename_arg))==FR_OK){
           printDebug("[iFDelete]File %s is deleted.\r\n",pviFilename_arg);
       }else{
           printDebug("[iFDelete]File %s : %s. can't delete.[\r\n",pviFilename_arg); 
           iFDisplayReturn(viFReturn);
           printDebug("]\r\n");
       }
    }else{
       printDebug("[iFDelete]directory : %s.[\r\n",pviDirPath_arg); 
       iFDisplayReturn(viFReturn);
       printDebug("]\r\n"); 
    }
}

iChar_t iFSize(iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long *pviOutFileSize_arg){
    FIL *viFilePtr;
    FRESULT viFReturn; 
    iChar_t viReturn = -1; 
    
    viFilePtr = (FIL * const)malloc(sizeof(FIL)); 
    if(viFilePtr!=NULL){
       if((viFReturn=f_chdir(pviDirPath_arg))==FR_OK){
           printDebug("[iFSize]Current directory : %s.\r\n",pviDirPath_arg);  
           if((viFReturn=f_open(viFilePtr,pviFilename_arg,FA_WRITE|FA_OPEN_ALWAYS))==FR_OK){
               printDebug("[iFSize]File %s is openned.\r\n",pviFilename_arg);
                   
               /* Move to end of the file to append data */ 
               printDebug("[iFSize]File size(%ld).\r\n",viFilePtr->fsize);
               *pviOutFileSize_arg = viFilePtr->fsize;
                   
               if((viFReturn=f_close(viFilePtr))==FR_OK){ 
               }else{
                  iFDisplayReturn(viFReturn);
               }
               viReturn = 1;
           }else{
               printDebug("[iFSize]File %s : %s.[\r\n",pviFilename_arg); 
               iFDisplayReturn(viFReturn);
               printDebug("]\r\n");
           }
        }else{
           printDebug("[iFSize]directory : %s.[\r\n",pviDirPath_arg); 
           iFDisplayReturn(viFReturn);
           printDebug("]\r\n"); 
        } 
        free(viFilePtr);
    }else{
        printDebug("[iFSize]can't allocate mem.\r\n");
    }
    return viReturn;
}

void iFCreateFileDaily(iChar_t *pviFilename_arg){
    FRESULT viFReturn;
    DIR viOutDir;
    FIL *viFilePtr;

    if((viFReturn=f_chdir(viDirDataPath))==FR_OK){
        printDebug("[iFCreateFileDaily]Current directory : %s.\r\n",viDirDataPath);  
        if(viFReturn == FR_OK){
            printDebug("[iFCreateFileDaily]Data dir is exist.\r\n");  
            if((viFReturn=f_open(viFilePtr,pviFilename_arg,FA_OPEN_ALWAYS))==FR_OK){
                printDebug("[iGenFileDaily]File %s is openned.\r\n",pviFilename_arg);  
                if((viFReturn=f_close(viFilePtr))==FR_OK){ 
                }else{
                   iFDisplayReturn(viFReturn);
                }
            }else{
                iFDisplayReturn(viFReturn);
                printDebug("\r\n");
            }
        }else{
            iFDisplayReturn(viFReturn);
        } 
    }else{
       printDebug("[iFCreateFileDaily]directory : %s.[\r\n",viDirDataPath); 
       iFDisplayReturn(viFReturn);
       printDebug("]\r\n"); 
    }
}

iChar_t iFRead(iChar_t * pviDataBuff,iUInt_t ivLength_arg,iChar_t *pviDirPath_arg,iChar_t *pviFilename_arg,unsigned long const * const pviReadPtr_arg){
    FIL *viFilePtr;
    FRESULT viFReturn;
    unsigned int nbytes; 
    iChar_t viReturn = -1;

    //print_payload(pviDataBuffRef,ivLength_arg);
    viFilePtr = (FIL * const)malloc(sizeof(FIL));
    if(viFilePtr!=NULL){
        if((viFReturn=f_chdir(pviDirPath_arg))==FR_OK){
           printDebug("[iFRead]Current directory : %s.\r\n",pviDirPath_arg);  
           if((viFReturn=f_open(viFilePtr,pviFilename_arg,FA_READ))==FR_OK){
               printDebug("[iFRead]File %s is openned.\r\n",pviFilename_arg);
               
               /* Move to end of the file to append data */ 
               //printDebug("[iFRead]File size(%ld).\r\n",viFilePtr->fsize);
               if((viFReturn=f_lseek(viFilePtr, *pviReadPtr_arg))==FR_OK){        //seek end of file in order append 
                   printDebug("[iFRead]Pointer(seek) of File moved(%ld).\r\n",*pviReadPtr_arg); 
                   //print_payload((const iChar_t *)pviDataBuff,ivLength_arg);
                  // print_payload(pviDataBuffRef,ivLength_arg);
                   if((viFReturn=f_read(viFilePtr,pviDataBuff,ivLength_arg,&nbytes))==FR_OK){    //viFReturn=f_write(viFilePtr,&pviDataBuff[0],strlen(pviDataBuff),&nbytes)
                       printDebug("[iFRead]%d bytes Reading of %d\r\n",nbytes,ivLength_arg);   
                       viReturn = 1;
                   }else{
                       printDebug("[iFRead]Read error(%d)\r\n",viFReturn);
                       iFDisplayReturn(viFReturn);
                   }  
               }else{
                   iFDisplayReturn(viFReturn);
               }
               
               if((viFReturn=f_close(viFilePtr))==FR_OK){ 
               }else{
                  iFDisplayReturn(viFReturn);
               }
           }else{
               printDebug("[iFRead]File %s : %s.[\r\n",pviFilename_arg); 
               iFDisplayReturn(viFReturn);
               printDebug("]\r\n");
           }
        }else{
           printDebug("[iFRead]directory : %s.[\r\n",pviDirPath_arg); 
           iFDisplayReturn(viFReturn);
           printDebug("]\r\n"); 
        }
        free(viFilePtr);
    }else{
        printDebug("[iFRead]can't allocate mem.\r\n");
    }
    return viReturn;
}
#include "io.h"
#include "ds1672.h" 
#include "smaProtocol.h"
#include "debug.h"
#include "sensorTask.h"

extern iData_t viRXData;
extern iData_t viTXData;

iPTC_t iPTC_RX;
iPTC_t iPTC_TX;

//iChar_t * pviPTCData;

iChar_t iPTCParser(iData_t *pviOutData_arg){
     char *pviCheck;
     iChar_t viReturn = PTC_INVALID;
     iInt_t indexParser = 0;
     iUInt_t viDataLen = 0;
     iInt_t viCheckSD = 0;
     iInt_t viCheckSum = 0; 
     iInt_t viCheckLen = 0;
     int i = 0;
     
     
     //Prepair data that correct
     pviCheck = viRXData.value;
     if(*pviCheck == 0x7e){
        printDebug("[iPTCparser]Start Delimiter OK(%02x).\r\n",*pviCheck);
        viCheckSD = PTC_DS_SUCCESS;
     }else{
        viCheckSD = PTC_DS_INVALID;
     }
     
     viDataLen = (viRXData.value[1]<<8) | viRXData.value[2];
     if(viDataLen>4 && viDataLen<512){
        viCheckLen = PTC_LEN_SUCCESS;
        printDebug("[iPTCparser]Length (%d).\r\n",viDataLen);
     }else{
        viCheckLen = PTC_LEN_INVALID;
     } 
     
     
     pviCheck += (3 + viDataLen);         //3= SD(1)+Length(2)
     viCheckSum = *pviCheck;
     printDebug("[iPTCparser]Check sum (%02x).\r\n",viCheckSum); 
     viCheckSum = iCheckSum(&viRXData.value[3],(viRXData.length+1));
     if(viCheckSum == 0){
         printDebug("[iPTCparser]Check sum correct(%d).\r\n",viCheckSum); 
         viCheckSum = PTC_CS_SUCCESS;
     }else{
         printDebug("[iPTCparser]Check sum incorrect(%d).\r\n",viCheckSum);
         viCheckSum = PTC_CS_INVALID;
     }
     
     if((viCheckSD == PTC_DS_SUCCESS) && (viCheckLen == PTC_LEN_SUCCESS) && (viCheckSum == PTC_CS_SUCCESS)){ 
         memcpy(&iPTC_RX.hd,&viRXData.value[1],sizeof(iPTC_RX.hd));
         indexParser += sizeof(iPTC_RX.hd);
         iPTC_RX.dat.value = &viRXData.value[19];        //sd(1),len(2),mti(2),tid(8),tim(4),fid(2)   = 1+2+2+8+4+2 = 19 
         iPTC_RX.dat.len = (viDataLen + 3)-19;                               //(viDataLen) + sd(3) - 19
         //printDebug("[iPTCParser]Data will process(%d).[\r\n",iPTC_RX.dat.len); 
         //print_payload(iPTC_RX.dat.value, iPTC_RX.dat.len);   
         
         pviOutData_arg->length = iPTC_RX.dat.len;
         memcpy(&pviOutData_arg->value[0],&viRXData.value[19],iPTC_RX.dat.len); 
         
         viReturn = PTC_SUCCESS;
     }else{
         viReturn = PTC_INVALID;
     }
     return viReturn; 
}



iUChar_t iPTCCheckHostReq(iData_t *pviData_arg){
   iUChar_t iReturn = PTC_HOST_NOT_REQ;
   printDebug("[iPTCCheckHostReq]MTI(%02x%02x)",iPTC_RX.hd.mti[0],iPTC_RX.hd.mti[1]);
   if((iPTC_RX.hd.mti[1] & 0x01) == 1){
      printDebug("[iPTCCheckHostReq]Host Request.\r\n");
      iReturn = PTC_HOST_REQ;
   }else{
      printDebug("[iPTCCheckHostReq]Host is not Request.\r\n");
   }
   return iReturn;
}

iInt_t iCheckSum(iChar_t buf_ags[], iInt_t len_ags) {
    iUInt_t 	i = 0;
    iUInt_t 	sum = 0;      
	
    for (i = 0; i < len_ags; i++) {
        sum += buf_ags[i];
    }                 
    return (0xFF - (sum & 0xFF));
}

void iHostResponse(){}

iInt_t iGenFID(){
   iInt_t viFID = 0;
   do{
      viFID = rand();
   }while((viFID == 0) && (viFID == 0xffff));
   return viFID;
}


void iAckHostReq(){
    //7E0014080005050505050505055173C95524224B024A0010 
    //7E001008000505050505050505D072C95500006F 
    
//    iChat_t mit[2] = {0x80,0x00}; 
//    iInt_t viFrameID = 0;
//    viFrameID = iGenFID();  
//    iPTCPack(iData_t *pviOutData_arg, iChar_t *pviData_arg, iUInt_t viLength_arg, iChar_t *pviMTI_arg, iChar_t *pviTID_arg)
}

void iPTCPack(iData_t *pviOutData_arg, iChar_t *pviData_arg, iUInt_t viLength_arg, iChar_t *pviMTI_arg, iChar_t *pviTID_arg){
    //iChar_t * pviPTCData;
    iUInt_t viIndexWrite = 0;
    iUInt_t viLength = 0; 
    iUChar_t viCharLength[2]; 
    iChar_t viCharMTI[2];
    iChar_t viSD = 0x7e; 
    iUInt_t viCheckSum; 
    iUInt_t viFrameCSLen = 0;
    iInt_t viFrameID = 0;
    unsigned long int timestamp;
    
    viFrameCSLen = 2 + 8 + 4 + 2 + viLength_arg + 1;     //2(mti) + 8(tid) + 4(tim) + 2(fid) + ?(data) +1(cs)
    //pviPTCData = (iChar_t *)malloc((viLength+20)); //sd(1),len(2),mti(2),tid(8),tim(4),fid(2),cs(1) 
               
    //if(pviPTCData != NULL){
        //sd
        memcpy(&pviOutData_arg->value[viIndexWrite],&viSD,1); 
        viIndexWrite += 1;
        
        //len
        viLength = viLength_arg+16;
        viCharLength[0] = (viLength<<8);
        viCharLength[1] = viLength;
        memcpy(&pviOutData_arg->value[viIndexWrite],&viCharLength[0],2);  
        viIndexWrite += 2;
        
        //mti 
        memcpy(&pviOutData_arg->value[viIndexWrite],&pviMTI_arg[0],2); 
        viIndexWrite += 2; 
        
        //tid 
        memcpy(&pviOutData_arg->value[viIndexWrite],&pviTID_arg[0],8); 
        viIndexWrite += 8;
        
        //tim 
        timestamp = DS1672_read();
        memcpy(&pviOutData_arg->value[viIndexWrite],&timestamp,sizeof(timestamp)); 
        viIndexWrite += sizeof(timestamp);  
        
        memcpy(&viCharMTI[0],&pviMTI_arg[0],2);
        if((viCharMTI[0] == 0x08) && (viCharMTI[1] == 0x00)){
            //fid 
            if(viLength_arg==0){
                viFrameID = 0; 
                memcpy(&pviOutData_arg->value[viIndexWrite],&viFrameID,2); 
                viIndexWrite += 2; 
            }else{
                //fid
                viFrameID = iGenFID(); 
                memcpy(&pviOutData_arg->value[viIndexWrite],&viFrameID,2); 
                viIndexWrite += 2;
            }
        }else{
            //fid
            viFrameID = iGenFID(); 
            memcpy(&pviOutData_arg->value[viIndexWrite],&viFrameID,2); 
            viIndexWrite += 2;
        }
            
        
        
        if(viLength_arg != 0 ){
            //data
            memcpy(&pviOutData_arg->value[viIndexWrite],&pviData_arg[0],viLength_arg); 
            viIndexWrite += viLength_arg;
        }
        
        
        //cs 
        viCheckSum = iCheckSum(&pviOutData_arg->value[3],viFrameCSLen); 
        memcpy(&pviOutData_arg->value[viIndexWrite],&viCheckSum,1); 
        viIndexWrite += 1;          
        
        pviOutData_arg->length = viLength_arg+20;
        
        //printDebug("[iPTCPack]Print:\r\n");
        //print_payload(pviOutData_arg->value, (viLength_arg+20));  
        
//    }else{
//        printDebug("[iPTCPack]can 't allocate mem.\r\n");
//        //pviPTCData = 0;
//    } 
    //free(pviPTCData);
    //return pviPTCData;
}

//iChar_t * iPTCPack(iData_t *pviData_arg, iChar_t *pviData_arg, iUInt_t viLength_arg, iChar_t *pviMTI_arg, iChar_t *pviTID_arg){
//    //iChar_t * pviPTCData;
//    iUInt_t viIndexWrite = 0;
//    iUInt_t viLength = 0; 
//    iUChar_t viCharLength[2];
//    iChar_t viSD = 0x7e; 
//    iUInt_t viCheckSum; 
//    iUInt_t viFrameCSLen = 0;
//    iInt_t viFrameID = 0;
//    unsigned long int timestamp;
//    
//    viFrameCSLen = 2 + 8 + 4 + 2 + viLength_arg + 1;     //2(mti) + 8(tid) + 4(tim) + 2(fid) + ?(data) +1(cs)
//    pviPTCData = (iChar_t *)malloc((viLength+20)); //sd(1),len(2),mti(2),tid(8),tim(4),fid(2),cs(1) 
//    
//    if(pviPTCData != NULL){
//        //sd
//        memcpy(&pviPTCData[viIndexWrite],&viSD,1); 
//        viIndexWrite += 1;
//        
//        //len
//        viLength = viLength_arg+16;
//        viCharLength[0] = (viLength<<8);
//        viCharLength[1] = viLength;
//        memcpy(&pviPTCData[viIndexWrite],&viCharLength[0],2);  
//        viIndexWrite += 2;
//        
//        //mti 
//        memcpy(&pviPTCData[viIndexWrite],&pviMTI_arg[0],2); 
//        viIndexWrite += 2; 
//        
//        //tid 
//        memcpy(&pviPTCData[viIndexWrite],&pviTID_arg[0],8); 
//        viIndexWrite += 8;
//        
//        //tim 
//        timestamp = DS1672_read();
//        memcpy(&pviPTCData[viIndexWrite],&timestamp,sizeof(timestamp)); 
//        viIndexWrite += sizeof(timestamp);  
//        
//        //fid
//        viFrameID = iGenFID(); 
//        memcpy(&pviPTCData[viIndexWrite],&viFrameID,2); 
//        viIndexWrite += 2;
//        
//        //data
//        memcpy(&pviPTCData[viIndexWrite],&pviData_arg[0],viLength_arg); 
//        viIndexWrite += viLength_arg;
//        
//        //cs 
//        viCheckSum = iCheckSum(&pviPTCData[3],viFrameCSLen); 
//        memcpy(&pviPTCData[viIndexWrite],&viCheckSum,1); 
//        viIndexWrite += 1;          
//        
//        pviData_arg.length = viLength_arg+20;
//        
//        printDebug("[iPTCPack]Print:\r\n");
//        print_payload(pviPTCData, (viLength_arg+20));  
//        
//    }else{
//        printDebug("[iPTCPack]can 't allocate mem.\r\n");
//        pviPTCData = 0;
//    } 
//    //free(pviPTCData);
//    return pviPTCData;
//}

//void iDataReportPack(iChar_t *pviOutData,iDataReport_t *pviInData){
//
//}
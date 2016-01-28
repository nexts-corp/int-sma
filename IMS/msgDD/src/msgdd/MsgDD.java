/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package msgdd;
import java.io.*;
import java.net.*;


/**
 *
 * @author bongkot-bpg
 */
public class MsgDD {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        // TODO code application logic here
        MessageIMS msgIms = new MessageIMS();
        msgIms.nxGetData();
        
//        int i=1;
//        //String urlString = "http://messagedd.com/httpapi/sendsms/sendsms.aspx?username=GLOBAL_DETECT&password=0135554014045&to=0892517885&text=URL-TX-data&sender=IMS";
//        for(i=1;i<4;i++){
//            String urlString = "http://messagedd.com/httpapi/sendsms/sendsms.aspx?username=GLOBAL_DETECT&password=0135554014045&to=0814899956&text=AlarmInfo-"+i+"&sender=IMS";
//            System.out.println(getHTML(urlString));
//            try {
//                Thread.sleep(1000);                 //1000 milliseconds is one second.
//            } catch(InterruptedException ex) {
//                Thread.currentThread().interrupt();
//            }   
//        }
        
    }
    
    
}

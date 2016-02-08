/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package msgdd;

//import com.sun.xml.internal.fastinfoset.util.CharArray;
import java.io.*;
import java.net.*;
//import java.util.logging.Level;
//import java.util.logging.Logger;

/**
 *
 * @author bongkot-bpg
 */
public class MsgDDApi {

    String nxUrlString;

    public String getNxUrlString() {
        return nxUrlString;
    }

    public void setNxUrlString(String nxUrlString) {
        this.nxUrlString = nxUrlString;
    }

    public MsgDDApi() {
    }

    public MsgDDApi(MsgProvider nxMsgProvider) throws UnsupportedEncodingException {
        String encoded = null;
        this.nxUrlString = nxMsgProvider.nxServerName
                + "username=" + nxMsgProvider.nxUserName
                + "&password=" + nxMsgProvider.nxPassword
                + "&to=" + nxMsgProvider.nxTo
                + "&text=" + nxMsgProvider.nxText
                + "&sender=" + nxMsgProvider.nxSender;

        this.nxUrlString = nxMsgProvider.nxServerName;
        encoded = URLEncoder.encode(nxMsgProvider.nxUserName, "UTF-8");
        this.nxUrlString += "username=" + encoded;
        encoded = URLEncoder.encode(nxMsgProvider.nxPassword, "UTF-8");
        this.nxUrlString += "&password=" + encoded;
        encoded = URLEncoder.encode(nxMsgProvider.nxTo, "UTF-8");
        this.nxUrlString += "&to=" + encoded;
        encoded = URLEncoder.encode(nxMsgProvider.nxText, "UTF-8");
        this.nxUrlString += "&text=" + encoded;
        encoded = URLEncoder.encode(nxMsgProvider.nxSender, "UTF-8");
        this.nxUrlString += "&sender=" + encoded;

        //this.nxUrlString = "http://messagedd.com/httpapi/sendsms/sendsms.aspx?username=GLOBAL_DETECT&password=0135554014045&to=0892517885&text=URL-TX-data&sender=IMS";
    }

    public MasterData getHTML(String urlToRead) throws Exception {
        MasterData nxReturn = new MasterData();
        char[] c = new char[1024];
        int i;
        int countReaderCh;
        //StringBuilder result = new StringBuilder();
        URL url = new URL(urlToRead);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        String line;
//        while ((line = rd.readLine()) != null) {
//            result.append(line);
//        }
        countReaderCh = 0;
        while ((i = rd.read()) != -1) {
            // int to character
            c[countReaderCh++] = (char) i;
        }
        if (countReaderCh > 0) {
            String StringResult = null;
            StringResult = new String(String.valueOf(c));
            System.out.println("Character Read: " + StringResult);
            
            String[] parts = StringResult.split(":");
            String part1 = parts[0]; // 004
            String part2 = parts[1]; // 034556
            if (part1.equals("0")) {
                //System.out.println("Field Password is exsit.");
                nxReturn.setNxKey(part1);
                nxReturn.setNxValue(part2);
            }else{
                //nxReturn.setNxKey("0");
                //nxReturn.setNxValue("0");
            }
        }
        rd.close();
        //return result.toString();
        return nxReturn;
    }

}

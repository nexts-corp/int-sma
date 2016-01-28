/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package msgdd;

import java.net.URLEncoder;
import java.sql.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.commons.codec.binary.Base64;
//import java.util.Base64;

/**
 *
 * @author bongkot-bpg
 */
public class MessageIMS {

    String vnxMsg = "";
    boolean vnxFlag = false;

    // JDBC driver name and database URL
    static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";
    static final String DB_URL = "jdbc:mysql://localhost/ims_etm";
    static final String DB_URL_Log = "jdbc:mysql://localhost/ims_etm_log";

    //  Database credentials
    static final String USER = "root";
    static final String PASS = "password";

    Connection conn = null;
    Statement stmt = null;
    MsgProvider nxMsgProvider = new MsgProvider();

    void nxGetData() throws SQLException {
//        byte[]   bytesEncoded = Base64.encodeBase64("0135554014045".getBytes());
//        System.out.println("ecncoded value is " + new String(bytesEncoded ));
//        String orig = "0135554014045";
//
//        //encoding  byte array into base 64
//        byte[] encoded = Base64.encodeBase64(orig.getBytes());
//
//        System.out.println("Original String: " + orig);
//        System.out.println("Base64 Encoded String : " + new String(encoded));
//
//        //decoding byte array into base64
//        byte[] decoded = Base64.decodeBase64("527C95B40C033077965651130529399D".getBytes());
//        System.out.println("Base 64 Decoded  String : " + new String(decoded));

        if (nxGetMsgProvider()) {
            System.out.println("Message provider OK.");
            try {
                Class.forName("com.mysql.jdbc.Driver");
                System.out.println("Connecting to database...");
                conn = DriverManager.getConnection(DB_URL_Log, USER, PASS);

                System.out.println("Creating statement...");
                stmt = conn.createStatement();
                String sql;
                sql = "SELECT * FROM `sms_log` "
                        + "WHERE `SUCCESS` = 0 ORDER BY `SMS_ID` DESC LIMIT 1";
                ResultSet rs = stmt.executeQuery(sql);

                while (rs.next()) {
                    //Retrieve by column name
                    int nxId = rs.getInt("SMS_ID");
                    String nxUserName = rs.getString("USER_USERNAME");
                    String nxDistination = rs.getString("DESTINATION");
                    String nxMsg1 = rs.getString("MESSAGE1");
                    String nxMsg2 = rs.getString("MESSAGE2");
                    String nxProviderName = rs.getString("PROVIDER_NAME");
                    String nxTimestamp = rs.getString("TIMESTAMP");
                    int nxSuccess = rs.getInt("SUCCESS");

                    //Display values
                    System.out.print("id: " + nxId);
                    System.out.print(", user: " + nxUserName);
                    System.out.print(", des-tel: " + nxDistination);
                    System.out.print(", msg1: " + nxMsg1);
                    //System.out.print(", msg2: " + nxMsg2);
                    //System.out.print(", PROVIDER_NAME: " + nxProviderName);
                    System.out.print(", timestamp: " + nxTimestamp);
                    System.out.println(", success: " + nxSuccess);

                    nxMsgProvider.setNxTo(nxDistination);
                    nxMsgProvider.setNxText(nxMsg1);
                    
                    
                    //test
                    nxMsgProvider.setNxPassword("0135554014045");
                    nxMsgProvider.setNxTo("0892517885");
                    System.out.println("setNxText Length: " + nxMsgProvider.getNxText().length());
                    
                    nxMsgProvider.setNxText("Test-IMS-Send-to");
                    //nxMsgProvider.setNxSender("IMS-RAMA"); no work

                    MsgDDApi nxMsgApi = new MsgDDApi(nxMsgProvider);
                    System.out.println("URL MsgDD API: " + nxMsgApi.getNxUrlString());

//                    byte[] bytes = nxMsgProvider.getNxPassword().getBytes(); // Charset to encode into
//                    String s2 = new String(bytes); // Charset with which bytes were encoded 
//                    System.out.println("Password decode: " + s2);

                    try {
                        
                        nxMsgApi.getHTML(nxMsgApi.getNxUrlString());
                    } catch (Exception ex) {
                        Logger.getLogger(MsgDDApi.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }

                rs.close();
                stmt.close();
                conn.close();
            } catch (SQLException se) {
                //Handle errors for JDBC
                se.printStackTrace();
            } catch (Exception e) {
                //Handle errors for Class.forName
                e.printStackTrace();
            } finally {
                //finally block used to close resources
                try {
                    if (stmt != null) {
                        stmt.close();
                    }
                } catch (SQLException se2) {
                }// nothing we can do
                try {
                    if (conn != null) {
                        conn.close();
                    }
                } catch (SQLException se) {
                    se.printStackTrace();
                }//end finally try
            }//end try
        } else {
            System.out.println("Message provider fail.");
        }

    }

    void nxEditData() {

    }

    boolean nxGetMsgProvider() {
        boolean nxReturn = false;
        try {
            Class.forName("com.mysql.jdbc.Driver");
            System.out.println("Connecting to database...");
            conn = DriverManager.getConnection(DB_URL, USER, PASS);

            System.out.println("Creating statement...");
            stmt = conn.createStatement();
            String sql;
            sql = "SELECT * FROM `sms_provider` "
                    + "ORDER BY `sms_provider_id` DESC LIMIT 1";
            ResultSet rs = stmt.executeQuery(sql);

            while (rs.next()) {
                //Retrieve by column name
                int nxId = rs.getInt("sms_provider_id");
                String nxProviderName = rs.getString("sms_provider_name");
                String nxUserName = rs.getString("username");
                String nxPassword = rs.getString("password");
                String nxSenderName = rs.getString("sender_name");
                String nxMsg1 = rs.getString("message1");
                //String nxMsg2 = rs.getString("message2");

                //Display values
                System.out.print("id: " + nxId);
                System.out.print(", provider: " + nxProviderName);
                System.out.print(", user: " + nxUserName);
                System.out.print(", password: " + nxPassword);
                System.out.print(", sender: " + nxSenderName);
                System.out.println(", msg1: " + nxMsg1);
                //System.out.print(", msg2: " + nxMsg2);
                //System.out.print(", PROVIDER_NAME: " + nxProviderName);
                //System.out.print(", timestamp: " + nxTimestamp);
                //System.out.println(", success: " + nxSuccess);

                nxMsgProvider.setNxServerName("http://messagedd.com/httpapi/sendsms/sendsms.aspx?");
                nxMsgProvider.setNxUserName(nxUserName);
                nxMsgProvider.setNxPassword(nxPassword);
                //nxMsgProvider.setNxTo("");
                //nxMsgProvider.setNxText("");
                nxMsgProvider.setNxSender(nxSenderName);

                nxReturn = true;
            }

            rs.close();
            stmt.close();
            conn.close();
        } catch (SQLException se) {
            //Handle errors for JDBC
            se.printStackTrace();
        } catch (Exception e) {
            //Handle errors for Class.forName
            e.printStackTrace();
        } finally {
            //finally block used to close resources
            try {
                if (stmt != null) {
                    stmt.close();
                }
            } catch (SQLException se2) {
            }// nothing we can do
            try {
                if (conn != null) {
                    conn.close();
                }
            } catch (SQLException se) {
                se.printStackTrace();
            }//end finally try
        }//end try
        return nxReturn;
    }

}

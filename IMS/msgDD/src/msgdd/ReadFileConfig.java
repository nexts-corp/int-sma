/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package msgdd;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

/**
 *
 * @author bongkot-bpg
 */
public class ReadFileConfig {

    public ReadFileConfig() {
    }

    MasterData nxReadFile() throws FileNotFoundException, IOException {
        MasterData nxMapValue = new MasterData();
        BufferedReader br = new BufferedReader(new FileReader("c:/ims-MsgDD-config.txt"));
        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();

            while (line != null) {
                String[] parts = line.split(":");
                String part1 = parts[0]; // 004
                String part2 = parts[1]; // 034556
                if (part1.equals("Password")) {
                    System.out.println("Field Password is exsit.");
                    nxMapValue.setNxKey(part1);
                    nxMapValue.setNxValue(part2);
                    break;
                }
                sb.append(line);
                sb.append(System.lineSeparator());
                line = br.readLine();
            }
            String everything = sb.toString();
        } finally {
            System.out.println("Field is not exsit.");
            br.close();
        }
        return nxMapValue;
    }
    
    MasterData nxReadFile(String nxKey_arg) throws FileNotFoundException, IOException {
        MasterData nxMapValue = new MasterData();
        BufferedReader br = new BufferedReader(new FileReader("c:/ims-MsgDD-config.txt"));
        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();

            while (line != null) {
                String[] parts = line.split(":");
                String part1 = parts[0]; // 004
                String part2 = parts[1]; // 034556
                if (part1.equals(nxKey_arg)) {
                    System.out.println("Field Password is exsit.");
                    nxMapValue.setNxKey(part1);
                    nxMapValue.setNxValue(part2);
                    break;
                }
                sb.append(line);
                sb.append(System.lineSeparator());
                line = br.readLine();
            }
            String everything = sb.toString();
        } finally {
            System.out.println("Field is not exsit.");
            br.close();
        }
        return nxMapValue;
    }
}

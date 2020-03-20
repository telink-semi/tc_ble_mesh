package com.telink.ble.mesh.util;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Created by kee on 2019/9/26.
 */

public class Test {
    static byte[] buf = {0x2a, (byte) 0xa7, 0x17, 0x79, 0x01, (byte) 0x9a, 0x48, (byte) 0xb2, (byte) 0xb5, 0x64, 0x53, (byte) 0xa8, (byte) 0xc8, 0x12, 0x49, (byte) 0xa3,
            0x2c, 0x30, 0x30, 0x30, 0x31, 0x38, 0x62, 0x62, 0x38, 0x2c, 0x38, 0x30, 0x64, 0x61, 0x30, 0x37,
            0x63, 0x63, 0x64, 0x61, 0x30, 0x62, 0x2c, 0x38, 0x30, 0x66, 0x65, 0x64, 0x35, 0x39, 0x61, 0x65,
            0x37, 0x65, 0x66, 0x38, 0x38, 0x61, 0x30, 0x65, 0x38, 0x34, 0x37, 0x66, 0x66, 0x66, 0x62, 0x30,
            0x61, 0x30, 0x33, 0x36, 0x32, 0x61, 0x38};

    public static String str = "drfiHgbsvomOieog,000293e2,abcdf0f1f2f3,atFY1tGDCo4MQSVCGVDqtti3PvBI5WXb";

    public static void main(String args[]) {
        String re = SHA(str.getBytes(), "SHA-256");
        System.out.print(re);
    }


    private static String SHA(byte[] data, final String strType) {
        String strResult = null;
        try {
            MessageDigest messageDigest = MessageDigest.getInstance(strType);
            messageDigest.update(data);
            byte byteBuffer[] = messageDigest.digest();

            StringBuffer strHexString = new StringBuffer();
            for (int i = 0; i < byteBuffer.length; i++) {
                String hex = Integer.toHexString(0xff & byteBuffer[i]);
                if (hex.length() == 1) {
                    strHexString.append('0');
                }
                strHexString.append(hex);
            }
            strResult = strHexString.toString();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return strResult;
    }


}

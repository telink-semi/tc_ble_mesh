/********************************************************************************************************
 * @file     Arrays.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.util;

import java.io.UnsupportedEncodingException;
import java.security.SecureRandom;
import java.util.Formatter;

/**
 * array util
 */
public final class Arrays {

    private Arrays() {
    }

    /**
     * reverse array
     *
     * @param a
     * @return
     */
    public static byte[] reverse(byte[] a) {

        if (a == null)
            return null;

        int p1 = 0, p2 = a.length;
        byte[] result = new byte[p2];

        while (--p2 >= 0) {
            result[p2] = a[p1++];
        }

        return result;
    }

    /**
     * reverse part of array
     *
     * @param arr
     * @param begin
     * @param end
     * @return
     */
    public static byte[] reverse(byte[] arr, int begin, int end) {

        while (begin < end) {
            byte temp = arr[end];
            arr[end] = arr[begin];
            arr[begin] = temp;
            begin++;
            end--;
        }

        return arr;
    }

    /**
     * compare two arrays
     *
     * @param array1
     * @param array2
     * @return
     */
    public static boolean equals(byte[] array1, byte[] array2) {

        if (array1 == array2) {
            return true;
        }

        if (array1 == null || array2 == null || array1.length != array2.length) {
            return false;
        }

        for (int i = 0; i < array1.length; i++) {
            if (array1[i] != array2[i]) {
                return false;
            }
        }

        return true;
    }

    public static String bytesToString(byte[] array) {

        if (array == null) {
            return "null";
        }

        if (array.length == 0) {
            return "[]";
        }

        StringBuilder sb = new StringBuilder(array.length * 6);
        sb.append('[');
        sb.append(array[0]);
        for (int i = 1; i < array.length; i++) {
            sb.append(", ");
            sb.append(array[i]);
        }
        sb.append(']');
        return sb.toString();
    }

    public static String bytesToString(byte[] data, String charsetName) throws UnsupportedEncodingException {
        return new String(data, charsetName);
    }


    public static String bytesToHexString(byte[] array) {
        return bytesToHexString(array, "");
    }

    /**
     * array to hex string
     */
    public static String bytesToHexString(byte[] array, String separator) {
        if (array == null || array.length == 0)
            return "";

        StringBuilder sb = new StringBuilder();

        Formatter formatter = new Formatter(sb);
        formatter.format("%02X", array[0]);

        for (int i = 1; i < array.length; i++) {

//            if (!Strings.isEmpty(separator))
            sb.append(separator);

            formatter.format("%02X", array[i]);
        }

        formatter.flush();
        formatter.close();

        return sb.toString();
    }

    public static byte[] hexToBytes(String hexStr) {
        if (hexStr == null) return null;
        if (hexStr.length() == 1) {
            hexStr = "0" + hexStr;
        }
        int length = hexStr.length() / 2;
        byte[] result = new byte[length];

        for (int i = 0; i < length; i++) {
            result[i] = (byte) Integer.parseInt(hexStr.substring(i * 2, i * 2 + 2), 16);
        }

        return result;
    }

    public static byte[] generateRandom(int length) {
        byte[] data = new byte[length];
        SecureRandom secureRandom = new SecureRandom();
        secureRandom.nextBytes(data);
        return data;
    }
}

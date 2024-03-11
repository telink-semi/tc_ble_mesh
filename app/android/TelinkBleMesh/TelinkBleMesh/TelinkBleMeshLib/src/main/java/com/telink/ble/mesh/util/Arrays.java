/********************************************************************************************************
 * @file Arrays.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.util;

import java.io.UnsupportedEncodingException;
import java.security.SecureRandom;

/**
 * Provide APIs for byte array
 */
public final class Arrays {
    public static final char[] HEX_BASIC = "0123456789ABCDEF".toCharArray();

    private Arrays() {
    }

    /**
     * reverse array
     *
     * @param a input
     * @return reversed array
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
     * @param arr   input
     * @param begin start offset
     * @param end   end offset
     * @return output array
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
     * @param array1 array-1
     * @param array2 array-2
     * @return is equals
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

    /**
     * convert byte array to string,
     * output format is "[1, 2]"
     *
     * @param array input
     * @return DEC string
     */
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

    /**
     * convert byte to string by target charset
     *
     * @param data        input
     * @param charsetName charset
     * @return string
     * @throws UnsupportedEncodingException throw exception if charset not support
     */
    public static String bytesToString(byte[] data, String charsetName) throws UnsupportedEncodingException {
        return new String(data, charsetName);
    }

    /**
     * convert byte array to hex string, without separator
     * for example: {0x1A, 0x2B, 0x3C} -> "1A2B3C"
     *
     * @param array input
     * @return converted HEX string
     */
    public static String bytesToHexString(byte[] array) {
        return bytesToHexString(array, "");
    }

    /**
     * convert byte array to hex string, without separator
     * for example: input array={0x1A, 0x2B, 0x3C}, separator=":" , return "1A:2B:3C"
     *
     * @param array     input
     * @param separator separator
     * @return converted HEX string
     */
    public static String bytesToHexString(byte[] array, String separator) {
        if (array == null || array.length == 0)
            return "";

//        final boolean sepNul = TextUtils.isEmpty(separator);
        final boolean sepNul = separator == null || separator.length() == 0;
        StringBuilder hexResult = new StringBuilder();
        int ai;
        for (int i = 0; i < array.length; i++) {
            ai = array[i] & 0xFF;
            if (i != 0 && !sepNul) {
                hexResult.append(separator);
            }
            hexResult.append(HEX_BASIC[ai >>> 4]).append(HEX_BASIC[ai & 0x0F]);
        }
        return hexResult.toString();
    }

    /**
     * convert HEX string to byte array
     * for example: "1A2B3C" -> {0x1A, 0x2B, 0x3C}
     *
     * @param hexStr input
     * @return converted byte array
     */
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

    /**
     * generate random byte array
     *
     * @param length byte array length
     * @return output
     */
    public static byte[] generateRandom(int length) {
        byte[] data = new byte[length];
        SecureRandom secureRandom = new SecureRandom();
        secureRandom.nextBytes(data);
        return data;
    }
}

/********************************************************************************************************
 * @file Strings.java
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

import java.nio.charset.Charset;

/**
 * @deprecated
 */

/**
 * The Strings class provides utility methods for converting strings to bytes and vice versa,
 * as well as checking if a string is empty.
 * <p>
 * This class is final and cannot be extended.
 */
public final class Strings {

    /**
     * Private constructor to prevent instantiation of the class.
     */
    private Strings() {
    }

    /**
     * Converts a given string to bytes with a specified length.
     * If the length is less than or equal to 0, the default charset is used.
     *
     * @param str    the string to convert
     * @param length the length of the resulting byte array
     * @return the byte array representation of the string
     */
    public static byte[] stringToBytes(String str, int length) {
        byte[] srcBytes;

        if (length <= 0) {
            return str.getBytes(Charset.defaultCharset());
        }

        byte[] result = new byte[length];
        srcBytes = str.getBytes(Charset.defaultCharset());

        if (srcBytes.length <= length) {
            System.arraycopy(srcBytes, 0, result, 0, srcBytes.length);
        } else {
            System.arraycopy(srcBytes, 0, result, 0, length);
        }

        return result;
    }

    /**
     * Converts a given string to bytes using the default charset.
     *
     * @param str the string to convert
     * @return the byte array representation of the string
     */
    public static byte[] stringToBytes(String str) {
        return stringToBytes(str, 0);
    }

    /**
     * Converts a given byte array to a string using the default charset.
     *
     * @param data the byte array to convert
     * @return the string representation of the byte array
     */
    public static String bytesToString(byte[] data) {
        return data == null || data.length <= 0 ? null : new String(data, Charset.defaultCharset()).trim();
    }

    /**
     * Checks if a given string is empty.
     *
     * @param str the string to check
     * @return true if the string is empty, false otherwise
     */
    public static boolean isEmpty(String str) {
        return str == null || str.trim().isEmpty();
    }
}

package com.telink.ble.mesh.util;

import java.nio.charset.Charset;

public final class Strings {

    private Strings() {
    }

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

    public static byte[] stringToBytes(String str) {
        return stringToBytes(str, 0);
    }

    public static String bytesToString(byte[] data) {
        return data == null || data.length <= 0 ? null : new String(data, Charset.defaultCharset()).trim();
    }

    public static boolean isEmpty(String str) {
        return str == null || str.trim().isEmpty();
    }
}

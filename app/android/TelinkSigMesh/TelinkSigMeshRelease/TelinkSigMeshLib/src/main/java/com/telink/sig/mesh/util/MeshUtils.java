/********************************************************************************************************
 * @file MeshUtils.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.util;

import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.model.OpcodeType;

import java.nio.ByteOrder;
import java.security.SecureRandom;
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Random;

public final class MeshUtils {

    //    public static final int DEVICE_ADDRESS_MAX = 0xFFFE; // 0x00FF

    public static final int DEVICE_ADDRESS_MAX = 0x7FFF; // 0x7F00
    public static final int DEVICE_ADDRESS_MIN = 0x0001; // 0x0001

    public static final String CHARS = "123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ+-*/<>/?!@#$%^&;'[]{}|,.";

    // 1970 -- 2000 offset second
    public static final long TAI_OFFSET_SECOND = 946684800;

    private static SecureRandom rng;

    private MeshUtils() {
    }

    public static byte[] generateRandom(int length) {

        byte[] data = new byte[length];

        synchronized (MeshUtils.class) {
            if (rng == null) {
                rng = new SecureRandom();
            }
        }

        rng.nextBytes(data);

        return data;
    }

    public static long getTaiTime() {
        return Calendar.getInstance().getTimeInMillis() / 1000 - TAI_OFFSET_SECOND;
    }

    /**
     * Mesh V1.0 3.7.3.1 Operation codes
     * Opcode Format
     * Notes
     * 0xxxxxxx (excluding 01111111)
     * 1-octet Opcodes
     * 01111111
     * Reserved for Future Use
     * 10xxxxxx xxxxxxxx
     * 2-octet Opcodes
     * 11xxxxxx zzzzzzzz
     * 3-octet Opcodes
     */
    public static OpcodeType getOpType(int opVal) {
//        final int opVal = getValue();
        return (opVal & bit(7)) != 0
                ?
                ((opVal & bit(6)) != 0 ? OpcodeType.VENDOR : OpcodeType.SIG_2)
                :
                OpcodeType.SIG_1;
    }

    public static int bit(int n) {
        return 1 << n;
    }

    public static byte[] generateChars(int length) {

        int charLen = CHARS.length() - 1;
        int charAt;

        byte[] data = new byte[length];

        for (int i = 0; i < length; i++) {
            charAt = (int) Math.round(Math.random() * charLen);
            data[i] = (byte) CHARS.charAt(charAt);
        }

        return data;
    }


    /**
     * convert byte buffer to integer
     *
     * @param buffer target buffer
     * @param order  {@link ByteOrder#BIG_ENDIAN} and {@link ByteOrder#LITTLE_ENDIAN}
     * @return int value, max 32-bit length
     */
    public static int bytes2Integer(byte[] buffer, ByteOrder order) {
        int re = 0;
        int valLen = (buffer.length > 4 ? 4 : buffer.length);
        for (int i = 0; i < valLen; i++) {
            if (order == ByteOrder.LITTLE_ENDIAN) {
                re |= (buffer[i] & 0xFF) << (8 * i);
            } else if (order == ByteOrder.BIG_ENDIAN) {
                re |= (buffer[i] & 0xFF) << (8 * (valLen - i - 1));
            }
        }
        return re;
    }

    public static byte[] integer2Bytes(int i, int size, ByteOrder order) {
        if (size > 4) return null;
        byte[] re = new byte[size];
        for (int j = 0; j < size; j++) {
            if (order == ByteOrder.LITTLE_ENDIAN) {
                re[j] = (byte) (i >> (8 * j));
            } else {
                re[size - j - 1] = (byte) (i >> (8 * j));
            }
        }
        return re;
    }

}

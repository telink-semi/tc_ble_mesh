/********************************************************************************************************
 * @file FirmwareUpdateStatusParser.java
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
package com.telink.sig.mesh.light.parser;

public final class FirmwareUpdateStatusParser {

    private FirmwareUpdateStatusParser() {
    }

    public static FirmwareUpdateStatusParser create() {
        return new FirmwareUpdateStatusParser();
    }

    // 62:06:1A:CB:02:00:03:64:64:FF:00:00:00:00:00:00
    public FirmwareUpdateStatus parse(byte[] params) {
        final int MIN_LEN = 13; // min len of params
        if (params == null || params.length < MIN_LEN) return null;
        FirmwareUpdateStatus updateStatus = new FirmwareUpdateStatus();

        int index = 0;
        updateStatus.status = params[index++];
        updateStatus.phase = (byte) (params[index] & 0b111); // 3 bit
        updateStatus.additionalInfo = (byte) (params[index++] >> 3); // 5 bit

        updateStatus.cid = (params[index++] & 0xFF) | ((params[index++] & 0xFF) << 8);
        int firmIdLen = params.length - MIN_LEN + 1;
        byte[] firmwareId = new byte[firmIdLen];
        System.arraycopy(params, index, firmwareId, 0, firmIdLen);
        updateStatus.firmwareId = firmwareId;

        index += firmIdLen;

        updateStatus.objectId = (params[index++] & 0xFF)
                + ((params[index++] & 0xFF) << 8)
                + ((params[index++] & 0xFF) << 16)
                + ((params[index++] & 0xFF) << 24)
                + ((long)(params[index++] & 0xFF) << 32)
                + ((long)(params[index++] & 0xFF) << 40)
                + ((long)(params[index++] & 0xFF) << 48)
                + ((long)(params[index] & 0xFF) << 56);
        // F0:16:06:00:01:00:B6:08 : 00:00:11:02:21:00:00:FF:11:22:33:44:55:66:77:88

        return updateStatus;
    }

    public class FirmwareUpdateStatus {
        // 8 bits
        // 0 success
        public byte status;

        // 3 bits
        public byte phase;

        // 5 bits
        public byte additionalInfo;

        // company id: 16 bits
        public int cid;

        // 8 + n*8 bits
        public byte[] firmwareId;

        // 64 bits
        long objectId;
    }
}

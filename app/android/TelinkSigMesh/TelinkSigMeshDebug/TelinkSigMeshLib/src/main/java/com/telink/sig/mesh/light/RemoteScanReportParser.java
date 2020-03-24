/********************************************************************************************************
 * @file PublicationStatusParser.java
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
package com.telink.sig.mesh.light;

import com.telink.sig.mesh.model.RemoteDevice;
import com.telink.sig.mesh.util.Arrays;


public final class RemoteScanReportParser {

    private RemoteScanReportParser() {
    }

    public static RemoteScanReportParser create() {
        return new RemoteScanReportParser();
    }


    // sample data src: 02:00: params : CA:D6:DD:00:55:A9:13:63:39:B0:AE:8B:04:FE:87:6A:B6:00:00

    /**
     * u16 src;
     * s8 rssi;
     * u8 uuid[16];
     * u16 oob;
     */
    public RemoteDevice parse(byte[] params) {
        int index = 0;
        RemoteDevice info = new RemoteDevice();
        try {
            info.rssi = params[index++];
            int len = 16;
            info.uuid = new byte[len];
            System.arraycopy(params, index, info.uuid, 0, len);
            index += 16;
            info.oob = (params[index++] & 0xFF) | ((params[index] & 0xFF) << 8);

            int macLen = 6;
            byte[] macBytes = new byte[macLen];
            System.arraycopy(info.uuid, 10, macBytes, 0, macLen);
            // 小端转大端
            macBytes = Arrays.reverse(macBytes);
            info.mac = Arrays.bytesToHexString(macBytes, ":").toUpperCase();
            return info;
        } catch (IndexOutOfBoundsException e) {
            return null;
        }
    }


}

/********************************************************************************************************
 * @file     CtlStatusNotificationParser.java 
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
package com.telink.sig.mesh.light.parser;

import com.telink.sig.mesh.TelinkApplication;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.util.TelinkLog;

public final class NotificationInfoParser {

    private NotificationInfoParser() {
    }

    public static NotificationInfoParser create() {
        return new NotificationInfoParser();
    }


    public NotificationInfo parseInfo(byte[] rawData) {
        if (rawData == null || rawData.length < 2) return null;
        int index = 0;
        byte cmdRsp = rawData[index++];
        int len = rawData[index++] & 0xFF;

        if (rawData.length - 2 != len) return null;

        try {
            int srcAdr = (rawData[index++] & 0xFF) | ((rawData[index++] & 0xFF) << 8);

            int destAdr = (rawData[index++] & 0xFF) | ((rawData[index++] & 0xFF) << 8);

            byte prefix = rawData[index];
            int opcode;
            boolean vendorRsp = false;
            if ((prefix & 0xFF) >= (0xC0 & 0xFF)) {
                // vendor
                opcode = (rawData[index++] & 0xFF)
                        | ((rawData[index++] & 0xFF) << 8)
                        | ((rawData[index] & 0xFF) << 16);
                vendorRsp = true;
            } else if ((prefix & 0xFF) >= (0x80 & 0xFF)) {
                // sig 2 bytes
                opcode = (rawData[index++] & 0xFF) | ((rawData[index] & 0xFF) << 8);
                vendorRsp = false;
            } else {
                // sig 1 byte
                opcode = prefix & 0xFF;
            }
            index++;
            byte[] params = new byte[rawData.length - index];
            System.arraycopy(rawData, index, params, 0, params.length);

            return new NotificationInfo(srcAdr, destAdr, opcode, vendorRsp, params);
        } catch (IndexOutOfBoundsException ie) {
            return null;
        }

    }
}

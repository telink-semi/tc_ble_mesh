/********************************************************************************************************
 * @file     PublicationStatusParser.java 
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
package com.telink.sig.mesh.light;

import java.util.ArrayList;
import java.util.List;

public final class PublicationStatusParser {

    private PublicationStatusParser() {
    }

    public static PublicationStatusParser create() {
        return new PublicationStatusParser();
    }


    public StatusInfo parse(byte[] data) {

// publication rsp: F0:12:02:00:01:00:80:19: 00:02:00:FF:FF:00:10:FF:32:15:00:10
        int index = 0;
        StatusInfo info = new StatusInfo();
        try {
            info.status = data[index++];
            info.elementAddress = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            info.publishAddress = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            info.appKeyIndex = (data[index++] & 0xFF) | ((data[index] & 0b1111) << 8);
            info.credentialFlag = ((data[index] >> 4) & 0b1) == 1;
            info.rfu = (data[index] >> 5) & 0b111;
            index++;
            info.ttl = data[index++];
            info.period = data[index++];
            info.retransmit = data[index++];

            info.modelId = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            if ((index + 2) <= data.length) {
                info.modelId |= ((data[index++] & 0xFF) << 16) | ((data[index] & 0xFF) << 24);
            }
            return info;
        } catch (IndexOutOfBoundsException e) {
            return null;
        }
    }

    public class StatusInfo {
        public byte status;
        public int elementAddress;
        public int publishAddress;
        public int appKeyIndex;
        public boolean credentialFlag;
        public int rfu;
        public int ttl;
        public int period;
        public int retransmit;
        public int modelId;
    }

}

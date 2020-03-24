/********************************************************************************************************
 * @file CtlStatusNotificationParser.java
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

import java.util.ArrayList;
import java.util.List;

public final class OnlineStatusInfoParser {

    private OnlineStatusInfoParser() {
    }

    public static OnlineStatusInfoParser create() {
        return new OnlineStatusInfoParser();
    }

    // 62:06:1A:CB:02:00:03:64:64:FF:00:00:00:00:00:00
    public List<OnlineStatusInfo> parseInfo(byte[] rawData) {
        if (rawData == null || rawData.length < 4) return null;
        final int len = rawData.length;
        final int MIN_NODE_LEN = 3;
        int index = 0;
        byte type = rawData[index++];
        if (type != 0x62) {
            return null;
        }

        // low 4 bit
        int nodeLen = rawData[index++] & 0x0F;

        int statusLen = nodeLen - MIN_NODE_LEN;
        if (statusLen <= 0) return null;

        int sno = (rawData[index++] & 0xFF) | ((rawData[index++] & 0xFF) << 8);

        List<OnlineStatusInfo> statusInfoList = null;

        int address;
        byte sn;
        byte[] status;
        OnlineStatusInfo statusInfo;
        while (index + nodeLen <= len) {
            // 15 bit
            address = (rawData[index++] & 0xFF) | ((rawData[index++] & 0x7F) << 8);

            sn = rawData[index++];
            status = new byte[statusLen];

            System.arraycopy(rawData, index, status, 0, statusLen);
            index += statusLen;

            if (address == 0x00)
                break;


            statusInfo = new OnlineStatusInfo();
            statusInfo.address = address;
            statusInfo.sn = sn;
            statusInfo.status = status;
            if (statusInfoList == null) {
                statusInfoList = new ArrayList<>();
            }
            statusInfoList.add(statusInfo);
        }

        return statusInfoList;
    }

    public class OnlineStatusInfo {


        /*typedef struct{
            u8 type;
            u8 len_node_st_val  : 4;
            u8 rsv              : 4;
            u16 sno;        // for iv index
            mesh_node_st_val_t node[2];
            u8 mic[ONLINE_ST_MIC_LEN_GATT];
        }online_st_report_t;    // for GATT report */


        /*typedef struct{
            u16 dev_adr :15;     // don't change include type
            u16 rsv     :1;
            u8 sn;          // don't change include type
            u8 par[MESH_NODE_ST_PAR_LEN];  //lumen-rsv,
        }mesh_node_st_val_t;*/
        /*byte type;
        byte len;
        byte rsv;
        int sno;*/

        public int address;

//        byte rsv; // 1 bit

        // sn: 0 offline
        public byte sn;

        public byte[] status;
    }
}

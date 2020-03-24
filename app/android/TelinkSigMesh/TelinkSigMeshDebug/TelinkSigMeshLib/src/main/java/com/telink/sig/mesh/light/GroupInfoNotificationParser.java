/********************************************************************************************************
 * @file     GroupInfoNotificationParser.java 
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

import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.util.TelinkLog;
import com.telink.sig.mesh.util.UnitConvert;

import java.util.ArrayList;
import java.util.List;

public final class GroupInfoNotificationParser {

    private GroupInfoNotificationParser() {
    }

    public static GroupInfoNotificationParser create() {
        return new GroupInfoNotificationParser();
    }


    public GroupInfo parse(byte[] data) {

//        F0:0D:01:00:00:7F:80:1F:00:01:00:03:C0:00:10
        if (data == null || data.length < 2) return null;
        int index = 0;
        byte cmdRsp = data[index++];
        byte len = data[index++];
        if (data.length - 2 != len) return null;

        int srcAddr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);

        int destAddr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);


        if (srcAddr == destAddr) {
            return null;
        }
        int opCode = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        if (opCode == Opcode.CFG_SIG_MODEL_SUB_LIST.getValue()) {
            index++; // skip no meaning byte
            int src = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            int type = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            //  type == MeshLib.Constant.SIG_MD_G_ONOFF_S &&
            if ((data.length - index) % 2 == 0) {

                List<Integer> subList = new ArrayList<>();
                int forL = (data.length - index) / 2;
                for (int i = 0; i < forL; i++) {
                    int groupAddr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
                    subList.add(groupAddr);
                }
                GroupInfo groupInfo = new GroupInfo();
                groupInfo.model = type;
                groupInfo.meshAddress = src;
                groupInfo.subList = subList;
                return groupInfo;
            }
        }
        return null;

    }

    public final class GroupInfo {
        int model;
        public int meshAddress;
        public List<Integer> subList;
    }

}

/********************************************************************************************************
 * @file     SchedulerNotificationParser.java 
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

import com.telink.sig.mesh.model.Scheduler;

import java.util.ArrayList;
import java.util.List;

public final class SchedulerNotificationParser {

    private SchedulerNotificationParser() {
    }

    public static SchedulerNotificationParser create() {
        return new SchedulerNotificationParser();
    }

    public SchedulerInfo parse(byte[] data) {

//        F0:13:02:00:01:00:5F: 20:F9:FF:8C:FB:FE:1F:00:00:00: 00:00:00:00
        if (data == null || data.length < 2) return null;
        int index = 0;
        byte cmdRsp = data[index++];
        int len = data[index++] & 0xFF;

        if (data.length - 2 != len) return null;

        int srcAddr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);

        int destAddr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);


        if (srcAddr == destAddr) {
            return null;
        }
        int opCode = (data[index++] & 0xFF);
        if (opCode == Opcode.SCHD_ACTION_STATUS.getValue()) {
            long register = 0L;
            for (int i = 0; i < 8; i++) {
                register |= ((long) data[index++] & 0xffL) << (8 * i);
            }

            short sceneId = (short) ((data[index++] & 0xFF)
                    | ((data[index] & 0xFF) << 8));
            return new SchedulerInfo(register, sceneId);
        }
        return null;

    }

    public final class SchedulerInfo {
        public long register;
        public short sceneId;

        private SchedulerInfo(long register, short sceneId) {
            this.register = register;
            this.sceneId = sceneId;
        }


    }

}

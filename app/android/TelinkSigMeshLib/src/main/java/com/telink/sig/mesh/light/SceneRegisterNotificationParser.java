/********************************************************************************************************
 * @file     SceneRegisterNotificationParser.java 
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

public final class SceneRegisterNotificationParser {

    private SceneRegisterNotificationParser() {
    }

    public static SceneRegisterNotificationParser create() {
        return new SceneRegisterNotificationParser();
    }

    // raw data: F0:0D:02:00:01:00:82:45: 00:37:00:01:00:37:00
    // param data: 00:37:00:01:00:37:00
    public SceneRegisterInfo parse(byte[] params) {
        if (params == null || params.length == 0) {
            return null;
        }
        try {
            SceneRegisterInfo registerInfo = new SceneRegisterInfo();
            registerInfo.status = params[0];
            registerInfo.curScene = (params[1] & 0xFF) | ((params[2] & 0xFF) << 8);
            int rest = params.length - 3;
            if (rest == 0 || rest % 2 != 0) {
                registerInfo.scenes = null;
            } else {
                int len = rest / 2;
                int index = 3;
                registerInfo.scenes = new int[len];
                for (int i = 0; i < len; i++) {
                    registerInfo.scenes[i] = (params[index++] & 0xFF) | ((params[index++] & 0xFF) << 8);
                }
            }
            return registerInfo;
        } catch (IndexOutOfBoundsException ioe) {
            return null;
        }
    }

    public final class SceneRegisterInfo {
        public int status;
        public int curScene;
        public int[] scenes;
    }
}

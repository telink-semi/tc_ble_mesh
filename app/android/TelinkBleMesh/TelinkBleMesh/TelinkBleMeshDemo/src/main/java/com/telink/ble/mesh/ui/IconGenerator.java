/********************************************************************************************************
 * @file IconGenerator.java
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
package com.telink.ble.mesh.ui;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;

/**
 * Created by kee on 2018/9/29.
 */

public class IconGenerator {

    /**
     * @param pid         0: common device, 1: lpn
     * @param onlineState -1: offline; 0: off, 1: on
     * @return res
     */
    public static int getIcon(int pid, OnlineState onlineState) {
        switch (pid) {
            case AppSettings.PID_LPN:
                return R.drawable.ic_low_power;
            case AppSettings.PID_REMOTE:
                return R.drawable.ic_rmt;
            default:
                if (onlineState == OnlineState.OFFLINE) {
                    return R.drawable.ic_bulb_offline;
                } else if (onlineState == OnlineState.OFF) {
                    return R.drawable.ic_bulb_off;
                } else {
            /*if (deviceInfo.lum == 100) {
                return R.drawable.ic_bulb_on;
            } else {
                return R.drawable.ic_bulb_on_half;
            }*/
                    return R.drawable.ic_bulb_on;
                }
        }
    }

    public static int generateDeviceIconRes(int onOff) {
//        return R.drawable.ic_low_power;
        if (onOff == -1) {
            return R.drawable.ic_bulb_offline;
        } else if (onOff == 0) {
            return R.drawable.ic_bulb_off;
        } else {
            /*if (deviceInfo.lum == 100) {
                return R.drawable.ic_bulb_on;
            } else {
                return R.drawable.ic_bulb_on_half;
            }*/
            return R.drawable.ic_bulb_on;
        }
    }

}

/********************************************************************************************************
 * @file IconGenerator.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.ui;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.AppSettings;
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

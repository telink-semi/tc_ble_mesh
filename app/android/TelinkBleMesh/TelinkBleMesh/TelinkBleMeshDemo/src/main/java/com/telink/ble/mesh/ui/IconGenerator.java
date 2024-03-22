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

import com.telink.ble.mesh.core.DeviceProperty;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeSensorState;
import com.telink.ble.mesh.model.OnlineState;

/**
 * Created by kee on 2018/9/29.
 */

public class IconGenerator {


    /**
     * @return resId
     */
    public static int getIcon(NodeInfo device) {
        return getIcon(device, device.getOnlineState());
    }


    /**
     * @return resId
     */
    public static int getIcon(NodeInfo device, OnlineState onlineState) {
        boolean isSensor = device.isSensor();
        final int pid = device.compositionData != null ? device.compositionData.pid : 0;
        if (device.compositionData != null) {
            if (isSensor) {
                NodeSensorState sensorState = device.getFirstSensorState();
                if (sensorState != null) {
                    int sensorStateId = sensorState.propertyID;
                    if (sensorStateId == DeviceProperty.PRESENT_AMBIENT_LIGHT_LEVEL.id) {
                        if (onlineState == OnlineState.OFFLINE) {
                            return R.drawable.ic_light_sensor_offline;
                        }
                        return R.drawable.ic_light_sensor_on;
                    } else if (sensorStateId == DeviceProperty.MOTION_SENSED.id) {
                        if (onlineState == OnlineState.OFFLINE) {
                            return R.drawable.ic_motion_sensor_offline;
                        }
                        return R.drawable.ic_motion_sensor_on;
                    }
                }
                if (onlineState == OnlineState.OFFLINE) {
                    return R.drawable.ic_sensor_offline;
                }else{
                    return R.drawable.ic_sensor_on;
                }
            }
        }
        if (AppSettings.isLpn(pid)) {
            return R.drawable.ic_low_power;
        } else if (AppSettings.isRemote(pid)) {
            return R.drawable.ic_rmt;
        } else {
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


    /**
     * @param pid         0: common device, 1: lpn
     * @param onlineState -1: offline; 0: off, 1: on
     * @return res
     * @deprecated
     */
    public static int getDevIcon(int pid, OnlineState onlineState, boolean isSensor, Integer sensorStateId) {
        if (isSensor) {
            if (sensorStateId != null) {
                if (sensorStateId == DeviceProperty.PRESENT_AMBIENT_LIGHT_LEVEL.id) {
                    if (onlineState == OnlineState.OFFLINE) {
                        return R.drawable.ic_light_sensor_offline;
                    }
                    return R.drawable.ic_light_sensor_on;
                } else if (sensorStateId == DeviceProperty.MOTION_SENSED.id) {
                    if (onlineState == OnlineState.OFFLINE) {
                        return R.drawable.ic_motion_sensor_offline;
                    }
                    return R.drawable.ic_motion_sensor_on;
                }
            }
            if (onlineState == OnlineState.OFFLINE) {
                return R.drawable.ic_sensor_offline;
            }
            return R.drawable.ic_sensor_on;
        }
        if (AppSettings.isLpn(pid)) {
            return R.drawable.ic_low_power;
        } else if (AppSettings.isRemote(pid)) {
            return R.drawable.ic_rmt;
        } else {
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

    /**
     * @deprecated
     * @param onOff
     * @return
     */
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

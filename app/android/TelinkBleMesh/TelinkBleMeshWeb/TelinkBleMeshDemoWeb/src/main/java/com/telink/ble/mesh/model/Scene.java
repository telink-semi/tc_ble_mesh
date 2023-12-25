/********************************************************************************************************
 * @file Scene.java
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
package com.telink.ble.mesh.model;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * scene
 * Created by kee on 2018/10/8.
 */

public class Scene implements Serializable {


    /**
     * scene name
     */
    public String name = "Telink-Scene";

    /**
     * scene id
     */
    public int id;

    public List<SceneState> states = new ArrayList<>();

    public static class SceneState implements Serializable {
        /**
         * address
         * device unicast address(0x01 -- 0x7FFF) or group address (C000 - 0xFEFF)
         */
        public int address;

        /**
         * on off value
         * -1 unknown
         */
        public int onOff;

        /**
         * lum(lightness 0-100) value
         * -1 unknown
         */
        public int lum;

        /**
         * temperature value
         * -1 unknown
         */
        public int temp;

        public SceneState() {
        }

        public SceneState(int address) {
            this.address = address;
            this.onOff = -1;
            this.lum = -1;
            this.temp = -1;
        }
    }


    //    public List<Group> innerGroups = new ArrayList<>();
//    public List<DeviceInfo> innerDevices = new ArrayList<>();

    public void saveFromDeviceInfo(NodeInfo deviceInfo) {
        for (SceneState state : states) {
            if (state.address == deviceInfo.meshAddress) {
                state.onOff = deviceInfo.getOnlineState().st;
                state.lum = deviceInfo.lum;
                state.temp = deviceInfo.temp;
                return;
            }
        }
        SceneState state = new SceneState();
        state.address = deviceInfo.meshAddress;
        state.onOff = deviceInfo.getOnlineState().st;
        state.lum = deviceInfo.lum;
        state.temp = deviceInfo.temp;
        states.add(state);
    }

    public void removeByAddress(int address) {
        Iterator<SceneState> iterator = states.iterator();
        while (iterator.hasNext()) {
            if (iterator.next().address == address) {
                iterator.remove();
                return;
            }
        }
    }

    /*public void insertDevice(DeviceInfo deviceInfo) {
        for (DeviceInfo device : innerDevices) {
            if (device.meshAddress == deviceInfo.meshAddress) {
                device.macAddress = deviceInfo.macAddress;
                device.setOnOff(deviceInfo.getOnOff());
                device.lum = deviceInfo.lum;
                device.temp = deviceInfo.temp;
                return;
            }
        }
        DeviceInfo device = new DeviceInfo();
        device.meshAddress = deviceInfo.meshAddress;
        device.macAddress = deviceInfo.macAddress;
        device.setOnOff(deviceInfo.getOnOff());
        device.lum = deviceInfo.lum;
        device.temp = deviceInfo.temp;
        innerDevices.add(device);
    }*/

    /*public void deleteDevice(int address) {
        Iterator<DeviceInfo> iterator = innerDevices.iterator();
        while (iterator.hasNext()) {
            if (iterator.next().meshAddress == address) {
                iterator.remove();
                return;
            }
        }
    }*/

    /*public boolean contains(Group group) {
        for (Group inner : innerGroups) {
            if (inner.address == group.address) {
                return true;
            }
        }
        return false;
    }*/

    public boolean contains(NodeInfo device) {
        for (SceneState inner : states) {
            if (inner.address == device.meshAddress) {
                return true;
            }
        }
        return false;
    }
}

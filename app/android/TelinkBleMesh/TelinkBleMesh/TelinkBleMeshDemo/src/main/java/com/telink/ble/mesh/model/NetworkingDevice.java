/********************************************************************************************************
 * @file NetworkingDevice.java
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

import android.bluetooth.BluetoothDevice;
import android.graphics.Color;

import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

public class NetworkingDevice {

    public NetworkingState state = NetworkingState.IDLE;

    public BluetoothDevice bluetoothDevice;

    /**
     * oob info in scan record
     */
    public int oobInfo;

    public NodeInfo nodeInfo;

    // rssi
    public int rssi;

    public int serverAddress;

    public List<LogInfo> logs = new ArrayList<>();

    public boolean logExpand = false;

    public static final String TAG_SCAN = "scan";

    public static final String TAG_PROVISION = "provision";

    public static final String TAG_BIND = "bind";

    public static final String TAG_PUB_SET = "pub-set";

    public NetworkingDevice(NodeInfo nodeInfo) {
        this.nodeInfo = nodeInfo;
    }

    public int getStateColor() {
        return Color.YELLOW;
    }

    public boolean isProcessing() {
        return state == NetworkingState.PROVISIONING || state == NetworkingState.BINDING || state == NetworkingState.TIME_PUB_SETTING;
    }

    public void addLog(String tag, String log) {
        logs.add(new LogInfo(tag, log, MeshLogger.LEVEL_DEBUG));
    }
}

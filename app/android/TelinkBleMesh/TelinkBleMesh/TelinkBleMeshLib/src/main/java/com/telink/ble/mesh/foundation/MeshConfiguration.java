/********************************************************************************************************
 * @file MeshConfiguration.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.foundation;

import android.util.SparseArray;

/**
 * Mesh info use when provisioning/binding/auto connecting
 * NO variable can be NULL
 * Created by kee on 2019/9/6.
 */

public class MeshConfiguration {

    /**
     * network key index
     */
    public int netKeyIndex;

    /**
     * network key
     */
    public byte[] networkKey;

    /**
     * appKeyIndex and appKey map
     */
    public SparseArray<byte[]> appKeyMap;

    /**
     * iv index
     */
    public int ivIndex;

    /**
     * sequence number used in network pdu
     */
    public int sequenceNumber;

    /**
     * provisioner address
     */
    public int localAddress;

    /**
     * unicastAddress and deviceKey map, required for mesh configuration message
     */
    public SparseArray<byte[]> deviceKeyMap;

    public int getDefaultAppKeyIndex() {
        return appKeyMap.size() > 0 ? appKeyMap.keyAt(0) : 0;
    }

    public byte[] getDefaultAppKey() {
        return appKeyMap.size() > 0 ? appKeyMap.valueAt(0) : null;
    }
}

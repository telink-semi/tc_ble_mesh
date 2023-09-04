/********************************************************************************************************
 * @file HeartbeatStatus.java
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
package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.MeshUtils;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/23.
 */
public class HeartbeatStatus {

    /**
     * Reserved for Future Use
     */
    private int rfu;

    /**
     * 7 bits
     * Initial TTL used when sending the message
     */
    private int initTTL;

    /**
     * Bit field of currently active features of the node
     */
    private int features;

    public void parse(byte[] transportPdu) {
        this.rfu = (transportPdu[0] & 0xFF) >> 7;
        this.initTTL = transportPdu[0] & 0x7F;
        this.features = MeshUtils.bytes2Integer(new byte[]{transportPdu[1], transportPdu[2]},
                ByteOrder.BIG_ENDIAN);
    }


    public int getRfu() {
        return rfu;
    }

    public int getInitTTL() {
        return initTTL;
    }

    public int getFeatures() {
        return features;
    }
}

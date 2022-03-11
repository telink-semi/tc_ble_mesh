/********************************************************************************************************
 * @file ConfigMessage.java
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
package com.telink.ble.mesh.core.message.config;

import androidx.annotation.IntRange;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * configuration message
 * use device key for enc/dec
 */
public abstract class ConfigMessage extends MeshMessage {

    public ConfigMessage() {
    }

    public ConfigMessage(@IntRange(from = 1, to = 0x7FFF) int destinationAddress) {
        this.destinationAddress = destinationAddress;
        // default rsp max
        this.responseMax = 1;
    }

    /**
     * for config message , AKF is 0
     *
     * @return device key type
     */
    @Override
    public AccessType getAccessType() {
        return AccessType.DEVICE;
    }

}

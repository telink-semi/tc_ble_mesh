/********************************************************************************************************
 * @file SubnetBridgeSetMessage.java
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

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2021/1/14.
 */

public class SubnetBridgeSetMessage extends ConfigMessage {

    public byte subnetBridgeState;

    public SubnetBridgeSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public SubnetBridgeSetMessage(int destinationAddress, byte bridgeState) {
        super(destinationAddress);
        this.subnetBridgeState = bridgeState;
    }

    @Override
    public int getOpcode() {
        return Opcode.SUBNET_BRIDGE_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.SUBNET_BRIDGE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{subnetBridgeState};
    }


}

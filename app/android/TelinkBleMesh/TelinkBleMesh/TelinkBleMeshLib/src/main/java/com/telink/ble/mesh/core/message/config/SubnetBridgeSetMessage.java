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
 * This class represents a message for setting the state of a subnet bridge.
 * It extends the ConfigMessage class.
 */
public class SubnetBridgeSetMessage extends ConfigMessage {
    public byte subnetBridgeState;

    /**
     * Constructs a SubnetBridgeSetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public SubnetBridgeSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Constructs a SubnetBridgeSetMessage object with the specified destination address and bridge state.
     *
     * @param destinationAddress The destination address of the message.
     * @param bridgeState        The state of the subnet bridge.
     */
    public SubnetBridgeSetMessage(int destinationAddress, byte bridgeState) {
        super(destinationAddress);
        this.subnetBridgeState = bridgeState;
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.SUBNET_BRIDGE_SET.value;
    }

    /**
     * Returns the opcode of the response message.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.SUBNET_BRIDGE_STATUS.value;
    }

    /**
     * Returns the parameters of the message.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{subnetBridgeState};
    }
}
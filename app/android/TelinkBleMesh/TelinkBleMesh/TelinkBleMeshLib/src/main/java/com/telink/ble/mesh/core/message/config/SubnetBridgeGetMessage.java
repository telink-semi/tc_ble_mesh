/********************************************************************************************************
 * @file SubnetBridgeGetMessage.java
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
 * This class represents a message for retrieving information about a subnet bridge.
 * It extends the ConfigMessage class and provides methods for getting the opcode, response opcode,
 * and parameters of the message.
 */
public class SubnetBridgeGetMessage extends ConfigMessage {

    /**
     * Constructs a SubnetBridgeGetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address to which the message will be sent.
     */
    public SubnetBridgeGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Constructs a SubnetBridgeGetMessage object with the specified destination address and bridge state.
     *
     * @param destinationAddress The destination address to which the message will be sent.
     * @param bridgeState        The state of the bridge.
     */
    public SubnetBridgeGetMessage(int destinationAddress, byte bridgeState) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the SubnetBridgeGetMessage, which is Opcode.SUBNET_BRIDGE_GET.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.SUBNET_BRIDGE_GET.value;
    }

    /**
     * Returns the response opcode value for the SubnetBridgeGetMessage, which is Opcode.SUBNET_BRIDGE_STATUS.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.SUBNET_BRIDGE_STATUS.value;
    }

    /**
     * Returns the parameters of the SubnetBridgeGetMessage.
     *
     * @return The parameters.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}
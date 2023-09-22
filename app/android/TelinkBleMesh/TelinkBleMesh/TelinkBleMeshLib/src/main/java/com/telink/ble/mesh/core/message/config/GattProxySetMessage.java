/********************************************************************************************************
 * @file GattProxySetMessage.java
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
 * The GattProxySetMessage class represents an acknowledged message used to set the GATT Proxy state of a node.
 * This message is used to enable or disable the GATT Proxy feature.
 * <p>
 * The response to a GattProxySetMessage is a GattProxyStatusMessage.
 * <p>
 * Example usage:
 * GattProxySetMessage message = GattProxySetMessage.getSimple(destinationAddress, gattProxy);
 *
 * @see ConfigMessage
 * @see GattProxyStatusMessage
 */
public class GattProxySetMessage extends ConfigMessage {

    /**
     * The new GATT Proxy state.
     * Possible values:
     * - 00: enabled
     * - 01: disabled
     */
    public byte gattProxy;

    /**
     * Constructs a GattProxySetMessage with the specified destination address.
     *
     * @param destinationAddress The address of the node to which the message is being sent.
     */
    public GattProxySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Creates a simple GattProxySetMessage with the specified destination address and GATT Proxy state.
     *
     * @param destinationAddress The address of the node to which the message is being sent.
     * @param gattProxy          The new GATT Proxy state.
     * @return GattProxySetMessage instance.
     */
    public static GattProxySetMessage getSimple(int destinationAddress, byte gattProxy) {
        GattProxySetMessage instance = new GattProxySetMessage(destinationAddress);
        instance.gattProxy = gattProxy;
        return instance;
    }

    /**
     * Gets the opcode of the GattProxySetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_GATT_PROXY_SET.value;
    }

    /**
     * Gets the response opcode of the GattProxySetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_GATT_PROXY_STATUS.value;
    }

    /**
     * Gets the parameters of the GattProxySetMessage.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{gattProxy};
    }
}
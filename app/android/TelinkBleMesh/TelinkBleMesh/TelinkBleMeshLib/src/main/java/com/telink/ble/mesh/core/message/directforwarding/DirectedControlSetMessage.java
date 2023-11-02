/********************************************************************************************************
 * @file FirmwareUpdateCancelMessage.java
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
package com.telink.ble.mesh.core.message.directforwarding;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Represents a message for setting directed control parameters.
 * Extends the ConfigMessage class.
 */
public class DirectedControlSetMessage extends ConfigMessage {
    /**
     * Represents the network key index.
     * Stored as a 16-bit integer.
     */
    public int netKeyIndex;
    /**
     * Represents the directed forwarding parameter.
     * Stored as an 8-bit byte.
     */
    public byte directedForwarding;
    /**
     * Represents the directed relay parameter.
     * Stored as an 8-bit byte.
     */
    public byte directedRelay;
    /**
     * Represents the directed proxy parameter.
     * Stored as an 8-bit byte.
     */
    private byte directedProxy;
    /**
     * Represents the directed proxy use directed default parameter.
     * Stored as an 8-bit byte.
     */
    private byte directedProxyUseDirectedDefault;
    /**
     * Represents the directed friend parameter.
     * Stored as an 8-bit byte.
     */
    public byte directedFriend;

    /**
     * Returns a DirectedControlSetMessage object with the specified destination address and network key index.
     * Sets the response maximum to 1.
     *
     * @param destinationAddress The destination address of the message.
     * @param netKeyIndex        The network key index.
     * @return A DirectedControlSetMessage object.
     */
    public static DirectedControlSetMessage getSimple(int destinationAddress, int netKeyIndex) {
        DirectedControlSetMessage message = new DirectedControlSetMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    /**
     * Returns a DirectedControlSetMessage object with the specified destination address, network key index, and directed forwarding parameter.
     * Sets the response maximum to 1.
     *
     * @param destinationAddress     The destination address of the message.
     * @param netKeyIndex            The network key index.
     * @param enableDirectForwarding The directed forwarding parameter.
     * @return A DirectedControlSetMessage object.
     */
    public static DirectedControlSetMessage getDFEnable(int destinationAddress, int netKeyIndex, boolean enableDirectForwarding) {
        DirectedControlSetMessage message = new DirectedControlSetMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        message.directedForwarding = (byte) (enableDirectForwarding ? 1 : 0);
        return message;
    }

    /**
     * Constructs a DirectedControlSetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public DirectedControlSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode for the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.DIRECTED_CONTROL_SET.value;
    }

    /**
     * Returns the response opcode for the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.DIRECTED_CONTROL_STATUS.value;
    }

    /**
     * Sets the directed proxy parameter.
     * Also sets the directed proxy use directed default parameter to the same value.
     *
     * @param directedProxy The directed proxy parameter.
     */
    public void setDirectedProxy(byte directedProxy) {
        this.directedProxy = directedProxy;
        this.directedProxyUseDirectedDefault = directedProxy;
    }

    /**
     * Sets the directed proxy use directed default parameter.
     *
     * @param directedProxyUseDirectedDefault The directed proxy use directed default parameter.
     */
    public void setDirectedProxyUseDirectedDefault(byte directedProxyUseDirectedDefault) {
        this.directedProxyUseDirectedDefault = directedProxyUseDirectedDefault;
    }

    /**
     * Returns the byte array representation of the message parameters.
     *
     * @return The byte array representation of the parameters.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) netKeyIndex)
                .put(directedForwarding)
                .put(directedRelay)
                .put(directedProxy)
                .put(directedProxyUseDirectedDefault)
                .put(directedFriend).array();
    }
}
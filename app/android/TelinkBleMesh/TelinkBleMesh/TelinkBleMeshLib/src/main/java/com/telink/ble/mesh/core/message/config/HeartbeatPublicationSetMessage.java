/********************************************************************************************************
 * @file HeartbeatPublicationSetMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Heartbeat Publication Set is an acknowledged message used to set the current Heartbeat Publication state of an element
 * * <p>
 * The HeartbeatPublicationSetMessage class represents a Config Heartbeat Publication Set message.
 * This message is used to set the current Heartbeat Publication state of an element.
 * The response to this message is a Config Heartbeat Publication Status message.
 */
public class HeartbeatPublicationSetMessage extends ConfigMessage {
    private int destination; // The destination address of the message
    private byte countLog; // The logarithm of the Heartbeat Publication count
    private byte periodLog; // The logarithm of the Heartbeat Publication period
    private byte hbTtl; // The TTL value for the Heartbeat messages
    private int features; // The features supported by the element
    private int netKeyIndex; // The index of the Network Key

    /**
     * Constructs a new HeartbeatPublicationSetMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message
     */
    public HeartbeatPublicationSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Gets the opcode of the HeartbeatPublicationSetMessage.
     *
     * @return The opcode value
     */
    @Override
    public int getOpcode() {
        return Opcode.HEARTBEAT_PUB_SET.value;
    }

    /**
     * Gets the parameters of the HeartbeatPublicationSetMessage.
     *
     * @return The parameters as a byte array
     */
    @Override
    public byte[] getParams() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) destination)
                .put(countLog)
                .put(periodLog)
                .put(hbTtl)
                .putShort((short) features)
                .putShort((short) netKeyIndex);
        return byteBuffer.array();
    }

    /**
     * Sets the destination address of the message.
     *
     * @param destination The destination address to set
     */
    public void setDestination(int destination) {
        this.destination = destination;
    }

    /**
     * Sets the logarithm of the Heartbeat Publication count.
     *
     * @param countLog The count logarithm to set
     */
    public void setCountLog(byte countLog) {
        this.countLog = countLog;
    }

    /**
     * Sets the logarithm of the Heartbeat Publication period.
     *
     * @param periodLog The period logarithm to set
     */
    public void setPeriodLog(byte periodLog) {
        this.periodLog = periodLog;
    }

    /**
     * Sets the TTL value for the Heartbeat messages.
     *
     * @param hbTtl The TTL value to set
     */
    public void setHbTtl(byte hbTtl) {
        this.hbTtl = hbTtl;
    }

    /**
     * Sets the features supported by the element.
     *
     * @param features The features to set
     */
    public void setFeatures(int features) {
        this.features = features;
    }

    /**
     * Sets the index of the Network Key.
     *
     * @param netKeyIndex The Network Key index to set
     */
    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }
}
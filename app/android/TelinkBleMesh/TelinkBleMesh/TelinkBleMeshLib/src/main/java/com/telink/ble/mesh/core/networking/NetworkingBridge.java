/********************************************************************************************************
 * @file NetworkingBridge.java
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
package com.telink.ble.mesh.core.networking;

import com.telink.ble.mesh.core.proxy.ProxyPDU;

/**
 * This interface represents a bridge for networking operations.
 */
public interface NetworkingBridge {

    /**
     * This method is called when a command is prepared to be sent over the network.
     *
     * @param type proxy pdu typeValue {@link ProxyPDU#type}
     * @param data gatt data
     */
    void onCommandPrepared(byte type, byte[] data);

    /**
     * application layer should save updated network info
     * This method is called when the network information is updated.
     *
     * @param sequenceNumber The sequence number.
     * @param ivIndex        The IV index.
     */
    void onNetworkInfoUpdate(int sequenceNumber, int ivIndex);

    /**
     * mesh model message
     * This method is called when a mesh model message is received.
     *
     * @param src    The source address.
     * @param dst    The destination address.
     * @param opcode The opcode of the message.
     * @param params The parameters of the message.
     */
    void onMeshMessageReceived(int src, int dst, int opcode, byte[] params);

    /**
     * received proxy status message when set filter type, or add/remove address
     *
     * @param success True if the initialization was successful, false otherwise.
     * @param address The unicast address of the connected node.
     */
    void onProxyInitComplete(boolean success, int address);


    /**
     * heartbeat message received
     *
     * @param data heartbeat data
     */
    void onHeartbeatMessageReceived(int src, int dst, byte[] data);

    /**
     * This method is called when a reliable message is complete.
     *
     * @param success  True if the response was received, false otherwise.
     * @param opcode   The command opcode.
     * @param rspMax   The maximum expected response count.
     * @param rspCount The received response count.
     */
    void onReliableMessageComplete(boolean success, int opcode, int rspMax, int rspCount);

    /**
     * This method is called when a segment message is complete.
     *
     * @param success True if the segment message was successfully received, false otherwise.
     */
    void onSegmentMessageComplete(boolean success);
}

/********************************************************************************************************
 * @file ResetNetworkMessage.java
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
package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteOrder;
/**
 * This class represents a message to reset the network.
 * It extends the GenericMessage class.
 */
public class ResetNetworkMessage extends GenericMessage {
    /**
     * The delay in milliseconds.
     * It is represented by 2 bytes.
     */
    private int delay;
    
    /**
     * Creates a new ResetNetworkMessage with the given destination address, app key index, and delay.
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     */
    public ResetNetworkMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }
    
    /**
     * Returns a simple ResetNetworkMessage with the given destination address, app key index, and delay.
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     * @param delay The delay in milliseconds.
     * @return A ResetNetworkMessage object.
     */
    public static ResetNetworkMessage getSimple(int destinationAddress, int appKeyIndex, int delay) {
        ResetNetworkMessage message = new ResetNetworkMessage(destinationAddress, appKeyIndex);
        message.delay = delay;
        return message;
    }
    
    /**
     * Returns the opcode of the message.
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_RESET_NETWORK.value;
    }
    
    /**
     * Returns the response opcode of the message.
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }
    
    /**
     * Returns the parameters of the message as a byte array.
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(delay, 2, ByteOrder.LITTLE_ENDIAN);
    }
    
    /**
     * Sets the delay of the message.
     * @param delay The delay in milliseconds.
     */
    public void setDelay(int delay) {
        this.delay = delay;
    }
}
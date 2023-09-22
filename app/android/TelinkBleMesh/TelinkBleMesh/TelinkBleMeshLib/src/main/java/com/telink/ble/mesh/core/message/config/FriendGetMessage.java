/********************************************************************************************************
 * @file FriendGetMessage.java
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
 * The Config Friend Get message is used to request the current Friend state of a node.
 * It is an acknowledged message, meaning that the recipient will respond with a Config Friend Status message.
 * {@link FriendStatusMessage}
 */
public class FriendGetMessage extends ConfigMessage {

    /**
     * Constructs an empty FriendGetMessage object.
     */
    public FriendGetMessage() {
    }

    /**
     * Constructs a FriendGetMessage object with the specified destination address.
     *
     * @param destinationAddress The address of the node to which the message is being sent.
     */
    public FriendGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode of the Friend Get message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_FRIEND_GET.value;
    }

    /**
     * Returns the opcode of the expected response message to the Friend Get message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_FRIEND_STATUS.value;
    }

    /**
     * Returns the parameters of the Friend Get message.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}
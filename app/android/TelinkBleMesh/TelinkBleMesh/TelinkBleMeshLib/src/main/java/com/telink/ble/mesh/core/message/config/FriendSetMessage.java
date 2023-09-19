/********************************************************************************************************
 * @file FriendSetMessage.java
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
 * The Config Friend Set is an acknowledged message used to set the Friend state of a node.
 * <p>
 * The response to a Config Friend Get message is a Config Friend Status message.
 * * {@link FriendStatusMessage}
 */

public class FriendSetMessage extends ConfigMessage {

    /**
     * New Friend state
     */
    public byte friend;

    public FriendSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static FriendSetMessage getSimple(int destinationAddress, byte friend) {
        FriendSetMessage instance = new FriendSetMessage(destinationAddress);
        instance.friend = friend;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_FRIEND_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_FRIEND_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{friend};
    }
}

/********************************************************************************************************
 * @file LevelGetMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * This class represents a generic level get message.
 * It extends the GenericMessage class.
 * It is used to request the current level of a device.
 * The message includes the destination address and the application key index.
 * The opcode for this message is G_LEVEL_GET.
 * The parameters for this message are null.
 */
public class LevelGetMessage extends GenericMessage {
    /**
     * Constructs a new LevelGetMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public LevelGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode for the level get message.
     *
     * @return The opcode value for G_LEVEL_GET.
     */
    @Override
    public int getOpcode() {
        return Opcode.G_LEVEL_GET.value;
    }

    /**
     * Gets the parameters for the level get message.
     *
     * @return The parameters for the message, which are null in this case.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}
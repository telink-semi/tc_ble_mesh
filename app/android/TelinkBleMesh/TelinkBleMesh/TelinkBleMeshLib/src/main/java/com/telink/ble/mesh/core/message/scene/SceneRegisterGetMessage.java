/********************************************************************************************************
 * @file SceneRegisterGetMessage.java
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
package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * This class represents a Scene Register Get Message, which is a type of Lighting Message.
 * It is used to retrieve information about scene registration from a specific destination address and app key index.
 */
public class SceneRegisterGetMessage extends LightingMessage {

    /**
     * Creates a new SceneRegisterGetMessage with the given destination address and app key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     */
    public SceneRegisterGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns a new SceneRegisterGetMessage with the given destination address, app key index, and response maximum value.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     * @param rspMax The response maximum value of the message.
     * @return A new SceneRegisterGetMessage object.
     */
    public static SceneRegisterGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        SceneRegisterGetMessage message = new SceneRegisterGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Returns the opcode value of the Scene Register Get Message.
     *
     * @return The opcode value of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.SCENE_REG_GET.value;
    }

    /**
     * Returns the response opcode value of the Scene Register Get Message.
     *
     * @return The response opcode value of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.SCENE_REG_STATUS.value;
    }
}
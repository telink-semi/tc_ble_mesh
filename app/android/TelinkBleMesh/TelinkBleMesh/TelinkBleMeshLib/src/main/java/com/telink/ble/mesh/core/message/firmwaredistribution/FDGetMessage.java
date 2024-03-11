/********************************************************************************************************
 * @file FDGetMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

/**
 * This class represents a FDGetMessage, which is used to retrieve information about a specific destination address and app key index.
 * It extends the UpdatingMessage class.
 */
public class FDGetMessage extends UpdatingMessage {

    /**
     * Constructs a new FDGetMessage with the specified destination address and app key index.
     *
     * @param destinationAddress The destination address to retrieve information from.
     * @param appKeyIndex        The app key index associated with the destination address.
     */
    public FDGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDGetMessage with the specified destination address and app key index.
     * This method sets the response max to 1.
     *
     * @param destinationAddress The destination address to retrieve information from.
     * @param appKeyIndex        The app key index associated with the destination address.
     * @return The created FDGetMessage.
     */
    public static FDGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDGetMessage message = new FDGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Returns the opcode value for the FDGetMessage.
     *
     * @return The opcode value for the FDGetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_GET.value;
    }

    /**
     * Returns the response opcode value for the FDGetMessage.
     *
     * @return The response opcode value for the FDGetMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_STATUS.value;
    }
}
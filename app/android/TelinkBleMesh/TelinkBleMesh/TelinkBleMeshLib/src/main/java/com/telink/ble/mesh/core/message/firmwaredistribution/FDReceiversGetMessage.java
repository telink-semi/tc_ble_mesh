/********************************************************************************************************
 * @file FDReceiversGetMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a message used to request the Firmware Distribution Receivers List from a server.
 * It extends the UpdatingMessage class and includes the necessary parameters for the request.
 */
public class FDReceiversGetMessage extends UpdatingMessage {

    /**
     * first index
     * Index of the first requested entry from the Distribution Receivers List state
     * 2 bytes
     */
    public int firstIndex;

    /**
     * Entries Limit
     * Maximum number of entries that the server includes in a Firmware Distribution Receivers List message
     * 2 bytes
     */
    public int entriesLimit;

    /**
     * Constructs a new FDReceiversGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The index of the application key used for encryption.
     */
    public FDReceiversGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDReceiversGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The index of the application key used for encryption.
     * @return The created FDReceiversGetMessage object.
     */
    public static FDReceiversGetMessage getSimple(int destinationAddress, int appKeyIndex, int firstIndex, int entriesLimit) {
        FDReceiversGetMessage message = new FDReceiversGetMessage(destinationAddress, appKeyIndex);
        message.firstIndex = firstIndex;
        message.entriesLimit = entriesLimit;
        message.setResponseMax(1);
        return message;
    }

    /**
     * Gets the opcode value for the FDReceiversGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_RECEIVERS_GET.value;
    }

    /**
     * Gets the response opcode value for the FDReceiversGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_RECEIVERS_LIST.value;
    }

    /**
     * Gets the byte array representation of the parameters in the FDReceiversGetMessage.
     *
     * @return The byte array representation of the parameters.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) firstIndex).putShort((short) entriesLimit).array();
    }
}

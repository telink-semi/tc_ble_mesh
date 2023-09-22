/********************************************************************************************************
 * @file FDReceiversAddMessage.java
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
import java.util.List;

/**
 * This class represents a message for adding receivers in a Firmware Distribution (FD) operation.
 * It extends the UpdatingMessage class.
 * The message contains a list of ReceiverEntry objects, where each entry represents a receiver.
 * Each receiver entry consists of a 2-byte unicast address and a 1-byte firmware image index.
 * The message is used to add receivers for updating firmware in a specific destination address and application key index.
 * The opcode for this message is FD_RECEIVERS_ADD.
 * The response opcode for this message is FD_RECEIVERS_STATUS.
 * The getParams() method converts the list of receiver entries into a byte array by allocating 3 bytes for each entry.
 * The byte array is then returned as the parameters for the message.
 */

public class FDReceiversAddMessage extends UpdatingMessage {

    /**
     * Receiver Entry: every entry is 3 bytes
     */
    public List<ReceiverEntry> entries;

    /**
     * Constructs a new FDReceiversAddMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public FDReceiversAddMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDReceiversAddMessage with the specified destination address, application key index, and receiver entries.
     * The response maximum is set to 1.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     * @param entries            The list of receiver entries for the message.
     * @return The created FDReceiversAddMessage.
     */
    public static FDReceiversAddMessage getSimple(int destinationAddress, int appKeyIndex, List<ReceiverEntry> entries) {
        FDReceiversAddMessage message = new FDReceiversAddMessage(destinationAddress, appKeyIndex);
        message.entries = entries;
        message.setResponseMax(1);
        return message;
    }

    /**
     * Gets the opcode for the message.
     *
     * @return The opcode for the message (FD_RECEIVERS_ADD).
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_RECEIVERS_ADD.value;
    }

    /**
     * Gets the response opcode for the message.
     *
     * @return The response opcode for the message (FD_RECEIVERS_STATUS).
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_RECEIVERS_STATUS.value;
    }

    /**
     * Converts the list of receiver entries into a byte array.
     *
     * @return The byte array representing the receiver entries.
     */
    @Override
    public byte[] getParams() {
        if (entries == null || entries.size() == 0) {
            return null;
        }
        ByteBuffer buffer = ByteBuffer.allocate(3 * entries.size()).order(ByteOrder.LITTLE_ENDIAN);
        for (ReceiverEntry entry : entries) {
            buffer.putShort((short) entry.address).put((byte) entry.imageIndex);
        }
        return buffer.array();
    }

    /**
     * This class represents a receiver entry in the FDReceiversAddMessage.
     * Each entry contains a 2-byte unicast address and a 1-byte firmware image index.
     */
    static public class ReceiverEntry {
        /**
         * The unicast address of the Updating node
         * 2 bytes
         */
        public int address;

        /**
         * Update Firmware Image Index
         * The index of the firmware image in the Firmware Information List state to be updated
         * 1 byte
         */
        public int imageIndex;

        /**
         * Constructs a new ReceiverEntry with the specified unicast address and firmware image index.
         *
         * @param address    The unicast address of the updating node.
         * @param imageIndex The index of the firmware image in the Firmware Information List state to be updated.
         */
        public ReceiverEntry(int address, int imageIndex) {
            this.address = address;
            this.imageIndex = imageIndex;
        }
    }
}

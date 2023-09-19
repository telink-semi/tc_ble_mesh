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

public class FDReceiversAddMessage extends UpdatingMessage {

    /**
     * Receiver Entry: every entry is 3 bytes
     */
    public List<ReceiverEntry> entries;

    public FDReceiversAddMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    public static FDReceiversAddMessage getSimple(int destinationAddress, int appKeyIndex, List<ReceiverEntry> entries) {
        FDReceiversAddMessage message = new FDReceiversAddMessage(destinationAddress, appKeyIndex);
        message.entries = entries;
        message.setResponseMax(1);
        return message;
    }

    @Override
    public int getOpcode() {
        return Opcode.FD_RECEIVERS_ADD.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FD_RECEIVERS_STATUS.value;
    }

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

        public ReceiverEntry(int address, int imageIndex) {
            this.address = address;
            this.imageIndex = imageIndex;
        }
    }


}

/********************************************************************************************************
 * @file ProxyRemoveAddressMessage.java
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
package com.telink.ble.mesh.core.proxy;

import androidx.annotation.NonNull;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a proxy remove address message in a proxy configuration.
 * It extends the ProxyConfigurationMessage class.
 * It contains an array of integer addresses to be removed.
 */
public class ProxyRemoveAddressMessage extends ProxyConfigurationMessage {

    private int[] addressArray;

    /**
     * Constructs a ProxyRemoveAddressMessage object with the specified address array.
     *
     * @param addressArray the array of integer addresses to be removed
     */
    public ProxyRemoveAddressMessage(@NonNull int[] addressArray) {
        this.addressArray = addressArray;
    }

    /**
     * Returns the opcode for the remove address message.
     *
     * @return the opcode for the remove address message
     */
    @Override
    public byte getOpcode() {
        return OPCODE_REMOVE_ADDRESS;
    }

    /**
     * Converts the remove address message to a byte array.
     *
     * @return the byte array representation of the remove address message
     */
    @Override
    public byte[] toByteArray() {
        int len = 1 + addressArray.length * 2;
        ByteBuffer buffer = ByteBuffer.allocate(len).order(ByteOrder.BIG_ENDIAN).put(getOpcode());
        for (int address : addressArray) {
            buffer.putShort((short) address);
        }
        return buffer.array();
    }
}

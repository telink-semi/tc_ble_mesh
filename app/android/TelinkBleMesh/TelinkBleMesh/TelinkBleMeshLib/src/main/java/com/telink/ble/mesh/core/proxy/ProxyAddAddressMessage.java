/********************************************************************************************************
 * @file ProxyAddAddressMessage.java
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
 * Created by kee on 2019/8/26.
 */

/**
 * This class represents a message to add addresses to a proxy configuration.
 * It extends the ProxyConfigurationMessage class.
 */
public class ProxyAddAddressMessage extends ProxyConfigurationMessage {

    private int[] addressArray; // Array of addresses to be added

    /**
     * Constructs a new ProxyAddAddressMessage with the given address array.
     *
     * @param addressArray the array of addresses to be added
     */
    public ProxyAddAddressMessage(@NonNull int[] addressArray) {
        this.addressArray = addressArray;
    }

    /**
     * Returns the opcode for this message.
     *
     * @return the opcode for adding addresses
     */
    @Override
    public byte getOpcode() {
        return OPCODE_ADD_ADDRESS;
    }

    /**
     * Converts the message to a byte array for transmission.
     *
     * @return the byte array representation of the message
     */
    @Override
    public byte[] toByteArray() {
        int len = 1 + addressArray.length * 2; // Calculate the length of the byte array
        ByteBuffer buffer = ByteBuffer.allocate(len).order(ByteOrder.BIG_ENDIAN).put(getOpcode()); // Create a byte buffer with the calculated length
        for (int address : addressArray) { // Iterate over the address array
            buffer.putShort((short) address); // Convert each address to a short and add it to the buffer
        }
        return buffer.array(); // Return the byte array representation of the buffer
    }
}
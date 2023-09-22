/********************************************************************************************************
 * @file CompositionDataGetMessage.java
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
package com.telink.ble.mesh.core.message.largecps;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * The Config Composition Data Get is an acknowledged message used to read one page of the Composition Data
 * The response to a Config Composition Data Get message is a Config Composition Data Status message {@link CompositionDataStatusMessage}.
 * The LargeCompositionDataGetMessage class represents an acknowledged message used to read one page of the Composition Data.
 * This message is used to retrieve a large amount of composition data in chunks.
 * The response to a LargeCompositionDataGetMessage is a CompositionDataStatusMessage.
 */
public class LargeCompositionDataGetMessage extends ConfigMessage {
    private static final byte PAGE_ALL = (byte) 0xFF;
    private int offset = 0x00;

    /**
     * Constructs a LargeCompositionDataGetMessage with the specified destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public LargeCompositionDataGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Gets the offset value for retrieving the composition data.
     *
     * @return The offset value.
     */
    public int getOffset() {
        return offset;
    }

    /**
     * Sets the offset value for retrieving the composition data.
     *
     * @param offset The offset value to set.
     */
    public void setOffset(int offset) {
        this.offset = offset;
    }

    /**
     * Gets the opcode of the LargeCompositionDataGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.LARGE_CPS_GET.value;
    }

    /**
     * Gets the response opcode for the LargeCompositionDataGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.COMPOSITION_DATA_STATUS.value;
    }

    /**
     * Gets the parameters for the LargeCompositionDataGetMessage.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN).put(PAGE_ALL).putShort((short) offset).array();
    }
}
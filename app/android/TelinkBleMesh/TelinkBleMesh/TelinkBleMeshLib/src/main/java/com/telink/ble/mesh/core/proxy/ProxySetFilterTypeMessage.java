/********************************************************************************************************
 * @file ProxySetFilterTypeMessage.java
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

/**
 * This class represents a message for setting the filter type in a proxy configuration.
 * It extends the ProxyConfigurationMessage class.
 * The filter type is specified by a byte value.
 */
public class ProxySetFilterTypeMessage extends ProxyConfigurationMessage {

    private byte filterType;

    /**
     * Constructs a ProxySetFilterTypeMessage object with the specified filter type.
     *
     * @param filterType the byte value representing the filter type
     */
    public ProxySetFilterTypeMessage(byte filterType) {
        this.filterType = filterType;
    }

    /**
     * Returns the opcode for this message, which is OPCODE_SET_FILTER_TYPE.
     *
     * @return the opcode for this message
     */
    @Override
    public byte getOpcode() {
        return OPCODE_SET_FILTER_TYPE;
    }

    /**
     * Converts this message to a byte array representation.
     * The byte array consists of the opcode followed by the filter type.
     *
     * @return the byte array representation of this message
     */
    @Override
    public byte[] toByteArray() {
        return new byte[]{getOpcode(), filterType};
    }
}
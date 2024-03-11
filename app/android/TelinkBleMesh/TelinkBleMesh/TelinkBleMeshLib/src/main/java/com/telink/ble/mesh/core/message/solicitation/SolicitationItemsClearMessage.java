/********************************************************************************************************
 * @file SolicitationItemsClearMessage.java
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
package com.telink.ble.mesh.core.message.solicitation;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;


/**
 * SolicitationItemsClearMessage
 */
public class SolicitationItemsClearMessage extends ConfigMessage {
    public byte[] addressRange; // The address range for which the SAR Receiver state is requested 
    public boolean ack; // Flag indicating whether the message is an acknowledgement or not 

    /**
     * Constructs a new SolicitationItemsClearMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message
     */
    public SolicitationItemsClearMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode of the message based on the acknowledgement flag.
     *
     * @return The opcode of the message
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.SOLI_PDU_RPL_ITEM_CLEAR.value : Opcode.SOLI_PDU_RPL_ITEM_CLEAR_NACK.value;
    }

    /**
     * Returns the response opcode of the message based on the acknowledgement flag.
     *
     * @return The response opcode of the message
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SOLI_PDU_RPL_ITEM_STATUS.value : OPCODE_INVALID;
    }

    /**
     * Returns the parameters of the message, which is the address range.
     *
     * @return The parameters of the message
     */
    @Override
    public byte[] getParams() {
        return addressRange;
    }
}

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
package com.telink.ble.mesh.core.message.solicitation;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;


/**
 * A SAR_RECEIVER_GET message is an acknowledged message used to get the current SAR Receiver state of a node
 */
public class SolicitationItemsClearMessage extends ConfigMessage {

    /**
     * New On-Demand Private GATT Proxy state
     */
    public byte[] addressRange;

    public boolean ack;

    public SolicitationItemsClearMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.SOLI_PDU_RPL_ITEM_CLEAR.value : Opcode.SOLI_PDU_RPL_ITEM_CLEAR_NACK.value;
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SOLI_PDU_RPL_ITEM_STATUS.value : OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        return addressRange;
    }


}

/********************************************************************************************************
 * @file ProvisioningPduSendMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;

public class ProvisioningPduSendMessage extends RemoteProvisionMessage {

    private byte outboundPDUNumber;

    /**
     * 16 bytes
     */
    private byte[] provisioningPDU;

    /**
     * @param destinationAddress server address
     */
    public static ProvisioningPduSendMessage getSimple(int destinationAddress, int rspMax,
                                                       byte outboundPDUNumber,
                                                       byte[] provisioningPDU) {
        ProvisioningPduSendMessage message = new ProvisioningPduSendMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.outboundPDUNumber = outboundPDUNumber;
        message.provisioningPDU = provisioningPDU;
        return message;
    }

    public ProvisioningPduSendMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_PDU_SEND.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
//        return Opcode.REMOTE_PROV_PDU_OUTBOUND_REPORT.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(1 + provisioningPDU.length)
                .put(outboundPDUNumber)
                .put(provisioningPDU).array();
    }

    public void setOutboundPDUNumber(byte outboundPDUNumber) {
        this.outboundPDUNumber = outboundPDUNumber;
    }

    public void setProvisioningPDU(byte[] provisioningPDU) {
        this.provisioningPDU = provisioningPDU;
    }
}

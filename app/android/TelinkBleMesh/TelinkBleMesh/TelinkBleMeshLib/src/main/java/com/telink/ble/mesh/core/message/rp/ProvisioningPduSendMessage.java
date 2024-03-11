/********************************************************************************************************
 * @file ProvisioningPduSendMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;

/**
 * This class represents a message for sending a provisioning PDU (Protocol Data Unit) to a server.
 * It extends the RemoteProvisionMessage class.
 */
public class ProvisioningPduSendMessage extends RemoteProvisionMessage {
    private byte outboundPDUNumber;
    /**
     * 16 bytes
     */
    private byte[] provisioningPDU;

    /**
     * Creates and returns a ProvisioningPduSendMessage object with the specified parameters.
     *
     * @param destinationAddress The server address.
     * @param rspMax             The maximum response value.
     * @param outboundPDUNumber  The outbound PDU number.
     * @param provisioningPDU    The provisioning PDU.
     * @return The created ProvisioningPduSendMessage object.
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

    /**
     * Constructs a ProvisioningPduSendMessage object with the specified destination address.
     *
     * @param destinationAddress The server address.
     */
    public ProvisioningPduSendMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for this message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_PDU_SEND.value;
    }

    /**
     * Returns the response opcode value for this message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
//        return Opcode.REMOTE_PROV_PDU_OUTBOUND_REPORT.value;
    }

    /**
     * Returns the parameters of this message as a byte array.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(1 + provisioningPDU.length)
                .put(outboundPDUNumber)
                .put(provisioningPDU).array();
    }

    /**
     * Sets the outbound PDU number for this message.
     *
     * @param outboundPDUNumber The outbound PDU number.
     */
    public void setOutboundPDUNumber(byte outboundPDUNumber) {
        this.outboundPDUNumber = outboundPDUNumber;
    }

    /**
     * Sets the provisioning PDU for this message.
     *
     * @param provisioningPDU The provisioning PDU.
     */
    public void setProvisioningPDU(byte[] provisioningPDU) {
        this.provisioningPDU = provisioningPDU;
    }
}
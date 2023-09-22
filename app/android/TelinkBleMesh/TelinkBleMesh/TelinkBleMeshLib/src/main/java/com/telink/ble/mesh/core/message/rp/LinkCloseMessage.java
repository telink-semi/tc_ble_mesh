/********************************************************************************************************
 * @file LinkCloseMessage.java
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

/**
 * This class represents a message used to close a link in a remote provisioning process.
 * It extends the RemoteProvisionMessage class.
 */
public class LinkCloseMessage extends RemoteProvisionMessage {

    /**
     * The reason for closing the link.
     */
    private byte reason;

    /**
     * The reason indicating a successful link closure.
     */
    public static final byte REASON_SUCCESS = 0x00;

    /**
     * The reason indicating a prohibited link closure.
     */
    public static final byte REASON_PROHIBITED = 0x01;

    /**
     * The reason indicating a failed link closure.
     */
    public static final byte REASON_FAIL = 0x02;

    /**
     * Creates a new instance of LinkCloseMessage with the given destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public LinkCloseMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode for the LinkCloseMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_LINK_CLOSE.value;
    }

    /**
     * Returns the opcode for the response to the LinkCloseMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_LINK_STS.value;
    }

    /**
     * Returns the parameters of the LinkCloseMessage.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{reason};
    }

    /**
     * Sets the reason for closing the link.
     *
     * @param reason The reason value.
     */
    public void setReason(byte reason) {
        this.reason = reason;
    }

    /**
     * Creates a simple LinkCloseMessage with the given destination address, maximum response value, and reason.
     *
     * @param destinationAddress The destination address of the message.
     * @param rspMax             The maximum response value.
     * @param reason             The reason for closing the link.
     * @return The created LinkCloseMessage.
     */
    public static LinkCloseMessage getSimple(int destinationAddress, int rspMax, byte reason) {
        LinkCloseMessage message = new LinkCloseMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.reason = reason;
        return message;
    }
}
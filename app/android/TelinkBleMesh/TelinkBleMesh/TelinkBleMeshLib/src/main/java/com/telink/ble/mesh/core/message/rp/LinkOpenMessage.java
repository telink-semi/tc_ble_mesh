/********************************************************************************************************
 * @file LinkOpenMessage.java
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
 * This class represents a message used to open a link for remote provisioning.
 * It extends the RemoteProvisionMessage class.
 */
public class LinkOpenMessage extends RemoteProvisionMessage {
    /**
     * The UUID (Universally Unique Identifier) associated with the link.
     * It is a 16-byte array.
     */
    private byte[] uuid;

    /**
     * Creates a new instance of LinkOpenMessage with the given destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public LinkOpenMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the LinkOpenMessage, which is defined as Opcode.REMOTE_PROV_LINK_OPEN.
     *
     * @return The opcode value for the LinkOpenMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_LINK_OPEN.value;
    }

    /**
     * Returns the response opcode value for the LinkOpenMessage, which is defined as Opcode.REMOTE_PROV_LINK_STS.
     *
     * @return The response opcode value for the LinkOpenMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_LINK_STS.value;
    }

    /**
     * Returns the parameters of the LinkOpenMessage, which is the UUID associated with the link.
     *
     * @return The parameters of the LinkOpenMessage.
     */
    @Override
    public byte[] getParams() {
        return uuid;
    }

    /**
     * Sets the UUID for the LinkOpenMessage.
     *
     * @param uuid The UUID to be set for the LinkOpenMessage.
     */
    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }

    /**
     * Returns a simple LinkOpenMessage with the given destination address, maximum response value, and UUID.
     *
     * @param destinationAddress The destination address for the message.
     * @param rspMax             The maximum response value for the message.
     * @param uuid               The UUID associated with the link.
     * @return A simple LinkOpenMessage.
     */
    public static LinkOpenMessage getSimple(int destinationAddress, int rspMax, byte[] uuid) {
        LinkOpenMessage message = new LinkOpenMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.uuid = uuid;
        return message;
    }
}
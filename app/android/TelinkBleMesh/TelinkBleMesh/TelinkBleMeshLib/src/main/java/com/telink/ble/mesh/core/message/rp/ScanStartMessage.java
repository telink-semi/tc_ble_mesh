/********************************************************************************************************
 * @file ScanStartMessage.java
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
import java.nio.ByteOrder;

/**
 * This class represents a message to start scanning for remote provisioning.
 * It extends the RemoteProvisionMessage class.
 */
public class ScanStartMessage extends RemoteProvisionMessage {
    /**
     * The limit of scanned items (1 byte).
     */
    private byte scannedItemsLimit;
    /**
     * The timeout for scanning (1 byte).
     */
    private byte timeout;
    /**
     * The UUID of the device (optional).
     */
    private byte[] uuid;

    /**
     * Creates a new ScanStartMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public ScanStartMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_SCAN_START.value;
    }

    /**
     * Returns the opcode of the response message.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return super.getResponseOpcode();
    }

    /**
     * Returns the parameters of the message as a byte array.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        int len = uuid == null ? 2 : 18;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .put(scannedItemsLimit).put(timeout);
        if (uuid != null) {
            bf.put(uuid);
        }
        return bf.array();
    }

    /**
     * Sets the limit of scanned items.
     *
     * @param scannedItemsLimit The limit of scanned items.
     */
    public void setScannedItemsLimit(byte scannedItemsLimit) {
        this.scannedItemsLimit = scannedItemsLimit;
    }

    /**
     * Sets the timeout for scanning.
     *
     * @param timeout The timeout for scanning.
     */
    public void setTimeout(byte timeout) {
        this.timeout = timeout;
    }

    /**
     * Sets the UUID of the device.
     *
     * @param uuid The UUID of the device.
     */
    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }
}
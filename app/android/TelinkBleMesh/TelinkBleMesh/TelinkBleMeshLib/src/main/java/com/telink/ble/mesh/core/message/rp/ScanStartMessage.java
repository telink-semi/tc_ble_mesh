/********************************************************************************************************
 * @file ScanStartMessage.java
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
import java.nio.ByteOrder;

public class ScanStartMessage extends RemoteProvisionMessage {

    /**
     * 1 bytes
     */
    private byte scannedItemsLimit;

    /**
     * 1 bytes
     */
    private byte timeout;

    /**
     * Device UUID (Optional)
     */
    private byte[] uuid;

    public static ScanStartMessage getSimple(int destinationAddress, int rspMax, byte scannedItemsLimit, byte timeout) {
        ScanStartMessage message = new ScanStartMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.scannedItemsLimit = scannedItemsLimit;
        message.timeout = timeout;
        return message;
    }

    public ScanStartMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_SCAN_START.value;
    }

    @Override
    public int getResponseOpcode() {
//        return Opcode.REMOTE_PROV_SCAN_STS.value;
        return super.getResponseOpcode();
    }

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

    public void setScannedItemsLimit(byte scannedItemsLimit) {
        this.scannedItemsLimit = scannedItemsLimit;
    }

    public void setTimeout(byte timeout) {
        this.timeout = timeout;
    }

    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }
}

/********************************************************************************************************
 * @file ProvisioningRecordRequestPDU.java
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
package com.telink.ble.mesh.core.provisioning.pdu;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Provisioner sends a Provisioning Record Request PDU to request a provisioning record fragment
 */
public class ProvisioningRecordRequestPDU implements ProvisioningStatePDU {

    /**
     * Identifies the provisioning record for which the request is made
     * 2 bytes
     */
    public int recordID;

    /**
     * The starting offset of the requested fragment in the provisioning record data
     * 2 bytes
     */
    public int fragmentOffset;

    /**
     * The maximum size of the provisioning record fragment that the Provisioner can receive
     * 2 bytes
     */
    public int fragmentMaxSize;


    public ProvisioningRecordRequestPDU(int recordID, int fragmentOffset, int fragmentMaxSize) {
        this.recordID = recordID;
        this.fragmentOffset = fragmentOffset;
        this.fragmentMaxSize = fragmentMaxSize;
    }

    @Override
    public byte[] toBytes() {
        return ByteBuffer.allocate(6).order(ByteOrder.BIG_ENDIAN).putShort((short) recordID)
                .putShort((short) fragmentOffset)
                .putShort((short) fragmentMaxSize).array();
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RECORD_REQUEST;
    }
}

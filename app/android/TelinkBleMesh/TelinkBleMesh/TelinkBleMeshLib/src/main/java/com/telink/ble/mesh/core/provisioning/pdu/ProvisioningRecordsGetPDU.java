/********************************************************************************************************
 * @file ProvisioningRecordsGetPDU.java
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

/**
 * This class represents a Provisioning Records Get PDU.
 * It implements the ProvisioningStatePDU interface.
 */
public class ProvisioningRecordsGetPDU implements ProvisioningStatePDU {

    /**
     * Constructs a ProvisioningRecordsGetPDU object.
     */
    public ProvisioningRecordsGetPDU() {
    }

    /**
     * Converts the ProvisioningRecordsGetPDU object to a byte array.
     *
     * @return the byte array representation of the PDU
     */
    @Override
    public byte[] toBytes() {
        return null;
    }

    /**
     * Retrieves the state of the ProvisioningRecordsGetPDU object.
     *
     * @return the state of the PDU
     */
    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RECORDS_GET;
    }
}
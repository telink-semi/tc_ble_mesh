/********************************************************************************************************
 * @file ProvisioningConfirmPDU.java
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
 * The ProvisioningConfirmPDU class represents a Provisioning State PDU containing confirm data.
 * It implements the ProvisioningStatePDU interface.
 */
public class ProvisioningConfirmPDU implements ProvisioningStatePDU {

    /**
     * confirm data
     */
    public byte[] confirm;

    /**
     * constructor
     *
     * @param confirm confirm
     */
    public ProvisioningConfirmPDU(byte[] confirm) {
        this.confirm = confirm;
    }

    /**
     * Converts the PDU to a byte array.
     *
     * @return The byte array representation of the PDU.
     */
    @Override
    public byte[] toBytes() {
        return confirm;
    }

    /**
     * Gets the state of the PDU.
     *
     * @return The state of the PDU.
     */
    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_CONFIRMATION;
    }
}

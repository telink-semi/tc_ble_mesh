/********************************************************************************************************
 * @file ProvisioningRandomPDU.java
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
 * Created by kee on 2019/7/19.
 */

/**
 * represents a Provisioning Random PDU
 */
public class ProvisioningRandomPDU implements ProvisioningStatePDU {
    /**
     * random data
     */
    public byte[] random;

    /**
     * The constructor of the class takes a byte array parameter called "random" and assigns it to the "random" variable.
     *
     * @param random random
     */
    public ProvisioningRandomPDU(byte[] random) {
        this.random = random;
    }

    /**
     * @return random
     */
    @Override
    public byte[] toBytes() {
        return random;
    }

    /**
     * @return provision state
     */
    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RANDOM;
    }
}

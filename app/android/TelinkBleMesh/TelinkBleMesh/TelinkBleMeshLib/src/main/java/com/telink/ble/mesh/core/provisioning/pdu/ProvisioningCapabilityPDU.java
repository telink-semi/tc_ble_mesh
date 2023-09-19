/********************************************************************************************************
 * @file ProvisioningCapabilityPDU.java
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
 * Created by kee on 2019/7/18.
 */
// 03:01:
// 02:00:01:00:00:00:00:00:00:00:00
// from device
public class ProvisioningCapabilityPDU implements ProvisioningStatePDU {

    private static final int LEN = 11;

    /**
     * Algorithm - BTM_ECDH_P256_CMAC_AES128_AES_CCM
     */
    private static final int ALG_BIT_MASK_CMAC = 0b01;

    /**
     * Algorithm - BTM_ECDH_P256_HMAC_SHA256_AES_CCM
     */
    private static final int ALG_BIT_MASK_HMAC = 0b10;

    public byte[] rawData;


    /**
     * Number of elements supported by the device
     * 1 byte
     * 0: Prohibited
     * 0x01-0xFF
     */
    public byte eleNum;

    /**
     * Supported algorithms and other capabilities
     * 2 bytes
     * bit-0: FIPS P-256 Elliptic Curve
     * bit-1--15: Reserved for Future Use
     * <p>
     * <p>
     * update EPA
     * 0	BTM_ECDH_P256_CMAC_AES128_AES_CCM
     * 1	BTM_ECDH_P256_HMAC_SHA256_AES_CCM
     */
    public short algorithms;

    /**
     * Supported public key types
     * 1 byte
     * bit-0: Public Key OOB information available
     * bit-1--7: Prohibited
     */
    public byte publicKeyType;

    /**
     * Supported static OOB Types
     * 1 byte
     * bit 0: Static OOB information available
     * bit 1–7: Prohibited
     */
    public byte staticOOBType;

    /**
     * Maximum size of Output OOB supported
     * 1 byte
     * 0x00: The device does not support output OOB
     * 0x01–0x08: Maximum size in octets supported by the device
     * 0x09–0xFF: Reserved for Future Use
     */
    public byte outputOOBSize;

    /**
     * Supported Output OOB Actions
     * 2 bytes
     * bit-0: Blink
     * bit-1: Beep
     * bit-2: Vibrate
     * bit-3: Output Numeric
     * bit-4: Output Alphanumeric, Array of octets
     * other bits are RFU
     */
    public short outputOOBAction;

    /**
     * Maximum size in octets of Input OOB supported
     * 1 byte
     * 0x00: The device does not support Input OOB
     * 0x01–0x08: Maximum supported size in octets supported by the device
     * 0x09–0xFF: Reserved for Future Use
     */
    public byte inputOOBSize;

    /**
     * Supported Input OOB Actions
     * 2 bytes
     * bit-0: Push
     * bit-1: Twist
     * bit-2:Input Number
     * bit-3: Input Alphanumeric, Array of octets
     * bit-4--15 Reserved for Future Use
     */
    public short inputOOBAction;

    public static ProvisioningCapabilityPDU fromBytes(byte[] data) {
        if (data == null || data.length < LEN) {
            return null;
        }

        ProvisioningCapabilityPDU capability = new ProvisioningCapabilityPDU();
        capability.rawData = data;
        int index = 0;
        capability.eleNum = data[index++];
        capability.algorithms = (short) (((data[index++] & 0xFF) << 8) | (data[index++] & 0xFF));
        capability.publicKeyType = data[index++];
        capability.staticOOBType = data[index++];
        capability.outputOOBSize = data[index++];
        capability.outputOOBAction = (short) (((data[index++] & 0xFF) << 8) | (data[index++] & 0xFF));
        capability.inputOOBSize = data[index++];
        capability.inputOOBAction = (short) (((data[index++] & 0xFF) << 8) | (data[index] & 0xFF));
        return capability;
    }

    @Override
    public String toString() {
        return "ProvisioningCapabilityPDU{" +
                "eleNum=" + eleNum +
                ", algorithms=" + algorithms +
                ", publicKeyType=" + publicKeyType +
                ", staticOOBType=" + staticOOBType +
                ", outputOOBSize=" + outputOOBSize +
                ", outputOOBAction=" + outputOOBAction +
                ", inputOOBSize=" + inputOOBSize +
                ", inputOOBAction=" + inputOOBAction +
                '}';
    }

    @Override
    public byte[] toBytes() {
        return rawData;
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_CAPABILITIES;
    }

    public boolean isStaticOOBSupported() {
        return staticOOBType != 0;
    }

    public boolean isHMacAlgorithmSupported() {
        return (algorithms & ALG_BIT_MASK_HMAC) != 0;
    }
}

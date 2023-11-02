/********************************************************************************************************
 * @file ProvisioningParams.java
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
package com.telink.ble.mesh.core.provisioning;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * @see com.telink.ble.mesh.entity.ProvisioningDevice
 * @deprecated This class represents the provisioning parameters required for setting up a device on a network.
 * The parameters include the network key, network key index, key refresh flag, IV update flag,
 * IV index, and unicast address.
 * The provisioning parameters can be obtained using the getDefault() or getSimple() methods.
 * The toProvisioningData() method converts the parameters into a byte array.
 * The network key, network key index, key refresh flag, IV update flag, IV index, and unicast address
 * can be accessed and modified using the getter and setter methods.
 */

public class ProvisioningParams {

    /**
     * 16: key
     * 2: key index
     * 1: flags
     * 4: iv index
     * 2: unicast adr
     */
    private static final int DATA_PDU_LEN = 16 + 2 + 1 + 4 + 2;

    private byte[] networkKey;

    private int networkKeyIndex;

    /*
     * 0
     Key Refresh Flag
     0: Key Refresh Phase 0 1: Key Refresh Phase 2
     1
     IV Update Flag
     0: Normal operation 1: IV Update active
     2–7
     Reserved for Future Use
     */

    /**
     * 1 bit
     */
    private byte keyRefreshFlag;

    /**
     * 1 bit
     */
    private byte ivUpdateFlag;

    /**
     * 4 bytes
     */
    private int ivIndex;

    /**
     * unicast address for primary element
     * 2 bytes
     */
    private int unicastAddress;

    /**
     * Returns a default instance of ProvisioningParams with the given network key,
     * network key index, IV index, and unicast address.
     *
     * @param networkKey      The network key.
     * @param networkKeyIndex The network key index.
     * @param ivIndex         The IV index.
     * @param unicastAddress  The unicast address.
     * @return A default instance of ProvisioningParams.
     */
    public static ProvisioningParams getDefault(byte[] networkKey, int networkKeyIndex, int ivIndex, int unicastAddress) {
        ProvisioningParams params = new ProvisioningParams();
        params.networkKey = networkKey;
        params.networkKeyIndex = networkKeyIndex;
        params.keyRefreshFlag = 0;
        params.ivUpdateFlag = 0;
        params.ivIndex = ivIndex;
        params.unicastAddress = unicastAddress;
        return params;
    }

    /**
     * Returns a simple instance of ProvisioningParams with the given network key and unicast address.
     * The other parameters are set to their default values.
     *
     * @param networkKey     The network key.
     * @param unicastAddress The unicast address.
     * @return A simple instance of ProvisioningParams.
     */
    public static ProvisioningParams getSimple(byte[] networkKey, int unicastAddress) {
        ProvisioningParams params = new ProvisioningParams();
        params.networkKey = networkKey;
        params.networkKeyIndex = 0;
        params.keyRefreshFlag = 0;
        params.ivUpdateFlag = 0;
        params.ivIndex = 0;
        params.unicastAddress = unicastAddress;
        return params;
    }

    /**
     * Converts the ProvisioningParams object to a byte array representation of the provisioning data.
     *
     * @return The byte array representation of the provisioning data.
     */
    public byte[] toProvisioningData() {
        byte flags = (byte) ((keyRefreshFlag & 0b01) | (ivUpdateFlag & 0b10));
        ByteBuffer buffer = ByteBuffer.allocate(DATA_PDU_LEN).order(ByteOrder.BIG_ENDIAN);
        buffer.put(networkKey)
                .putShort((short) networkKeyIndex)
                .put(flags)
                .putInt(ivIndex)
                .putShort((short) unicastAddress);
        return buffer.array();
    }
    /**
     * Gets the network key.
     *
     * @return the network key
     */
    public byte[] getNetworkKey() {
        return networkKey;
    }

    /**
     * Sets the network key.
     *
     * @param networkKey the network key
     */
    public void setNetworkKey(byte[] networkKey) {
        this.networkKey = networkKey;
    }

    /**
     * Gets the network key index.
     *
     * @return the network key index
     */
    public int getNetworkKeyIndex() {
        return networkKeyIndex;
    }

    /**
     * Sets the network key index.
     *
     * @param networkKeyIndex the network key index
     */
    public void setNetworkKeyIndex(int networkKeyIndex) {
        this.networkKeyIndex = networkKeyIndex;
    }

    /**
     * Gets the key refresh flag.
     *
     * @return the key refresh flag
     */
    public byte getKeyRefreshFlag() {
        return keyRefreshFlag;
    }

    /**
     * Sets the key refresh flag.
     *
     * @param keyRefreshFlag the key refresh flag
     */
    public void setKeyRefreshFlag(byte keyRefreshFlag) {
        this.keyRefreshFlag = keyRefreshFlag;
    }

    /**
     * Gets the IV update flag.
     *
     * @return the IV update flag
     */
    public byte getIvUpdateFlag() {
        return ivUpdateFlag;
    }

    /**
     * Sets the IV update flag.
     *
     * @param ivUpdateFlag the IV update flag
     */
    public void setIvUpdateFlag(byte ivUpdateFlag) {
        this.ivUpdateFlag = ivUpdateFlag;
    }

    /**
     * Gets the IV index.
     *
     * @return the IV index
     */
    public int getIvIndex() {
        return ivIndex;
    }

    /**
     * Sets the IV index.
     *
     * @param ivIndex the IV index
     */
    public void setIvIndex(int ivIndex) {
        this.ivIndex = ivIndex;
    }

    /**
     * Gets the unicast address.
     *
     * @return the unicast address
     */
    public int getUnicastAddress() {
        return unicastAddress;
    }

    /**
     * Sets the unicast address.
     *
     * @param unicastAddress the unicast address
     */
    public void setUnicastAddress(int unicastAddress) {
        this.unicastAddress = unicastAddress;
    }
}

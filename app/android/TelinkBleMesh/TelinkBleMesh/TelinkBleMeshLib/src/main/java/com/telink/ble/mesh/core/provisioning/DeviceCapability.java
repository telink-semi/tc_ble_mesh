/********************************************************************************************************
 * @file DeviceCapability.java
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


/**
 * The DeviceCapability class represents the capability of a device.
 * It provides methods to retrieve various capabilities of the device.
 */
public class DeviceCapability {
    // Length of the raw data representing the device capability
    private static final int CPB_DATA_LEN = 11;

    // Raw data representing the device capability
    private byte[] rawData;

    /**
     * Private constructor to prevent direct instantiation of the DeviceCapability class.
     */
    private DeviceCapability() {
    }

    /**
     * Retrieves the DeviceCapability object for the given data.
     *
     * @param data The raw data representing the device capability
     * @return The DeviceCapability object if the data is valid, null otherwise
     */
    public static DeviceCapability getCapability(byte[] data) {
        if (data == null || data.length != CPB_DATA_LEN) {
            return null;
        }
        DeviceCapability capability = new DeviceCapability();
        capability.rawData = data;
        return capability;
    }

    /**
     * Retrieves the number of elements in the device capability.
     *
     * @return The number of elements
     */
    public int getElementCnt() {
        return rawData[0];
    }

    /**
     * Retrieves the supported algorithms of the device capability.
     *
     * @return The supported algorithms
     */
    public int getAlgorithms() {
        return ((rawData[1] & 0xFF) << 8) | (rawData[2] & 0xFF);
    }

    /**
     * Retrieves the public key type supported by the device capability.
     *
     * @return The public key type
     */
    public int getPublicKeyType() {
        return rawData[3];
    }

    /**
     * Retrieves the static Out-of-Band (OOB) type supported by the device capability.
     *
     * @return The static OOB type
     */
    public int getStaticOOBType() {
        return rawData[4];
    }

    /**
     * Retrieves the size of the output OOB supported by the device capability.
     *
     * @return The size of the output OOB
     */
    public int getOutputOOBSize() {
        return rawData[5];
    }

    /**
     * Retrieves the action associated with the output OOB supported by the device capability.
     *
     * @return The action of the output OOB
     */
    public int getOutputOOBAction() {
        return ((rawData[6] & 0xFF) << 8) | (rawData[7] & 0xFF);
    }

    /**
     * Retrieves the size of the input OOB supported by the device capability.
     *
     * @return The size of the input OOB
     */
    public int getInputOOBSize() {
        return rawData[8];
    }

    /**
     * Retrieves the action associated with the input OOB supported by the device capability.
     *
     * @return The action of the input OOB
     */
    public int getInputOOBAction() {
        return ((rawData[9] & 0xFF) << 8) | (rawData[10] & 0xFF);
    }
}
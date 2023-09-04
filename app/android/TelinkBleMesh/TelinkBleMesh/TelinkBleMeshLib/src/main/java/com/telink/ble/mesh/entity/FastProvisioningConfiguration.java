/********************************************************************************************************
 * @file FastProvisioningConfiguration.java
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
package com.telink.ble.mesh.entity;

import android.util.SparseIntArray;

import androidx.annotation.NonNull;

/**
 * fast provisioning configurations
 * Created by kee on 2020/03/09.
 */

public class FastProvisioningConfiguration {

    /**
     * 2000ms
     */
    public static final int DEFAULT_RESET_DELAY = 2000;

    public static final int PID_ALL = 0xFFFF;

    /**
     * device default ivIndex
     */
    public static final int DEFAULT_IV_INDEX = 0x12345678;

    /**
     * device default network key
     */
    public static final byte[] DEFAULT_NETWORK_KEY = {0x7d, (byte) 0xd7, 0x36, 0x4c, (byte) 0xd8,
            0x42, (byte) 0xad, 0x18, (byte) 0xc1, 0x7c, 0x74, 0x65, 0x6c, 0x69, 0x6e, 0x6b};

    /**
     * device default app key
     */
    public static final byte[] DEFAULT_APP_KEY = {0x63, (byte) 0x96, 0x47, 0x71, 0x73, 0x4f,
            (byte) 0xbd, 0x76, (byte) 0xe3, (byte) 0xb4, 0x74, 0x65, 0x6c, 0x69, 0x6e, 0x6b};

    public static final int DEFAULT_NETWORK_KEY_INDEX = 0;

    public static final int DEFAULT_APP_KEY_INDEX = 0;

    private int resetDelay;

    /**
     * provisioning start address
     */
    private int provisioningIndex;

    // element count for each pid
    // key: pid
    private SparseIntArray elementPidMap;
//    private int elementCount;
//
//    private int pid;
    /**
     * pid used in params when scanning for fast-provisioning device
     */
    private int scanningPid;

    /*****
     * device default network params
     *****/
    private int ivIndex;

    @NonNull
    private byte[] defaultNetworkKey;

    private int defaultNetworkKeyIndex = 0;

    @NonNull
    private byte[] defaultAppKey;

    private int defaultAppKeyIndex = 0;

    public static FastProvisioningConfiguration getDefault(int provisioningIndex, SparseIntArray elementPidMap) {
        FastProvisioningConfiguration configuration = new FastProvisioningConfiguration();
        configuration.resetDelay = DEFAULT_RESET_DELAY;
        configuration.provisioningIndex = provisioningIndex;
        configuration.elementPidMap = elementPidMap;
        configuration.scanningPid = PID_ALL;
        configuration.ivIndex = DEFAULT_IV_INDEX;
        configuration.defaultNetworkKey = DEFAULT_NETWORK_KEY;
        configuration.defaultNetworkKeyIndex = DEFAULT_NETWORK_KEY_INDEX;
        configuration.defaultAppKey = DEFAULT_APP_KEY;
        configuration.defaultAppKeyIndex = DEFAULT_APP_KEY_INDEX;
        return configuration;
    }

    /**
     * @return if pid exist in map
     */
    public boolean pidExist(int pid) {
        if (elementPidMap != null) {
            return elementPidMap.get(pid) != 0;
        }
        return false;
    }

    /**
     * @param pid target pid
     * @return 0 error
     */
    public int getElementCount(int pid) {
        if (elementPidMap != null) {
            return elementPidMap.get(pid);
        }
        return 0;
    }

    public int getProvisioningIndex() {
        return provisioningIndex;
    }

    public void setProvisioningIndex(int provisioningIndex) {
        this.provisioningIndex = provisioningIndex;
    }

    public void increaseProvisioningIndex(int elementCount) {
        this.provisioningIndex += elementCount;
    }

    public SparseIntArray getElementPidMap() {
        return elementPidMap;
    }

    public void setElementPidMap(SparseIntArray elementPidMap) {
        this.elementPidMap = elementPidMap;
    }

    public int getScanningPid() {
        return scanningPid;
    }

    public void setScanningPid(int scanningPid) {
        this.scanningPid = scanningPid;
    }

    public int getIvIndex() {
        return ivIndex;
    }

    public void setIvIndex(int ivIndex) {
        this.ivIndex = ivIndex;
    }

    @NonNull
    public byte[] getDefaultNetworkKey() {
        return defaultNetworkKey;
    }

    public void setDefaultNetworkKey(@NonNull byte[] defaultNetworkKey) {
        this.defaultNetworkKey = defaultNetworkKey;
    }

    public int getDefaultNetworkKeyIndex() {
        return defaultNetworkKeyIndex;
    }

    public void setDefaultNetworkKeyIndex(int defaultNetworkKeyIndex) {
        this.defaultNetworkKeyIndex = defaultNetworkKeyIndex;
    }

    @NonNull
    public byte[] getDefaultAppKey() {
        return defaultAppKey;
    }

    public void setDefaultAppKey(@NonNull byte[] defaultAppKey) {
        this.defaultAppKey = defaultAppKey;
    }

    public int getDefaultAppKeyIndex() {
        return defaultAppKeyIndex;
    }

    public void setDefaultAppKeyIndex(int defaultAppKeyIndex) {
        this.defaultAppKeyIndex = defaultAppKeyIndex;
    }

    public int getResetDelay() {
        return resetDelay;
    }

    public void setResetDelay(int resetDelay) {
        this.resetDelay = resetDelay;
    }

}

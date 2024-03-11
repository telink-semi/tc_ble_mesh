/********************************************************************************************************
 * @file Parameters.java
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
package com.telink.ble.mesh.foundation.parameter;


import com.telink.ble.mesh.core.ble.LeScanFilter;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.UUID;

/**
 * Mesh action params
 * This is an abstract class representing an event.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 * The event class is generic, allowing for different types of events to be created.
 * The sender and type properties represent the sender object and the type of the event, respectively.
 * The threadMode property represents the thread mode in which the event should be handled.
 * The default thread mode is set to Default.
 */
public class Parameters {

    /**
     * default params
     */
    private static final long DEFAULT_SCAN_SPACING_ABOVE_N = 6 * 1000;

    private static final long DEFAULT_SCAN_TIMEOUT = 10 * 1000;

    private static final long DEFAULT_CONNECT_TIMEOUT = 10 * 1000;

    private static final int DEFAULT_CONNECT_RETRY = 2;

    // min scan space milliseconds
    public static final String COMMON_SCAN_MIN_SPACING = "com.telink.ble.mesh.light.COMMON_SCAN_MIN_SPACING";

    // scan timeout default 10s
    public static final String COMMON_SCAN_TIMEOUT = "com.telink.ble.mesh.light.COMMON_SCAN_TIMEOUT";

    public static final String COMMON_CONNECT_TIMEOUT = "com.telink.ble.mesh.light.COMMON_CONNECT_TIMEOUT";

    // fail retry default 3
    public static final String COMMON_CONNECT_RETRY = "com.telink.ble.mesh.light.COMMON_CONNECT_RETRY";

    public static final String COMMON_PROXY_FILTER_INIT_NEEDED = "com.telink.ble.mesh.light.COMMON_PROXY_FILTER_INIT_NEEDED";

    // scan filter
    public static final String SCAN_FILTERS = "com.telink.ble.mesh.light.SCAN_FILTERS";
    // scan filter
    public static final String SCAN_SINGLE_MODE = "com.telink.ble.mesh.light.SCAN_SINGLE_MODE";

    public static final String ACTION_PROVISIONING_TARGET = "com.telink.ble.mesh.light.ACTION_PROVISIONING_TARGET";

    public static final String ACTION_BINDING_TARGET = "com.telink.ble.mesh.light.ACTION_BINDING_TARGET";

    // fast provision
    public static final String ACTION_FAST_PROVISION_CONFIG = "com.telink.ble.mesh.light.ACTION_FAST_PROVISION_CONFIG";


    // min scan period: default 3s for RSSI filter
    public static final String ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD";

    // scan rest: default 3s
    public static final String ACTION_AUTO_CONNECT_SCAN_REST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_SCAN_REST";


    // target mac list, deprecated
    public static final String ACTION_AUTO_CONNECT_TARGET_LIST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_TARGET_LIST";

    // sparseArray<byte[]>: unicast and node identity key map
    public static final String ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST";

    public static final String ACTION_AUTO_CONNECT_NETWORK_KEY = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_NETWORK_KEY";

    /**
     * OTA firmware
     */
    public static final String ACTION_OTA_FIRMWARE = "com.telink.ble.mesh.light.ACTION_OTA_FIRMWARE";

    /**
     * target filter for direct mesh connection
     */
    public static final String ACTION_CONNECTION_FILTER = "com.telink.ble.mesh.light.ACTION_CONNECTION_FILTER";

    /**
     * target mac address for direct mesh connection
     */
    public static final String ACTION_CONNECTION_MAC_ADDRESS = "com.telink.ble.mesh.light.ACTION_CONNECTION_MAC_ADDRESS";

    /**
     * target name for direct mesh connection: device name should be unique
     */
    public static final String ACTION_CONNECTION_DEVICE_NAME = "com.telink.ble.mesh.light.ACTION_CONNECTION_DEVICE_NAME";

    /**
     * mesh ota targets: int[] addresses
     */
    public static final String ACTION_MESH_OTA_CONFIG = "com.telink.ble.mesh.light.ACTION_MESH_OTA_CONFIG";

    /**
     * mesh ota firmware: byte[]
     */
    public static final String ACTION_MESH_OTA_FIRMWARE = "com.telink.ble.mesh.light.ACTION_MESH_OTA_FIRMWARE";


    protected Map<String, Object> mParams;

    protected Parameters() {
        mParams = new LinkedHashMap<>();
        /*if (ContextUtil.versionAboveN()) {
            this.setScanMinSpacing(DEFAULT_SCAN_SPACING_ABOVE_N);
        } else {
            this.setScanMinSpacing(0);
        }*/
        this.setScanMinSpacing(0);
        this.setScanTimeout(DEFAULT_SCAN_TIMEOUT);
        this.setConnectTimeout(DEFAULT_CONNECT_TIMEOUT);
        this.setConnectRetry(DEFAULT_CONNECT_RETRY);

        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, true);
    }

    /**
     * Sets a value for the specified key.
     *
     * @param key   the key for the parameter
     * @param value the value for the parameter
     */
    protected void set(String key, Object value) {
        if (mParams != null) mParams.put(key, value);
    }

    /**
     * Retrieves the value for the specified key.
     *
     * @param key the key for the parameter
     * @return the value for the parameter, or null if the key does not exist
     */
    public Object get(String key) {
        if (mParams != null) return mParams.get(key);
        return null;
    }

    /**
     * Retrieves the boolean value for the specified key.
     *
     * @param key          the key for the parameter
     * @param defaultValue the default value to return if the key does not exist
     * @return the boolean value for the parameter, or the default value if the key does not exist
     */
    public boolean getBool(String key, boolean defaultValue) {
        if (mParams != null && mParams.containsKey(key)) {
            return (boolean) mParams.get(key);
        }
        return defaultValue;
    }

    /**
     * Retrieves the integer value for the specified key.
     *
     * @param key          the key for the parameter
     * @param defaultValue the default value to return if the key does not exist
     * @return the integer value for the parameter, or the default value if the key does not exist
     */
    public int getInt(String key, int defaultValue) {
        if (mParams != null) {
            return (int) mParams.get(key);
        }
        return defaultValue;
    }

    /**
     * Retrieves the long value for the specified key.
     *
     * @param key          the key for the parameter
     * @param defaultValue the default value to return if the key does not exist
     * @return the long value for the parameter, or the default value if the key does not exist
     */
    public long getLong(String key, long defaultValue) {
        if (mParams != null) {
            return (long) mParams.get(key);
        }
        return defaultValue;
    }

    /**
     * Retrieves the string value for the specified key.
     *
     * @param key the key for the parameter
     * @return the string value for the parameter, or null if the key does not exist
     */
    public String getString(String key) {
        if (mParams != null) {
            return (String) mParams.get(key);
        }
        return null;
    }

    /**
     * Retrieves the string array value for the specified key.
     *
     * @param key the key for the parameter
     * @return the string array value for the parameter, or null if the key does not exist
     */
    public String[] getStringArray(String key) {
        if (mParams != null) {
            return (String[]) mParams.get(key);
        }
        return null;
    }

    /**
     * Retrieves the byte array value for the specified key.
     *
     * @param key the key for the parameter
     * @return the byte array value for the parameter, or null if the key does not exist
     */
    public byte[] getByteArray(String key) {
        if (mParams != null) {
            return (byte[]) mParams.get(key);
        }
        return null;
    }

    /**
     * Sets the minimum scan spacing.
     *
     * @param spacing the minimum scan spacing in milliseconds
     */
    public void setScanMinSpacing(long spacing) {
        this.set(COMMON_SCAN_MIN_SPACING, spacing);
    }

    /**
     * Sets the scan timeout.
     *
     * @param timeout the scan timeout in milliseconds
     */
    public void setScanTimeout(long timeout) {
        this.set(COMMON_SCAN_TIMEOUT, timeout);
    }

    /**
     * Sets the connect timeout.
     *
     * @param timeout the connect timeout in milliseconds
     */
    public void setConnectTimeout(long timeout) {
        this.set(COMMON_CONNECT_TIMEOUT, timeout);
    }

    /**
     * Sets the connect retry count.
     *
     * @param retry the connect retry count
     */
    public void setConnectRetry(int retry) {
        this.set(COMMON_CONNECT_RETRY, retry);
    }

    /**
     * Sets the scan filter.
     *
     * @param scanFilter the scan filter to set
     */
    public void setScanFilter(LeScanFilter scanFilter) {
        this.set(SCAN_FILTERS, scanFilter);
    }

    /**
     * Creates a scan filter with the specified UUIDs.
     *
     * @param uuid the UUIDs to include in the scan filter
     * @return the created scan filter
     */
    public LeScanFilter createScanFilter(UUID[] uuid) {
        LeScanFilter scanFilter = new LeScanFilter();
        scanFilter.uuidInclude = uuid;
        return scanFilter;
    }

    /**
     * Creates a scan filter with the specified UUID.
     *
     * @param uuid the UUID to include in the scan filter
     * @return the created scan filter
     */
    public LeScanFilter createScanFilter(UUID uuid) {
        LeScanFilter scanFilter = new LeScanFilter();
        scanFilter.uuidInclude = new UUID[]{uuid};
        return scanFilter;
    }
}
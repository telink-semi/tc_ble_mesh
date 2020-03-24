package com.telink.ble.mesh.foundation.parameter;


import com.telink.ble.mesh.core.ble.LeScanFilter;
import com.telink.ble.mesh.util.ContextUtil;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.UUID;

/**
 * Mesh action params
 * Created by kee on 2017/11/23.
 */

public class Parameters {

    /**
     * default params
     */
    private static final long DEFAULT_SCAN_SPACING_ABOVE_N = 6 * 1000;

    private static final long DEFAULT_SCAN_TIMEOUT = 10 * 1000;

    private static final long DEFAULT_CONNECT_TIMEOUT = 10 * 1000;

    private static final int DEFAULT_CONNECT_RETRY = 2;

    // 最小扫描间隔
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
    // auto connect

    public static final String ACTION_AUTO_CONNECT_FILTER_TYPE = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_FILTER_TYPE";

    // 最长扫描周期 default 10s
//    public static final String ACTION_AUTO_CONNECT_SCAN_MAX_PERIOD = "";

    // 最短扫描周期 default 3s 针对RSSI过滤
    public static final String ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD";

    // 未扫到时 扫描休息时间 default 3s
    public static final String ACTION_AUTO_CONNECT_SCAN_REST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_SCAN_REST";


    // 目标设备列表 mac
    public static final String ACTION_AUTO_CONNECT_TARGET_LIST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_TARGET_LIST";

    // sparseArray<byte[]>: unicast and node identity key map
    public static final String ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST";

    public static final String ACTION_AUTO_CONNECT_NETWORK_KEY = "com.telink.ble.mesh.light.ACTION_AUTO_CONNECT_NETWORK_KEY";

    /**
     * OTA firmware
     */
    public static final String ACTION_OTA_FIRMWARE = "com.telink.ble.mesh.light.ACTION_OTA_FIRMWARE";

    /**
     * OTA mac address
     */
    public static final String ACTION_OTA_MESH_ADDRESS = "com.telink.ble.mesh.light.ACTION_OTA_MESH_ADDRESS";

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

    protected void set(String key, Object value) {
        if (mParams != null) mParams.put(key, value);
    }

    public Object get(String key) {
        if (mParams != null) return mParams.get(key);
        return null;
    }

    public boolean getBool(String key, boolean defaultValue) {
        if (mParams != null && mParams.containsKey(key)) {
            return (boolean) mParams.get(key);
        }
        return defaultValue;
    }

    public int getInt(String key, int defaultValue) {
        if (mParams != null) {
            return (int) mParams.get(key);
        }
        return defaultValue;
    }

    public long getLong(String key, long defaultValue) {
        if (mParams != null) {
            return (long) mParams.get(key);
        }
        return defaultValue;
    }

    public String getString(String key) {
        if (mParams != null) {
            return (String) mParams.get(key);
        }
        return null;
    }

    public String[] getStringArray(String key) {
        if (mParams != null) {
            return (String[]) mParams.get(key);
        }
        return null;
    }

    public byte[] getByteArray(String key) {
        if (mParams != null) {
            return (byte[]) mParams.get(key);
        }
        return null;
    }


    public void setScanMinSpacing(long spacing) {
        this.set(COMMON_SCAN_MIN_SPACING, spacing);
    }

    /**
     * 设置扫描超时 millisecond
     *
     * @param timeout millisecond
     */
    public void setScanTimeout(long timeout) {
        this.set(COMMON_SCAN_TIMEOUT, timeout);
    }

    public void setConnectTimeout(long timeout) {
        this.set(COMMON_CONNECT_TIMEOUT, timeout);
    }

    public void setConnectRetry(int retry) {
        this.set(COMMON_CONNECT_RETRY, retry);
    }

    public void setScanFilter(LeScanFilter scanFilter) {
        this.set(SCAN_FILTERS, scanFilter);
    }

    public LeScanFilter createScanFilter(UUID[] uuid) {
        LeScanFilter scanFilter = new LeScanFilter();
        scanFilter.uuidInclude = uuid;
        return scanFilter;
    }

    public LeScanFilter createScanFilter(UUID uuid) {
        LeScanFilter scanFilter = new LeScanFilter();
        scanFilter.uuidInclude = new UUID[]{uuid};
        return scanFilter;
    }

}

/********************************************************************************************************
 * @file Parameters.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.light.parameter;

import com.telink.sig.mesh.ble.LeScanFilter;
import com.telink.sig.mesh.util.ContextUtil;

import java.util.LinkedHashMap;
import java.util.Map;

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
    public static final String COMMON_SCAN_MIN_SPACING = "com.telink.sig.mesh.light.COMMON_SCAN_MIN_SPACING";

    // scan timeout default 10s
    public static final String COMMON_SCAN_TIMEOUT = "com.telink.sig.mesh.light.COMMON_SCAN_TIMEOUT";

    public static final String COMMON_CONNECT_TIMEOUT = "com.telink.sig.mesh.light.COMMON_CONNECT_TIMEOUT";

    // fail retry default 3
    public static final String COMMON_CONNECT_RETRY = "com.telink.sig.mesh.light.COMMON_CONNECT_RETRY";

    // scan filter
    public static final String SCAN_FILTERS = "com.telink.sig.mesh.light.SCAN_FILTERS";
    // scan filter
    public static final String SCAN_SINGLE_MODE = "com.telink.sig.mesh.light.SCAN_SINGLE_MODE";

    public static final String ACTION_PROVISION_TARGET = "com.telink.sig.mesh.light.ACTION_PROVISION_TARGET";

    //    public static final String ACTION_PROVISION_NETWORK_KEY = "com.telink.sig.mesh.light.ACTION_PROVISION_NETWORK_KEY";
    public static final String ACTION_PROVISION_DATA = "com.telink.sig.mesh.light.ACTION_PROVISION_DATA";

    // auto connect
    // 最长扫描周期 default 10s
//    public static final String ACTION_AUTO_CONNECT_SCAN_MAX_PERIOD = "";

    // 最短扫描周期 default 3s 针对RSSI过滤
    public static final String ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD = "com.telink.sig.mesh.light.ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD";

    // 未扫到时 扫描休息时间 default 3s
    public static final String ACTION_AUTO_CONNECT_SCAN_REST = "com.telink.sig.mesh.light.ACTION_AUTO_CONNECT_SCAN_REST";


    // 目标设备列表 mac
    public static final String ACTION_AUTO_CONNECT_TARGET_LIST = "com.telink.sig.mesh.light.ACTION_AUTO_CONNECT_TARGET_LIST";

    // sparseArray<byte[]>: unicast and node identity key map
    public static final String ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST = "com.telink.sig.mesh.light.ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST";

    public static final String ACTION_AUTO_CONNECT_NETWORK_KEY = "com.telink.sig.mesh.light.ACTION_AUTO_CONNECT_NETWORK_KEY";

    /**
     * OTA firmware
     */
    public static final String ACTION_OTA_FIRMWARE = "com.telink.sig.mesh.light.ACTION_OTA_FIRMWARE";

    /**
     * OTA mac address
     */
    public static final String ACTION_OTA_MAC = "com.telink.sig.mesh.light.ACTION_OTA_MAC";

    /**
     * mesh ota targets: int[] addresses
     */
    public static final String ACTION_MESH_OTA_TARGETS = "com.telink.sig.mesh.light.ACTION_MESH_OTA_TARGETS";

    /**
     * mesh ota firmware: byte[]
     */
    public static final String ACTION_MESH_OTA_FIRMWARE = "com.telink.sig.mesh.light.ACTION_MESH_OTA_FIRMWARE";

    // key bind action
    public static final String ACTION_BIND_FAST_BIND = "com.telink.sig.mesh.light.ACTION_BIND_FAST_BIND";

    public static final String ACTION_BIND_APP_KEY = "com.telink.sig.mesh.light.ACTION_BIND_APP_KEY";

    public static final String ACTION_BIND_APP_KEY_INDEX = "com.telink.sig.mesh.light.ACTION_BIND_APP_KEY_INDEX";

    public static final String ACTION_BIND_NET_KEY_INDEX = "com.telink.sig.mesh.light.ACTION_BIND_NET_KEY_INDEX";

    public static final String ACTION_BIND_DEVICE_INFO = "com.telink.sig.mesh.light.ACTION_BIND_DEVICE_INFO";

    public static final String ACTION_BIND_MODEL_LIST = "com.telink.sig.mesh.light.ACTION_BIND_MODEL_LIST";

    public static final String ACTION_REMOTE_PROVISION_DEVICE = "com.telink.sig.mesh.light.ACTION_REMOTE_PROVISION_DEVICE";

    public static final String ACTION_REMOTE_BIND_DEVICE = "com.telink.sig.mesh.light.ACTION_REMOTE_PROVISION_DEVICE";

    public static final String ACTION_FAST_PROVISION_ADDRESS = "com.telink.sig.mesh.light.ACTION_FAST_PROVISION_ADDRESS";

    public static final String ACTION_FAST_PROVISION_PID = "com.telink.sig.mesh.light.ACTION_FAST_PROVISION_PID";

    public static final String ACTION_FAST_PROVISION_ELE_CNT = "com.telink.sig.mesh.light.ACTION_FAST_PROVISION_ELE_CNT";
    protected Map<String, Object> mParams;

    protected Parameters() {
        mParams = new LinkedHashMap<>();
        if (ContextUtil.versionAboveN()) {
            this.setScanMinSpacing(DEFAULT_SCAN_SPACING_ABOVE_N);
        } else {
            this.setScanMinSpacing(0);
        }
        this.setScanTimeout(DEFAULT_SCAN_TIMEOUT);
        this.setConnectTimeout(DEFAULT_CONNECT_TIMEOUT);
        this.setConnectRetry(DEFAULT_CONNECT_RETRY);
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

}

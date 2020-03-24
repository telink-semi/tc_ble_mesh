/********************************************************************************************************
 * @file AutoConnectParameters.java
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

import android.util.SparseArray;

import com.telink.sig.mesh.ble.LeScanFilter;
import com.telink.sig.mesh.light.UuidInfo;

import java.util.Set;
import java.util.UUID;

/**
 * Auto connect params
 * Created by kee on 2017/11/23.
 */

public class AutoConnectParameters extends Parameters {
    /**
     * default scan min period for Rssi filter
     */
    private static final long DEFAULT_SCAN_MIN_PERIOD = 3 * 1000;

    /**
     * default scan rest when no scan result until timeout
     */
    private static final long DEFAULT_SCAN_REST = 3 * 1000;

    /**
     * default refresh lum count after connect mesh success
     */
    private static final int DEFAULT_REFRESH_STATUS_COUNT = 1;

    /**
     * refresh lum spacing
     */
    private static final long DEFAULT_REFRESH_STATUS_SPACING = 2 * 1000;

    /**
     * get default settings by node identity or network id
     *
     * @return instance
     */
    public static AutoConnectParameters getDefault(int[] unicastAddresses, byte[] networkKey) {
        AutoConnectParameters parameters = new AutoConnectParameters();
        LeScanFilter filter = new LeScanFilter();
        filter.uuidInclude = new UUID[]{UuidInfo.PROXY_SERVICE_UUID};
        parameters.setScanFilter(filter);
        parameters.setNodeList(unicastAddresses);
        parameters.setNetworkKey(networkKey);
        parameters.setScanMinPeriod(DEFAULT_SCAN_MIN_PERIOD);
        parameters.setScanRest(DEFAULT_SCAN_REST);
        return parameters;
    }

    /**
     * will be deprecated next version
     *
     * @param targets mac addresses
     * @return instance
     */
    public static AutoConnectParameters getDefault(Set<String> targets) {
        AutoConnectParameters parameters = new AutoConnectParameters();

        LeScanFilter filter = new LeScanFilter();
        if (targets != null && targets.size() != 0) {
            filter.macInclude = targets.toArray(new String[]{});
        }
        filter.uuidInclude = new UUID[]{UuidInfo.PROXY_SERVICE_UUID};
        parameters.setScanFilter(filter);

        parameters.setTargets(targets);
        parameters.setScanMinPeriod(DEFAULT_SCAN_MIN_PERIOD);
        parameters.setScanRest(DEFAULT_SCAN_REST);
        return parameters;
    }

    public void setNodeList(int[] addresses) {
        this.set(ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST, addresses);
    }

    public void setNetworkKey(byte[] networkKey) {
        this.set(ACTION_AUTO_CONNECT_NETWORK_KEY, networkKey);
    }

    public void setTargets(Set<String> mac) {
        this.set(ACTION_AUTO_CONNECT_TARGET_LIST, mac);
    }

    public void setScanMinPeriod(long period) {
        this.set(ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD, period);
    }

    public void setScanRest(long period) {
        this.set(ACTION_AUTO_CONNECT_SCAN_REST, period);
    }


}

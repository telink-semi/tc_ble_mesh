/********************************************************************************************************
 * @file KeyBindParameters.java
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
import com.telink.sig.mesh.light.UuidInfo;
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.UUID;

/**
 * Created by kee on 2017/11/23.
 */

public class KeyBindParameters extends Parameters {

    /**
     * @param deviceInfo  target device
     * @param appKey      application key
     * @param appKeyIndex key index
     * @param netKeyIndex network key index
     * @param fastBind    is fast bind mode,
     * @return instance
     */
    public static KeyBindParameters getDefault(DeviceInfo deviceInfo, byte[] appKey, int appKeyIndex, int netKeyIndex, boolean fastBind) {
        KeyBindParameters parameters = new KeyBindParameters();
        parameters.setAppKey(appKey);
        parameters.setAppKeyIndex(appKeyIndex);
        parameters.setNetKeyIndex(netKeyIndex);
        parameters.setFastBind(fastBind);
        LeScanFilter filter = new LeScanFilter();
        filter.macInclude = new String[]{deviceInfo.macAddress};
        filter.uuidInclude = new UUID[]{UuidInfo.PROXY_SERVICE_UUID};
        parameters.setScanFilter(filter);
        parameters.setDeviceInfo(deviceInfo);
//        parameters.setModels(models);
        return parameters;
    }

    public void setFastBind(boolean fastBind) {
        this.set(ACTION_BIND_FAST_BIND, fastBind);
    }

    public void setAppKey(byte[] data) {
        this.set(ACTION_BIND_APP_KEY, data);
    }

    public void setAppKeyIndex(int index) {
        this.set(ACTION_BIND_APP_KEY_INDEX, index);
    }

    public void setNetKeyIndex(int index) {
        this.set(ACTION_BIND_NET_KEY_INDEX, index);
    }

    public void setDeviceInfo(DeviceInfo deviceInfo) {
        this.set(ACTION_BIND_DEVICE_INFO, deviceInfo);
    }

    public void setModels(short[] models) {
        this.set(ACTION_BIND_MODEL_LIST, models);
    }
}

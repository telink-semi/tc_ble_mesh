/********************************************************************************************************
 * @file Group.java
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
package com.telink.sig.mesh.model;

import java.io.Serializable;
import java.util.Arrays;

/**
 * device info upload when scan remote devices during remote provision
 */
public class RemoteDevice implements Serializable {
    /**
     * node address : remote scanned from
     * valued by notification event
     */
    public int src;

    /**
     * rssi, uuid, oob
     * valued by
     *
     * @see com.telink.sig.mesh.light.RemoteScanReportParser
     */
    public byte rssi;
    public byte[] uuid;
    public int oob;

    public String mac;
    /**
     * provision primary address
     * valued before do remote provision action
     */
    public int pvAddress;

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        RemoteDevice that = (RemoteDevice) o;

        return Arrays.equals(uuid, that.uuid);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(uuid);
    }


    @Override
    public String toString() {
        return "RemoteDevice{" +
                "src=" + src +
                ", rssi=" + rssi +
                ", uuid=" + com.telink.sig.mesh.util.Arrays.bytesToHexString(uuid, ":") +
                ", oob=" + oob +
                ", mac='" + mac + '\'' +
                ", pvAddress=" + pvAddress +
                '}';
    }
}

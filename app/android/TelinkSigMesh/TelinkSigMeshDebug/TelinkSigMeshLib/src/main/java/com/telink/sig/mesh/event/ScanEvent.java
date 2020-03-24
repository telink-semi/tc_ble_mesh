/********************************************************************************************************
 * @file     ScanEvent.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.event;

import com.telink.sig.mesh.ble.AdvertisingDevice;

/**
 * Scan events
 * Created by kee on 2018/7/19.
 */

public class ScanEvent extends Event<String> {

    // find target device
    public static final String DEVICE_FOUND = "com.telink.sig.mesh.ScanEvent.LE_SCAN";

    // scan timeout
    public static final String SCAN_TIMEOUT = "com.telink.sig.mesh.ScanEvent.SCAN_TIMEOUT";

    public static final String SCAN_FAIL = "com.telink.sig.mesh.ScanEvent.SCAN_FAIL";


    public AdvertisingDevice advertisingDevice;

    public ScanEvent(Object sender, String type, AdvertisingDevice advertisingDevice) {
        super(sender, type);
        this.advertisingDevice = advertisingDevice;
    }

    public ScanEvent(Object sender, String type) {
        super(sender, type);
    }

}

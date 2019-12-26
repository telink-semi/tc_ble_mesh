/********************************************************************************************************
 * @file     OnlineStatusEvent.java
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

public class OnlineStatusEvent extends Event<String> {

    // find target device
    public static final String ONLINE_STATUS_NOTIFY = "com.telink.sig.mesh.ONLINE_STATUS_NOTIFY";

    public byte[] rawData;


    public OnlineStatusEvent(Object sender, String type, byte[] data) {
        super(sender, type);
        this.rawData = data;
    }

    public OnlineStatusEvent(Object sender, String type) {
        super(sender, type);
    }


}

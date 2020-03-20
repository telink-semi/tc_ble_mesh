/********************************************************************************************************
 * @file     OtaEvent.java 
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

import com.telink.sig.mesh.model.OtaDeviceInfo;

/**
 * Created by kee on 2017/8/30.
 */

public class OtaEvent extends Event<String> {
    
    public static final String EVENT_TYPE_OTA_SUCCESS = "com.telink.sig.mesh.OTA_SUCCESS";

    public static final String EVENT_TYPE_OTA_FAIL = "com.telink.sig.mesh.OTA_FAIL";

    public static final String EVENT_TYPE_OTA_PROGRESS = "com.telink.sig.mesh.OTA_PROGRESS";

    private OtaDeviceInfo deviceInfo;
    private int progress;

    public OtaEvent(Object sender, String type, OtaDeviceInfo deviceInfo) {
        super(sender, type);
        this.deviceInfo = deviceInfo;
    }

    public OtaDeviceInfo getDeviceInfo() {
        return deviceInfo;
    }
}

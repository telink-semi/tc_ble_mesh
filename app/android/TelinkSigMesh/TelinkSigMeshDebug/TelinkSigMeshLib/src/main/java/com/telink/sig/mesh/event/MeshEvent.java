/********************************************************************************************************
 * @file     MeshEvent.java 
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

import com.telink.sig.mesh.model.DeviceInfo;

/**
 * Created by kee on 2017/8/30.
 */

public class MeshEvent extends Event<String> {

    public static final String EVENT_TYPE_MESH_EMPTY = "com.telink.sig.mesh.MESH_EMPTY";

    public static final String EVENT_TYPE_PROVISION_SUCCESS = "com.telink.sig.mesh.PROVISION_SUCCESS";

    public static final String EVENT_TYPE_PROVISION_COMPLETE = "com.telink.sig.mesh.EVENT_TYPE_PROVISION_COMPLETE";

    public static final String EVENT_TYPE_PROVISION_FAIL = "com.telink.sig.mesh.PROVISION_FAIL";

    public static final String EVENT_TYPE_KEY_BIND_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_KEY_BIND_SUCCESS";

    public static final String EVENT_TYPE_KEY_BIND_FAIL = "com.telink.sig.mesh.EVENT_TYPE_KEY_BIND_FAIL";

    public static final String EVENT_TYPE_AUTO_CONNECT_LOGIN = "com.telink.sig.mesh.AUTO_CONNECT_LOGIN";

    public static final String EVENT_TYPE_DISCONNECTED = "com.telink.sig.mesh.DISCONNECTED";

    public static final String EVENT_TYPE_DEVICE_OFFLINE = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_OFFLINE";

    // 网络被重新配置
    public static final String EVENT_TYPE_MESH_RESET = "com.telink.sig.mesh.EVENT_TYPE_MESH_RESET";


    public static final String EVENT_TYPE_REMOTE_PROVISION_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISION_SUCCESS";

    public static final String EVENT_TYPE_REMOTE_PROVISION_FAIL = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISION_FAIL";

    public static final String EVENT_TYPE_REMOTE_BIND_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_BIND_SUCCESS";

    public static final String EVENT_TYPE_REMOTE_BIND_FAIL = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_BIND_FAIL";

    // set device address success
    public static final String EVENT_TYPE_FAST_PROVISION_DEVICE_ADDRESS_SET = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISION_DEVICE_ADDRESS_SET";

    // all success
    public static final String EVENT_TYPE_FAST_PROVISION_COMPLETE = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISION_COMPLETE";

    // all fail
    public static final String EVENT_TYPE_FAST_PROVISION_FAIL = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISION_FAIL";

    private DeviceInfo deviceInfo;

    public MeshEvent(Object sender, String type, DeviceInfo deviceInfo) {
        super(sender, type);
        this.deviceInfo = deviceInfo;
    }

    public DeviceInfo getDeviceInfo() {
        return deviceInfo;
    }

}

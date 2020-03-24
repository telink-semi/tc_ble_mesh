/********************************************************************************************************
 * @file UnprovisionedDevice.java
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
package com.telink.sig.mesh.demo.model;

import com.telink.sig.mesh.model.NodeInfo;

/**
 * Created by kee on 2019/04/01.
 */

public class ProvisioningDevice {


    public static final int STATE_PROVISION_FAIL = -1;

    public static final int STATE_PROVISIONING = 0;

    // binding equals provision success
    public static final int STATE_BINDING = 1;

    public static final int STATE_BIND_SUCCESS = 2;

    public static final int STATE_PUB_SET_SUCCESS = 3;

    public static final int STATE_PUB_SET_FAIL = 4;

    public static final int STATE_BIND_FAIL = -2;


    //
    public byte[] uuid;

    public String macAddress;

    /**
     * -1: provision fail
     * 0: provisioning
     * 1: binding
     * 2: bind success
     * 3: bind fail
     */
    public int state;

    public int unicastAddress;

    public int elementCnt;

    public String failReason;

    public NodeInfo nodeInfo;

    public boolean didRemoteScan = false;

//    public int retryCnt = 0;

    public ProvisioningDevice() {

    }

    public String getStateDesc() {
        switch (state) {
            case STATE_PROVISION_FAIL:
                return "Provision Fail";

            case STATE_PROVISIONING:
                return "Provisioning";

            case STATE_BINDING:
                return "Key Binding";

            case STATE_BIND_SUCCESS:
                return "Key Bind Success";

            case STATE_BIND_FAIL:
                return "Key Bind Fail";

            case STATE_PUB_SET_SUCCESS:
                return "Pub Set Success";

            case STATE_PUB_SET_FAIL:
                return "Pub Set Fail";
        }
        return "UNKNOWN";
    }

}
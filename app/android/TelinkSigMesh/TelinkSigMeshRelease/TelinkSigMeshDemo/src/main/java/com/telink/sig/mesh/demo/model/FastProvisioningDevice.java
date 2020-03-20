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

public class FastProvisioningDevice {


    public String macAddress;

    /**
     * -1: provision fail
     * 0: provisioning
     * 1: binding
     * 2: bind success
     * 3: bind fail
     */
    public int deviceKey;

    public int unicastAddress;

    public int elementCnt;

    public NodeInfo nodeInfo;


    public FastProvisioningDevice() {

    }

}
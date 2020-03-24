/********************************************************************************************************
 * @file ProvisionParameters.java
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

import com.telink.sig.mesh.ble.UnprovisionedDevice;
import com.telink.sig.mesh.model.RemoteDevice;

import java.util.Set;

/**
 * Created by kee on 2017/11/23.
 */

public class RemoteProvisionParameters extends Parameters {

    /**
     * @param data @see #ProvisionDataGenerator
     * @return
     */
    public static RemoteProvisionParameters getDefault(byte[] data, RemoteDevice remoteDevice) {
        RemoteProvisionParameters parameters = new RemoteProvisionParameters();
        parameters.setProvisionData(data);
        parameters.setRemoteDevice(remoteDevice);
        return parameters;
    }

    public void setRemoteDevice(RemoteDevice device) {
        this.set(ACTION_REMOTE_PROVISION_DEVICE, device);
    }


    public void setProvisionData(byte[] data) {
        this.set(ACTION_PROVISION_DATA, data);
    }


}

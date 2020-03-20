/********************************************************************************************************
 * @file     ProvisionParameters.java 
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
package com.telink.sig.mesh.light.parameter;

import com.telink.sig.mesh.ble.UnprovisionedDevice;

/**
 * Created by kee on 2017/11/23.
 */

public class ProvisionParameters extends Parameters {

    /**
     * @param data  @see #ProvisionDataGenerator
     * @param targets unprovisioned devices
     * @return
     */
    public static ProvisionParameters getDefault(byte[] data, UnprovisionedDevice targets) {
        ProvisionParameters parameters = new ProvisionParameters();
        parameters.setProvisionData(data);
        parameters.setTargets(targets);
        return parameters;
    }

    public void setTargets(UnprovisionedDevice devices) {
        this.set(ACTION_PROVISION_TARGET, devices);
    }

    public void setProvisionData(byte[] data) {
        this.set(ACTION_PROVISION_DATA, data);
    }


}

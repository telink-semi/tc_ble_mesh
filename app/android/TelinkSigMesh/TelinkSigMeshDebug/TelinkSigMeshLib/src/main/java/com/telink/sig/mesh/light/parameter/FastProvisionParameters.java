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

import java.util.UUID;

/**
 * Created by kee on 2017/11/23.
 */

public class FastProvisionParameters extends Parameters {


    /**
     * @param addressIndex provisioning index
     * @param pid          pid
     * @return instance
     */
    public static FastProvisionParameters getDefault(int addressIndex, int pid, int elementCnt) {
        FastProvisionParameters parameters = new FastProvisionParameters();
        parameters.setAddressIndex(addressIndex);
        parameters.setPid(pid);
        parameters.setElementCount(elementCnt);

        LeScanFilter filter = new LeScanFilter();
        filter.uuidInclude = new UUID[]{UuidInfo.PROVISION_SERVICE_UUID};
        parameters.setScanFilter(filter);
        return parameters;
    }

    public void setAddressIndex(int addressIndex) {
        this.set(ACTION_FAST_PROVISION_ADDRESS, addressIndex);
    }

    public void setPid(int pid) {
        this.set(ACTION_FAST_PROVISION_PID, pid);
    }

    public void setElementCount(int elementCount) {
        this.set(ACTION_FAST_PROVISION_ELE_CNT, elementCount);
    }
}

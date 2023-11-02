/********************************************************************************************************
 * @file BindingParameters.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.entity.BindingDevice;


/**
 * This class represents the binding parameters for a target device.
 * It extends the Parameters class.
 */
public class BindingParameters extends Parameters {

    /**
     * The BindingParameters constructor takes a BindingDevice object as a parameter, which represents the target device.
     * It sets the scan filter using the createScanFilter method and passing the UUIDInfo.SERVICE_PROXY parameter.
     * It also sets the ACTION_BINDING_TARGET parameter to the provided device.
     *
     * @param device target device
     */
    public BindingParameters(BindingDevice device) {
        this.setScanFilter(createScanFilter(UUIDInfo.SERVICE_PROXY));
        this.set(ACTION_BINDING_TARGET, device);
    }


}

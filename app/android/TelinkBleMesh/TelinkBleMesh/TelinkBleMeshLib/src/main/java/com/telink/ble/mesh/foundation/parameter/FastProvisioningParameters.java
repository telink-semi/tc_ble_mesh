/********************************************************************************************************
 * @file FastProvisioningParameters.java
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
import com.telink.ble.mesh.entity.FastProvisioningConfiguration;


/**
 * This class represents the parameters for fast provisioning.
 * It extends the Parameters class.
 */
public class FastProvisioningParameters extends Parameters {

    /**
     * Constructs a new FastProvisioningParameters object with the given fast provisioning configuration.
     *
     * @param fastProvisioningConfiguration The fast provisioning configuration to be set.
     */
    public FastProvisioningParameters(FastProvisioningConfiguration fastProvisioningConfiguration) {
        this.setScanFilter(createScanFilter(UUIDInfo.SERVICE_PROVISION));
        this.set(ACTION_FAST_PROVISION_CONFIG, fastProvisioningConfiguration);
        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, false);
    }
}
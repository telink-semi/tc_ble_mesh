/********************************************************************************************************
 * @file ScanParameters.java
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


import com.telink.ble.mesh.core.ble.LeScanFilter;
import com.telink.ble.mesh.core.ble.UUIDInfo;

import java.util.UUID;

/**
 * Scan params
 * This class represents the scan parameters for Bluetooth scanning.
 * It extends the Parameters class.
 */
public class ScanParameters extends Parameters {

    private LeScanFilter filter; // The scan filter used for filtering scan results.

    /**
     * Default constructor for ScanParameters.
     * Initializes the scan filter.
     */
    public ScanParameters() {
        filter = new LeScanFilter();
    }

    /**
     * Returns the default scan parameters.
     *
     * @param provisioned - a boolean indicating if the device is provisioned
     * @param single      - a boolean indicating if the scan is in single mode
     * @return the default ScanParameters object
     */
    public static ScanParameters getDefault(boolean provisioned, boolean single) {
        ScanParameters parameters = new ScanParameters();

        if (provisioned) {
            parameters.filter.uuidInclude = new UUID[]{UUIDInfo.SERVICE_PROXY};
        } else {
            parameters.filter.uuidInclude = new UUID[]{UUIDInfo.SERVICE_PROVISION};
        }

        parameters.setScanFilter(parameters.filter);
        parameters.singleMode(single);

        return parameters;
    }

    /**
     * Sets the MAC addresses to be included in the scan.
     *
     * @param macs - an array of MAC addresses to be included
     */
    public void setIncludeMacs(String[] macs) {
        if (filter != null) {
            filter.macInclude = macs;
        }
    }

    /**
     * Sets the MAC addresses to be excluded from the scan.
     *
     * @param macs - an array of MAC addresses to be excluded
     */
    public void setExcludeMacs(String[] macs) {
        if (filter != null) {
            filter.macExclude = macs;
        }
    }

    /**
     * Sets the scan mode to single or continuous.
     *
     * @param single - a boolean indicating if the scan is in single mode
     */
    public void singleMode(boolean single) {
        this.set(SCAN_SINGLE_MODE, single);
    }
}
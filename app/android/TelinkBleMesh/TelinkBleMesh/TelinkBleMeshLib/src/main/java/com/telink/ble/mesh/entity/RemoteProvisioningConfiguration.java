/********************************************************************************************************
 * @file RemoteProvisioningConfiguration.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.entity;

/**
 * remote provisioning configurations
 * Created by kee on 2019/11/27.
 */

public class RemoteProvisioningConfiguration {
    // scan for 2 devices
    private static final int DEFAULT_SCAN_LIMIT = 2;

    // scan 3 seconds
    private static final int DEFAULT_SCAN_TIMEOUT = 3;

    // if app key binding needed
    private boolean bindNeed = true;

    // default bind
    private boolean defaultBind = false;

    private int scanLimit;

    private int scanTimeout;

    private int provisioningIndex;


    public RemoteProvisioningConfiguration(int provisioningIndex) {
        this.provisioningIndex = provisioningIndex;
    }

    public boolean isBindNeed() {
        return bindNeed;
    }

    public void setBindNeed(boolean bindNeed) {
        this.bindNeed = bindNeed;
    }

    public boolean isDefaultBind() {
        return defaultBind;
    }

    public void setDefaultBind(boolean defaultBind) {
        this.defaultBind = defaultBind;
    }

    public int getScanLimit() {
        return scanLimit;
    }

    public void setScanLimit(int scanLimit) {
        this.scanLimit = scanLimit;
    }

    public int getScanTimeout() {
        return scanTimeout;
    }

    public void setScanTimeout(int scanTimeout) {
        this.scanTimeout = scanTimeout;
    }

    public int getProvisioningIndex() {
        return provisioningIndex;
    }

    public void setProvisioningIndex(int provisioningIndex) {
        this.provisioningIndex = provisioningIndex;
    }
}

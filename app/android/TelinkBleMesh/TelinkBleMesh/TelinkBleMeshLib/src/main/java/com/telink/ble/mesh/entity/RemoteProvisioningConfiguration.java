/********************************************************************************************************
 * @file RemoteProvisioningConfiguration.java
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
package com.telink.ble.mesh.entity;


/**
 * @deprecated remote provisioning configurations
 * This class represents the configuration settings for remote provisioning.
 * It includes options for scanning devices, app key binding, and provisioning index.
 */
public class RemoteProvisioningConfiguration {
    // The default number of devices to scan for
    private static final int DEFAULT_SCAN_LIMIT = 2;
    // The default scan timeout in seconds
    private static final int DEFAULT_SCAN_TIMEOUT = 3;
    // Indicates if app key binding is needed
    private boolean bindNeed = true;
    // Indicates if default binding is enabled
    private boolean defaultBind = false;
    private int scanLimit; // The number of devices to scan for
    private int scanTimeout; // The scan timeout in seconds
    private int provisioningIndex; // The provisioning index

    /**
     * Constructs a new RemoteProvisioningConfiguration with the given provisioning index.
     *
     * @param provisioningIndex The provisioning index to set
     */
    public RemoteProvisioningConfiguration(int provisioningIndex) {
        this.provisioningIndex = provisioningIndex;
    }

    /**
     * Checks if app key binding is needed.
     *
     * @return True if app key binding is needed, false otherwise
     */
    public boolean isBindNeed() {
        return bindNeed;
    }

    /**
     * Sets the flag indicating if app key binding is needed.
     *
     * @param bindNeed True if app key binding is needed, false otherwise
     */
    public void setBindNeed(boolean bindNeed) {
        this.bindNeed = bindNeed;
    }

    /**
     * Checks if default binding is enabled.
     *
     * @return True if default binding is enabled, false otherwise
     */
    public boolean isDefaultBind() {
        return defaultBind;
    }

    /**
     * Sets the flag indicating if default binding is enabled.
     *
     * @param defaultBind True if default binding is enabled, false otherwise
     */
    public void setDefaultBind(boolean defaultBind) {
        this.defaultBind = defaultBind;
    }

    /**
     * Gets the number of devices to scan for.
     *
     * @return The number of devices to scan for
     */
    public int getScanLimit() {
        return scanLimit;
    }

    /**
     * Sets the number of devices to scan for.
     *
     * @param scanLimit The number of devices to scan for
     */
    public void setScanLimit(int scanLimit) {
        this.scanLimit = scanLimit;
    }

    /**
     * Gets the scan timeout in seconds.
     *
     * @return The scan timeout in seconds
     */
    public int getScanTimeout() {
        return scanTimeout;
    }

    /**
     * Sets the scan timeout in seconds.
     *
     * @param scanTimeout The scan timeout in seconds
     */
    public void setScanTimeout(int scanTimeout) {
        this.scanTimeout = scanTimeout;
    }

    /**
     * Gets the provisioning index.
     *
     * @return The provisioning index
     */
    public int getProvisioningIndex() {
        return provisioningIndex;
    }

    /**
     * Sets the provisioning index.
     *
     * @param provisioningIndex The provisioning index to set
     */
    public void setProvisioningIndex(int provisioningIndex) {
        this.provisioningIndex = provisioningIndex;
    }
}
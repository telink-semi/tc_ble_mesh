/********************************************************************************************************
 * @file AutoConnectFilterType.java
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

/**
 * Enum representing the types of filters for auto-connect functionality.
 * Each filter type has a flag indicating whether it supports node identity, network id, or both.
 */
public enum AutoConnectFilterType {
    /**
     * Filter type that supports node identity only.
     */
    NODE_IDENTITY(true, false),

    /**
     * Filter type that supports network id only.
     */
    NETWORK_ID(false, true),

    /**
     * Filter type that supports both node identity and network id.
     */
    AUTO(true, true);

    /**
     * Flag indicating whether the filter type supports node identity.
     */
    public final boolean isNodeIdentitySupport;

    /**
     * Flag indicating whether the filter type supports network id.
     */
    public final boolean isNetworkIdSupport;

    /**
     * Constructor for the AutoConnectFilterType enum.
     *
     * @param isNodeIdentitySupport - Flag indicating whether the filter type supports node identity.
     * @param isNetworkIdSupport    - Flag indicating whether the filter type supports network id.
     */
    AutoConnectFilterType(boolean isNodeIdentitySupport, boolean isNetworkIdSupport) {
        this.isNodeIdentitySupport = isNodeIdentitySupport;
        this.isNetworkIdSupport = isNetworkIdSupport;
    }
}
/********************************************************************************************************
 * @file GattConnectionParameters.java
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

import androidx.annotation.NonNull;

import com.telink.ble.mesh.entity.ConnectionFilter;


/**
 * This class represents the parameters for a GATT connection. It extends the Parameters class.
 * It takes a ConnectionFilter object as a parameter in its constructor.
 */
public class GattConnectionParameters extends Parameters {

    /**
     * Constructs a new GattConnectionParameters object with the specified ConnectionFilter.
     * Initializes the COMMON_PROXY_FILTER_INIT_NEEDED parameter to true and sets the ACTION_CONNECTION_FILTER parameter to the given filter.
     *
     * @param filter The ConnectionFilter object to be set as the ACTION_CONNECTION_FILTER parameter.
     */
    public GattConnectionParameters(@NonNull ConnectionFilter filter) {
        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, true);
        this.set(ACTION_CONNECTION_FILTER, filter);
    }
}